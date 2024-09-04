#include <sys.h>
#include <device_lock.h>
#include "ota_8710c.h"
#include "lwip/netdb.h"
#include "wdt_api.h"
#include "sys_api.h"
#include "osdep_service.h"
#include "hal_misc.h"
#include "hal_wdt.h"

extern uint32_t sys_update_ota_get_curr_fw_idx(void);
extern uint32_t sys_update_ota_prepare_addr(void);
extern void sys_disable_fast_boot(void);

sys_thread_t TaskOTA = NULL;
#define STACK_SIZE		1024
#define TASK_PRIORITY	tskIDLE_PRIORITY + 1
static flash_t flash_ota;

// Checksum check before appending signature
// Please make sure target OTA firmware did contains 4 bytes checksum value, or the checksum check would always fail
// User can check target firmware postbuild routine. Please refer AN0500 application note OTA section for more detail
#define USE_CHECKSUM 0

void *update_malloc(unsigned int size)
{
	return pvPortMalloc(size);
}

void update_free(void *buf)
{
	vPortFree(buf);
}

void ota_platform_reset(void)
{
	sys_disable_fast_boot();
	hal_misc_rst_by_wdt();
	while (1) {
		osDelay(1000);
	}
}

int update_ota_connect_server(update_cfg_local_t *cfg)
{
	struct sockaddr_in server_addr;
	int server_socket;

	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket < 0) {
		printf("\n\r[%s] Create socket failed", __FUNCTION__);
		return -1;
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = cfg->ip_addr;
	server_addr.sin_port = cfg->port;

	if (connect(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
		printf("\n\r[%s] Socket connect failed", __FUNCTION__);
		return -1;
	}

	return server_socket;
}

int update_ota_erase_upg_region(uint32_t img_len, uint32_t NewFWLen, uint32_t NewFWAddr)
{
	uint32_t NewFWBlkSize = 0;

	if (NewFWLen == 0) {
		NewFWLen = img_len;
		printf("\n\r[%s] NewFWLen %d", __FUNCTION__, NewFWLen);
		if ((int)NewFWLen > 0) {
			NewFWBlkSize = ((NewFWLen - 1) / 4096) + 1;
			printf("\n\r[%s] NewFWBlkSize %d  0x%x", __FUNCTION__, NewFWBlkSize, NewFWAddr);
			device_mutex_lock(RT_DEV_LOCK_FLASH);
			for (int i = 0; i < NewFWBlkSize; i++) {
				flash_erase_sector(&flash_ota, NewFWAddr + i * 4096);
			}
			device_mutex_unlock(RT_DEV_LOCK_FLASH);
		} else {
			printf("\n\r[%s] Size INVALID", __FUNCTION__);
			return -1;
		}
	}

	return NewFWLen;
}

int update_ota_signature(unsigned char *sig_backup, uint32_t NewFWAddr)
{
	int ret = 0;
	unsigned char sig_readback[32] = {0};
	printf("\n\r[%s] Append OTA signature", __FUNCTION__);
	device_mutex_lock(RT_DEV_LOCK_FLASH);
	if (flash_burst_write(&flash_ota, NewFWAddr + 16, 16, sig_backup + 16) < 0) {
		printf("\n\r[%s] Write stream failed", __FUNCTION__);
		ret = -1;
	} else {
		if (flash_burst_write(&flash_ota, NewFWAddr, 16, sig_backup) < 0) {
			printf("\n\r[%s] Write stream failed", __FUNCTION__);
			ret = -1;
		}
	}
	flash_stream_read(&flash_ota, NewFWAddr, 32, sig_readback);
	device_mutex_unlock(RT_DEV_LOCK_FLASH);

	printf("\n\r[%s] signature:\n\r", __FUNCTION__);
	for (int i = 0; i < 32; i++) {
		printf(" %02X", sig_readback[i]);
		if (i == 15) {
			printf("\n\r");
		}
	}
	return ret;
}

static void update_ota_local_task(void *param)
{
	int server_socket = 0;
	unsigned char *buf;
	unsigned char sig_backup[32];
	int read_bytes = 0, idx = 0;
	update_cfg_local_t *cfg = (update_cfg_local_t *)param;
	uint32_t address = 0;
	uint32_t NewFWLen = 0, NewFWAddr = 0, file_info[3];
	int ret = -1 ;
	uint32_t curr_fw_idx = 0;
	uint32_t fw_len = 0;
#if USE_CHECKSUM
	uint32_t flash_checksum = 0;
	_file_checksum file_checksum;
	file_checksum.u = 0;
#endif

#if defined(configENABLE_TRUSTZONE) && (configENABLE_TRUSTZONE == 1)
	rtw_create_secure_context(configMINIMAL_SECURE_STACK_SIZE);
#endif
	printf("\n\r[%s] Update task start", __FUNCTION__);
	buf = update_malloc(BUF_SIZE);
	if (!buf) {
		printf("\n\r[%s] Alloc buffer failed", __FUNCTION__);
		goto update_ota_exit;
	}

	// Connect server
	server_socket = update_ota_connect_server(cfg);
	if (server_socket == -1) {
		goto update_ota_exit;
	}

	NewFWAddr = sys_update_ota_prepare_addr();
	if (NewFWAddr == -1) {
		goto update_ota_exit;
	}

	// Get file size from server.
	memset(file_info, 0, sizeof(file_info));
	if (file_info[0] == 0) {
		printf("\n\r[%s] Read info first", __FUNCTION__);
		read_bytes = read(server_socket, file_info, sizeof(file_info));
		if (read_bytes <= 0) {
			printf("\n\r[%s] Read socket failed or socket closed", __FUNCTION__);
			goto update_ota_exit;
		}
		// !X!X!X!X!X!X!X!X!X!X!X!X!X!X!X!X!X!X!X!X
		// !W checksum !W padding 0 !W file size !W
		// !X!X!X!X!X!X!X!X!X!X!X!X!X!X!X!X!X!X!X!X
		printf("\n\r[%s] info %d bytes", __FUNCTION__, read_bytes);
		printf("\n\r[%s] tx file size 0x%x", __FUNCTION__, file_info[2]);
		if (file_info[2] == 0) {
			printf("\n\r[%s] No file size", __FUNCTION__);
			goto update_ota_exit;
		}
	}

	curr_fw_idx = sys_update_ota_get_curr_fw_idx();
	printf("\n\r[%s] Current firmware index is %d\r\n", __FUNCTION__, curr_fw_idx);
	fw_len = file_info[2];
	NewFWLen = update_ota_erase_upg_region(fw_len, NewFWLen, NewFWAddr);
	if (NewFWLen == -1) {
		goto update_ota_exit;
	}

	// Write New FW sector
	if (NewFWAddr != ~0x0) {
		address = NewFWAddr;
		printf("\n\r[%s] Start to read data %d bytes\r\n", __FUNCTION__, NewFWLen);
		while (1) {
			int rest_len = NewFWLen - idx;
			int recv_len = rest_len > BUF_SIZE ? BUF_SIZE : rest_len;

			memset(buf, 0, BUF_SIZE);
			read_bytes = 0;

			while (read_bytes < recv_len) {
				read_bytes += read(server_socket, &buf[read_bytes], recv_len - read_bytes);
				if (read_bytes < 0) {
					printf("\n\r[%s] Read socket failed", __FUNCTION__);
					goto update_ota_exit;
				}
			}
			if (read_bytes == 0) {
				break;    // Read end
			}

			printf(".");

			if ((idx + read_bytes) > NewFWLen) {
				printf("\n\r[%s] Redundant bytes received", __FUNCTION__);
				read_bytes = NewFWLen - idx;
			}

			// back up signature and only write it to flash till the end of OTA
			if (idx < 32) {
				memcpy(sig_backup + idx, buf, (idx + read_bytes > 32 ? (32 - idx) : read_bytes));
				memset(buf, 0xFF, (idx + read_bytes > 32 ? (32 - idx) : read_bytes));
				printf("\n\r[%s] sig_backup for %d bytes from index %d\n\r", __FUNCTION__, (idx + read_bytes > 32 ? (32 - idx) : read_bytes), idx);
			}

			device_mutex_lock(RT_DEV_LOCK_FLASH);
			if (flash_burst_write(&flash_ota, address + idx, read_bytes, buf) < 0) {
				printf("\n\r[%s] Write stream failed", __FUNCTION__);
				device_mutex_unlock(RT_DEV_LOCK_FLASH);
				goto update_ota_exit;
			}
			device_mutex_unlock(RT_DEV_LOCK_FLASH);

#if USE_CHECKSUM
			// checksum attached at file end
			if (idx + read_bytes > NewFWLen - 4) {
				file_checksum.c[0] = buf[read_bytes - 4];
				file_checksum.c[1] = buf[read_bytes - 3];
				file_checksum.c[2] = buf[read_bytes - 2];
				file_checksum.c[3] = buf[read_bytes - 1];
			}
#endif

			idx += read_bytes;

			if (idx == NewFWLen) {
				break;
			}
		}
		printf("\n\rRead data finished\r\n");

#if USE_CHECKSUM
		// read flash data back and calculate checksum
		for (int i = 0; i < NewFWLen - 4; i += BUF_SIZE) {
			int k;
			int rlen = (idx - 4 - i) > BUF_SIZE ? BUF_SIZE : (idx - 4 - i);
			device_mutex_lock(RT_DEV_LOCK_FLASH);
			flash_stream_read(&flash_ota, NewFWAddr + i, rlen, buf);
			device_mutex_unlock(RT_DEV_LOCK_FLASH);
			for (k = 0; k < rlen; k++) {
				if (i + k < 32) {
					flash_checksum += sig_backup[i + k];
				} else {
					flash_checksum += buf[k];
				}
			}
		}

		printf("\n\rflash checksum 0x%8x attached checksum 0x%8x", flash_checksum, file_checksum.u);

		if (file_checksum.u != flash_checksum) {
			printf("\n\r[%s] The checksume is wrong!\n\r", __FUNCTION__);
			goto update_ota_exit;
		}
#endif

		// update ota signature at the end of OTA process
		ret = update_ota_signature(sig_backup, NewFWAddr);
		if (ret == -1) {
			printf("\r\n[%s] Update signature fail\r\n", __FUNCTION__);
			goto update_ota_exit;
		}
	}
update_ota_exit:
	if (buf) {
		update_free(buf);
	}
	if (server_socket >= 0) {
		close(server_socket);
	}
	if (param) {
		update_free(param);
	}
	TaskOTA = NULL;
	printf("\n\r[%s] Update task exit", __FUNCTION__);
	if (!ret) {
		printf("\n\r[%s] Ready to reboot", __FUNCTION__);
		osDelay(100);
		ota_platform_reset();
	}
	vTaskDelete(NULL);
	return;
}

int update_ota_local(char *ip, int port)
{
	update_cfg_local_t *pUpdateCfg;

	if (TaskOTA) {
		printf("\n\r[%s] Update task has created.", __FUNCTION__);
		return 0;
	}
	pUpdateCfg = update_malloc(sizeof(update_cfg_local_t));
	if (pUpdateCfg == NULL) {
		printf("\n\r[%s] Alloc update cfg failed", __FUNCTION__);
		return -1;
	}
	pUpdateCfg->ip_addr = inet_addr(ip);
	pUpdateCfg->port = ntohs(port);

	if (xTaskCreate(update_ota_local_task, "OTA_server", STACK_SIZE, pUpdateCfg, TASK_PRIORITY, &TaskOTA) != pdPASS) {
		update_free(pUpdateCfg);
		printf("\n\r[%s] Create update task failed", __FUNCTION__);
	}
	return 0;
}

void cmd_update(int argc, char **argv)
{
	int port;
	if (argc != 3) {
		printf("\n\r[%s] Usage: update IP PORT", __FUNCTION__);
		return;
	}
	port = atoi(argv[2]);
	update_ota_local(argv[1], port);
}

// choose the activated image. 0: default image / 1: upgrade image
void cmd_ota_image(bool cmd)
{
	if (cmd == 1) {
		sys_recover_ota_signature();
	} else {
		sys_clear_ota_signature();
	}
}


#if (defined HTTP_OTA_UPDATE) || (defined HTTPS_OTA_UPDATE)
static char *redirect = NULL;
static int redirect_len;
static uint16_t redirect_server_port;
static char *redirect_server_host = NULL;
static char *redirect_resource = NULL;
int  parser_url(char *url, char *host, uint16_t *port, char *resource)
{

	if (url) {
		char *http = NULL, *pos = NULL;

		http = strstr(url, "http://");
		if (http) { // remove http
			url += strlen("http://");
		}
		memset(host, 0, redirect_len);

		pos = strstr(url, ":");	// get port
		if (pos) {
			memcpy(host, url, (pos - url));
			pos += 1;
			*port = atoi(pos);
		} else {
			pos = strstr(url, "/");
			if (pos) {
				memcpy(host, url, (pos - url));
				url = pos;
			}
			*port = 80;
		}
		printf("server: %s\n\r", host);
		printf("port: %d\n\r", *port);

		memset(resource, 0, redirect_len);
		pos = strstr(url, "/");
		if (pos) {
			memcpy(resource, pos + 1, strlen(pos + 1));
		}
		printf("resource: %s\n\r", resource);

		return 0;
	}
	return -1;
}

/******************************************************************************************************************
** Function Name  : parse_http_response
** Description    : Parse the http response to get some useful parameters
** Input          : response	: The http response got from server
**					response_len: The length of http response
**					result		: The struct that store the usful infor about the http response
** Return         : Parse OK:	1 -> Only got the status code
**								3 -> Got the status code and content_length, but didn't get the full header
**								4 -> Got all the information needed
**					Failed:		-1
*******************************************************************************************************************/
int parse_http_response(uint8_t *response, uint32_t response_len, http_response_result_t *result)
{
	uint32_t i, p, q, m;
	uint32_t header_end = 0;

	//Get status code
	if (0 == result->parse_status) { //didn't get the http response
		uint8_t status[4] = {0};
		i = p = q = m = 0;
		for (; i < response_len; ++i) {
			if (' ' == response[i]) {
				++m;
				if (1 == m) {//after HTTP/1.1
					p = i;
				} else if (2 == m) { //after status code
					q = i;
					break;
				}
			}
		}
		if (!p || !q || q - p != 4) { //Didn't get the status code
			return -1;
		}
		memcpy(status, response + p + 1, 3); //get the status code
		result->status_code = atoi((char const *)status);
		if (result->status_code == 200) {
			result->parse_status = 1;
		} else if (result->status_code == 302) {
			char *tmp = NULL;
			const uint8_t *location1 = (uint8_t *)"LOCATION";
			const uint8_t *location2 = (uint8_t *)"Location";
			printf("response 302:%s \r\n", response);

			if ((tmp = strstr((char const *)response, (char const *)location1)) || (tmp = strstr((char const *)response, (char const *)location2))) {
				redirect_len = strlen(tmp + 10);
				printf("Location len = %d\r\n", redirect_len);
				if (redirect == NULL) {
					redirect = update_malloc(redirect_len);
					if (redirect == NULL) {
						return -1;
					}
				}
				memset(redirect, 0, redirect_len);
				memcpy(redirect, tmp + 10, strlen(tmp + 10));
			}

			if (redirect_server_host == NULL) {
				redirect_server_host = update_malloc(redirect_len);
				if (redirect_server_host == NULL) {
					return -1;
				}
			}

			if (redirect_resource == NULL) {
				redirect_resource = update_malloc(redirect_len);
				if (redirect_resource == NULL) {
					return -1;
				}
			}

			memset(redirect_server_host, 0, redirect_len);
			memset(redirect_resource, 0, redirect_len);
			if (parser_url(redirect, redirect_server_host, &redirect_server_port, redirect_resource) < 0) {
				return -1;
			}
			return -1;
		} else {
			printf("\n\r[%s] The http response status code is %d", __FUNCTION__, result->status_code);
			return -1;
		}
	}

	//if didn't receive the full http header
	if (3 == result->parse_status) { //didn't get the http response
		p = q = 0;
		for (i = 0; i < response_len; ++i) {
			if (response[i] == '\r' && response[i + 1] == '\n' &&
				response[i + 2] == '\r' && response[i + 3] == '\n') { //the end of header
				header_end = i + 4;
				result->parse_status = 4;
				result->header_len = header_end;
				result->body = response + header_end;
				break;
			}
		}
		if (3 == result->parse_status) {//Still didn't receive the full header
			result->header_bak = update_malloc(HEADER_BAK_LEN + 1);
			memset(result->header_bak, 0, strlen((char const *)result->header_bak));
			memcpy(result->header_bak, response + response_len - HEADER_BAK_LEN, HEADER_BAK_LEN);
		}
	}

	//Get Content-Length
	if (1 == result->parse_status) { //didn't get the content length
		const uint8_t *content_length_buf1 = (uint8_t *)"CONTENT-LENGTH";
		const uint8_t *content_length_buf2 = (uint8_t *)"Content-Length";
		const uint32_t content_length_buf_len = strlen((char const *)content_length_buf1);
		p = q = 0;

		for (i = 0; i < response_len; ++i) {
			if (response[i] == '\r' && response[i + 1] == '\n') {
				q = i;//the end of the line
				if (!memcmp(response + p, content_length_buf1, content_length_buf_len) ||
					!memcmp(response + p, content_length_buf2, content_length_buf_len)) { //get the content length
					int j1 = p + content_length_buf_len, j2 = q - 1;
					while (j1 < q && (*(response + j1) == ':' || *(response + j1) == ' ')) {
						++j1;
					}
					while (j2 > j1 && *(response + j2) == ' ') {
						--j2;
					}
					uint8_t len_buf[12] = {0};
					if ((j2 - j1 + 1) <= sizeof(len_buf)) {
						memcpy(len_buf, response + j1, j2 - j1 + 1);
					}
					result->body_len = atoi((char const *)len_buf);
					result->parse_status = 2;
				}
				p = i + 2;
			}
			if (response[i] == '\r' && response[i + 1] == '\n' &&
				response[i + 2] == '\r' && response[i + 3] == '\n') { //Get the end of header
				header_end = i + 4; //p is the start of the body
				if (result->parse_status == 2) { //get the full header and the content length
					result->parse_status = 4;
					result->header_len = header_end;
					result->body = response + header_end;
				} else { //there are no content length in header
					printf("\n\r[%s] No Content-Length in header", __FUNCTION__);
					return -1;
				}
				break;
			}
		}

		if (1 == result->parse_status) {//didn't get the content length and the full header
			result->header_bak = update_malloc(HEADER_BAK_LEN + 1);
			memset(result->header_bak, 0, strlen((char const *)result->header_bak));
			memcpy(result->header_bak, response + response_len - HEADER_BAK_LEN, HEADER_BAK_LEN);
		} else if (2 == result->parse_status) { //didn't get the full header but get the content length
			result->parse_status = 3;
			result->header_bak = update_malloc(HEADER_BAK_LEN + 1);
			memset(result->header_bak, 0, strlen((char const *)result->header_bak));
			memcpy(result->header_bak, response + response_len - HEADER_BAK_LEN, HEADER_BAK_LEN);
		}
	}

	return result->parse_status;
}

#ifdef HTTP_OTA_UPDATE
/******************************************************************************************************************
** Function Name  : update_ota_http_connect_server
** Description    : Connect to the OTA server
** Input          : server_socket: the socket used
**                  host: host address of the OTA server
**                  port: port of the OTA server
** Return         : Connect ok: socket value
**                  Failed:     -1
*******************************************************************************************************************/
int update_ota_http_connect_server(int server_socket, char *host, int port)
{
	struct sockaddr_in server_addr;
	struct hostent *server;

	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket < 0) {
		printf("\n\r[%s] Create socket failed", __FUNCTION__);
		return -1;
	}

	server = gethostbyname(host);
	if (server == NULL) {
		printf("[ERROR] Get host ip failed\n");
		return -1;
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	memcpy(&server_addr.sin_addr.s_addr, server->h_addr, 4);

	if (connect(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		printf("\n\r[%s] Socket connect failed", __FUNCTION__);
		return -1;
	}

	return server_socket;
}

/******************************************************************************************************************
** Function Name  : http_update_ota
** Description    : OTA update through http
** Input          : host: host address of the http server
**                  port: port of the http server
**                  resource: resource path
** Return         : Success: 0
**                  Failed:  -1
*******************************************************************************************************************/
int http_update_ota(char *host, int port, char *resource)
{
	int server_socket = -1;
	unsigned char *buf = NULL, *request = NULL;
	unsigned char sig_backup[32];
	int read_bytes = 0;
	int read_rtn = 0;
	uint32_t address = 0;
	uint32_t NewFWLen = 0, NewFWAddr = 0;
	int ret = -1;
	uint32_t curr_fw_idx = 0;
	uint32_t fw_len = 0;
	http_response_result_t rsp_result = {0};
#if USE_CHECKSUM
	uint32_t flash_checksum = 0;
	_file_checksum file_checksum;
	file_checksum.u = 0;
#endif

restart_http_ota:
	redirect_server_port = 0;

	buf = update_malloc(BUF_SIZE);
	if (!buf) {
		printf("\n\r[%s] Alloc buffer failed", __FUNCTION__);
		goto update_ota_exit;
	}

	// Connect server
	server_socket = update_ota_http_connect_server(server_socket, host, port);
	if (server_socket == -1) {
		goto update_ota_exit;
	}

	NewFWAddr = sys_update_ota_prepare_addr();
	if (NewFWAddr == -1) {
		goto update_ota_exit;
	}

	// Write New FW sector
	if (NewFWAddr != ~0x0) {
		uint32_t idx = 0;
		int data_len = 0;
		printf("\n\r");

		//send http request
		request = (unsigned char *) update_malloc(strlen("GET /") + strlen(resource) + strlen(" HTTP/1.1\r\nHost: ")
				  + strlen(host) + strlen("\r\n\r\n") + 1);
		sprintf((char *)request, "GET /%s HTTP/1.1\r\nHost: %s\r\n\r\n", resource, host);

		ret = write(server_socket, request, strlen((char const *)request));
		if (ret < 0) {
			printf("\n\r[%s] Send HTTP request failed", __FUNCTION__);
			goto update_ota_exit;
		}

		// read and parse http response
		while (3 >= rsp_result.parse_status) { //still read header
			if (0 == rsp_result.parse_status) { //didn't get the http response
				memset(buf, 0, BUF_SIZE);
				read_bytes = read(server_socket, buf, BUF_SIZE);
				if (read_bytes <= 0) {
					printf("\n\r[%s] Read socket failed", __FUNCTION__);
					goto update_ota_exit;
				}

				idx = read_bytes;
				memset(&rsp_result, 0, sizeof(rsp_result));
				if (parse_http_response(buf, idx, &rsp_result) == -1) {
					goto update_ota_exit;
				}
			} else if ((1 == rsp_result.parse_status) || (3 == rsp_result.parse_status)) { //just get the status code
				memset(buf, 0, BUF_SIZE);
				memcpy(buf, rsp_result.header_bak, HEADER_BAK_LEN);
				update_free(rsp_result.header_bak);
				rsp_result.header_bak = NULL;
				read_bytes = read(server_socket, buf + HEADER_BAK_LEN, (BUF_SIZE - HEADER_BAK_LEN));

				if (read_bytes <= 0) {
					printf("\n\r[%s] Read socket failed", __FUNCTION__);
					goto update_ota_exit;
				}

				idx = read_bytes + HEADER_BAK_LEN;

				if (parse_http_response(buf, read_bytes + HEADER_BAK_LEN, &rsp_result) == -1) {
					goto update_ota_exit;
				}
			}
		}

		if (0 == rsp_result.body_len) {
			printf("\n\r[%s] New firmware size = 0 !", __FUNCTION__);
			goto update_ota_exit;
		} else {
			printf("\n\r[%s] Download new firmware begin, total size : %d\n\r", __FUNCTION__, rsp_result.body_len);
		}


		curr_fw_idx = sys_update_ota_get_curr_fw_idx();
		printf("\n\r[%s] Current firmware index is %d\r\n", __FUNCTION__, curr_fw_idx);
		fw_len = rsp_result.body_len;
		address = NewFWAddr;
		printf("\n\r[%s] fw size %d, NewFWAddr %08X\n\r",  __FUNCTION__, fw_len, address);
		NewFWLen = update_ota_erase_upg_region(fw_len, NewFWLen, NewFWAddr);
		if (NewFWLen == -1) {
			goto update_ota_exit;
		}

		// remove http header_len from buf
		read_bytes = idx - rsp_result.header_len;
		idx = 0;
		if (read_bytes > 0) {
			memcpy(buf, buf + rsp_result.header_len, read_bytes);
			memset(buf + read_bytes, 0, BUF_SIZE - read_bytes);
			goto skip_read;
		}

		// download new firmware
		while (idx < NewFWLen) {
			printf(".");
			data_len = NewFWLen - idx;
			if (data_len > BUF_SIZE) {
				data_len = BUF_SIZE;
			}

			memset(buf, 0, BUF_SIZE);
			read_bytes = 0;

			while (read_bytes < data_len) {
				read_rtn = read(server_socket, &buf[read_bytes], data_len - read_bytes);
				if (read_rtn <= 0) {
					printf("\n\r[%s] Read socket failed", __FUNCTION__);
					goto update_ota_exit;
				}
				read_bytes += read_rtn;
			}

skip_read:
			if ((idx + read_bytes) > NewFWLen) {
				printf("\n\r[%s] Redundant bytes received", __FUNCTION__);
				read_bytes = NewFWLen - idx;
			}

			// back up signature
			if (idx < 32) {
				memcpy(sig_backup + idx, buf, (idx + read_bytes > 32 ? (32 - idx) : read_bytes));
				memset(buf, 0xFF, (idx + read_bytes > 32 ? (32 - idx) : read_bytes));
				printf("\n\r[%s] sig_backup for %d bytes from %d index\n\r", __FUNCTION__, (idx + read_bytes > 32 ? (32 - idx) : read_bytes), idx);
			}

			device_mutex_lock(RT_DEV_LOCK_FLASH);
			if (flash_burst_write(&flash_ota, address + idx, read_bytes, buf) < 0) {
				printf("\n\r[%s] Write sector failed", __FUNCTION__);
				device_mutex_unlock(RT_DEV_LOCK_FLASH);
				goto update_ota_exit;
			}
			device_mutex_unlock(RT_DEV_LOCK_FLASH);

#if USE_CHECKSUM
			// checksum attached at file end
			if (idx + read_bytes > NewFWLen - 4) {
				file_checksum.c[0] = buf[read_bytes - 4];
				file_checksum.c[1] = buf[read_bytes - 3];
				file_checksum.c[2] = buf[read_bytes - 2];
				file_checksum.c[3] = buf[read_bytes - 1];
			}
#endif

			idx += read_bytes;
		}
		printf("\n\r[%s] Download new firmware %d bytes completed\n\r", __FUNCTION__, idx);

#if USE_CHECKSUM
		// read flash data back and calculate checksum
		for (int i = 0; i < NewFWLen - 4; i += BUF_SIZE) {
			int k;
			int rlen = (idx - 4 - i) > BUF_SIZE ? BUF_SIZE : (idx - 4 - i);
			device_mutex_lock(RT_DEV_LOCK_FLASH);
			flash_stream_read(&flash_ota, NewFWAddr + i, rlen, buf);
			device_mutex_unlock(RT_DEV_LOCK_FLASH);
			for (k = 0; k < rlen; k++) {
				if (i + k < 32) {
					flash_checksum += sig_backup[i + k];
				} else {
					flash_checksum += buf[k];
				}
			}
		}

		printf("\n\rflash checksum 0x%8x attached checksum 0x%8x", flash_checksum, file_checksum.u);

		if (file_checksum.u != flash_checksum) {
			printf("\n\r[%s] The checksume is wrong!\n\r", __FUNCTION__);
			goto update_ota_exit;
		}
#endif

		//update ota signature at the end of OTA process
		ret = update_ota_signature(sig_backup, NewFWAddr);
		if (ret == -1) {
			printf("\n\r[%s] Update signature fail!\n\r", __FUNCTION__);
			goto update_ota_exit;
		}
	}
update_ota_exit:
	if (buf) {
		update_free(buf);
	}
	if (request) {
		update_free(request);
	}
	if (server_socket >= 0) {
		close(server_socket);
	}

	// redirect_server_port != 0 means there is redirect URL can be downloaded
	if (redirect_server_port != 0) {
		host = redirect_server_host;
		resource = redirect_resource;
		port = redirect_server_port;
		printf("\n\r[%s] OTA redirect host: %s, port: %d, resource: %s", __FUNCTION__, host, port, resource);
		goto restart_http_ota;
	}

	if (redirect) {
		update_free(redirect);
	}
	if (redirect_server_host) {
		update_free(redirect_server_host);
	}
	if (redirect_resource) {
		update_free(redirect_resource);
	}

	return ret;
}
#endif

#ifdef HTTPS_OTA_UPDATE
static int my_random(void *p_rng, unsigned char *output, size_t output_len)
{
	/* To avoid gcc warnings */
	(void) p_rng;

	rtw_get_random_bytes(output, output_len);
	return 0;
}

static void *my_calloc(size_t nelements, size_t elementSize)
{
	size_t size;
	void *ptr = NULL;

	size = nelements * elementSize;
	ptr = pvPortMalloc(size);

	if (ptr) {
		memset(ptr, 0, size);
	}

	return ptr;
}

static char *https_itoa(int value)
{
	char *val_str;
	int tmp = value, len = 1;

	while ((tmp /= 10) > 0) {
		len ++;
	}

	val_str = (char *) pvPortMalloc(len + 1);
	sprintf(val_str, "%d", value);

	return val_str;
}

/******************************************************************************************************************
** Function Name  : update_ota_https_connect_server
** Description    : Connect to the https server
** Input          : server_fd: socket to use
**                  ssl: SSL context
**                  conf: SSL configuration to use
**                  host: host address of the OTA server
**                  port: port of the OTA server
** Return         : Connect ok: 0
**                  Failed:     -1
*******************************************************************************************************************/
int update_ota_https_connect_server(mbedtls_net_context *server_fd, mbedtls_ssl_context *ssl, mbedtls_ssl_config *conf, char *host, int port)
{
	int ret = -1;
	int retry_count = 0;
	char *port_str = NULL;

#if defined(configENABLE_TRUSTZONE) && (configENABLE_TRUSTZONE == 1) && defined(CONFIG_SSL_CLIENT_PRIVATE_IN_TZ) && (CONFIG_SSL_CLIENT_PRIVATE_IN_TZ == 1)
	extern int NS_ENTRY secure_mbedtls_platform_set_calloc_free(void);
	secure_mbedtls_platform_set_calloc_free();
	extern void NS_ENTRY secure_set_ns_device_lock(void (*device_mutex_lock_func)(uint32_t), void (*device_mutex_unlock_func)(uint32_t));
	secure_set_ns_device_lock(device_mutex_lock, device_mutex_unlock);
#endif
#if MBEDTLS_VERSION_NUMBER==0x02100300 //if is mbedtls 2.16.4
	mbedtls_platform_setup(NULL);
#endif
	mbedtls_platform_set_calloc_free(my_calloc, vPortFree);

	// Initalization
	mbedtls_net_init(server_fd);
	mbedtls_ssl_init(ssl);
	mbedtls_ssl_config_init(conf);

#ifdef SSL_CLIENT_EXT
	if ((ret = ssl_client_ext_init()) != 0) {
		printf("\n\r[%s] ssl_client_ext_init failed, ret(%d)\n", __FUNCTION__, ret);
		return -1;
	}
#endif

	// Connect to server
	port_str = https_itoa(port);
	ret = mbedtls_net_connect(server_fd, host, port_str, MBEDTLS_NET_PROTO_TCP);
	update_free(port_str);
	if (ret != 0) {
		printf("\n\r[%s] mbedtls_net_connect failed, ret(%d)\n", __FUNCTION__, ret);
		return -1;
	}

	// Setup stuff
	mbedtls_ssl_set_bio(ssl, server_fd, mbedtls_net_send, mbedtls_net_recv, mbedtls_net_recv_timeout);
	if ((ret = mbedtls_ssl_config_defaults(conf,
										   MBEDTLS_SSL_IS_CLIENT,
										   MBEDTLS_SSL_TRANSPORT_STREAM,
										   MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {

		printf("\n\r[%s] mbedtls_ssl_config_defaults failed, ret(%d)\n", __FUNCTION__, ret);
		return -1;
	}

	mbedtls_ssl_conf_authmode(conf, MBEDTLS_SSL_VERIFY_NONE);
	mbedtls_ssl_conf_rng(conf, my_random, NULL);

#if (defined(MBEDTLS_SSL_IN_CONTENT_LEN) && (MBEDTLS_SSL_IN_CONTENT_LEN  == 4096)) ||  \
	(defined(MBEDTLS_SSL_MAX_CONTENT_LEN) && (MBEDTLS_SSL_MAX_CONTENT_LEN  == 4096))
	if ((ret = mbedtls_ssl_conf_max_frag_len(conf, MBEDTLS_SSL_MAX_FRAG_LEN_4096)) < 0) {
		printf("\n\r[%s] mbedtls_ssl_conf_max_frag_len failed, ret(%d)\n", __FUNCTION__, ret);
		return -1;
	}
#endif

#ifdef SSL_CLIENT_EXT
	if ((ret = ssl_client_ext_setup(conf)) != 0) {
		printf("\n\r[%s] ssl_client_ext_setup failed, ret (%d)\n", __FUNCTION__, ret);
		return -1;
	}
#endif

	if ((ret = mbedtls_ssl_setup(ssl, conf)) != 0) {
		printf("\n\r[%s] mbedtls_ssl_setup failed, ret (%d)\n", __FUNCTION__, ret);
		return -1;
	}

	// Handshake
	while ((ret = mbedtls_ssl_handshake(ssl)) != 0) {
		if ((ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE
			 && ret != MBEDTLS_ERR_NET_RECV_FAILED) || retry_count >= 5) {
			printf("\n\r[%s] mbedtls_ssl_handshake failed, ret(-0x%x)\n", __FUNCTION__, ret);
			return -1;
		}
		retry_count++;
	}

	printf("\n\r[%s] SSL ciphersuite %s\n", __FUNCTION__, mbedtls_ssl_get_ciphersuite(ssl));
	return 0;
}

/******************************************************************************************************************
** Function Name  : https_update_ota
** Description    : OTA update through https
** Input          : host: host address of the https server
**                  port: port of the https server
**                  resource: resource path
** Return         : Success: 0
**                  Failed:  -1
*******************************************************************************************************************/
int https_update_ota(char *host, int port, char *resource)
{
	mbedtls_net_context server_fd;
	mbedtls_ssl_context ssl;
	mbedtls_ssl_config conf;
	unsigned char *buf = NULL, *request = NULL;
	unsigned char sig_backup[32];
	int read_bytes = 0;
	int read_rtn = 0;
	uint32_t address = 0;
	uint32_t NewFWLen = 0, NewFWAddr = 0;
	int ret = -1;
	uint32_t curr_fw_idx = 0;
	uint32_t fw_len = 0;
	http_response_result_t rsp_result = {0};
#if USE_CHECKSUM
	uint32_t flash_checksum = 0;
	_file_checksum file_checksum;
	file_checksum.u = 0;
#endif

restart_http_ota:
	redirect_server_port = 0;

	buf = update_malloc(BUF_SIZE);
	if (!buf) {
		printf("\n\r[%s] Alloc buffer failed", __FUNCTION__);
		goto update_ota_exit1;
	}

	// connect https server
	ret = update_ota_https_connect_server(&server_fd, &ssl, &conf, host, port);
	if (ret == -1) {
		goto update_ota_exit;
	}

	NewFWAddr = sys_update_ota_prepare_addr();
	if (NewFWAddr == -1) {
		goto update_ota_exit;
	}

	// Write New FW sector
	if (NewFWAddr != ~0x0) {
		uint32_t idx = 0;
		int data_len = 0;
		printf("\n\r");

		// send https request
		request = (unsigned char *) update_malloc(strlen("GET /") + strlen(resource) + strlen(" HTTP/1.1\r\nHost: ")
				  + strlen(host) + strlen("\r\n\r\n") + 1);
		sprintf((char *)request, "GET /%s HTTP/1.1\r\nHost: %s\r\n\r\n", resource, host);

		ret = mbedtls_ssl_write(&ssl, request, strlen((char const *)request));

		if (ret < 0) {
			printf("\n\r[%s] Send HTTP request failed", __FUNCTION__);
			goto update_ota_exit;
		}

		// read and parse https response
		while (3 >= rsp_result.parse_status) { //still read header
			if (0 == rsp_result.parse_status) { //didn't get the http response
				memset(buf, 0, BUF_SIZE);

				read_bytes = mbedtls_ssl_read(&ssl, buf, BUF_SIZE);
				if (read_bytes <= 0) {
					printf("\n\r[%s] Read socket failed", __FUNCTION__);
					goto update_ota_exit;
				}

				idx = read_bytes;
				memset(&rsp_result, 0, sizeof(rsp_result));
				if (parse_http_response(buf, idx, &rsp_result) == -1) {
					goto update_ota_exit;
				}
			} else if ((1 == rsp_result.parse_status) || (3 == rsp_result.parse_status)) { //just get the status code
				memset(buf, 0, BUF_SIZE);
				memcpy(buf, rsp_result.header_bak, HEADER_BAK_LEN);
				update_free(rsp_result.header_bak);
				rsp_result.header_bak = NULL;

				read_bytes = mbedtls_ssl_read(&ssl, buf + HEADER_BAK_LEN, (BUF_SIZE - HEADER_BAK_LEN));
				if (read_bytes <= 0) {
					printf("\n\r[%s] Read socket failed", __FUNCTION__);
					goto update_ota_exit;
				}

				idx = read_bytes + HEADER_BAK_LEN;

				if (parse_http_response(buf, read_bytes + HEADER_BAK_LEN, &rsp_result) == -1) {
					goto update_ota_exit;
				}
			}
		}

		if (0 == rsp_result.body_len) {
			printf("\n\r[%s] New firmware size = 0 !", __FUNCTION__);
			goto update_ota_exit;
		} else {
			printf("\n\r[%s] Download new firmware begin, total size : %d\n\r", __FUNCTION__, rsp_result.body_len);
		}

		curr_fw_idx = sys_update_ota_get_curr_fw_idx();
		printf("\n\r[%s] Current firmware index is %d\r\n", __FUNCTION__, curr_fw_idx);
		fw_len = rsp_result.body_len;
		address = NewFWAddr;
		printf("\n\r[%s] fw size %d, NewFWAddr %08X\n\r",  __FUNCTION__, fw_len, address);
		NewFWLen = update_ota_erase_upg_region(fw_len, NewFWLen, NewFWAddr);
		if (NewFWLen == -1) {
			goto update_ota_exit;
		}

		// remove https header_len from buf
		read_bytes = idx - rsp_result.header_len;
		idx = 0;
		if (read_bytes > 0) {
			memcpy(buf, buf + rsp_result.header_len, read_bytes);
			memset(buf + read_bytes, 0, BUF_SIZE - read_bytes);
			goto skip_read;
		}

		// download new firmware
		while (idx < NewFWLen) {
			printf(".");
			data_len = NewFWLen - idx;
			if (data_len > BUF_SIZE) {
				data_len = BUF_SIZE;
			}

			memset(buf, 0, BUF_SIZE);
			read_bytes = 0;

			while (read_bytes < data_len) {
				read_rtn = mbedtls_ssl_read(&ssl, &buf[read_bytes], data_len - read_bytes);
				if (read_rtn <= 0) {
					printf("\n\r[%s] Read socket failed", __FUNCTION__);
					goto update_ota_exit;
				}
				read_bytes += read_rtn;
			}

skip_read:
			if ((idx + read_bytes) > NewFWLen) {
				printf("\n\r[%s] Redundant bytes received", __FUNCTION__);
				read_bytes = NewFWLen - idx;
			}

			// back up signature
			if (idx < 32) {
				memcpy(sig_backup + idx, buf, (idx + read_bytes > 32 ? (32 - idx) : read_bytes));
				memset(buf, 0xFF, (idx + read_bytes > 32 ? (32 - idx) : read_bytes));
				printf("\n\r[%s] sig_backup for %d bytes from %d index\n\r", __FUNCTION__, (idx + read_bytes > 32 ? (32 - idx) : read_bytes), idx);
			}

			device_mutex_lock(RT_DEV_LOCK_FLASH);
			if (flash_burst_write(&flash_ota, address + idx, read_bytes, buf) < 0) {
				printf("\n\r[%s] Write sector failed", __FUNCTION__);
				device_mutex_unlock(RT_DEV_LOCK_FLASH);
				goto update_ota_exit;
			}
			device_mutex_unlock(RT_DEV_LOCK_FLASH);

#if USE_CHECKSUM
			// checksum attached at file end
			if (idx + read_bytes > NewFWLen - 4) {
				file_checksum.c[0] = buf[read_bytes - 4];
				file_checksum.c[1] = buf[read_bytes - 3];
				file_checksum.c[2] = buf[read_bytes - 2];
				file_checksum.c[3] = buf[read_bytes - 1];
			}
#endif

			idx += read_bytes;
		}
		printf("\n\r[%s] Download new firmware %d bytes completed\n\r", __FUNCTION__, idx);

		mbedtls_ssl_close_notify(&ssl);

#if USE_CHECKSUM
		// read flash data back and calculate checksum
		for (int i = 0; i < NewFWLen - 4; i += BUF_SIZE) {
			int k;
			int rlen = (idx - 4 - i) > BUF_SIZE ? BUF_SIZE : (idx - 4 - i);
			device_mutex_lock(RT_DEV_LOCK_FLASH);
			flash_stream_read(&flash_ota, NewFWAddr + i, rlen, buf);
			device_mutex_unlock(RT_DEV_LOCK_FLASH);
			for (k = 0; k < rlen; k++) {
				if (i + k < 32) {
					flash_checksum += sig_backup[i + k];
				} else {
					flash_checksum += buf[k];
				}
			}
		}

		printf("\n\rflash checksum 0x%8x attached checksum 0x%8x", flash_checksum, file_checksum.u);

		if (file_checksum.u != flash_checksum) {
			printf("\n\r[%s] The checksume is wrong!\n\r", __FUNCTION__);
			goto update_ota_exit;
		}
#endif

		//update ota signature at the end of OTA process
		ret = update_ota_signature(sig_backup, NewFWAddr);
		if (ret == -1) {
			printf("\n\r[%s] Update signature fail!\n\r", __FUNCTION__);
			goto update_ota_exit;
		}
	}

update_ota_exit:
	mbedtls_net_free(&server_fd);
	mbedtls_ssl_free(&ssl);
	mbedtls_ssl_config_free(&conf);
#ifdef SSL_CLIENT_EXT
	ssl_client_ext_free();
#endif

update_ota_exit1:
	if (buf) {
		update_free(buf);
	}
	if (request) {
		update_free(request);
	}

	// redirect_server_port != 0 means there is redirect URL can be downloaded
	if (redirect_server_port != 0) {
		host = redirect_server_host;
		resource = redirect_resource;
		port = redirect_server_port;
		printf("\n\r[%s] OTA redirect host: %s, port: %d, resource: %s", __FUNCTION__, host, port, resource);
		goto restart_http_ota;
	}

	if (redirect) {
		update_free(redirect);
	}
	if (redirect_server_host) {
		update_free(redirect_server_host);
	}
	if (redirect_resource) {
		update_free(redirect_resource);
	}

	return ret;
}
#endif
#endif
