#include "FreeRTOS.h"
#include "task.h"
#include "diag.h"
#include "platform_opts.h"

void vAssertCalled(uint32_t ulLine, const char *pcfile)
{
	//
	volatile int lock_assert = 1;
	rt_printf("line %d file: %s\n\r", ulLine, pcfile);
	while (lock_assert);
}
#ifndef ENABLE_AMAZON_COMMON
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
	asm(" nop");
	rt_printf("=== stack overflow === \n\r");
	rt_printf("Task name : %s, TCB : %x\n\r", pcTaskName, xTask);
	rt_printf("PSP %x PSPLIM %x\n\r", __get_PSP(), __get_PSPLIM());
	rt_printf("MSP %x MSPLIM %x\n\r", __get_MSP(), __get_MSPLIM());
	rt_printf("====================== \n\r");
}
#endif

void vApplicationTickHook(void)
{
	asm(" nop");
}

#ifndef ENABLE_AMAZON_COMMON
void vApplicationMallocFailedHook(void)
{
	char *pcCurrentTask = "NoTsk";
	if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
		pcCurrentTask = pcTaskGetName(NULL);
	}
	rt_printf("[%s]Malloc failed [free heap size: %d]\r\n",  pcCurrentTask, xPortGetFreeHeapSize());
	taskDISABLE_INTERRUPTS();
	for (;;);
}
#endif

// defined in port.c
void vPortUsageFaultHandler(void);
void osUsageFaultHook(void)
{
	vPortUsageFaultHandler();
}
