#include "FreeRTOS.h"
#include "task.h"
#include "diag.h"
#include "main.h"
#include <flash_api.h>
#include <device_lock.h>
#include <platform_opts.h>
#include "fault_handler.h"

char bt_message[1024];
char fault_message[6 * 1024];

#if defined(configENABLE_TRUSTZONE) && (configENABLE_TRUSTZONE == 1)
static void ns_flash_erase(uint32_t address)
{
	flash_t flash;
	device_mutex_lock(RT_DEV_LOCK_FLASH);
	flash_erase_sector(&flash, address);
	device_mutex_unlock(RT_DEV_LOCK_FLASH);
}

static int ns_flash_read(uint32_t address, uint32_t len, uint8_t *data)
{
	int ret;
	flash_t flash;

	device_mutex_lock(RT_DEV_LOCK_FLASH);
	ret = flash_stream_read(&flash, address, len, data);
	device_mutex_unlock(RT_DEV_LOCK_FLASH);

	return ret;
}

static int ns_flash_write(uint32_t address, uint32_t len, uint8_t *data)
{
	int ret;
	flash_t flash;
	device_mutex_lock(RT_DEV_LOCK_FLASH);
	ret = flash_stream_write(&flash, address, len, data);
	device_mutex_unlock(RT_DEV_LOCK_FLASH);

	return ret;
}

void fault_handler_set_ns_func(void)
{
	uint8_t *ns_buf = (uint8_t *) malloc(FAULT_FLASH_SECTOR_SIZE);
	if (ns_buf == NULL) {
		goto exit;
	}

	ns_func_t func = {ns_flash_erase, ns_flash_read, ns_flash_write, device_mutex_lock, device_mutex_unlock};

	secure_set_ns_func(&func, ns_buf, FAULT_FLASH_SECTOR_SIZE);

exit:
	return;
}
#endif

void fault_log(char *msg, int len)
{
	flash_t fault_flash;

	device_mutex_lock(RT_DEV_LOCK_FLASH);
	flash_erase_sector(&fault_flash, FAULT_LOG1);
	flash_erase_sector(&fault_flash, FAULT_LOG1 + (4 * 1024));
	flash_stream_write(&fault_flash, FAULT_LOG1, len, (uint8_t *)msg);
	device_mutex_unlock(RT_DEV_LOCK_FLASH);
}

void bt_log(char *msg, int len)
{
	flash_t fault_flash;

	device_mutex_lock(RT_DEV_LOCK_FLASH);
	flash_erase_sector(&fault_flash, FAULT_LOG2);
	flash_stream_write(&fault_flash, FAULT_LOG2, len, (uint8_t *)msg);
	device_mutex_unlock(RT_DEV_LOCK_FLASH);
}

void read_last_fault_log(void)
{
	flash_t fault_flash;
	uint8_t *log = malloc(FAULT_FLASH_SECTOR_SIZE);
	if (!log) {
		return;
	}
	memset(log, 0xff, FAULT_FLASH_SECTOR_SIZE);
	device_mutex_lock(RT_DEV_LOCK_FLASH);
	flash_stream_read(&fault_flash, FAULT_LOG1, FAULT_FLASH_SECTOR_SIZE, log);
	device_mutex_unlock(RT_DEV_LOCK_FLASH);

	if (log[0] != 0xFF) {
		dbg_printf("\n\r>>>>>>> Dump fault log <<<<<<<<\n\r");
		dbg_printf("%s", log);
	}

	device_mutex_lock(RT_DEV_LOCK_FLASH);
	flash_stream_read(&fault_flash, FAULT_LOG2, FAULT_FLASH_SECTOR_SIZE, log);
	device_mutex_unlock(RT_DEV_LOCK_FLASH);

	if (log[0] != 0xFF) {
		dbg_printf("\n\r>>>>>>> Dump Backtrace log <<<<<<<<\n\r");
		dbg_printf("%s", log);
	}

	memset(log, 0xff, FAULT_FLASH_SECTOR_SIZE);
	device_mutex_lock(RT_DEV_LOCK_FLASH);
	flash_erase_sector(&fault_flash, FAULT_LOG1);
	flash_erase_sector(&fault_flash, FAULT_LOG1 + (4 * 1024));
	flash_erase_sector(&fault_flash, FAULT_LOG2);
	device_mutex_unlock(RT_DEV_LOCK_FLASH);
	free(log);
}
