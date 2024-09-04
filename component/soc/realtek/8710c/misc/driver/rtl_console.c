/******************************************************************************
 *
 * Copyright(c) 2007 - 2016 Realtek Corporation. All rights reserved.
 *
 *
 ******************************************************************************/
#include "osdep_service.h"
#include "device.h"
#include "serial_api.h"
#include "log_service.h"
#include "osdep_service.h"
#include "serial_ex_api.h"
#include "pinmap.h"

#include "cmsis.h"
#include "mpu.h"
//#include "shell.h"
#include "cmsis_os.h"               // CMSIS RTOS header file
#include "hal.h"
//#include "memory.h"
//#include "diag.h"

#include "stdio_port_func.h"
#if defined(configUSE_TICKLESS_IDLE) && (configUSE_TICKLESS_IDLE > 0)
#include "freertos_pmu.h"
#include "rtl8710c_freertos_pmu.h"
#include "sys_api.h"
#include "gpio_irq_api.h"
#include "gpio_irq_ex_api.h"
#include "power_mode_api.h"
static gpio_irq_t loguart_rx_as_wakeup_pin;
#endif

extern hal_uart_adapter_t log_uart;

#define KEY_CTRL_D      0x4
#define KEY_NL			0xa // '\n'
#define KEY_ENTER		0xd // '\r'
#define KEY_BS    		0x8
#define KEY_ESC    		0x1B
#define KEY_LBRKT  		0x5B
#define STR_END_OF_MP_FORMAT	"\r\n\r\r#"


#define CMD_HISTORY_LEN	4	// max number of executed command saved
#if SUPPORT_LOG_SERVICE
extern char log_buf[LOG_SERVICE_BUFLEN];
#ifdef PLATFORM_OHOS
extern _sema log_rx_interrupt_sema;
#else
extern xSemaphoreHandle log_rx_interrupt_sema;
#endif
#endif
char cmd_history[CMD_HISTORY_LEN][LOG_SERVICE_BUFLEN];
static unsigned int cmd_history_count = 0;

void uart_put_char(u8 c)
{
	stdio_port_putc(c);
}

void uart_send_string(char *str)
{
	unsigned int i = 0;
	while (str[i] != '\0') {
		stdio_port_putc(str[i]);
		i++;
	}
}

void uart_send_buf(u8 *buf, u32 len)
{
	unsigned char *st_p = buf;
	if (!len || (!buf)) {
		return;
	}
	while (len) {
		stdio_port_putc(*st_p);
		st_p++;
		len--;
	}
}

_WEAK void uart_set_dbgmon_pending(void) {}

static void uart_irq(u32 id, u32 event)
{
	unsigned char rc = 0;
	static unsigned char temp_buf[LOG_SERVICE_BUFLEN] = "\0";
	static unsigned char combo_key = 0;
	static short buf_count = 0;
	static unsigned char key_enter = 0;
	static char cmd_history_index = 0;
	if (event == RxIrq) {
		stdio_port_getc((char *)&rc);
		if (key_enter && rc == KEY_NL) {
			return;
		}
		if (rc == KEY_ESC) {
			combo_key = 1;
		} else if (rc == KEY_CTRL_D) {
			uart_set_dbgmon_pending();
		} else if (combo_key == 1) {
			if (rc == KEY_LBRKT) {
				combo_key = 2;
			} else {
				combo_key = 0;
			}
		} else if (combo_key == 2) {
			if (rc == 'A' || rc == 'B') { // UP or Down
				if (rc == 'A') {
					cmd_history_index--;
					if (cmd_history_index < 0) {
						cmd_history_index = (cmd_history_count > CMD_HISTORY_LEN) ? CMD_HISTORY_LEN - 1 : (cmd_history_count - 1) % CMD_HISTORY_LEN;
					}
				} else {
					cmd_history_index++;
					if (cmd_history_index > (cmd_history_count > CMD_HISTORY_LEN ? CMD_HISTORY_LEN - 1 : (cmd_history_count - 1) % CMD_HISTORY_LEN)) {
						cmd_history_index = 0;
					}
				}

				if (cmd_history_count > 0) {
					buf_count = strlen((char const *)temp_buf);
					memset(temp_buf, '\0', buf_count);
					while (--buf_count >= 0) {
						stdio_port_putc(KEY_BS);
						stdio_port_putc(' ');
						stdio_port_putc(KEY_BS);
					}
					uart_send_string(cmd_history[cmd_history_index % CMD_HISTORY_LEN]);
					strcpy((char *)temp_buf, cmd_history[cmd_history_index % CMD_HISTORY_LEN]);
					buf_count = strlen((char const *)temp_buf);
				}
			}
			// exit combo
			combo_key = 0;
		} else if (rc == KEY_ENTER) {
#if SUPPORT_LOG_SERVICE
			key_enter = 1;
			if (buf_count > 0) {
				stdio_port_putc(KEY_NL);
				stdio_port_putc(KEY_ENTER);
				memset(log_buf, '\0', LOG_SERVICE_BUFLEN);
				strncpy(log_buf, (char *)&temp_buf[0], buf_count);
				rtw_up_sema_from_isr((_sema *)&log_rx_interrupt_sema);
				memset(temp_buf, '\0', buf_count);

				/* save command */
				memset(cmd_history[((cmd_history_count) % CMD_HISTORY_LEN)], '\0', buf_count + 1);
				strcpy(cmd_history[((cmd_history_count++) % CMD_HISTORY_LEN)], log_buf);
				cmd_history_index = cmd_history_count % CMD_HISTORY_LEN;
				buf_count = 0;
			} else {
				uart_send_string(STR_END_OF_MP_FORMAT);
#if defined(configUSE_TICKLESS_IDLE) && (configUSE_TICKLESS_IDLE > 0)
				pmu_acquire_wakelock(PMU_LOGUART_DEVICE);
#endif
			}
#endif
		} else if (rc == KEY_BS) {
			if (buf_count > 0) {
				buf_count--;
				temp_buf[buf_count] = '\0';
				stdio_port_putc(rc);
				stdio_port_putc(' ');
				stdio_port_putc(rc);
			}
		} else {
			/* cache input characters */
			if (buf_count < (LOG_SERVICE_BUFLEN - 1)) {
				temp_buf[buf_count] = rc;
				buf_count++;
				stdio_port_putc(rc);
				key_enter = 0;
			} else if (buf_count == (LOG_SERVICE_BUFLEN - 1)) {
				temp_buf[buf_count] = '\0';
				uart_send_string("\r\nERROR:exceed size limit"STR_END_OF_ATCMD_RET);
			}
		}
	}
}

//======================================================
#if defined(BUFFERED_PRINTF) && (BUFFERED_PRINTF == 1)
//print log buffer length, if buffer get full, the extra logs will be discarded.
#define MAX_PRINTF_BUF_LEN 4096

#if defined(configENABLE_TRUSTZONE) && (configENABLE_TRUSTZONE == 1)
extern int NS_ENTRY secure_set_buffered_printf(void *(*malloc_func)(size_t), void (*free_func)(void *), int (*buffered_printf_func)(char *));
#endif

xTaskHandle print_task = NULL;
EventGroupHandle_t print_event = NULL;
char print_buffer[MAX_PRINTF_BUF_LEN];
int flush_idx = 0;
int used_length = 0;
char tmp_buffer[2048] = {0,};

int available_space(void)
{
	return MAX_PRINTF_BUF_LEN - used_length;
}

int buffered_printf_handle(char *tmp_buffer)
{
	if ((print_task == NULL) || (print_event == NULL)) {
		return 0;
	}

	static int print_idx = 0;
	int cnt;

	cnt = strlen(tmp_buffer);
	if (cnt < available_space()) {
		if (print_idx >= flush_idx) {
			if (MAX_PRINTF_BUF_LEN - print_idx >= cnt) {
				memcpy(&print_buffer[print_idx], tmp_buffer, cnt);
			} else {
				memcpy(&print_buffer[print_idx], tmp_buffer, MAX_PRINTF_BUF_LEN - print_idx);
				memcpy(&print_buffer[0], &tmp_buffer[MAX_PRINTF_BUF_LEN - print_idx], cnt - (MAX_PRINTF_BUF_LEN - print_idx));
			}
		} else { // space is flush_idx - print_idx, and available space is enough
			memcpy(&print_buffer[print_idx], tmp_buffer, cnt);
		}

		// protection needed
		//taskENTER_CRITICAL();
		used_length += cnt;
		//taskEXIT_CRITICAL();
		print_idx += cnt;
		if (print_idx >= MAX_PRINTF_BUF_LEN) {
			print_idx -= MAX_PRINTF_BUF_LEN;
		}
	} else {
		// skip
		cnt = 0;
	}

	return cnt;
}

int buffered_printf(const char *fmt, ...)
{
	int cnt;
	va_list arglist;

	taskENTER_CRITICAL();
	va_start(arglist, fmt);
	vsnprintf(tmp_buffer, sizeof(tmp_buffer), fmt, arglist);
	va_end(arglist);
	cnt = buffered_printf_handle(tmp_buffer);
	taskEXIT_CRITICAL();

	if (xEventGroupGetBits(print_event) != 1) {
		xEventGroupSetBits(print_event, 1);
	}
	return cnt;
}

int buffered_printf_s(char *tmp_buffer)
{
	int cnt;

	taskENTER_CRITICAL();
	cnt = buffered_printf_handle(tmp_buffer);
	taskEXIT_CRITICAL();

	if (xEventGroupGetBits(print_event) != 1) {
		xEventGroupSetBits(print_event, 1);
	}
	return cnt;
}

void printing_task(void *arg)
{
	while (1) {
		//wait event
		if (xEventGroupWaitBits(print_event, 1,  pdFALSE, pdFALSE, 100) == 1) {
			while (used_length > 0) {
				uart_put_char(print_buffer[flush_idx]);
				taskENTER_CRITICAL();
				flush_idx++;
				if (flush_idx >= MAX_PRINTF_BUF_LEN) {
					flush_idx -= MAX_PRINTF_BUF_LEN;
				}
				//taskENTER_CRITICAL();
				used_length--;
				taskEXIT_CRITICAL();
			}
			// clear event
			xEventGroupClearBits(print_event, 1);
		}
		vTaskDelay(100);
	}
}

void rtl_printf_init()
{
	if (print_event == NULL) {
		print_event = xEventGroupCreate();
		if (print_event == NULL) {
			printf("\n\rprint event init fail!\n");
		}
	}
	if (print_task == NULL) {
		if (xTaskCreate(printing_task, (const char *)"print_task", 512, NULL, tskIDLE_PRIORITY + 2, &print_task) != pdPASS) {
			printf("\n\rprint task init fail!\n");
		}
	}
}
#endif
//======================================================

extern void log_service_init(void);
extern void console_reinit_uart(void);
#if defined(configUSE_CUSTOMIZED_TICKLESS_IDLE) && (configUSE_CUSTOMIZED_TICKLESS_IDLE > 0)
u32 rtl8710c_loguart_suspend(u32 sleep_mode, void *param)
{
	sys_log_uart_off();
	gpio_irq_init(&loguart_rx_as_wakeup_pin, (PinName)log_uart.rx_pin, NULL, (uint32_t)&loguart_rx_as_wakeup_pin);
	gpio_irq_pull_ctrl(&loguart_rx_as_wakeup_pin, PullUp);
	gpio_irq_set(&loguart_rx_as_wakeup_pin, IRQ_FALL, 1);
	return TRUE;
}

u32 rtl8710c_loguart_resume(u32 sleep_mode, void *param)
{
	u32 wake_event = HAL_READ32(0x40000000, 0x108);
	gpio_irq_deinit(&loguart_rx_as_wakeup_pin);
	sys_log_uart_on();
	console_reinit_uart();
	if (wake_event | BIT4) { //wakeup by GPIO
		pmu_set_sysactive_time(100);
	}
	return TRUE;
}
#endif
void console_init(void)
{
	int i;
	for (i = 0; i < CMD_HISTORY_LEN; i++) {
		memset(cmd_history[i], '\0', LOG_SERVICE_BUFLEN);
	}
#if SUPPORT_LOG_SERVICE
	log_service_init();
#endif
	hal_uart_reset_rx_fifo(&log_uart);
	hal_uart_rxind_hook(&log_uart, uart_irq, 0, 0);
#if defined(configUSE_CUSTOMIZED_TICKLESS_IDLE) && (configUSE_CUSTOMIZED_TICKLESS_IDLE > 0)
	//uart0 can be wakeup source
	if (log_uart.uart_idx != Uart0) {
		pmu_register_sleep_callback(PMU_LOGUART_DEVICE, rtl8710c_loguart_suspend, &log_uart, rtl8710c_loguart_resume, &log_uart);
		if (pmu_get_sleep_type() == SLEEP_CG) {
			pmu_set_wakeup_event(SLP_GPIO, log_uart.rx_pin);
		} else if (pmu_get_sleep_type() == SLEEP_PG) {
			pmu_set_wakeup_event(DSTBY_GPIO, log_uart.rx_pin);
		}
	}
#endif

#if defined(BUFFERED_PRINTF) && (BUFFERED_PRINTF == 1)
#if defined(configENABLE_TRUSTZONE) && (configENABLE_TRUSTZONE == 1)
	secure_set_buffered_printf(pvPortMalloc, vPortFree, buffered_printf_s);
#endif
	rtl_printf_init();
#endif
}

void console_reinit_uart(void)
{
	hal_uart_reset_rx_fifo(&log_uart);
	hal_uart_rxind_hook(&log_uart, uart_irq, 0, 0);
}

