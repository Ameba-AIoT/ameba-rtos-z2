/*
 * Copyright(c) 2007 - 2019 Realtek Corporation. All rights reserved.
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
 */
#include <string.h>
#include "i2c_api.h"
#include "pinmap.h"
#include "i2c_ex_api.h"

#define MBED_I2C_MTR_SDA    PA_20
#define MBED_I2C_MTR_SCL    PA_19

#define MBED_I2C_SLV_SDA    PA_20
#define MBED_I2C_SLV_SCL    PA_19

#define MBED_I2C_SLAVE_ADDR0    0xAA
#define MBED_I2C_BUS_CLK        100000  //hz

#define I2C_DATA_LENGTH         64
char	i2cdatasrc[I2C_DATA_LENGTH]__attribute__((aligned(32))) = {0};
char	i2cdatadst[I2C_DATA_LENGTH]__attribute__((aligned(32))) = {0};
char	i2cdatardsrc[I2C_DATA_LENGTH]__attribute__((aligned(32))) = {0};
char	i2cdatarddst[I2C_DATA_LENGTH]__attribute__((aligned(32))) = {0};

#define I2C_MASTER_DEVICE
#ifndef I2C_MASTER_DEVICE
#define I2C_SLAVE_DEVICE
#endif

// RESTART verification
//#define I2C_RESTART_DEMO

// Slave
// RX
#define CLEAR_SLV_RXC_FLAG	(slaveRXC = 0)
#define SET_SLV_RXC_FLAG	(slaveRXC = 1)
#define WAIT_SLV_RXC		while(slaveRXC == 0){;}
// Tx
#define CLEAR_SLV_TXC_FLAG	(slaveTXC = 0)
#define SET_SLV_TXC_FLAG	(slaveTXC = 1)
#define WAIT_SLV_TXC		while(slaveTXC == 0){;}
// Read Request
#define CLEAR_SLV_RD_REQ_FLAG	(slaveRdReq = 0)
#define SET_SLV_RD_REQ_FLAG		(slaveRdReq = 1)
#define WAIT_SLV_RD_REQ			while(slaveRdReq == 0){;}

// Master
// Rx
#define CLEAR_MST_RXC_FLAG	(masterRXC = 0)
#define SET_MST_RXC_FLAG	(masterRXC = 1)
#define WAIT_MST_RXC		while(masterRXC == 0){;}
// Tx
#define CLEAR_MST_TXC_FLAG	(masterTXC = 0)
#define SET_MST_TXC_FLAG	(masterTXC = 1)
#define WAIT_MST_TXC		while(masterTXC == 0){;}

i2c_t i2cmaster;
i2c_t i2cslave;

volatile int masterTXC;
volatile int masterRXC;
volatile int slaveTXC;
volatile int slaveRXC;
volatile int slaveRdReq;

void i2c_slave_rxc_callback(void *userdata)
{
	int i2clocalcnt;
	int result = 0;

	i2c_enable_control(&i2cslave, 0);
	for (i2clocalcnt = 0; i2clocalcnt < I2C_DATA_LENGTH; i2clocalcnt += 2) {
//        dbg_printf("i2c data: %02x \t %02x\n",i2cdatadst[i2clocalcnt],i2cdatadst[i2clocalcnt+1]);
	}
	//HalDelayUs(5000);

	// verify result
	result = 1;
#if !defined(I2C_RESTART_DEMO)
	for (i2clocalcnt = 0; i2clocalcnt < I2C_DATA_LENGTH; i2clocalcnt++) {
		if (i2cdatasrc[i2clocalcnt] != i2cdatadst[i2clocalcnt]) {
			result = 0;
			break;
		}
	}
#else
	if (i2cdatasrc[0] == i2cdatadst[0]) {
		if (i2cdatasrc[0] != i2cdatadst[0]) {
			result = 0;
		}
	} else if (i2cdatasrc[1] == i2cdatadst[0]) {
		for (i2clocalcnt = 1; i2clocalcnt < I2C_DATA_LENGTH; i2clocalcnt++) {
			if (i2cdatasrc[i2clocalcnt] != i2cdatadst[i2clocalcnt - 1]) {
//                dbg_printf("idx:%d, src:%x, dst:%x\n", i2clocalcnt, i2cdatasrc[i2clocalcnt], i2cdatadst[i2clocalcnt]);
				for (i2clocalcnt = 0; i2clocalcnt < I2C_DATA_LENGTH; i2clocalcnt += 2) {
//                    dbg_printf("i2c data: %02x \t %02x\n",i2cdatadst[i2clocalcnt],i2cdatadst[i2clocalcnt+1]);
				}
				result = 0;
				break;
			}
		}
	} else {
		for (i2clocalcnt = 0; i2clocalcnt < I2C_DATA_LENGTH; i2clocalcnt++) {
			if (i2cdatasrc[i2clocalcnt] != i2cdatadst[i2clocalcnt]) {
//                dbg_printf("idx:%d, src:%x, dst:%x\n", i2clocalcnt, i2cdatasrc[i2clocalcnt], i2cdatadst[i2clocalcnt]);
				result = 0;
				break;
			}
		}
	}
#endif

	dbg_printf("\r\nSlave receive: Result is %s\r\n", (result) ? "success" : "fail");
	memset(&i2cdatadst[0], 0x00, I2C_DATA_LENGTH);
	SET_SLV_RXC_FLAG;
	i2c_enable_control(&i2cslave, 1);
}


void i2c_master_rxc_callback(void *userdata)
{
	int     i2clocalcnt;
	int     result = 0;

	dbg_printf("show master received data>>>\n");
	for (i2clocalcnt = 0; i2clocalcnt < I2C_DATA_LENGTH; i2clocalcnt += 2) {
//        dbg_printf("i2c data: %02x \t %02x\n",i2cdatarddst[i2clocalcnt],i2cdatarddst[i2clocalcnt+1]);
	}

	// verify result
	result = 1;
	for (i2clocalcnt = 0; i2clocalcnt < I2C_DATA_LENGTH; i2clocalcnt++) {
		if (i2cdatarddst[i2clocalcnt] != i2cdatardsrc[i2clocalcnt]) {
			result = 0;
			break;
		}
	}
	dbg_printf("\r\nMaster receive: Result is %s\r\n", (result) ? "success" : "fail");
	SET_MST_RXC_FLAG;
}

void i2c_slave_txc_callback(void *userdata)
{
	SET_SLV_TXC_FLAG;
}

void i2c_master_txc_callback(void *userdata)
{
	SET_MST_TXC_FLAG;
}

void i2c_master_err_callback(void *userdata)
{
	dbg_printf("ERR:%x\n", i2cmaster.i2c_adp.err_type);
}

void i2c_slave_rd_req_callback(void *userdata)
{
	//dbg_printf("rd req\n");
	i2c_slave_set_for_rd_req(&i2cslave, 0);
	SET_SLV_RD_REQ_FLAG;
}

extern void i2c_set_user_callback(i2c_t *obj, I2CCallback i2ccb, void(*i2c_callback)(void *));
void demo_i2c_master_enable(void)
{
	memset(&i2cmaster, 0x00, sizeof(i2c_t));
	i2c_op_mode(I2CModeDMA);//should set op_mode before init the i2c.
	i2c_init(&i2cmaster, MBED_I2C_MTR_SDA, MBED_I2C_MTR_SCL);
	i2c_frequency(&i2cmaster, MBED_I2C_BUS_CLK);
	i2c_set_user_callback(&i2cmaster, I2C_RX_COMPLETE, i2c_master_rxc_callback);
	i2c_set_user_callback(&i2cmaster, I2C_TX_COMPLETE, i2c_master_txc_callback);
	i2c_set_user_callback(&i2cmaster, I2C_ERR_OCCURRED, i2c_master_err_callback);
#ifdef I2C_RESTART_DEMO
	i2c_restart_enable(&i2cmaster);
#endif
}

void demo_i2c_slave_enable(void)
{
	memset(&i2cslave, 0x00, sizeof(i2c_t));
	i2c_op_mode(I2CModeDMA);//should set op_mode before init the i2c.
	i2c_init(&i2cslave, MBED_I2C_SLV_SDA, MBED_I2C_SLV_SCL);
	i2c_frequency(&i2cslave, MBED_I2C_BUS_CLK);
	i2c_slave_address(&i2cslave, 0, MBED_I2C_SLAVE_ADDR0, 0xFF);
	i2c_slave_mode(&i2cslave, 1);
	i2c_set_user_callback(&i2cslave, I2C_RX_COMPLETE, i2c_slave_rxc_callback);
	i2c_set_user_callback(&i2cslave, I2C_TX_COMPLETE, i2c_slave_txc_callback);
	i2c_set_user_callback(&i2cslave, I2C_RD_REQ_COMMAND, i2c_slave_rd_req_callback);
}

void demo_i2c_master_write_1byte(void)
{
	dbg_printf("Mst-W1\n");
	CLEAR_MST_TXC_FLAG;
	i2c_write(&i2cmaster, MBED_I2C_SLAVE_ADDR0, &i2cdatasrc[0], 1, 0);
	WAIT_MST_TXC;
	dbg_printf("Mst-W1 is complete and STOP bit is NOT sent.\n");
}

void demo_i2c_master_write_n_1byte(void)
{
	dbg_printf("Mst-Wn1\n");
	CLEAR_MST_TXC_FLAG;
	i2c_write(&i2cmaster, MBED_I2C_SLAVE_ADDR0, &i2cdatasrc[1], (I2C_DATA_LENGTH - 1), 1);
	//wait for master TXC
	WAIT_MST_TXC;
}

void demo_i2c_master_write(void)
{
	dbg_printf("Mst-W\n");
	CLEAR_MST_TXC_FLAG;
	i2c_write(&i2cmaster, MBED_I2C_SLAVE_ADDR0, &i2cdatasrc[0], I2C_DATA_LENGTH, 1);
	//wait for master TXC
	WAIT_MST_TXC;
}

void demo_i2c_master_read(void)
{
	dbg_printf("Mst-R\n");
	//dbg_printf("Mst-R need to wait Slv-W complete.\n");
	CLEAR_MST_RXC_FLAG;
	i2c_read(&i2cmaster, MBED_I2C_SLAVE_ADDR0, &i2cdatarddst[0], I2C_DATA_LENGTH, 1);
	//WAIT_MST_RXC;
}

void demo_i2c_slave_read(void)
{
	dbg_printf("Slv-R\n");
	CLEAR_SLV_RXC_FLAG;
	i2c_slave_read(&i2cslave, &i2cdatadst[0], I2C_DATA_LENGTH);
	//WAIT_SLV_RXC;
}

void demo_i2c_slave_read_1byte(void)
{
	dbg_printf("Slv-R1\n");
	CLEAR_SLV_RXC_FLAG;
	i2c_slave_read(&i2cslave, &i2cdatadst[0], 1);
	WAIT_SLV_RXC;
}

void demo_i2c_slave_read_n_1byte(void)
{
	dbg_printf("Slv-Rn1\n");
	CLEAR_SLV_RXC_FLAG;
	i2c_slave_read(&i2cslave, &i2cdatadst[1], I2C_DATA_LENGTH - 1);
	WAIT_SLV_RXC;
}

void demo_i2c_slave_write(void)
{

	dbg_printf("Slv-W\n");
	CLEAR_SLV_RD_REQ_FLAG;
	i2c_slave_set_for_rd_req(&i2cslave, 1);
	WAIT_SLV_RD_REQ;
	CLEAR_SLV_TXC_FLAG;
	i2c_slave_write(&i2cslave, &i2cdatardsrc[0], I2C_DATA_LENGTH);
	WAIT_SLV_TXC;
}

void main(void)
{
	int i2clocalcnt;

	dbg_printf("Slave address: 0x%x\n", MBED_I2C_SLAVE_ADDR0);
#ifdef I2C_RESTART_DEMO
	dbg_printf("Enable restart\n");
#endif

	// prepare for transmission
	memset(&i2cdatasrc[0], 0x00, I2C_DATA_LENGTH);
	memset(&i2cdatadst[0], 0x00, I2C_DATA_LENGTH);
	memset(&i2cdatardsrc[0], 0x00, I2C_DATA_LENGTH);
	memset(&i2cdatarddst[0], 0x00, I2C_DATA_LENGTH);

	for (i2clocalcnt = 0; i2clocalcnt < I2C_DATA_LENGTH; i2clocalcnt++) {
		i2cdatasrc[i2clocalcnt] = i2clocalcnt + 0x2;
	}

	for (i2clocalcnt = 0; i2clocalcnt < I2C_DATA_LENGTH; i2clocalcnt++) {
		i2cdatardsrc[i2clocalcnt] = i2clocalcnt + 1;
	}

//================================================================

#ifdef I2C_MASTER_DEVICE
	demo_i2c_master_enable();

	// Master write - Slave read
#ifdef I2C_RESTART_DEMO
	demo_i2c_master_write_1byte();
	demo_i2c_master_write_n_1byte();    // n-1 bytes
#else
	demo_i2c_master_write();
#endif
	hal_delay_ms(300);
	// Master read - Slave write
	demo_i2c_master_read();
#endif // #ifdef I2C_MASTER_DEVICE


#ifdef I2C_SLAVE_DEVICE
	demo_i2c_slave_enable();
	i2c_slave_set_for_data_nak(&i2cslave, 1);
	hal_delay_ms(500);
	i2c_slave_set_for_data_nak(&i2cslave, 0);

#ifdef I2C_RESTART_DEMO
	demo_i2c_slave_read_1byte();
	demo_i2c_slave_read_n_1byte();
#else
	// Master write - Slave read
	demo_i2c_slave_read();
#endif
	demo_i2c_slave_write();
#endif // #ifdef I2C_SLAVE_DEVICE

	while (1) {}
}


