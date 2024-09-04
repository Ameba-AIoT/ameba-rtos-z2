#ifndef OTA_8710C_H
#define OTA_8710C_H

#include "osdep_service.h"
#include <platform_stdlib.h>
#include <flash_api.h>
#include <lwip/sockets.h>

#ifdef __cplusplus
extern "C" {
#endif

/************************Related setting****************************/
#define HTTP_OTA_UPDATE
#define HTTPS_OTA_UPDATE
#ifdef HTTPS_OTA_UPDATE
//#define SSL_CLIENT_EXT
#endif

#define BUF_SIZE		4096
#define HEADER_BAK_LEN	32
/*******************************************************************/


/****************Define the structures used*************************/
typedef struct {
	uint32_t	ip_addr;
	uint16_t	port;
} update_cfg_local_t;

typedef struct {
	uint32_t	status_code;
	uint32_t	header_len;
	uint8_t		*body;
	uint32_t	body_len;
	uint8_t		*header_bak;
	uint32_t	parse_status;
} http_response_result_t;

typedef union {
	uint32_t u;
	unsigned char c[4];
} _file_checksum;
/*******************************************************************/


/****************General functions used by ota update***************/
void *update_malloc(unsigned int size);
void update_free(void *buf);
void ota_platform_reset(void);
int update_ota_connect_server(update_cfg_local_t *cfg);
uint32_t update_ota_prepare_addr(void);
int update_ota_erase_upg_region(uint32_t img_len, uint32_t NewFWLen, uint32_t NewFWAddr);
int update_ota_signature(unsigned char *sig_backup, uint32_t NewFWAddr);
/*******************************************************************/


/*******************Functions called by AT CMD**********************/
void cmd_update(int argc, char **argv);
void cmd_ota_image(bool cmd);
/*******************************************************************/


/*************************************************************************************************
** Function Name  : update_ota_local
** Description    : Starting a thread of OTA updating through socket
** Input          : ip:The IP address of OTA server
**					port:The Port of OTA server
** Return         : 0: Task created OK
**					-1: Task created failed
**************************************************************************************************/
int update_ota_local(char *ip, int port);


#ifdef HTTP_OTA_UPDATE
int update_ota_http_connect_server(int server_socket, char *host, int port);

/*************************************************************************************************
** Function Name  : http_update_ota
** Description    : OTA update through http
** Input          : host: host address of the http server
**                  port: port of the http server
**                  resource: resource path
** Return         : Success: 0
**                  Failed:  -1
**************************************************************************************************/
int http_update_ota(char *host, int port, char *resource);
#endif

#ifdef HTTPS_OTA_UPDATE
#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif
#include <mbedtls/platform.h>
#include <mbedtls/net_sockets.h>
#include <mbedtls/ssl.h>
#include "mbedtls/version.h"

#ifdef SSL_CLIENT_EXT
extern int ssl_client_ext_init(void);
extern int ssl_client_ext_setup(mbedtls_ssl_config *conf);
extern void ssl_client_ext_free(void);
#endif
int update_ota_https_connect_server(mbedtls_net_context *server_fd, mbedtls_ssl_context *ssl, mbedtls_ssl_config *conf, char *host, int port);

/*************************************************************************************************
** Function Name  : https_update_ota
** Description    : OTA update through https
** Input          : host: host address of the https server
**                  port: port of the https server
**                  resource: resource path
** Return         : Success: 0
**                  Failed:  -1
**************************************************************************************************/
int https_update_ota(char *host, int port, char *resource);
#endif


#ifdef __cplusplus
}
#endif

#endif
