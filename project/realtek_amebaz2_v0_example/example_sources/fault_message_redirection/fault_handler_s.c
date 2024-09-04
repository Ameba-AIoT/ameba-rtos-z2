#include "FreeRTOS.h"
#include "task.h"
#include "diag.h"
#include "main.h"
#include <flash_api.h>
#include <device_lock.h>
#include <platform_opts.h>
#include "fault_handler_s.h"

#if defined(__ICCARM__)
static void (__cmse_nonsecure_call *ns_flash_erase)(uint32_t) = NULL;
static int (__cmse_nonsecure_call *ns_flash_read)(uint32_t, uint32_t, uint8_t *) = NULL;
static int (__cmse_nonsecure_call *ns_flash_write)(uint32_t, uint32_t, uint8_t *) = NULL;
static void (__cmse_nonsecure_call *ns_device_lock)(uint32_t) = NULL;
static void (__cmse_nonsecure_call *ns_device_unlock)(uint32_t) = NULL;
#else
static void __attribute__((cmse_nonsecure_call))(*ns_flash_erase)(uint32_t) = NULL;
static int __attribute__((cmse_nonsecure_call))(*ns_flash_read)(uint32_t, uint32_t, uint8_t *) = NULL;
static int __attribute__((cmse_nonsecure_call))(*ns_flash_write)(uint32_t, uint32_t, uint8_t *) = NULL;
static void __attribute__((cmse_nonsecure_call))(*ns_device_lock)(uint32_t) = NULL;
static void __attribute__((cmse_nonsecure_call))(*ns_device_unlock)(uint32_t) = NULL;
#endif

static char *ns_buf = NULL;
static uint32_t ns_buf_len = 0;

char bt_message[1024];
char fault_message[6 * 1024];

void NS_ENTRY secure_set_ns_func(ns_func_t *func, char *buf, uint32_t buf_len)
{
#if defined(__ICCARM__)
	ns_flash_erase = (void (__cmse_nonsecure_call *)(uint32_t)) func->flash_erase;
	ns_flash_read = (int (__cmse_nonsecure_call *)(uint32_t, uint32_t, uint8_t *)) func->flash_read;
	ns_flash_write = (int (__cmse_nonsecure_call *)(uint32_t, uint32_t, uint8_t *)) func->flash_write;
	ns_device_lock = (void (__cmse_nonsecure_call *)(uint32_t)) func->device_lock;
	ns_device_unlock = (void (__cmse_nonsecure_call *)(uint32_t)) func->device_unlock;
#else
	ns_flash_erase = cmse_nsfptr_create(func->flash_erase);
	ns_flash_read = cmse_nsfptr_create(func->flash_read);
	ns_flash_write = cmse_nsfptr_create(func->flash_write);
	ns_device_lock = cmse_nsfptr_create(func->device_lock);
	ns_device_unlock = cmse_nsfptr_create(func->device_unlock);
#endif
	ns_buf = buf;
	ns_buf_len = buf_len;
}

void fault_log(char *msg, int len)
{
	memset(ns_buf, 0, ns_buf_len);
	memcpy(ns_buf, msg, len);
	ns_flash_erase(FAULT_LOG1);
	ns_flash_erase(FAULT_LOG1 + (4 * 1024));
	ns_flash_write(FAULT_LOG1, len, ns_buf);
}

void bt_log(char *msg, int len)
{
	memset(ns_buf, 0, ns_buf_len);
	memcpy(ns_buf, msg, len);
	ns_flash_erase(FAULT_LOG2);
	ns_flash_write(FAULT_LOG2, len, ns_buf);
}

void secure_fault_handler_override(void)
{
	fault_handler_override(fault_log, bt_log);
}
