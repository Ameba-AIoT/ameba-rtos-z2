#include <platform_opts_bt.h>
#if defined(CONFIG_BT_OTA_CENTRAL_CLIENT) && CONFIG_BT_OTA_CENTRAL_CLIENT
#if defined(CONFIG_BT_OTA_CENTRAL_CLIENT_SPLIT) && CONFIG_BT_OTA_CENTRAL_CLIENT_SPLIT
#include <string.h>
#include <trace_app.h>
#include <gap_bond_le.h>
#include <gap_scan.h>
#include <gap.h>
#include <gap_conn_le.h>
#include <gcs_client.h>
#include "bt_ota_central_client_app.h"
#include "log_service.h"
#include "atcmd_bt.h"
#include "bt_ota_central_client_at_cmd.h"
#include "os_msg.h"
#include <platform/platform_stdlib.h>
#include "os_sched.h"
#include "os_mem.h"
#include "bt_ota_central_client_app_flags.h"
#if defined(CONFIG_BT_OTA_CENTRAL_CLIENT_W_REQ_CONFLICT) && CONFIG_BT_OTA_CENTRAL_CLIENT_W_REQ_CONFLICT
#include "insert_write.h"
#endif

extern void *bt_ota_central_client_io_queue_handle;
extern void *bt_ota_central_client_evt_queue_handle;
extern T_GAP_DEV_STATE bt_ota_central_client_gap_dev_state;

#define BD_ADDR_LEN							6


static u8 ctoi(char c)
{
	if((c >= 'A') && (c <= 'F')) {
		return (c - 'A' + 0x0A);
	}

	if((c >= 'a') && (c <= 'f')) {
		return (c - 'a' + 0x0A);
	}

	if((c >= '0') && (c <= '9')) {
		return (c - '0' + 0x00);
	}

	return 0xFF;
}

static u8 hex_str_to_bd_addr(u32 str_len, s8 *str, u8 *num_arr)
{
	num_arr += str_len/2 -1;
	u32 n = 0;
	u8 num = 0;

	if (str_len < 2) {
		return FALSE;
	}
	while (n < str_len) {
		if ((num = ctoi(str[n++])) == 0xFF) {
			return FALSE;
		}
		*num_arr = num << 4;
		if ((num = ctoi(str[n++])) == 0xFF) {
			return FALSE;
		}
		*num_arr |= num;
		num_arr--;
	}
	return TRUE;
}


void bt_ota_central_client_at_cmd_send_msg(uint16_t sub_type)
{
	uint8_t event = EVENT_IO_TO_APP;

	T_IO_MSG io_msg;

	io_msg.type = IO_MSG_TYPE_QDECODE;
	io_msg.subtype = sub_type;

	if (bt_ota_central_client_evt_queue_handle != NULL && bt_ota_central_client_io_queue_handle != NULL) {
		if (os_msg_send(bt_ota_central_client_io_queue_handle, &io_msg, 0) == false) {
			printf("bt ota central client at cmd send msg fail: subtype 0x%x\r\n", io_msg.subtype);
		} else if (os_msg_send(bt_ota_central_client_evt_queue_handle, &event, 0) == false) {
			printf("bt ota central client at cmd send event fail: subtype 0x%x\r\n", io_msg.subtype);
		}
	}
}

int bt_ota_central_client_at_cmd_connect(int argc, char **argv)
{
	(void) argc;
	u8 DestAddr[6] = {0};
	u8 DestAddrType = GAP_REMOTE_ADDR_LE_PUBLIC;
#if (F_BT_LE_USE_RANDOM_ADDR==1)
	T_GAP_LOCAL_ADDR_TYPE local_addr_type = GAP_LOCAL_ADDR_LE_RANDOM;
#elif (F_BT_LE_USE_RANDOM_ADDR==2) && F_BT_LE_LOCAL_IRK_SETTING_SUPPORT
	T_GAP_LOCAL_ADDR_TYPE local_addr_type = GAP_LOCAL_ADDR_LE_RANDOM;
#else
	T_GAP_LOCAL_ADDR_TYPE local_addr_type = GAP_LOCAL_ADDR_LE_PUBLIC;
#endif
	T_GAP_LE_CONN_REQ_PARAM conn_req_param;

	if(strcmp(argv[1], "P") == 0)
		DestAddrType = GAP_REMOTE_ADDR_LE_PUBLIC;
	else if(strcmp(argv[1], "R") == 0)
		DestAddrType = GAP_REMOTE_ADDR_LE_RANDOM;

	if (strlen(argv[2]) != 2*BD_ADDR_LEN)
		return -1;

	hex_str_to_bd_addr(strlen(argv[2]), ( s8 *)argv[2], (u8*)DestAddr);

	conn_req_param.scan_interval = 0xA0;	//100ms
	conn_req_param.scan_window = 0x80;		//80ms
	conn_req_param.conn_interval_min = 0x60;	//120ms
	conn_req_param.conn_interval_max = 0x60;	//120ms
	conn_req_param.conn_latency = 0;
	conn_req_param.supv_tout = 1000;
	conn_req_param.ce_len_min = 2 * (conn_req_param.conn_interval_min - 1);
	conn_req_param.ce_len_max = 2 * (conn_req_param.conn_interval_max - 1);
	le_set_conn_param(GAP_CONN_PARAM_1M, &conn_req_param);

	printf("cmd_con, DestAddr: 0x%02X:0x%02X:0x%02X:0x%02X:0x%02X:0x%02X\r\n",
			DestAddr[5], DestAddr[4], DestAddr[3], DestAddr[2], DestAddr[1], DestAddr[0]);

	le_connect(0, DestAddr, (T_GAP_REMOTE_ADDR_TYPE)DestAddrType, local_addr_type, 1000);

	return 0;
}


int bt_ota_central_client_at_cmd_scan(int argc, char **argv)
{
	u8 scan_filter_policy = GAP_SCAN_FILTER_ANY;
	uint8_t scan_filter_duplicate = GAP_SCAN_FILTER_DUPLICATE_ENABLE;
	u8 scan_enable = 0;
	static u8 scan_is_processing = 0;

	if (argc >= 2) {
		scan_enable = atoi(argv[1]);
		if (scan_enable == 1) {
			if (argc == 3)
				scan_filter_policy = atoi(argv[2]);
			else if (argc == 4) {
				scan_filter_policy = atoi(argv[2]);
				scan_filter_duplicate = atoi(argv[3]);
			}
		}
	}

	if (bt_ota_central_client_gap_dev_state.gap_init_state)
	{
		if (scan_enable) {
			if (scan_is_processing) {
				printf("Scan is processing, please stop it first\r\n");
			} else {
				scan_is_processing = 1;
				printf("Start scan, scan_filter_policy = %d, scan_filter_duplicate = %d\r\n", scan_filter_policy, scan_filter_duplicate);
				le_scan_set_param(GAP_PARAM_SCAN_FILTER_POLICY, sizeof(scan_filter_policy), &scan_filter_policy);
				le_scan_set_param(GAP_PARAM_SCAN_FILTER_DUPLICATES, sizeof(scan_filter_duplicate), &scan_filter_duplicate);
				le_scan_start();
			}
		} else {
			if (scan_is_processing) {
				le_scan_stop();
				printf("Stop scan\r\n");
				scan_is_processing = 0;
			} else
				printf("There is no scan\r\n");
		}
	}
	return 0;
}

extern void bt_ota_central_client_app_discov_services(uint8_t conn_id, bool start);
int bt_ota_central_client_at_cmd_ota_start(int argc, char **argv)
{
	int conn_id = atoi(argv[1]);
#if defined(CONFIG_BT_OTA_CENTRAL_CLIENT_W_REQ_CONFLICT) && CONFIG_BT_OTA_CENTRAL_CLIENT_W_REQ_CONFLICT
	if(if_queue_in(1, conn_id, 0, 0, 0, NULL) == 0)
#endif
	{
		bt_ota_central_client_app_discov_services(conn_id, true);
	}

	return 0;
}

int bt_ota_central_client_parse_param(char *buf, char **argv)
{
	int argc = 1;
	char str_buf[256];
	memset(str_buf, 0, 256);
	int str_count = 0;
	int buf_cnt = 0;
	static char temp_buf[256];
	char *buf_pos = temp_buf;
	memset(temp_buf, 0, sizeof(temp_buf));

	if(buf == NULL)
		goto exit;
	strncpy(temp_buf, buf, sizeof(temp_buf));

	while((argc < MAX_ARGC) && (*buf_pos != '\0')) {
		while((*buf_pos == ',') || (*buf_pos == '[') || (*buf_pos == ']')){
			if((*buf_pos == ',') && (*(buf_pos+1) == ',')){
				argv[argc] = NULL;
				argc++;
			}
			*buf_pos = '\0';
			buf_pos++;
		}

		if(*buf_pos == '\0')
			break;
		else if(*buf_pos == '"'){
			memset(str_buf,'\0',256);
			str_count = 0;
			buf_cnt = 0;
			*buf_pos = '\0';
			buf_pos ++;
			if(*buf_pos == '\0')
			break;
			argv[argc] = buf_pos;
			while((*buf_pos != '"')&&(*buf_pos != '\0')){
				if(*buf_pos == '\\'){
				buf_pos ++;
					buf_cnt++;
				}
				str_buf[str_count] = *buf_pos;
				str_count++;
				buf_cnt++;
				buf_pos ++;
			}
			*buf_pos = '\0';
			memcpy(buf_pos-buf_cnt,str_buf,buf_cnt);
		}
		else{
			argv[argc] = buf_pos;
		}
		argc++;
		buf_pos++;

		while( (*buf_pos != ',')&&(*buf_pos != '\0')&&(*buf_pos != '[')&&(*buf_pos != ']') )
			buf_pos++;
	}
exit:
	return argc;
}

int bt_ota_central_client_app_handle_at_cmd(uint16_t subtype, void *arg)
{
	int common_cmd_flag = 0;
	int argc = 0;
	char *argv[MAX_ARGC] = {0};

	if (arg) {
		argc = bt_ota_central_client_parse_param(arg, argv);
	}
	switch (subtype) {
		case BT_ATCMD_SCAN:
			bt_ota_central_client_at_cmd_scan(argc, argv);
			break;
		case BT_ATCMD_CONNECT:
			bt_ota_central_client_at_cmd_connect(argc, argv);
			break;
		case BT_ATCMD_OTA_START:
			bt_ota_central_client_at_cmd_ota_start(argc, argv);
			break;
		default:
			break;
	}

	return common_cmd_flag;
}
#endif
#endif
