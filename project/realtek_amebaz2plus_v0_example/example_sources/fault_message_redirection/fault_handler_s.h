/**************************************************************************//**
* @file        fault_handler_s.h
* @brief       This file implements the fault handler API for secure region.
******************************************************************************/

#ifndef FAULT_HANDLER_S_H
#define FAULT_HANDLER_S_H

#include "fault_handler_ram.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
  * @brief  The structure is used to register non-secure function for secure region.
  */
typedef struct ns_func_s {
	void (*flash_erase)(uint32_t);
	int (*flash_read)(uint32_t, uint32_t, uint8_t *);
	int (*flash_write)(uint32_t, uint32_t, uint8_t *);
	void (*device_lock)(uint32_t);
	void (*device_unlock)(uint32_t);
} ns_func_t;

/**
  * @brief  Used to set non-secure function for secure region to utilize
  * @param  func: register non-secure functions
  * @param  buf: register non-secure buffer
  * @param  buf_len: indicates the length of buf
  */
void NS_ENTRY secure_set_ns_func(ns_func_t *func, char *buf, uint32_t buf_len);

/**
  * @brief  The callback functions to handle fault event logs, which includes register and stack memory dump logs
  * @param  msg:  fault event logs which includes register and stack memory dump logs
  * @param  len:  indicates the length of msg
  */
void fault_log(char *msg, int len);

/**
  * @brief  The callback functions to handle stack backtrace logs
  * @param  msg:  backtrace logs which includes msp and psp information
  * @param  len:  indicates the length of msg
  */
void bt_log(char *msg, int len);

/**
  * @brief  Used to redeclare fault handler in the secure region
  */
void secure_fault_handler_override(void);

#ifdef __cplusplus
}
#endif

#endif /* FAULT_HANDLER_S_H */
