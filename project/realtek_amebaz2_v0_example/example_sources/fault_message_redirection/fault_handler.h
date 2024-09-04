/**************************************************************************//**
* @file        fault_handler.h
* @brief       This file implements the fault handler API for non-secure region.
******************************************************************************/

#ifndef FAULT_HANDLER_H
#define FAULT_HANDLER_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(configENABLE_TRUSTZONE) && (configENABLE_TRUSTZONE == 1)
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
  * @brief  Used to initialize the non-secure function to be utilize by the secure region
  */
void fault_handler_set_ns_func(void);
#endif

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
  * @brief  Reads the last fault event logs and backtrace logs
  */
void read_last_fault_log(void);

#ifdef __cplusplus
}
#endif

#endif /* FAULT_HANDLER_H */
