#include "serial_api.h"
#include "serial_ex_api.h"
#include "FreeRTOSConfig.h"
#include "memory.h"

#define RX_TIMEOUT (40 / portTICK_PERIOD_MS)

//UART0
#define UART_TX    PA_14
#define UART_RX    PA_13
#define BAUDRATE 115200
#define NUM_DMA_BUFF 10
#define IDLE_TIMEOUT 50
#define BUFFER_LENGTH 16

BaseType_t xHigherPriorityTaskWoken = pdFALSE;

typedef enum {
	Buffer_Free = 0,
	Buffer_Occupied = 1,
} dma_buffer_status_t;

typedef struct dma_buffer {
	struct dma_buffer *next;
	uint16_t status;
	uint16_t valid_entries;
	uint8_t buf[BUFFER_LENGTH];
} dma_buffer_t;

typedef struct uart_dma_mul_rx {
	serial_t uart;
	dma_buffer_t *node;
	QueueHandle_t uart_queue;
} uart_dma_mul_rx_t;

//interrupt handler when dma rx done and buffer is fully filled
void uart_dma_multi_buffer_rx_comp_done(void  *arg)
{
	dma_buffer_t *nodeSend = NULL;
	uart_dma_mul_rx_t *uart_info = (uart_dma_mul_rx_t *)arg;

	//store node to send
	uart_info->node->valid_entries = BUFFER_LENGTH;
	nodeSend = uart_info->node;
	//get new node for dam rx
	uart_info->node = uart_info->node->next;
	if (uart_info->node->status == Buffer_Occupied) {
		printf("ERR: Buffer Overflow\n");
	} else {
		uart_info->node->status = Buffer_Occupied;
	}
	//reinit uart dma
	serial_recv_stream_dma(&uart_info->uart, (char *)uart_info->node->buf, BUFFER_LENGTH);
	//send node with data to queue
	xQueueSendToBackFromISR(uart_info->uart_queue, (void *)&nodeSend, &xHigherPriorityTaskWoken);
}

//interrupt when uart idle timeout and buffer is partially filled
void uart_dma_multi_buffer_rx_idle_done(void  *arg)
{
	uint32_t byte = 0;
	dma_buffer_t *nodeSend = NULL;
	uart_dma_mul_rx_t *uart_info = (uart_dma_mul_rx_t *)arg;

	//retrieve remaining data in the uart buffer
	byte = hal_uart_recv_abort(&(uart_info->uart.uart_adp));
	//handle case when data rx length == BUFFER_LENGTH, both handle will enter (complete handler enters first)
	//resume stream dma with the same buffer
	if (byte != 0) {
		//store node to send
		uart_info->node->valid_entries = byte;
		nodeSend = uart_info->node;
		//get new node for dam rx
		uart_info->node = uart_info->node->next;
		if (uart_info->node->status == Buffer_Occupied) {
			printf("ERR: Buffer Overflow\n");
		} else {
			uart_info->node->status = Buffer_Occupied;
		}
	}
	//reinit uart dma
	serial_recv_stream_dma(&uart_info->uart, (char *)uart_info->node->buf, BUFFER_LENGTH);
	//send node with data to queue
	if (byte != 0) {
		xQueueSendToBackFromISR(uart_info->uart_queue, (void *)&nodeSend, &xHigherPriorityTaskWoken);
	}
}


void uart_dma_init(uart_dma_mul_rx_t *uart_info)
{
	serial_init(&uart_info->uart, UART_TX, UART_RX);
	serial_baud(&uart_info->uart, BAUDRATE);
	serial_format(&uart_info->uart, 8, ParityNone, 1);
	serial_recv_comp_handler(&uart_info->uart, (void *)uart_dma_multi_buffer_rx_comp_done, (uint32_t)uart_info);
	hal_uart_rx_idle_timeout_en(&(uart_info->uart.uart_adp), IDLE_TIMEOUT, uart_dma_multi_buffer_rx_idle_done, (void *)uart_info);
}

int uart_info_init(uart_dma_mul_rx_t *uart_info)
{
	uint32_t ret = 0;

	dma_buffer_t *current_buf;
	dma_buffer_t *first_buf;
	dma_buffer_t *last_buf;

	for (int i = 0; i < NUM_DMA_BUFF; i++) {
		current_buf = (dma_buffer_t *) malloc(sizeof(dma_buffer_t));
		if (i == 0) {
			first_buf = current_buf;
		} else {
			last_buf->next = current_buf;
			if (i == (NUM_DMA_BUFF - 1)) {
				current_buf->next = first_buf;
			}
		}

		current_buf->status = Buffer_Free;
		current_buf->valid_entries = 0;
		memset(current_buf->buf, 0, BUFFER_LENGTH);

		last_buf = current_buf;
	}
	uart_info->node = first_buf;
	uart_info->uart_queue = xQueueCreate(NUM_DMA_BUFF, sizeof(dma_buffer_t *));
	if (uart_info->uart_queue == NULL) {
		ret = -1;
		return ret;
	}

	return ret;
}

void uart_info_deinit(uart_dma_mul_rx_t *uart_info)
{
	dma_buffer_t *node_next;
	dma_buffer_t *node_current = uart_info->node;
	for (int i = 0; i < NUM_DMA_BUFF; i++) {
		node_next = node_current->next;
		free(node_current);
		node_current = node_next;
	}

	vQueueDelete(uart_info->uart_queue);
}

void stop_uart_dma_multi_buffer_rx(uart_dma_mul_rx_t *uart_info)
{
	hal_uart_rx_idle_timeout_dis(&uart_info->uart.uart_adp);
	serial_recv_stream_abort(&uart_info->uart);
	serial_free(&uart_info->uart);
	uart_info_deinit(uart_info);
	printf("available heap after free: %d\n", rtw_getFreeHeapSize());
}

int start_uart_dma_multi_buffer_rx(uart_dma_mul_rx_t *uart_info)
{

	uint32_t ret = 0;

	ret = uart_info_init(uart_info);
	if (ret != 0) {
		printf("ERR = %d, uart_info_init\n", ret);
		return -1;
	}

	uart_dma_init(uart_info);
	//start uart dma
	uart_info->node->status = Buffer_Occupied;
	ret = serial_recv_stream_dma(&uart_info->uart, (char *)uart_info->node->buf, BUFFER_LENGTH);
	return ret;
}

//dump the data received
void dma_buffer_print(dma_buffer_t *node)
{
	if (node->status == Buffer_Occupied) {
		for (int i = 0; i < node->valid_entries; i++) {
			printf("0x%X ", node->buf[i]);
		}
		printf("\n");

		node->status = Buffer_Free;
		node->valid_entries = 0;
		memset(node->buf, 0, BUFFER_LENGTH);
	}
}

//task to start UART DAM multiple buffer RX and process
int uart_dma_multi_buffer_rx_thread(void *param)
{
	uint32_t ret, count = 0;
	uart_dma_mul_rx_t uart_info;
	dma_buffer_t *node_p = NULL;
#if defined(configENABLE_TRUSTZONE) && (configENABLE_TRUSTZONE == 1)
	rtw_create_secure_context(512);
#endif
	printf("available heap size: %d\n", rtw_getFreeHeapSize());
	ret = start_uart_dma_multi_buffer_rx(&uart_info);
	if (ret != 0) {
		printf("ERR = %d, start_uart_dma_multi_buffer_rx\n", ret);
	}
	printf("\r\nUART DMA MULTI BUFFER START... ...\r\n");
	while (1) {
		if (xQueueReceive(uart_info.uart_queue, &node_p, RX_TIMEOUT)) {
			count += node_p->valid_entries;
			dma_buffer_print(node_p);
			printf("counter = %d\n", count);
		}
		vTaskDelay(50);
	}
	stop_uart_dma_multi_buffer_rx(&uart_info);
	vTaskDelete(NULL);
}

int main(void)
{
	if (xTaskCreate((TaskFunction_t)uart_dma_multi_buffer_rx_thread, "uart_dma_multi_buffer_rx_thread", (1024), NULL, (tskIDLE_PRIORITY + 1), NULL) != pdPASS) {
		printf("Cannot create uart_dma_multi_buffer_rx_thread \r\n");
	}
	vTaskStartScheduler();
}
