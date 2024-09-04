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

#include "cmsis.h"
#include "shell.h"
#include "cmsis_os.h"               // CMSIS RTOS header file
#include <math.h>
#include "hal_timer.h"
#include "string.h"
#include "crypto_api.h"

#define STACKSIZE    1024
#define DEBUG_LOG_BUF_SIZE      256

log_buf_type_t debug_log;
char debug_log_buf[DEBUG_LOG_BUF_SIZE];

hal_timer_adapter_t test_timer;

#if CONFIG_CMSIS_RTX_EN | CONFIG_CMSIS_FREERTOS_EN
void cmd_shell_task(void const *argument);          // thread function
void log_buf_task(void const *argument);          // thread function

osThreadId tid_cmd_shell;                             // thread id
osThreadDef(cmd_shell_task, osPriorityNormal, 1, 2048);       // thread object

osThreadId tid_log_buf;                             // thread id
osThreadDef(log_buf_task, osPriorityNormal, 1, 0);       // thread object
#endif


#if CONFIG_CMSIS_RTX_EN | CONFIG_CMSIS_FREERTOS_EN
void cmd_shell_task(void const *argument)
{
	dbg_printf("cmd_shell_task==>\r\n");

#if defined(configENABLE_TRUSTZONE) && (configENABLE_TRUSTZONE == 1)
	rtw_create_secure_context(STACKSIZE);
#endif

	while (1) {
		shell_task();
	}
}

void log_buf_task(void const *argument)
{
	dbg_printf("log_buf_task==>\r\n");
	while (1) {
		log_buf_show(&debug_log);
	}
}
#endif  // #if CONFIG_RTOS_RTX_EN

int vrf_crypto_rng(int argc, char **argv)
{
	int ret;
	uint32_t rng_buf;

#if !defined(CONFIG_BUILD_NONSECURE)
	static uint32_t crypto_inited = 0;
	if (crypto_inited == 0) {
		ret = crypto_init();
		if (ret != SUCCESS) {
			dbg_printf("crypto_init fail!%d\r\n", ret);
			goto verify_crypto_rng_end;
		} else {
			crypto_inited = 1;
			dbg_printf("crypto_init success!\r\n");
		}
	}
#endif
	while (1) {
		ret = crypto_random_generate((uint8_t *)&rng_buf, 4);
		if (ret != SUCCESS) {
			dbg_printf("crypto_random_generate fail! ret = %d\r\n", ret);
			break;
		} else {
			dbg_printf("0x%08x\r\n", rng_buf);
		}
	}
verify_crypto_rng_end:
	return ret;
}

/// default main
int main(void)
{
	uint32_t ret;

	log_buf_init(&debug_log);
	log_buf_set_msg_buf(&debug_log, debug_log_buf, DEBUG_LOG_BUF_SIZE);

	// register new shell command
	shell_register((shell_program_t)vrf_crypto_rng, "vrf_rng", "\t verify crypto_rng\r\n");

#if CONFIG_CMSIS_RTX_EN | CONFIG_CMSIS_FREERTOS_EN
	ret = osKernelInitialize();                     // initialize CMSIS-RTOS
	if (ret != osOK) {
		dbg_printf("KernelInitErr(0x%x)\r\n", ret);
		while (1);
	}

	// initialize peripherals here
	// create 'thread' functions that start executing,
	// example: tid_name = osThreadCreate (osThread(name), NULL);
	tid_cmd_shell = osThreadCreate(osThread(cmd_shell_task), NULL);
	if (tid_cmd_shell == 0) {
		dbg_printf("Create shell task error\r\n");
	}

	tid_log_buf = osThreadCreate(osThread(log_buf_task), NULL);
	if (tid_cmd_shell == 0) {
		dbg_printf("Create log buffer task error\r\n");
	}
	ret = osKernelStart();                          // start thread execution
	while (1);
#else
	while (1) {
		shell_task();
		log_buf_show(&debug_log);
	}
#endif
	return 0;
}

