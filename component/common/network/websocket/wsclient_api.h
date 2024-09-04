/**
  ******************************************************************************
  * @file    wsclient_api.h
  * @author
  * @version
  * @brief   This file provides user interface for websocket client.
  ******************************************************************************
  */

#ifndef WSCLIENT_H
#define WSCLIENT_H

/** @addtogroup websocket WebSocket
 *  @ingroup    network
 *  @brief      WebSocket functions
 *  @{
 */

/** @addtogroup client WebSocket Client
 *  @brief      WebSocket client functions
 *  @{
 */

#include <websocket/libwsclient.h>

/****************Define if using the polarssl*******************/
#define USING_SSL

/******************Define the function used*********************/
#ifdef USING_SSL
int wss_set_fun_ops(wsclient_context *wsclient);
#define wsclient_set_fun_ops(wsclient) wss_set_fun_ops(wsclient)
#else
int ws_set_fun_ops(wsclient_context *wsclient);
#define wsclient_set_fun_ops(wsclient) ws_set_fun_ops(wsclient)
#endif
/***************************************************************/

/**
 * @brief Creating the websocket client context structure
 * @param[in] url: websocket server's url
 * @param[in] port: websocket server's port, if not given, default 80 for "ws", 443 for "wss"
 * @param[in] origin: the address or url of your self
 * @param[in] buf_len: the length of tx/rx/received buffer. It determine the maximum bytes of data send and receive.
 * @param[in] max_queue_size: max size of queue to buffer messages which are going to send to webserver.
 * @return Created: websocket client context structure
 * @return Failed:  NULL
 */
wsclient_context *create_wsclient(char *url, int port, char *path, char *origin, int buf_len, int max_queue_size);

/**
 * @brief Connecting to the websocket server
 * @param[in] wsclient: the websocket client context created by create_wsclientfunction
 * @return Connected: the socket value
 * @return Failed:    -1
 */
int ws_connect_url(wsclient_context *wsclient);

/**
 * @brief Set global value for KeepAlive socket option. These options will be set within ws_connect_url(), so the api must be invoked before ws_connect_url() or it would not take effect.
 * @param[in] keepalive_idle: value for TCP_KEEPIDLE option
 * @param[in] keepalive_interval: value for TCP_KEEPINTVL option
 * @param[in] keepalive_count: value for TCP_KEEPCNT option
 * @return None
 */
void ws_setsockopt_keepalive(uint32_t keepalive_idle, uint32_t keepalive_interval, uint32_t keepalive_count);

/**
 * @brief Set global value for RCVTO/SNDTO socket option. These options will be set within ws_connect_url() and take effect in connect process, so the api must be invoked before ws_connect_url() or it would not take effect.
 * @param[in] recv_timeout: value for SO_RCVTIMEO option
 * @param[in] send_timeout: value for SO_SNDTIMEO option
 * @return None
 */
void ws_setsockopt_timeout(uint32_t recv_timeout, uint32_t send_timeout);

/**
 * @brief Create the sending string data and copy to queue
 * @param[in] message: the string that send to server(cannot exceeding the MAX_DATA_LEN
 * @param[in] message_len: the length of the string
 * @param[in] use_mask: 0/1; 1 means using mask for bynary
 * @param[in] wsclient: the websocket client context
 * @return 0: send message to queue successfully
 * @return -1: fail to send message to queue
 */
int ws_send(char *message, int message_len, int use_mask, wsclient_context *wsclient);

/**
 * @brief Create the sending binary data and copy to queue
 * @param[in] message: the binary that send to server(cannot exceeding the MAX_DATA_LEN
 * @param[in] message_len: the length of the binary
 * @param[in] use_mask: 0/1; 1 means using mask for bynary
 * @param[in] wsclient: the websocket client context
 * @return 0: send message to queue successfully
 * @return -1: fail to send message to queue
 */
int ws_sendBinary(uint8_t *message, int message_len, int use_mask, wsclient_context *wsclient);

/**
 * @brief Sending Ping to websocket server
 * @param[in] use_mask: 0/1; 1 means using mask for bynary
 * @param[in] wsclient: the websocket client context
 * @return 0: send message to queue successfully
 * @return -1: fail to send message to queue
 */
int ws_sendPing(int use_mask, wsclient_context *wsclient);

/**
 * @brief Receicing data from server and send the data in tx_buf
 * @param[in] timeout: timeout (in milliseconds)
 * @param[in] wsclient: the websocket client context
 * @return None
 */
void ws_poll(int timeout, wsclient_context **wsclient);

/**
 * @brief callback function when getting message from server
 * @param[in] function that resolve the message received and the message length
 * @return None
 */
void ws_dispatch(void (*callback)(wsclient_context **, int)) ;

/**
 * @brief Getting the connection status
 * @param[in] wsclient: the websocket client context
 * @return readyStateValues(3 types:CLOSING, CLOSED, OPEN)
 */
readyStateValues ws_getReadyState(wsclient_context *wsclient);

/**
 * @brief Closing the connection with websocket server
 * @param[in] wsclient: the websocket client context
 * @return None
 */
void ws_close(wsclient_context **wsclient);

/**
 * @brief set Sec-WebSocket-Protocol of handshake header
 * @param[in] wsclient: the websocket client context
 * @param[in] pro: the protocol, eg: "chat, superchat"
 * @param[in] len: length of string pro
 * @return result(0:ok, -1:fail)
 */
int ws_handshake_header_set_protocol(wsclient_context *wsclient, char *pro, int len);

/**
 * @brief set Sec-WebSocket-Version of handshake header
 * @param[in] wsclient: the websocket client context
 * @param[in] ver: the version, eg: "13"
 * @param[in] len: length of string ver
 * @return result(0:ok, -1:fail)
 */
int ws_handshake_header_set_version(wsclient_context *wsclient, char *ver, int len);

/**
 * @brief add custom token to handshake header
 * @param[in] wsclient: the websocket client context
 * @param[in] cus: the custom token, eg: "custom_token: value\r\n"
 * @param[in] len: length of string cus
 * @return result(0:ok, -1:fail)
 */
int ws_handshake_header_custom_token(wsclient_context *wsclient, char *cus, int len);

/**
 * @brief set multi-send queue options. Called once after create_wsclient() and before ws_poll() started.
 * @param[in] wsclient: the websocket client context
 * @param[in] stable_buf_num: the stable buffer number in send queue. This should be a positive value and
 *                            should not exceed max_queue_size in create_wsclient(). If free buffers exceed
 *                            the stable_buf_num, it will be dynamically free.
 * @return result(0:ok, -1:fail)
 */
int ws_multisend_opts(wsclient_context *wsclient, int stable_buf_num);
/**@}*/
/**@}*/
#endif
