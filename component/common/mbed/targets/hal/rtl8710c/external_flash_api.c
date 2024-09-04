/**************************************************************************//**
 * @file     external_flash_api.c
 * @brief    This file implements the EXTERNAL FLASH API functions.
 *
 * @version  V1.00
 * @date     2022-12-20
 *
 * @note     Only applicable to flash type of BH25D16A, which device identification is (68 40 15).
 ******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************/

#include "FreeRTOS.h"
#include "device_lock.h"
#include "external_flash_api.h"
#include "sys_api.h"
#include "spi_api.h"
#include "spi_ex_api.h"
#include "platform_stdlib.h"

static xSemaphoreHandle xTxSemaphore;
static xSemaphoreHandle xRxSemaphore;

void external_flash_tr_done_callback(void *pdata, SpiIrq event)
{
	static portBASE_TYPE xHigherPriorityTaskWoken;
	xHigherPriorityTaskWoken = pdFALSE;

	switch (event) {
	case SpiRxIrq:
		xSemaphoreGiveFromISR(xRxSemaphore, &xHigherPriorityTaskWoken);
		break;
	case SpiTxIrq:
		xSemaphoreGiveFromISR(xTxSemaphore, &xHigherPriorityTaskWoken);
		break;
	default:
		dbg_printf("unknown interrput evnent! \r\n");
	}
}

/* Master mode read a sequence of data
** rx_buffer：buffer for reading data
       Notes: The first four bytes are the dummy bytes of the command, and the data starts from the fifth byte
** length：length of data to be read
** offset: start address of reading
*/
int32_t external_flash_read_stream(spi_t *obj, char *rx_buffer, uint32_t length, uint32_t offset)
{
	int32_t ret = 0;
	char cmd[CMD_LENGTH_FOUR] = {0};

	device_mutex_lock(RT_DEV_LOCK_EXTERNAL_FLASH);

	/* transmit read cmd */
	cmd[0] = CMD_READ_DATA;
	cmd[1] = offset >> 16;
	cmd[2] = (offset >> 8) & 0xFF;
	cmd[3] = offset & 0xFF;

	spi_master_write_read_stream_dma(obj, cmd, rx_buffer, length + CMD_LENGTH_FOUR);
	xSemaphoreTake(xTxSemaphore, portMAX_DELAY);
	xSemaphoreTake(xRxSemaphore, portMAX_DELAY);

	device_mutex_unlock(RT_DEV_LOCK_EXTERNAL_FLASH);
	return ret;
}

/* Master mode fast read a sequence of data
** rx_buffer：buffer for reading data
       Notes: The first five bytes are the dummy bytes of the command, and the data starts from the sixth byte
** length：length of data to be read
** offset: start address of reading
*/
int32_t external_flash_fast_read_stream(spi_t *obj, char *rx_buffer, uint32_t length, uint32_t offset)
{
	int32_t ret = 0;
	char cmd[CMD_LENGTH_FIVE] = {0};

	device_mutex_lock(RT_DEV_LOCK_EXTERNAL_FLASH);

	/* transmit read cmd */
	cmd[0] = CMD_FAST_READ_DATA;
	cmd[1] = offset >> 16;
	cmd[2] = (offset >> 8) & 0xFF;
	cmd[3] = offset & 0xFF;

	spi_master_write_read_stream_dma(obj, cmd, rx_buffer, length + CMD_LENGTH_FIVE);
	xSemaphoreTake(xTxSemaphore, portMAX_DELAY);
	xSemaphoreTake(xRxSemaphore, portMAX_DELAY);

	device_mutex_unlock(RT_DEV_LOCK_EXTERNAL_FLASH);
	return ret;
}


/* Master mode erase sector
** length: length of erasing
** offset: start address of erasing
*/
int32_t external_flash_erase_stream(spi_t *obj, uint32_t length, uint32_t offset)
{
	int32_t ret = 0;
	int32_t i;
	char status[CMD_LENGTH_TWO] = {0};
	char cmd[CMD_LENGTH_FOUR] = {0};

	u8 start_sector_num = offset / SECTOR_SIZE;
	u8 end_sector_num = (offset + length - 1) / SECTOR_SIZE;

	device_mutex_lock(RT_DEV_LOCK_EXTERNAL_FLASH);
	for (i = start_sector_num; i <= end_sector_num; i++) {
		/* transmit write enable cmd */
		cmd[0] = CMD_WRITE_ENABLE;
		spi_master_write_stream_dma(obj, cmd, CMD_LENGTH_ONE);
		xSemaphoreTake(xTxSemaphore, portMAX_DELAY);
		spi_flush_rx_fifo(obj);

		/* check WIP  and WEL*/
		cmd[0] = CMD_READ_STATUS;

		do {
			spi_master_write_read_stream_dma(obj, cmd, status, CMD_LENGTH_TWO);
			xSemaphoreTake(xTxSemaphore, portMAX_DELAY);
			xSemaphoreTake(xRxSemaphore, portMAX_DELAY);
		} while (status[1] & WIP_BIT || !(status[1] & WEL_BIT));

		/* transmit erase cmd */
		cmd[0] = CMD_SECTOR_ERASE;
		cmd[1] = (0x1000 * i) >> 16;
		cmd[2] = ((0x1000 * i) >> 8) & 0xFF;
		cmd[3] = 0x00;

		ret = spi_master_write_stream_dma(obj, cmd, CMD_LENGTH_FOUR);
		xSemaphoreTake(xTxSemaphore, portMAX_DELAY);
		spi_flush_rx_fifo(obj);

		/* check WIP */
		cmd[0] = CMD_READ_STATUS;
		do {
			spi_master_write_read_stream_dma(obj, cmd, status, CMD_LENGTH_TWO);
			xSemaphoreTake(xTxSemaphore, portMAX_DELAY);
			xSemaphoreTake(xRxSemaphore, portMAX_DELAY);
		} while (status[1] & WIP_BIT);
	}

	device_mutex_unlock(RT_DEV_LOCK_EXTERNAL_FLASH);

	return ret;
}

/* Master mode write a sequence of data

** tx_buffer：data to be written
** length：length of data
** offset: start address of writting
*/
int32_t external_flash_write_stream(spi_t *obj, char *tx_buffer, uint32_t length, uint32_t offset)
{
	int32_t ret = 0;
	u8 status[CMD_LENGTH_TWO] = {0};
	uint32_t position = offset;

	/*cmd_length = 4, page_size = 256*/
	u8 cmd[CMD_LENGTH_FOUR + MAX_TX_SIZE] = {0};

	device_mutex_lock(RT_DEV_LOCK_EXTERNAL_FLASH);
	while (position < offset + length) {

		/* transmit write enable cmd */
		cmd[0] = CMD_WRITE_ENABLE;
		spi_master_write_stream_dma(obj, (char *)cmd, CMD_LENGTH_ONE);
		xSemaphoreTake(xTxSemaphore, portMAX_DELAY);
		spi_flush_rx_fifo(obj);

		/* check WIP  and WEL */
		cmd[0] = CMD_READ_STATUS;

		do {
			spi_master_write_read_stream_dma(obj, (char *)cmd, (char *)status, CMD_LENGTH_TWO);
			xSemaphoreTake(xTxSemaphore, portMAX_DELAY);
			xSemaphoreTake(xRxSemaphore, portMAX_DELAY);
		} while (status[1] & WIP_BIT || !(status[1] & WEL_BIT));

		/* transmit page program cmd */
		cmd[0] = CMD_PAGE_PROGRAM;
		cmd[1] = position >> 16;
		cmd[2] = (position >> 8) & 0xFF;
		cmd[3] = position & 0xFF;

		if (position % PAGE_SIZE == 0) {                    /* programm from the start address of a page*/
			memcpy((u8 *)cmd + CMD_LENGTH_FOUR, tx_buffer, PAGE_SIZE);

			/* transmit data */
			ret = spi_master_write_stream_dma(obj, (char *)cmd, CMD_LENGTH_FOUR + PAGE_SIZE);
			tx_buffer = (char *)tx_buffer + PAGE_SIZE;
			position += PAGE_SIZE;
		} else {                                            /* programm not from the start address of a page*/
			memcpy((u8 *)cmd + CMD_LENGTH_FOUR, tx_buffer, PAGE_SIZE - position % PAGE_SIZE);

			/* transmit data */
			ret = spi_master_write_stream_dma(obj, (char *)cmd, CMD_LENGTH_FOUR + PAGE_SIZE - position % PAGE_SIZE);
			tx_buffer = (char *)tx_buffer + (PAGE_SIZE - position % PAGE_SIZE);
			position += PAGE_SIZE - position % PAGE_SIZE;
		}

		xSemaphoreTake(xTxSemaphore, portMAX_DELAY);
		spi_flush_rx_fifo(obj);

		/* check WIP */
		cmd[0] = CMD_READ_STATUS;
		do {
			spi_master_write_read_stream_dma(obj, (char *)cmd, (char *)status, CMD_LENGTH_TWO);
			xSemaphoreTake(xTxSemaphore, portMAX_DELAY);
			xSemaphoreTake(xRxSemaphore, portMAX_DELAY);
		} while (status[1] & WIP_BIT);
	}

	device_mutex_unlock(RT_DEV_LOCK_EXTERNAL_FLASH);

	return ret;
}

void external_flash_spi_init(spi_t *obj, PinName mosi, PinName miso, PinName sclk, PinName ssel, int hz)
{
	if ((sclk == PA_3) || (ssel == PA_2)) {
		sys_jtag_off();
	}

	spi_init(obj, mosi, miso, sclk, ssel);
	spi_format(obj, 8, ((int)SPI_SCLK_IDLE_LOW | (int)SPI_SCLK_TOGGLE_MIDDLE), 0);
	spi_frequency(obj, hz);
	hal_ssi_toggle_between_frame(&obj->hal_ssi_adaptor, DISABLE);
	spi_irq_hook(obj, (spi_irq_handler)external_flash_tr_done_callback, (uint32_t)obj);

	xRxSemaphore = xSemaphoreCreateBinary();
	xTxSemaphore = xSemaphoreCreateBinary();
	if (xRxSemaphore == NULL || xTxSemaphore == NULL) {
		dbg_printf("Create Semaphore failed.\n");
	}
}

void external_flash_spi_free(spi_t *obj)
{
	spi_free(obj);
}

void external_flash_get_id(spi_t *obj)
{
	int32_t ret;
	u8 device_id[CMD_LENGTH_FOUR] = {0};
	u8 device_cmd[CMD_LENGTH_FOUR] = {0};

	device_mutex_lock(RT_DEV_LOCK_EXTERNAL_FLASH);

	/* read flash ID */
	device_cmd[0] = CMD_READ_DEVICE_ID;

	ret = spi_master_write_read_stream_dma(obj, (char *)device_cmd, (char *)device_id, CMD_LENGTH_FOUR);
	xSemaphoreTake(xTxSemaphore, portMAX_DELAY);
	xSemaphoreTake(xRxSemaphore, portMAX_DELAY);

	if (ret == HAL_OK) {
		dbg_printf("\n Manufacturer ID: 0x%02X\r\n"
				   "\n Memory Type ID : 0x%02X\r\n"
				   "\n Capacity ID    : 0x%02X\r\n", device_id[1], device_id[2], device_id[3]);
	} else {
		dbg_printf("get device id failed !!!\n");
	}
	device_mutex_unlock(RT_DEV_LOCK_EXTERNAL_FLASH);
}

