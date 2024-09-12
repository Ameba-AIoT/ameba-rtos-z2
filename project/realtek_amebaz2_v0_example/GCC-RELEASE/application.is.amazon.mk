
# Initialize tool chain
# -------------------------------------------------------------------

AMEBAZ2_TOOLDIR	= ../../../component/soc/realtek/8710c/misc/iar_utility
AMEBAZ2_GCCTOOLDIR	= ../../../component/soc/realtek/8710c/misc/gcc_utility
AMEBAZ2_BSPDIR = ../../../component/soc/realtek/8710c/misc/bsp
AMEBAZ2_BOOTLOADERDIR = $(AMEBAZ2_BSPDIR)/image
AMEBAZ2_ROMSYMDIR = $(AMEBAZ2_BSPDIR)/ROM

DUMP_START_ADDRESS = 0x98000000
DUMP_END_ADDRESS = 0x98200000

OS := $(shell uname)

CROSS_COMPILE = $(ARM_GCC_TOOLCHAIN)/arm-none-eabi-

# Compilation tools
AR = $(CROSS_COMPILE)ar
CC = $(CROSS_COMPILE)gcc
AS = $(CROSS_COMPILE)as
NM = $(CROSS_COMPILE)nm
LD = $(CROSS_COMPILE)gcc
GDB = $(CROSS_COMPILE)gdb
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump

OS := $(shell uname)

LDSCRIPT := ./rtl8710c_ram.ld


# Initialize target name and target object files
# -------------------------------------------------------------------

all: build_info application_is manipulate_images

mp: build_info application_is manipulate_images

TARGET=application_is

OBJ_DIR=$(TARGET)/Debug/obj
BIN_DIR=$(TARGET)/Debug/bin
INFO_DIR=$(TARGET)/Debug/info
BOOT_BIN_DIR=bootloader/Debug/bin


ROMIMG = 

# Decide if 64 bit time wrapper is to be included
# -------------------------------------------------------------------
#SYSTEM_TIME64_MAKE_OPTION = 1

# Include folder list
# -------------------------------------------------------------------

INCLUDES =
INCLUDES += -I../inc

INCLUDES += -I../../../component/common/api
INCLUDES += -I../../../component/common/api/at_cmd
INCLUDES += -I../../../component/common/api/platform
INCLUDES += -I../../../component/common/api/wifi
INCLUDES += -I../../../component/common/api/wifi/rtw_wpa_supplicant/src
INCLUDES += -I../../../component/common/api/wifi/rtw_wpa_supplicant/src/crypto
INCLUDES += -I../../../component/common/api/network/include
INCLUDES += -I../../../component/common/application
INCLUDES += -I../../../component/common/application/mqtt/MQTTClient
INCLUDES += -I../../../component/common/application/mqtt/MQTTPacket/V5
INCLUDES += -I../../../component/common/application/mqtt/MQTTPacket
INCLUDES += -I../../../component/common/example
INCLUDES += -I../../../component/common/file_system
INCLUDES += -I../../../component/common/file_system/dct
INCLUDES += -I../../../component/common/file_system/fatfs
INCLUDES += -I../../../component/common/file_system/fatfs/r0.10c/include
INCLUDES += -I../../../component/common/file_system/ftl
INCLUDES += -I../../../component/common/utilities
INCLUDES += -I../../../component/common/mbed/hal
INCLUDES += -I../../../component/common/mbed/hal_ext
INCLUDES += -I../../../component/common/mbed/targets/hal/rtl8710c
INCLUDES += -I../../../component/common/network
INCLUDES += -I../../../component/common/network/coap/include
INCLUDES += -I../../../component/common/network/libcoap/include
INCLUDES += -I../../../component/common/network/http2/nghttp2-1.31.0/includes
INCLUDES += -I../../../component/common/network/lwip/lwip_v2.0.2/src/include
INCLUDES += -I../../../component/common/network/lwip/lwip_v2.0.2/src/include/lwip
INCLUDES += -I../../../component/common/network/lwip/lwip_v2.0.2/port/realtek
INCLUDES += -I../../../component/common/network/lwip/lwip_v2.0.2/port/realtek/freertos
#INCLUDES += -I../../../component/common/network/ssl/mbedtls-2.4.0/include
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/include
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls_config
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls_utils
INCLUDES += -I../../../component/common/network/ssl/ssl_ram_map/rom
INCLUDES += -I../../../component/common/drivers/wlan/realtek/include
INCLUDES += -I../../../component/common/drivers/wlan/realtek/src/osdep
INCLUDES += -I../../../component/common/drivers/wlan/realtek/src/core/option
INCLUDES += -I../../../component/common/test
INCLUDES += -I../../../component/common/bluetooth/realtek/sdk
INCLUDES += -I../../../component/common/bluetooth/realtek/sdk/inc
INCLUDES += -I../../../component/common/bluetooth/realtek/sdk/inc/app
INCLUDES += -I../../../component/common/bluetooth/realtek/sdk/inc/bluetooth/gap
INCLUDES += -I../../../component/common/bluetooth/realtek/sdk/inc/bluetooth/profile
INCLUDES += -I../../../component/common/bluetooth/realtek/sdk/inc/bluetooth/profile/client
INCLUDES += -I../../../component/common/bluetooth/realtek/sdk/inc/bluetooth/profile/server
INCLUDES += -I../../../component/common/bluetooth/realtek/sdk/inc/os
INCLUDES += -I../../../component/common/bluetooth/realtek/sdk/inc/platform
INCLUDES += -I../../../component/common/bluetooth/realtek/sdk/inc/stack
INCLUDES += -I../../../component/common/bluetooth/realtek/sdk/board/amebaz2/lib/GCC
INCLUDES += -I../../../component/common/bluetooth/realtek/sdk/board/amebaz2/src
INCLUDES += -I../../../component/common/bluetooth/realtek/sdk/board/amebaz2/src/hci
INCLUDES += -I../../../component/common/bluetooth/realtek/sdk/board/amebaz2/src/os
INCLUDES += -I../../../component/common/bluetooth/realtek/sdk/board/amebaz2/src/vendor_cmd
INCLUDES += -I../../../component/common/bluetooth/realtek/sdk/example/bt_config
INCLUDES += -I../../../component/common/bluetooth/realtek/sdk/example/ble_central
INCLUDES += -I../../../component/common/bluetooth/realtek/sdk/example/ble_peripheral
INCLUDES += -I../../../component/common/bluetooth/realtek/sdk/example/ble_scatternet
INCLUDES += -I../../../component/common/bluetooth/realtek/sdk/example/bt_ota_central_client

INCLUDES += -I../../../component/common/bluetooth/realtek/sdk/board/common/inc
INCLUDES += -I../../../component/common/bluetooth/realtek/sdk/example/bt_airsync_config
INCLUDES += -I../../../component/common/application/airsync/1.0.4

INCLUDES += -I../../../component/soc/realtek/8710c/cmsis/rtl8710c/include
INCLUDES += -I../../../component/soc/realtek/8710c/cmsis/rtl8710c/lib/include
INCLUDES += -I../../../component/soc/realtek/8710c/fwlib/include
INCLUDES += -I../../../component/soc/realtek/8710c/fwlib/lib/include
INCLUDES += -I../../../component/soc/realtek/8710c/cmsis/cmsis-core/include
INCLUDES += -I../../../component/soc/realtek/8710c/app/rtl_printf/include
INCLUDES += -I../../../component/soc/realtek/8710c/app/shell
INCLUDES += -I../../../component/soc/realtek/8710c/app/stdio_port
INCLUDES += -I../../../component/soc/realtek/8710c/misc/utilities/include
INCLUDES += -I../../../component/soc/realtek/8710c/mbed-drivers/include
INCLUDES += -I../../../component/soc/realtek/8710c/misc/platform
INCLUDES += -I../../../component/soc/realtek/8710c/misc/driver
INCLUDES += -I../../../component/soc/realtek/8710c/misc/os

INCLUDES += -I../../../component/os/freertos
#INCLUDES += -I../../../component/os/freertos/freertos_v10.0.1/Source/include
#INCLUDES += -I../../../component/os/freertos/freertos_v10.0.1/Source/portable/GCC/ARM_RTL8710C
#os - freertos 10.4.3 start
INCLUDES += -I../../../component/os/freertos/freertos_v10.4.3/include
INCLUDES += -I../../../component/os/freertos/freertos_v10.4.3/portable/GCC/ARM_CM33_NTZ/non_secure
#os - freertos 10.4.3 end
INCLUDES += -I../../../component/os/os_dep/include

INCLUDES += -I../../../component/common/application/amazon/amazon-ffs/ffs_demo/common/include
INCLUDES += -I../../../component/common/application/amazon/amazon-ffs/ffs_demo/realtek/configs

#Amazon Includes
INCLUDES += -I../../../component/common/example/amazon_freertos
INCLUDES += -I../../../lib_amazon/amazon-freertos/libraries/freertos_plus/standard/utils/include
INCLUDES += -I../../../lib_amazon/amazon-freertos/libraries/logging/include
INCLUDES += -I../../../lib_amazon/amazon-freertos/libraries/abstractions/wifi/include
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/unity/src
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/unity/extras/fixture/src
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/libraries/c_sdk/standard/common/include
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/libraries/abstractions/platform/freertos/include
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/libraries/abstractions/platform/include
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/libraries/abstractions/secure_sockets/include
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/libraries/c_sdk/standard/common/include/private
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/libraries/abstractions/pkcs11/corePKCS11/source/include
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/libraries/abstractions/pkcs11/corePKCS11/source/dependency/3rdparty/pkcs11
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/libraries/c_sdk/standard/common/include/private
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/libraries/c_sdk/standard/common/include/private
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/libraries/abstractions/backoff_algorithm/source/include
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/libraries/coreHTTP/source/include
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/libraries/coreHTTP/source/interface
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/libraries/abstractions/transport/secure_sockets
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/libraries/coreMQTT/source/include
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/jsmn
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/libraries/freertos_plus/aws/ota/include
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls_utils
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/libraries/freertos_plus/standard/crypto/include
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/demos/dev_mode_key_provisioning/include
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/libraries/c_sdk/aws/defender/include
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/libraries/c_sdk/standard/mqtt/include
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/libraries/c_sdk/standard/serializer/include
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/libraries/c_sdk/aws/shadow/include
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/libraries/c_sdk/standard/mqtt/test/access
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/libraries/coreHTTP/source/dependency/3rdparty/http_parser
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/libraries/c_sdk/standard/https/include
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/libraries/c_sdk/standard/https/test/access
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/libraries/c_sdk/standard/mqtt/src
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/libraries/c_sdk/standard/mqtt/test/mock
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/tinycbor/src
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/libraries/freertos_plus/aws/ota/src
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/libraries/freertos_plus/standard/tls/include
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/demos/common/pkcs11_helpers
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/demos/include
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/demos/network_manager
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/vendors/realtek/boards/amebaZ2/aws_demos/config_files
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/demos/common/http_demo_helpers
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/libraries/device_shadow_for_aws/source/include
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/libraries/coreJSON/source/include
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/demos/common/mqtt_demo_helpers
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/libraries/freertos_plus/aws/ota/test
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/libraries/jobs_for_aws/source/include
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/libraries/device_defender_for_aws/source/include
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/demos/device_defender_for_aws
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/libraries/abstractions/platform/include/platform
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/libraries/coreMQTT-Agent/source/include
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/libraries/abstractions/mqtt_agent/include
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/demos/common/mqtt_subscription_manager
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/libraries/ota_for_aws/source/include
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/libraries/ota_for_aws/source
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/libraries/ota_for_aws/source/portable
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/libraries/ota_for_aws/source/portable/os
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/libraries/abstractions/mqtt_agent/include
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/vendors/realtek/boards/amebaZ2/ports/ota
INCLUDES += -I../../../component/common/application/amazon/amazon-freertos/libraries/coreMQTT-Agent/source/include

# Source file list
# -------------------------------------------------------------------

SRC_C =
SRAM_C =
ERAM_C =
#bluetooth - board
SRC_C += ../../../component/common/bluetooth/realtek/sdk/board/amebaz2/src/hci/bt_fwconfig.c
SRC_C += ../../../component/common/bluetooth/realtek/sdk/board/amebaz2/src/hci/bt_mp_patch.c
SRC_C += ../../../component/common/bluetooth/realtek/sdk/board/amebaz2/src/hci/bt_normal_patch.c
SRC_C += ../../../component/common/bluetooth/realtek/sdk/board/common/src/cycle_queue.c
SRC_C += ../../../component/common/file_system/ftl/ftl.c
SRC_C += ../../../component/common/bluetooth/realtek/sdk/board/amebaz2/src/hci/hci_board.c
SRC_C += ../../../component/common/bluetooth/realtek/sdk/board/amebaz2/src/hci/hci_uart.c
SRC_C += ../../../component/common/bluetooth/realtek/sdk/board/common/os/freertos/osif_freertos.c
SRC_C += ../../../component/common/bluetooth/realtek/sdk/board/amebaz2/src/platform_utils.c
SRC_C += ../../../component/common/bluetooth/realtek/sdk/board/amebaz2/src/rtk_coex.c
SRC_C += ../../../component/common/bluetooth/realtek/sdk/board/amebaz2/src/trace_uart.c
SRC_C += ../../../component/common/bluetooth/realtek/sdk/board/amebaz2/src/vendor_cmd/vendor_cmd.c

#bluetooth - common
SRC_C += ../../../component/common/bluetooth/realtek/sdk/src/ble/profile/client/ancs_client.c
SRC_C += ../../../component/common/bluetooth/realtek/sdk/src/ble/profile/server/bas.c
SRC_C += ../../../component/common/bluetooth/realtek/sdk/src/ble/profile/client/bas_client.c
SRC_C += ../../../component/common/bluetooth/realtek/sdk/board/common/src/bt_uart_bridge.c
SRC_C += ../../../component/common/bluetooth/realtek/sdk/src/ble/profile/server/dis.c
SRC_C += ../../../component/common/bluetooth/realtek/sdk/src/ble/profile/client/gaps_client.c
SRC_C += ../../../component/common/bluetooth/realtek/sdk/board/common/src/hci_adapter.c
SRC_C += ../../../component/common/bluetooth/realtek/sdk/board/common/src/hci_process.c
SRC_C += ../../../component/common/bluetooth/realtek/sdk/src/ble/profile/server/hids.c
#SRC_C += ../../../component/common/bluetooth/realtek/sdk/src/ble/profile/server/hids_kb.c
#SRC_C += ../../../component/common/bluetooth/realtek/sdk/src/ble/profile/server/hids_rmc.c
SRC_C += ../../../component/common/bluetooth/realtek/sdk/src/ble/profile/client/simple_ble_client.c
SRC_C += ../../../component/common/bluetooth/realtek/sdk/src/ble/profile/server/simple_ble_service.c
SRC_C += ../../../component/common/bluetooth/realtek/sdk/src/ble/profile/client/ota_client.c
SRC_C += ../../../component/common/bluetooth/realtek/sdk/src/ble/profile/client/dfu_client.c
SRC_C += ../../../component/common/bluetooth/realtek/sdk/board/common/src/trace_task.c

#bluetooth - example - ble_central
SRC_C += ../../../component/common/bluetooth/realtek/sdk/example/ble_central/ble_central_app_main.c
SRC_C += ../../../component/common/bluetooth/realtek/sdk/example/ble_central/ble_central_app_task.c
SRC_C += ../../../component/common/bluetooth/realtek/sdk/example/ble_central/ble_central_at_cmd.c
SRC_C += ../../../component/common/bluetooth/realtek/sdk/example/ble_central/ble_central_client_app.c
SRC_C += ../../../component/common/bluetooth/realtek/sdk/example/ble_central/ble_central_link_mgr.c
SRC_C += ../../../component/common/bluetooth/realtek/sdk/src/ble/profile/client/gcs_client.c

#bluetooth - example - ble_peripheral
SRC_C += ../../../component/common/bluetooth/realtek/sdk/example/ble_peripheral/app_task.c
SRC_C += ../../../component/common/bluetooth/realtek/sdk/example/ble_peripheral/ble_app_main.c
SRC_C += ../../../component/common/bluetooth/realtek/sdk/example/ble_peripheral/ble_peripheral_at_cmd.c
SRC_C += ../../../component/common/bluetooth/realtek/sdk/example/ble_peripheral/peripheral_app.c

#bluetooth - example - ble_scatternet
SRC_C += ../../../component/common/bluetooth/realtek/sdk/example/ble_scatternet/ble_scatternet_app.c
SRC_C += ../../../component/common/bluetooth/realtek/sdk/example/ble_scatternet/ble_scatternet_app_main.c
SRC_C += ../../../component/common/bluetooth/realtek/sdk/example/ble_scatternet/ble_scatternet_app_task.c
SRC_C += ../../../component/common/bluetooth/realtek/sdk/example/ble_scatternet/ble_scatternet_link_mgr.c

#bluetooth - example - bt_beacon
SRC_C += ../../../component/common/bluetooth/realtek/sdk/example/bt_beacon/bt_beacon_app.c
SRC_C += ../../../component/common/bluetooth/realtek/sdk/example/bt_beacon/bt_beacon_app_main.c
SRC_C += ../../../component/common/bluetooth/realtek/sdk/example/bt_beacon/bt_beacon_app_task.c

#bluetooth - example - bt_config
SRC_C += ../../../component/common/bluetooth/realtek/sdk/example/bt_config/bt_config_app_main.c
SRC_C += ../../../component/common/bluetooth/realtek/sdk/example/bt_config/bt_config_app_task.c
SRC_C += ../../../component/common/bluetooth/realtek/sdk/example/bt_config/bt_config_peripheral_app.c
SRC_C += ../../../component/common/bluetooth/realtek/sdk/example/bt_config/bt_config_service.c
SRC_C += ../../../component/common/bluetooth/realtek/sdk/example/bt_config/bt_config_wifi.c

#bluetooth - example - bt_airsync_config
SRC_C += ../../../component/common/bluetooth/realtek/sdk/example/bt_airsync_config/bt_airsync_config_app_main.c
SRC_C += ../../../component/common/bluetooth/realtek/sdk/example/bt_airsync_config/bt_airsync_config_app_task.c
SRC_C += ../../../component/common/bluetooth/realtek/sdk/example/bt_airsync_config/bt_airsync_config_peripheral_app.c
SRC_C += ../../../component/common/application/airsync/1.0.4/airsync_ble_service.c
SRC_C += ../../../component/common/application/airsync/1.0.4/check_endian.c
SRC_C += ../../../component/common/application/airsync/1.0.4/epb.c
SRC_C += ../../../component/common/application/airsync/1.0.4/epb_MmBp.c
SRC_C += ../../../component/common/application/airsync/1.0.4/epb_WristBand.c
SRC_C += ../../../component/common/application/airsync/1.0.4/wechat_airsync_protocol.c

#bluetooth - example
SRC_C += ../../../component/common/bluetooth/realtek/sdk/bt_example_entry.c

#cmsis
SRC_C += ../../../component/soc/realtek/8710c/cmsis/rtl8710c/source/ram_s/app_start.c
SRC_C += ../../../component/soc/realtek/8710c/cmsis/rtl8710c/source/ram/mpu_config.c
SRC_C += ../../../component/soc/realtek/8710c/cmsis/rtl8710c/source/ram/sys_irq.c

#libc api wrapper
SRC_C += ../../../component/soc/realtek/8710c/misc/utilities/source/ram/libc_wrap.c

#console
SRC_C += ../../../component/common/api/at_cmd/atcmd_bt.c
SRC_C += ../../../component/common/api/at_cmd/atcmd_lwip.c
SRC_C += ../../../component/common/api/at_cmd/atcmd_mp.c
SRC_C += ../../../component/common/api/at_cmd/atcmd_mp_ext2.c
SRC_C += ../../../component/common/api/at_cmd/atcmd_sys.c
SRC_C += ../../../component/common/api/at_cmd/atcmd_wifi.c
SRC_C += ../../../component/soc/realtek/8710c/app/shell/cmd_shell.c
SRC_C += ../../../component/soc/realtek/8710c/app/shell/ram_s/consol_cmds.c
SRC_C += ../../../component/common/api/at_cmd/log_service.c
SRC_C += ../../../component/soc/realtek/8710c/misc/driver/rtl_console.c

#network - api
SRC_C += ../../../component/common/api/lwip_netconf.c

#network - api - wifi
SRC_C += ../../../component/common/api/wifi/wifi_conf.c
SRC_C += ../../../component/common/api/wifi/wifi_ind.c
SRC_C += ../../../component/common/api/wifi/wifi_promisc.c
SRC_C += ../../../component/common/api/wifi/wifi_util.c

#network - api - wifi - rtw_wpa_supplicant
#SRC_C += ../../../component/common/api/wifi/rtw_wpa_supplicant/src/crypto/tls_polarssl.c
SRC_C += ../../../component/common/api/wifi/rtw_wpa_supplicant/wpa_supplicant/wifi_eap_config.c
SRC_C += ../../../component/common/api/wifi/rtw_wpa_supplicant/wpa_supplicant/wifi_wps_config.c

#network - app
SRC_C += ../../../component/soc/realtek/8710c/misc/platform/ota_8710c.c
SRC_C += ../../../component/common/api/network/src/ping_test.c
SRC_C += ../../../component/common/utilities/ssl_client.c
SRC_C += ../../../component/common/utilities/ssl_client_ext.c
SRC_C += ../../../component/common/utilities/tcptest.c
SRC_C += ../../../component/common/api/network/src/wlan_network.c

#utilities
SRC_C += ../../../component/common/utilities/cJSON.c
SRC_C += ../../../component/common/utilities/http_client.c
SRC_C += ../../../component/common/utilities/xml.c
SRC_C += ../../../component/common/utilities/gb2unicode.c

#network - app - mqtt
SRC_C += ../../../component/common/application/mqtt/MQTTClient/MQTTClient.c
SRC_C += ../../../component/common/application/mqtt/MQTTPacket/MQTTConnectClient.c
SRC_C += ../../../component/common/application/mqtt/MQTTPacket/MQTTConnectServer.c
SRC_C += ../../../component/common/application/mqtt/MQTTPacket/MQTTDeserializePublish.c
SRC_C += ../../../component/common/application/mqtt/MQTTPacket/MQTTFormat.c
SRC_C += ../../../component/common/application/mqtt/MQTTClient/MQTTFreertos.c
SRC_C += ../../../component/common/application/mqtt/MQTTPacket/MQTTPacket.c
SRC_C += ../../../component/common/application/mqtt/MQTTPacket/MQTTSerializePublish.c
SRC_C += ../../../component/common/application/mqtt/MQTTPacket/MQTTSubscribeClient.c
SRC_C += ../../../component/common/application/mqtt/MQTTPacket/MQTTSubscribeServer.c
SRC_C += ../../../component/common/application/mqtt/MQTTPacket/MQTTUnsubscribeClient.c
SRC_C += ../../../component/common/application/mqtt/MQTTPacket/MQTTUnsubscribeServer.c
SRC_C += ../../../component/common/application/mqtt/MQTTPacket/V5/MQTTProperties.c
SRC_C += ../../../component/common/application/mqtt/MQTTPacket/V5/MQTTV5Packet.c

#network - coap
SRC_C += ../../../component/common/network/coap/sn_coap_ameba_port.c
SRC_C += ../../../component/common/network/coap/sn_coap_builder.c
SRC_C += ../../../component/common/network/coap/sn_coap_header_check.c
SRC_C += ../../../component/common/network/coap/sn_coap_parser.c
SRC_C += ../../../component/common/network/coap/sn_coap_protocol.c

#network - http
SRC_C += ../../../component/common/network/httpc/httpc_tls.c
SRC_C += ../../../component/common/network/httpd/httpd_tls.c

#network
SRC_C += ../../../component/common/network/dhcp/dhcps.c
SRC_C += ../../../component/common/network/sntp/sntp.c

#network - lwip
#network - lwip - api
SRC_C += ../../../component/common/network/lwip/lwip_v2.0.2/src/api/api_lib.c
SRC_C += ../../../component/common/network/lwip/lwip_v2.0.2/src/api/api_msg.c
SRC_C += ../../../component/common/network/lwip/lwip_v2.0.2/src/api/err.c
SRC_C += ../../../component/common/network/lwip/lwip_v2.0.2/src/api/netbuf.c
SRC_C += ../../../component/common/network/lwip/lwip_v2.0.2/src/api/netdb.c
SRC_C += ../../../component/common/network/lwip/lwip_v2.0.2/src/api/netifapi.c
SRC_C += ../../../component/common/network/lwip/lwip_v2.0.2/src/api/sockets.c
SRC_C += ../../../component/common/network/lwip/lwip_v2.0.2/src/api/tcpip.c

#network - lwip - core
SRC_C += ../../../component/common/network/lwip/lwip_v2.0.2/src/core/def.c
SRC_C += ../../../component/common/network/lwip/lwip_v2.0.2/src/core/dns.c
SRC_C += ../../../component/common/network/lwip/lwip_v2.0.2/src/core/inet_chksum.c
SRC_C += ../../../component/common/network/lwip/lwip_v2.0.2/src/core/init.c
SRC_C += ../../../component/common/network/lwip/lwip_v2.0.2/src/core/ip.c
SRC_C += ../../../component/common/network/lwip/lwip_v2.0.2/src/core/mem.c
SRC_C += ../../../component/common/network/lwip/lwip_v2.0.2/src/core/memp.c
SRC_C += ../../../component/common/network/lwip/lwip_v2.0.2/src/core/netif.c
SRC_C += ../../../component/common/network/lwip/lwip_v2.0.2/src/core/pbuf.c
SRC_C += ../../../component/common/network/lwip/lwip_v2.0.2/src/core/raw.c
SRC_C += ../../../component/common/network/lwip/lwip_v2.0.2/src/core/stats.c
SRC_C += ../../../component/common/network/lwip/lwip_v2.0.2/src/core/sys.c
SRC_C += ../../../component/common/network/lwip/lwip_v2.0.2/src/core/tcp.c
SRC_C += ../../../component/common/network/lwip/lwip_v2.0.2/src/core/tcp_in.c
SRC_C += ../../../component/common/network/lwip/lwip_v2.0.2/src/core/tcp_out.c
SRC_C += ../../../component/common/network/lwip/lwip_v2.0.2/src/core/timeouts.c
SRC_C += ../../../component/common/network/lwip/lwip_v2.0.2/src/core/udp.c

#network - lwip - core - ipv4
SRC_C += ../../../component/common/network/lwip/lwip_v2.0.2/src/core/ipv4/autoip.c
SRC_C += ../../../component/common/network/lwip/lwip_v2.0.2/src/core/ipv4/dhcp.c
SRC_C += ../../../component/common/network/lwip/lwip_v2.0.2/src/core/ipv4/etharp.c
SRC_C += ../../../component/common/network/lwip/lwip_v2.0.2/src/core/ipv4/icmp.c
SRC_C += ../../../component/common/network/lwip/lwip_v2.0.2/src/core/ipv4/igmp.c
SRC_C += ../../../component/common/network/lwip/lwip_v2.0.2/src/core/ipv4/ip4.c
SRC_C += ../../../component/common/network/lwip/lwip_v2.0.2/src/core/ipv4/ip4_addr.c
SRC_C += ../../../component/common/network/lwip/lwip_v2.0.2/src/core/ipv4/ip4_frag.c

#network - lwip - core - ipv6
SRC_C += ../../../component/common/network/lwip/lwip_v2.0.2/src/core/ipv6/dhcp6.c
SRC_C += ../../../component/common/network/lwip/lwip_v2.0.2/src/core/ipv6/ethip6.c
SRC_C += ../../../component/common/network/lwip/lwip_v2.0.2/src/core/ipv6/icmp6.c
SRC_C += ../../../component/common/network/lwip/lwip_v2.0.2/src/core/ipv6/inet6.c
SRC_C += ../../../component/common/network/lwip/lwip_v2.0.2/src/core/ipv6/ip6.c
SRC_C += ../../../component/common/network/lwip/lwip_v2.0.2/src/core/ipv6/ip6_addr.c
SRC_C += ../../../component/common/network/lwip/lwip_v2.0.2/src/core/ipv6/ip6_frag.c
SRC_C += ../../../component/common/network/lwip/lwip_v2.0.2/src/core/ipv6/mld6.c
SRC_C += ../../../component/common/network/lwip/lwip_v2.0.2/src/core/ipv6/nd6.c

#network - lwip - netif
SRC_C += ../../../component/common/network/lwip/lwip_v2.0.2/src/netif/ethernet.c

#network - lwip - port
SRC_C += ../../../component/common/network/lwip/lwip_v2.0.2/port/realtek/freertos/ethernetif.c
SRC_C += ../../../component/common/drivers/wlan/realtek/src/osdep/lwip_intf.c
SRC_C += ../../../component/common/network/lwip/lwip_v2.0.2/port/realtek/freertos/sys_arch.c

#network - mdns
SRC_C += ../../../component/common/network/mDNS/mDNSPlatform.c

#network - ssl - mbedtls
#SRC_C += ../../../component/common/network/ssl/mbedtls-2.4.0/library/aesni.c
#SRC_C += ../../../component/common/network/ssl/mbedtls-2.4.0/library/blowfish.c
#SRC_C += ../../../component/common/network/ssl/mbedtls-2.4.0/library/camellia.c
#SRC_C += ../../../component/common/network/ssl/mbedtls-2.4.0/library/ccm.c
#SRC_C += ../../../component/common/network/ssl/mbedtls-2.4.0/library/certs.c
#SRC_C += ../../../component/common/network/ssl/mbedtls-2.4.0/library/cipher.c
#SRC_C += ../../../component/common/network/ssl/mbedtls-2.4.0/library/cipher_wrap.c
#SRC_C += ../../../component/common/network/ssl/mbedtls-2.4.0/library/cmac.c
#SRC_C += ../../../component/common/network/ssl/mbedtls-2.4.0/library/debug.c
#SRC_C += ../../../component/common/network/ssl/mbedtls-2.4.0/library/error.c
#SRC_C += ../../../component/common/network/ssl/mbedtls-2.4.0/library/gcm.c
#SRC_C += ../../../component/common/network/ssl/mbedtls-2.4.0/library/havege.c
#SRC_C += ../../../component/common/network/ssl/mbedtls-2.4.0/library/md.c
#SRC_C += ../../../component/common/network/ssl/mbedtls-2.4.0/library/md2.c
#SRC_C += ../../../component/common/network/ssl/mbedtls-2.4.0/library/md4.c
#SRC_C += ../../../component/common/network/ssl/mbedtls-2.4.0/library/md_wrap.c
#SRC_C += ../../../component/common/network/ssl/mbedtls-2.4.0/library/memory_buffer_alloc.c
#SRC_C += ../../../component/common/network/ssl/mbedtls-2.4.0/library/net_sockets.c
#SRC_C += ../../../component/common/network/ssl/mbedtls-2.4.0/library/padlock.c
#SRC_C += ../../../component/common/network/ssl/mbedtls-2.4.0/library/pkcs11.c
#SRC_C += ../../../component/common/network/ssl/mbedtls-2.4.0/library/pkcs12.c
#SRC_C += ../../../component/common/network/ssl/mbedtls-2.4.0/library/pkcs5.c
#SRC_C += ../../../component/common/network/ssl/mbedtls-2.4.0/library/pkparse.c
#SRC_C += ../../../component/common/network/ssl/mbedtls-2.4.0/library/platform.c
#SRC_C += ../../../component/common/network/ssl/mbedtls-2.4.0/library/ripemd160.c
#SRC_C += ../../../component/common/network/ssl/mbedtls-2.4.0/library/sha256.c
#SRC_C += ../../../component/common/network/ssl/mbedtls-2.4.0/library/ssl_cache.c
#SRC_C += ../../../component/common/network/ssl/mbedtls-2.4.0/library/ssl_ciphersuites.c
#SRC_C += ../../../component/common/network/ssl/mbedtls-2.4.0/library/ssl_cli.c
#SRC_C += ../../../component/common/network/ssl/mbedtls-2.4.0/library/ssl_cookie.c
#SRC_C += ../../../component/common/network/ssl/mbedtls-2.4.0/library/ssl_srv.c
#SRC_C += ../../../component/common/network/ssl/mbedtls-2.4.0/library/ssl_ticket.c
#SRC_C += ../../../component/common/network/ssl/mbedtls-2.4.0/library/ssl_tls.c
#SRC_C += ../../../component/common/network/ssl/mbedtls-2.4.0/library/threading.c
#SRC_C += ../../../component/common/network/ssl/mbedtls-2.4.0/library/timing.c
#SRC_C += ../../../component/common/network/ssl/mbedtls-2.4.0/library/version.c
#SRC_C += ../../../component/common/network/ssl/mbedtls-2.4.0/library/version_features.c
#SRC_C += ../../../component/common/network/ssl/mbedtls-2.4.0/library/x509.c
#SRC_C += ../../../component/common/network/ssl/mbedtls-2.4.0/library/x509_create.c
#SRC_C += ../../../component/common/network/ssl/mbedtls-2.4.0/library/x509_crl.c
#SRC_C += ../../../component/common/network/ssl/mbedtls-2.4.0/library/x509_crt.c
#SRC_C += ../../../component/common/network/ssl/mbedtls-2.4.0/library/x509_csr.c
#SRC_C += ../../../component/common/network/ssl/mbedtls-2.4.0/library/x509write_crt.c
#SRC_C += ../../../component/common/network/ssl/mbedtls-2.4.0/library/x509write_csr.c
#SRC_C += ../../../component/common/network/ssl/mbedtls-2.4.0/library/xtea.c

#amazon - mbedtls start
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/aes.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/aesni.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/arc4.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/aria.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/asn1parse.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/asn1write.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/base64.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls_rtk/bignum.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/blowfish.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/camellia.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/ccm.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/certs.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/chacha20.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/chachapoly.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/cipher.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/cipher_wrap.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/cmac.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/ctr_drbg.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/debug.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/des.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/dhm.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/ecdh.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/ecdsa.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/ecjpake.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/ecp.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/ecp_curves.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/entropy.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/entropy_poll.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/error.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/gcm.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/havege.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/hkdf.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/hmac_drbg.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/md.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/md2.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/md4.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/md5.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/md_wrap.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/memory_buffer_alloc.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls_rtk/net_sockets.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/nist_kw.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/oid.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/padlock.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/pem.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/pk.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/pk_wrap.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/pkcs11.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/pkcs12.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/pkcs5.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/pkparse.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/pkwrite.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/platform.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/platform_util.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/poly1305.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/ripemd160.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/rsa.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/rsa_internal.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/sha1.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/sha256.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/sha512.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/ssl_cache.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/ssl_ciphersuites.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/ssl_cli.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/ssl_cookie.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/ssl_srv.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/ssl_ticket.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/ssl_tls.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/threading.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/timing.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/version.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/version_features.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/x509.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/x509_create.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/x509_crl.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/x509_crt.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/x509_csr.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/x509write_crt.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/x509write_csr.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls/library/xtea.c

SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls_utils/mbedtls_error.c
SRC_C += ../../../component/common/application/amazon/amazon-freertos/libraries/3rdparty/mbedtls_utils/mbedtls_utils.c
#amazon - mbedtls end

#network - ssl - ssl_ram_map
SRC_C += ../../../component/common/network/ssl/ssl_ram_map/rom/rom_ssl_ram_map.c
SRC_C += ../../../component/common/network/ssl/ssl_func_stubs/ssl_func_stubs.c

#network - websocket
SRC_C += ../../../component/common/network/websocket/wsclient_tls.c
SRC_C += ../../../component/common/network/websocket/wsserver_tls.c

#os
SRC_C += ../../../component/os/freertos/cmsis_os.c
SRC_C += ../../../component/os/os_dep/device_lock.c
SRC_C += ../../../component/os/freertos/freertos_cb.c
SRC_C += ../../../component/os/freertos/freertos_service.c
SRC_C += ../../../component/os/os_dep/osdep_service.c
SRC_C += ../../../component/os/freertos/freertos_pmu.c

#os - freertos
#SRC_C += ../../../component/os/freertos/freertos_v10.0.1/Source/croutine.c
#SRC_C += ../../../component/os/freertos/freertos_v10.0.1/Source/event_groups.c
#SRC_C += ../../../component/os/freertos/freertos_v10.0.1/Source/list.c
#SRC_C += ../../../component/os/freertos/freertos_v10.0.1/Source/queue.c
#SRC_C += ../../../component/os/freertos/freertos_v10.0.1/Source/stream_buffer.c
#SRC_C += ../../../component/os/freertos/freertos_v10.0.1/Source/tasks.c
#SRC_C += ../../../component/os/freertos/freertos_v10.0.1/Source/timers.c

#os - freertos - portable
#SRC_C += ../../../component/os/freertos/freertos_v10.0.1/Source/portable/MemMang/heap_5.c
#SRC_C += ../../../component/os/freertos/freertos_v10.0.1/Source/portable/GCC/ARM_RTL8710C/port.c

#amazon - freertos 10.4.3 start
SRC_C += ../../../component/os/freertos/freertos_v10.4.3/croutine.c
SRC_C += ../../../component/os/freertos/freertos_v10.4.3/event_groups.c
SRC_C += ../../../component/os/freertos/freertos_v10.4.3/list.c
SRC_C += ../../../component/os/freertos/freertos_v10.4.3/queue.c
SRC_C += ../../../component/os/freertos/freertos_v10.4.3/stream_buffer.c
SRC_C += ../../../component/os/freertos/freertos_v10.4.3/tasks.c
SRC_C += ../../../component/os/freertos/freertos_v10.4.3/timers.c

SRC_C += ../../../component/os/freertos/freertos_v10.4.3/portable/GCC/ARM_CM33_NTZ/non_secure/port.c
SRC_C += ../../../component/os/freertos/freertos_v10.4.3/portable/GCC/ARM_CM33_NTZ/non_secure/portasm.c

SRC_C += ../../../component/os/freertos/freertos_heap_rtk.c
SRC_C += ../../../component/os/freertos/freertos_heap5_config.c
#amazon - freertos 10.4.3 end

#peripheral - api
SRC_C += ../../../component/common/mbed/targets/hal/rtl8710c/crypto_api.c
SRC_C += ../../../component/common/mbed/targets/hal/rtl8710c/dma_api.c
SRC_C += ../../../component/common/mbed/targets/hal/rtl8710c/efuse_api.c
SRC_C += ../../../component/common/mbed/targets/hal/rtl8710c/gpio_api.c
SRC_C += ../../../component/common/mbed/targets/hal/rtl8710c/gpio_irq_api.c
SRC_C += ../../../component/common/mbed/targets/hal/rtl8710c/i2c_api.c
SRC_C += ../../../component/common/mbed/targets/hal/rtl8710c/pinmap.c
SRC_C += ../../../component/common/mbed/targets/hal/rtl8710c/pinmap_common.c
SRC_C += ../../../component/common/mbed/targets/hal/rtl8710c/port_api.c
SRC_C += ../../../component/common/mbed/targets/hal/rtl8710c/pwmout_api.c
SRC_C += ../../../component/common/mbed/targets/hal/rtl8710c/rtc_api.c
SRC_C += ../../../component/common/mbed/targets/hal/rtl8710c/serial_api.c
SRC_C += ../../../component/common/mbed/targets/hal/rtl8710c/spdio_api.c
SRC_C += ../../../component/common/mbed/targets/hal/rtl8710c/spi_api.c
SRC_C += ../../../component/common/mbed/targets/hal/rtl8710c/sys_api.c
SRC_C += ../../../component/common/mbed/targets/hal/rtl8710c/timer_api.c
SRC_C += ../../../component/common/mbed/targets/hal/rtl8710c/us_ticker.c
SRC_C += ../../../component/common/mbed/targets/hal/rtl8710c/us_ticker_api.c
SRC_C += ../../../component/common/mbed/targets/hal/rtl8710c/wait_api.c
SRC_C += ../../../component/common/mbed/targets/hal/rtl8710c/wdt_api.c
SRC_C += ../../../component/common/mbed/targets/hal/rtl8710c/reset_reason_api.c
SRC_C += ../../../component/common/mbed/targets/hal/rtl8710c/external_flash_api.c

#peripheral - hal
SRC_C += ../../../component/soc/realtek/8710c/fwlib/source/ram_s/hal_efuse.c
SRC_C += ../../../component/soc/realtek/8710c/fwlib/source/ram/hal_gdma.c
SRC_C += ../../../component/soc/realtek/8710c/fwlib/source/ram/hal_gpio.c
SRC_C += ../../../component/soc/realtek/8710c/fwlib/source/ram_ns/hal_i2c.c
SRC_C += ../../../component/soc/realtek/8710c/fwlib/source/ram/hal_misc.c
SRC_C += ../../../component/soc/realtek/8710c/fwlib/source/ram_s/hal_pinmux_nsc.c
SRC_C += ../../../component/soc/realtek/8710c/fwlib/source/ram_ns/hal_pwm.c
SRC_C += ../../../component/soc/realtek/8710c/fwlib/source/ram_ns/hal_sdio_dev.c
SRC_C += ../../../component/soc/realtek/8710c/fwlib/source/ram_ns/hal_ssi.c
SRC_C += ../../../component/soc/realtek/8710c/fwlib/source/ram/hal_uart.c

#peripheral - wlan
#SRC_C += ../../../component/common/drivers/wlan/realtek/src/core/option/rtw_opt_rf_para_rtl8710c.c

#file_system - fatfs
SRC_C += ../../../component/common/file_system/fatfs/fatfs_ext/src/ff_driver.c
SRC_C += ../../../component/common/file_system/fatfs/r0.10c/src/diskio.c
SRC_C += ../../../component/common/file_system/fatfs/r0.10c/src/ff.c
SRC_C += ../../../component/common/file_system/fatfs/r0.10c/src/option/ccsbcs.c
SRC_C += ../../../component/common/file_system/fatfs/disk_if/src/flash_fatfs.c

#utilities - ffs and example
#SRC_C += ../../../component/common/application/amazon/amazon-ffs/libffs/src/ffs/common/ffs_base64.c  \
#	../../../component/common/application/amazon/amazon-ffs/libffs/src/ffs/common/ffs_base85.c  \
#	../../../component/common/application/amazon/amazon-ffs/libffs/src/ffs/common/ffs_configuration_map.c  \
#	../../../component/common/application/amazon/amazon-ffs/libffs/src/ffs/common/ffs_hex.c  \
#	../../../component/common/application/amazon/amazon-ffs/libffs/src/ffs/common/ffs_json.c  \
#	../../../component/common/application/amazon/amazon-ffs/libffs/src/ffs/common/ffs_logging.c  \
#	../../../component/common/application/amazon/amazon-ffs/libffs/src/ffs/common/ffs_result.c  \
#	../../../component/common/application/amazon/amazon-ffs/libffs/src/ffs/common/ffs_stream.c  \
#	../../../component/common/application/amazon/amazon-ffs/libffs/src/ffs/common/ffs_wifi.c  \
#	../../../component/common/application/amazon/amazon-ffs/libffs/src/ffs/conversion/ffs_convert_device_details.c  \
#	../../../component/common/application/amazon/amazon-ffs/libffs/src/ffs/conversion/ffs_convert_json_value.c  \
#	../../../component/common/application/amazon/amazon-ffs/libffs/src/ffs/conversion/ffs_convert_registration_details.c  \
#	../../../component/common/application/amazon/amazon-ffs/libffs/src/ffs/conversion/ffs_convert_registration_state.c  \
#	../../../component/common/application/amazon/amazon-ffs/libffs/src/ffs/conversion/ffs_convert_wifi_connection_attempt.c  \
#	../../../component/common/application/amazon/amazon-ffs/libffs/src/ffs/conversion/ffs_convert_wifi_connection_details.c  \
#	../../../component/common/application/amazon/amazon-ffs/libffs/src/ffs/conversion/ffs_convert_wifi_connection_state.c  \
#	../../../component/common/application/amazon/amazon-ffs/libffs/src/ffs/conversion/ffs_convert_wifi_credentials.c  \
#	../../../component/common/application/amazon/amazon-ffs/libffs/src/ffs/conversion/ffs_convert_wifi_provisionee_state.c  \
#	../../../component/common/application/amazon/amazon-ffs/libffs/src/ffs/conversion/ffs_convert_wifi_scan_result.c  \
#	../../../component/common/application/amazon/amazon-ffs/libffs/src/ffs/conversion/ffs_convert_wifi_security_protocol.c  \
#	../../../component/common/application/amazon/amazon-ffs/libffs/src/ffs/dss/model/ffs_dss_compute_configuration_data_request.c  \
#	../../../component/common/application/amazon/amazon-ffs/libffs/src/ffs/dss/model/ffs_dss_compute_configuration_data_response.c  \
#	../../../component/common/application/amazon/amazon-ffs/libffs/src/ffs/dss/model/ffs_dss_configuration.c  \
#	../../../component/common/application/amazon/amazon-ffs/libffs/src/ffs/dss/model/ffs_dss_device_details.c  \
#	../../../component/common/application/amazon/amazon-ffs/libffs/src/ffs/dss/model/ffs_dss_error_details.c  \
#	../../../component/common/application/amazon/amazon-ffs/libffs/src/ffs/dss/model/ffs_dss_get_wifi_credentials_request.c  \
#	../../../component/common/application/amazon/amazon-ffs/libffs/src/ffs/dss/model/ffs_dss_get_wifi_credentials_response.c  \
#	../../../component/common/application/amazon/amazon-ffs/libffs/src/ffs/dss/model/ffs_dss_post_wifi_scan_data_request.c  \
#	../../../component/common/application/amazon/amazon-ffs/libffs/src/ffs/dss/model/ffs_dss_post_wifi_scan_data_response.c  \
#	../../../component/common/application/amazon/amazon-ffs/libffs/src/ffs/dss/model/ffs_dss_registration_details.c  \
#	../../../component/common/application/amazon/amazon-ffs/libffs/src/ffs/dss/model/ffs_dss_registration_state.c  \
#	../../../component/common/application/amazon/amazon-ffs/libffs/src/ffs/dss/model/ffs_dss_report_request.c  \
#	../../../component/common/application/amazon/amazon-ffs/libffs/src/ffs/dss/model/ffs_dss_report_response.c  \
#	../../../component/common/application/amazon/amazon-ffs/libffs/src/ffs/dss/model/ffs_dss_report_result.c  \
#	../../../component/common/application/amazon/amazon-ffs/libffs/src/ffs/dss/model/ffs_dss_start_pin_based_setup_request.c  \
#	../../../component/common/application/amazon/amazon-ffs/libffs/src/ffs/dss/model/ffs_dss_start_pin_based_setup_response.c  \
#	../../../component/common/application/amazon/amazon-ffs/libffs/src/ffs/dss/model/ffs_dss_start_provisioning_session_request.c  \
#	../../../component/common/application/amazon/amazon-ffs/libffs/src/ffs/dss/model/ffs_dss_start_provisioning_session_response.c  \
#	../../../component/common/application/amazon/amazon-ffs/libffs/src/ffs/dss/model/ffs_dss_wifi_connection_attempt.c  \
#	../../../component/common/application/amazon/amazon-ffs/libffs/src/ffs/dss/model/ffs_dss_wifi_connection_details.c  \
#	../../../component/common/application/amazon/amazon-ffs/libffs/src/ffs/dss/model/ffs_dss_wifi_connection_state.c  \
#	../../../component/common/application/amazon/amazon-ffs/libffs/src/ffs/dss/model/ffs_dss_wifi_credentials.c  \
#	../../../component/common/application/amazon/amazon-ffs/libffs/src/ffs/dss/model/ffs_dss_wifi_provisionee_state.c  \
#	../../../component/common/application/amazon/amazon-ffs/libffs/src/ffs/dss/model/ffs_dss_wifi_scan_result.c  \
#	../../../component/common/application/amazon/amazon-ffs/libffs/src/ffs/dss/model/ffs_dss_wifi_security_protocol.c  \
#	../../../component/common/application/amazon/amazon-ffs/libffs/src/ffs/dss/ffs_dss_operation_compute_configuration_data.c  \
#	../../../component/common/application/amazon/amazon-ffs/libffs/src/ffs/dss/ffs_dss_operation_get_wifi_credentials.c  \
#	../../../component/common/application/amazon/amazon-ffs/libffs/src/ffs/dss/ffs_dss_operation_post_wifi_scan_data.c  \
#	../../../component/common/application/amazon/amazon-ffs/libffs/src/ffs/dss/ffs_dss_operation_report.c  \
#	../../../component/common/application/amazon/amazon-ffs/libffs/src/ffs/dss/ffs_dss_operation_start_pin_based_setup.c  \
#	../../../component/common/application/amazon/amazon-ffs/libffs/src/ffs/dss/ffs_dss_operation_start_provisioning_session.c  \
#	../../../component/common/application/amazon/amazon-ffs/ffs_linux/libffs/src/ffs/linux/ffs_linux_error_details.c  \
#	../../../component/common/application/amazon/amazon-ffs/ffs_linux/libffs/src/ffs/linux/ffs_wifi_connection_attempt_list.c  \
#	../../../component/common/application/amazon/amazon-ffs/libffs/src/ffs/wifi_provisionee/ffs_wifi_provisionee_encoded_setup_network.c  \
#	../../../component/common/application/amazon/amazon-ffs/libffs/src/ffs/wifi_provisionee/ffs_wifi_provisionee_state.c  \
#	../../../component/common/application/amazon/amazon-ffs/libffs/src/ffs/wifi_provisionee/ffs_wifi_provisionee_task.c  \
#	../../../component/common/application/amazon/amazon-ffs/ffs_demo/common/src/ffs/compat/ffs_rtk_configuration_map.c  \
#	../../../component/common/application/amazon/amazon-ffs/ffs_demo/common/src/ffs/compat/ffs_rtk_crypto.c  \
#	../../../component/common/application/amazon/amazon-ffs/ffs_demo/common/src/ffs/compat/ffs_rtk_http_client.c  \
#	../../../component/common/application/amazon/amazon-ffs/ffs_demo/common/src/ffs/compat/ffs_rtk_user_context.c  \
#	../../../component/common/application/amazon/amazon-ffs/ffs_demo/common/src/ffs/compat/ffs_rtk_wifi.c  \
#	../../../component/common/application/amazon/amazon-ffs/ffs_demo/common/src/ffs/network/ffs_rtk_dss_client.c  \
#	../../../component/common/application/amazon/amazon-ffs/ffs_demo/common/src/ffs/network/ffs_rtk_wifi_context.c  \
#	../../../component/common/application/amazon/amazon-ffs/ffs_demo/common/src/ffs/network/ffs_rtk_wifi_manager.c  \
#	../../../component/common/application/amazon/amazon-ffs/ffs_demo/common/src/ffs/network/ffs_rtk_wifi_scan.c  \
#	../../../component/common/application/amazon/amazon-ffs/ffs_demo/common/src/ffs/network/ffs_rtk_wifi_provisionee_setup_network.c  \
#	../../../component/common/application/amazon/amazon-ffs/ffs_demo/common/src/ffs/network/ffs_rtk_wifi_provisionee_user_network.c  \
#	../../../component/common/application/amazon/amazon-ffs/ffs_demo/common/src/ffs/platform/ffs_rtk_circular_buffer.c  \
#	../../../component/common/application/amazon/amazon-ffs/ffs_demo/common/src/ffs/platform/ffs_rtk_linked_list.c  \
#	../../../component/common/application/amazon/amazon-ffs/ffs_demo/common/src/ffs/platform/ffs_rtk_logging.c  \
#	../../../component/common/application/amazon/amazon-ffs/ffs_demo/realtek/application/ffs_api.c  \
#	../../../component/common/example/amazon_ffs/example_ffs.c

#utilities - example
SRC_C += ../../../component/common/example/amazon_freertos/example_amazon_freertos.c
SRC_C += ../../../component/common/example/bcast/example_bcast.c
SRC_C += ../../../component/common/example/cJSON/example_cJSON.c
SRC_C += ../../../component/common/example/coap/example_coap.c
SRC_C += ../../../component/common/example/coap_client/example_coap_client.c
SRC_C += ../../../component/common/example/coap_server/example_coap_server.c
SRC_C += ../../../component/common/example/dct/example_dct.c
SRC_C += ../../../component/common/example/eap/example_eap.c
SRC_C += ../../../component/common/example/example_entry.c
SRC_C += ../../../component/common/example/get_beacon_frame/example_get_beacon_frame.c
SRC_C += ../../../component/common/example/high_load_memory_use/example_high_load_memory_use.c
SRC_C += ../../../component/common/example/http_client/example_http_client.c
SRC_C += ../../../component/common/example/http_download/example_http_download.c
SRC_C += ../../../component/common/example/httpc/example_httpc.c
SRC_C += ../../../component/common/example/httpd/example_httpd.c
SRC_C += ../../../component/common/example/mbedtls_ecdhe/example_mbedtls_ecdhe.c
SRC_C += ../../../component/common/example/mcast/example_mcast.c
SRC_C += ../../../component/common/example/mqtt/example_mqtt.c
SRC_C += ../../../component/common/example/nonblock_connect/example_nonblock_connect.c
SRC_C += ../../../component/common/example/ota_http/example_ota_http.c
SRC_C += ../../../component/common/example/ota_https/example_ota_https.c
SRC_C += ../../../component/common/example/rarp/example_rarp.c
SRC_C += ../../../component/common/example/sntp_showtime/example_sntp_showtime.c
SRC_C += ../../../component/common/example/socket_select/example_socket_select.c
SRC_C += ../../../component/common/example/socket_tcp_trx/example_socket_tcp_trx_1.c
SRC_C += ../../../component/common/example/socket_tcp_trx/example_socket_tcp_trx_2.c
SRC_C += ../../../component/common/example/ssl_download/example_ssl_download.c
SRC_C += ../../../component/common/example/ssl_server/example_ssl_server.c
SRC_C += ../../../component/common/example/tcp_keepalive/example_tcp_keepalive.c
SRC_C += ../../../component/common/example/uart_atcmd/example_uart_atcmd.c
SRC_C += ../../../component/common/example/wifi_mac_monitor/example_wifi_mac_monitor.c
SRC_C += ../../../component/common/example/wifi_roaming/example_wifi_roaming.c
SRC_C += ../../../component/common/example/wlan_fast_connect/example_wlan_fast_connect.c
SRC_C += ../../../component/common/example/websocket_client/example_wsclient.c
SRC_C += ../../../component/common/example/websocket_server/example_ws_server.c
SRC_C += ../../../component/common/example/xml/example_xml.c
SRC_C += ../../../component/common/example/fatfs/example_fatfs.c
SRC_C += ../../../component/common/example/tickless_wifi_roaming/example_tickless_wifi_roaming.c
SRC_C += ../../../component/common/example/ipv6/example_ipv6.c

#user
SRC_C += ../src/main.c
#SRC_CPP = ../src/main.cpp

#SRAM
# -------------------------------------------------------------------
#@SRAM
SRC_C += ../../../component/common/mbed/targets/hal/rtl8710c/flash_api.c
SRC_C += ../../../component/soc/realtek/8710c/misc/driver/flash_api_ext.c
SRC_C += ../../../component/soc/realtek/8710c/fwlib/source/ram_ns/hal_flash.c
SRC_C += ../../../component/soc/realtek/8710c/fwlib/source/ram_ns/hal_spic.c
SRAM_C += ../../../component/common/mbed/targets/hal/rtl8710c/power_mode_api.c

# Generate obj list
# -------------------------------------------------------------------

SRC_O = $(patsubst %.c,%_$(TARGET).o,$(SRC_C))
SRAM_O = $(patsubst %.c,%_$(TARGET).o,$(SRAM_C))
ERAM_O = $(patsubst %.c,%_$(TARGET).o,$(ERAM_C))

SRC_C_LIST = $(notdir $(SRC_C)) $(notdir $(SRAM_C)) $(notdir $(ERAM_C))
OBJ_LIST = $(addprefix $(OBJ_DIR)/,$(patsubst %.c,%_$(TARGET).o,$(SRC_C_LIST)))
DEPENDENCY_LIST = $(addprefix $(OBJ_DIR)/,$(patsubst %.c,%_$(TARGET).d,$(SRC_C_LIST)))

SRC_OO += $(patsubst %.cpp,%_$(TARGET).oo,$(SRC_CPP))

SRC_CPP_LIST = $(notdir $(SRC_CPP))
OBJ_CPP_LIST = $(addprefix $(OBJ_DIR)/,$(patsubst %.cpp,%_$(TARGET).oo,$(SRC_CPP_LIST)))
DEPENDENCY_LIST += $(addprefix $(OBJ_DIR)/,$(patsubst %.cpp,%_$(TARGET).d,$(SRC_CPP_LIST)))
# Compile options
# -------------------------------------------------------------------

CFLAGS =
CFLAGS += -march=armv8-m.main+dsp -mthumb -mcmse -mfloat-abi=soft -D__thumb2__ -g -gdwarf-3 -Os
CFLAGS += -D__ARM_ARCH_8M_MAIN__=1 -gdwarf-3 -fstack-usage -fdata-sections -ffunction-sections 
CFLAGS += -fdiagnostics-color=always -Wall -Wpointer-arith -Wundef -Wno-write-strings --save-temps
CFLAGS += -Wno-maybe-uninitialized -c -MMD -fsigned-char
CFLAGS += -DCONFIG_PLATFORM_8710C -DCONFIG_BUILD_RAM=1
CFLAGS += -DV8M_STKOVF

CPPFLAGS := $(CFLAGS)

#for time64 
ifdef SYSTEM_TIME64_MAKE_OPTION
CFLAGS += -DCONFIG_SYSTEM_TIME64=1
CFLAGS += -include time64.h
else
CFLAGS += -DCONFIG_SYSTEM_TIME64=0
endif

CFLAGS += -Wstrict-prototypes 
CPPFLAGS += -std=c++11 -fno-use-cxa-atexit

CFLAGS += -DCONFIG_AMAZON_FREERTOS
CFLAGS += -DENABLE_AMAZON_COMMON
CFLAGS += -DMBEDTLS_CONFIG_FILE=\"aws_mbedtls_config.h\"

LFLAGS = 
LFLAGS += -Os -march=armv8-m.main+dsp -mthumb -mcmse -mfloat-abi=soft -nostartfiles -nodefaultlibs -nostdlib -specs=nosys.specs
LFLAGS += -Wl,--gc-sections -Wl,--warn-section-align -Wl,--cref -Wl,--build-id=none -Wl,--use-blx
LFLAGS += -Wl,-Map=$(BIN_DIR)/$(TARGET).map
# libc api wrapper
LFLAGS += -Wl,-wrap,strcat  -Wl,-wrap,strchr   -Wl,-wrap,strcmp
LFLAGS += -Wl,-wrap,strncmp -Wl,-wrap,strnicmp -Wl,-wrap,strcpy
LFLAGS += -Wl,-wrap,strncpy -Wl,-wrap,strlcpy  -Wl,-wrap,strlen
LFLAGS += -Wl,-wrap,strnlen -Wl,-wrap,strncat  -Wl,-wrap,strpbrk
LFLAGS += -Wl,-wrap,strspn  -Wl,-wrap,strstr   -Wl,-wrap,strtok
LFLAGS += -Wl,-wrap,strxfrm -Wl,-wrap,strsep   -Wl,-wrap,strtod
LFLAGS += -Wl,-wrap,strtof  -Wl,-wrap,strtold  -Wl,-wrap,strtoll
LFLAGS += -Wl,-wrap,strtoul -Wl,-wrap,strtoull -Wl,-wrap,atoi
LFLAGS += -Wl,-wrap,atoui   -Wl,-wrap,atol     -Wl,-wrap,atoul
LFLAGS += -Wl,-wrap,atoull  -Wl,-wrap,atof
LFLAGS += -Wl,-wrap,malloc  -Wl,-wrap,realloc
LFLAGS += -Wl,-wrap,calloc  -Wl,-wrap,free
LFLAGS += -Wl,-wrap,_malloc_r  -Wl,-wrap,_calloc_r  -Wl,-wrap,_realloc_r  -Wl,-wrap,_free_r
LFLAGS += -Wl,-wrap,memcmp  -Wl,-wrap,memcpy
LFLAGS += -Wl,-wrap,memmove -Wl,-wrap,memset
LFLAGS += -Wl,-wrap,printf  -Wl,-wrap,sprintf
LFLAGS += -Wl,-wrap,puts  -Wl,-wrap,putc -Wl,-wrap,putchar
LFLAGS += -Wl,-wrap,snprintf  -Wl,-wrap,vsnprintf
LFLAGS += -Wl,-wrap,aesccmp_construct_mic_iv
LFLAGS += -Wl,-wrap,aesccmp_construct_mic_header1
LFLAGS += -Wl,-wrap,aesccmp_construct_ctr_preload
LFLAGS += -Wl,-wrap,rom_psk_CalcGTK
LFLAGS += -Wl,-wrap,rom_psk_CalcPTK
LFLAGS += -Wl,-wrap,aes_80211_encrypt
LFLAGS += -Wl,-wrap,aes_80211_decrypt

ifdef SYSTEM_TIME64_MAKE_OPTION
LFLAGS += -Wl,-wrap,localtime -Wl,-wrap,mktime -Wl,-wrap,ctime
endif

LIBFLAGS =
LIBFLAGS += -L$(AMEBAZ2_ROMSYMDIR)
LIBFLAGS += -Wl,--start-group ../../../component/soc/realtek/8710c/fwlib/lib/lib/hal_pmc.a -Wl,--end-group
LIBFLAGS += -Wl,--start-group ../../../component/common/bluetooth/realtek/sdk/board/amebaz2/lib/GCC/btgap.a -Wl,--end-group
#LIBFLAGS += -Wl,--start-group ../../../component/common/application/jdsmart/2.1.22/joylink_ble/lib/libjoylink_ble.a -Wl,--end-group
#LIBFLAGS += -Wl,--start-group ../../../component/common/bluetooth/realtek/sdk/example/bt_mesh/lib/lib/amebaz2/btmesh_prov.a -Wl,--end-group
#LIBFLAGS += -Wl,--start-group ../../../component/common/bluetooth/realtek/sdk/example/bt_mesh/lib/lib/amebaz2/btmesh_dev.a -Wl,--end-group
all: LIBFLAGS += -Wl,--start-group -L../../../component/soc/realtek/8710c/misc/bsp/lib/common/GCC -l_soc_is -l_wlan -Wl,--end-group
mp: LIBFLAGS += -Wl,--start-group -L../../../component/soc/realtek/8710c/misc/bsp/lib/common/GCC -l_soc_is -l_wlan_mp -Wl,--end-group
LIBFLAGS += -L../../../component/soc/realtek/8710c/misc/bsp/lib/common/GCC -l_http -l_dct -l_eap -l_websocket -l_wps
#LIBFLAGS += -L../../../component/soc/realtek/8710c/misc/bsp/lib/common/GCC -l_coap
LIBFLAGS += -L../../../component/soc/realtek/8710c/misc/bsp/lib/common/GCC -l_amazon

RAMALL_BIN =
OTA_BIN = 

include toolchain.mk

# Compile
# -------------------------------------------------------------------

.PHONY: application_is
application_is: prerequirement $(SRC_O) $(SRAM_O) $(ERAM_O) $(SRC_OO)
	$(LD) $(LFLAGS) -o $(BIN_DIR)/$(TARGET).axf $(OBJ_CPP_LIST) -lstdc++ $(OBJ_LIST) $(ROMIMG) $(LIBFLAGS) -T$(LDSCRIPT)  
	$(OBJCOPY) -j .bluetooth_trace.text -Obinary $(BIN_DIR)/$(TARGET).axf $(BIN_DIR)/APP.trace
	$(OBJCOPY) -R .bluetooth_trace.text $(BIN_DIR)/$(TARGET).axf 
	$(OBJDUMP) -d $(BIN_DIR)/$(TARGET).axf > $(BIN_DIR)/$(TARGET).asm

# Manipulate Image
# -------------------------------------------------------------------

.PHONY: manipulate_images
manipulate_images: | application_is
	@echo ===========================================================
	@echo Image manipulating
	@echo ===========================================================
	cp $(AMEBAZ2_BOOTLOADERDIR)/bootloader.axf $(BOOT_BIN_DIR)/bootloader.axf
ifeq ($(findstring Linux, $(OS)), Linux)
	chmod 0774 $(ELF2BIN) $(CHKSUM)
endif
	$(ELF2BIN) keygen keycfg.json
	$(ELF2BIN) convert amebaz2_bootloader.json BOOTLOADER secure_bit=0
	$(ELF2BIN) convert amebaz2_bootloader.json PARTITIONTABLE secure_bit=0
	$(ELF2BIN) convert amebaz2_firmware_is.json FIRMWARE secure_bit=0
#ifeq ($(findstring Linux, $(OS)), Linux)
#	chmod 777 $(AMEBAZ2_GCCTOOLDIR)/LZMA_GenCompressedFW_linux
#endif
#	$(LZMA_PY) $(BIN_DIR)/firmware_is.bin

	$(CHKSUM) $(BIN_DIR)/firmware_is.bin
#	$(CHKSUM) $(BIN_DIR)/firmware_is_lzma.bin
	$(ELF2BIN) combine $(BIN_DIR)/flash_is.bin PTAB=partition.bin,BOOT=$(BOOT_BIN_DIR)/bootloader.bin,FW1=$(BIN_DIR)/firmware_is.bin

	$(CODE_ANALYZE_PYTHON)
# Generate build info
# -------------------------------------------------------------------

.PHONY: build_info
build_info:
	@echo \#define RTL_FW_COMPILE_TIME RTL8710CFW_COMPILE_TIME\ > .ver
	@echo \#define RTL_FW_COMPILE_DATE RTL8710CFW_COMPILE_DATE\ >> .ver
	@echo \#define UTS_VERSION \"`date +%Y/%m/%d-%T`\" >> .ver
	@echo \#define RTL8710CFW_COMPILE_TIME \"`date +%Y/%m/%d-%T`\" >> .ver
	@echo \#define RTL8710CFW_COMPILE_DATE \"`date +%Y%m%d`\" >> .ver
	@echo \#define RTL8710CFW_COMPILE_BY \"`id -u -n`\" >> .ver
	@echo \#define RTL8710CFW_COMPILE_HOST \"`$(HOSTNAME_APP)`\" >> .ver
	@if [ -x /bin/dnsdomainname ]; then \
		echo \#define RTL8710CFW_COMPILE_DOMAIN \"`dnsdomainname`\"; \
	elif [ -x /bin/domainname ]; then \
		echo \#define RTL8710CFW_COMPILE_DOMAIN \"`domainname`\"; \
	else \
		echo \#define RTL8710CFW_COMPILE_DOMAIN ; \
	fi >> .ver

	@echo \#define RTL8710CFW_COMPILER \"gcc `$(CC) $(CFLAGS) -dumpversion | tr --delete '\r'`\" >> .ver
	@mv -f .ver ../inc/$@.h

.PHONY: prerequirement
prerequirement:
	@if [ ! -d $(ARM_GCC_TOOLCHAIN) ]; then \
		echo ===========================================================; \
		echo Toolchain not found, \"make toolchain\" first!; \
		echo ===========================================================; \
		exit -1; \
	fi
	@echo ===========================================================
	@echo Build $(TARGET)
	@echo ===========================================================
	#$(SHELL) check_fw_version.sh
	mkdir -p $(OBJ_DIR)
	mkdir -p $(BIN_DIR)
	mkdir -p $(BOOT_BIN_DIR)
	mkdir -p $(INFO_DIR)

$(SRC_OO): %_$(TARGET).oo : %.cpp | prerequirement
	$(CC) $(CPPFLAGS) -c $< -o $@
	$(CC) $(CPPFLAGS) -c $< -MM -MT $@ -MF $(OBJ_DIR)/$(notdir $(patsubst %.oo,%.d,$@))
	cp $@ $(OBJ_DIR)/$(notdir $@)
	mv $(notdir $*.ii) $(INFO_DIR)
#	mv $(notdir $*.s) $(INFO_DIR)
	chmod 777 $(OBJ_DIR)/$(notdir $@)

$(SRC_O): %_$(TARGET).o : %.c | prerequirement
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -MM -MT $@ -MF $(OBJ_DIR)/$(notdir $(patsubst %.o,%.d,$@))
	cp $@ $(OBJ_DIR)/$(notdir $@)
	mv $(notdir $*.i) $(INFO_DIR)
	mv $(notdir $*.s) $(INFO_DIR)
	chmod 777 $(OBJ_DIR)/$(notdir $@)

$(SRAM_O): %_$(TARGET).o : %.c | prerequirement
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
	$(OBJCOPY) --prefix-alloc-sections .sram $@
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -MM -MT $@ -MF $(OBJ_DIR)/$(notdir $(patsubst %.o,%.d,$@))
	cp $@ $(OBJ_DIR)/$(notdir $@)
	mv $(notdir $*.i) $(INFO_DIR)
	mv $(notdir $*.s) $(INFO_DIR)
	chmod 777 $(OBJ_DIR)/$(notdir $@)

$(ERAM_O): %_$(TARGET).o : %.c | prerequirement
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
	$(OBJCOPY) --prefix-alloc-sections .psram $@
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -MM -MT $@ -MF $(OBJ_DIR)/$(notdir $(patsubst %.o,%.d,$@))
	cp $@ $(OBJ_DIR)/$(notdir $@)
	mv $(notdir $*.i) $(INFO_DIR)
	mv $(notdir $*.s) $(INFO_DIR)
	chmod 777 $(OBJ_DIR)/$(notdir $@)

-include $(DEPENDENCY_LIST)

# Only needed for FPGA phase
.PHONY: romdebug
romdebug:
ifeq ($(findstring CYGWIN, $(OS)), CYGWIN) 
#	$(FLASH_TOOLDIR)/Check_Jtag.sh
	cmd /c start $(GDB) -x ./rtl_gdb_debug_jlink.txt
else
	$(GDB) -x ./rtl_gdb_debug_jlink.txt	
endif

.PHONY: flash
flash:
#	@if [ ! -f $(FLASH_TOOLDIR)/rtl_gdb_flash_write.txt ] ; then echo Please do /"make setup GDB_SERVER=[jlink or openocd]/" first; echo && false ; fi
#ifeq ($(findstring CYGWIN, $(OS)), CYGWIN) 
#	$(FLASH_TOOLDIR)/Check_Jtag.sh
#endif
	@if [ ! -e $(AMEBAZ2_GCCTOOLDIR)/rtl_gdb_flashloader.txt ]; then \
		echo ===========================================================; \
		echo gdb script not found, \"make setup GDB_SERVER=[jlink, pyocd or openocd]\" first!; \
		echo ===========================================================; \
		exit -1; \
	fi
	chmod +rx $(AMEBAZ2_GCCTOOLDIR)/flashloader.sh
	$(AMEBAZ2_GCCTOOLDIR)/flashloader.sh application_is/Debug/bin/flash_is.bin
	$(GDB) -x $(AMEBAZ2_GCCTOOLDIR)/rtl_gdb_flashloader.txt

.PHONY: debug
debug:
	@if [ ! -e $(AMEBAZ2_GCCTOOLDIR)/rtl_gdb_debug.txt ]; then \
		echo ===========================================================; \
		echo gdb script not found, \"make setup GDB_SERVER=[jlink, pyocd or openocd]\" first!; \
		echo ===========================================================; \
		exit -1; \
	fi
	chmod +rx $(AMEBAZ2_GCCTOOLDIR)/debug.sh
	$(AMEBAZ2_GCCTOOLDIR)/debug.sh $(BIN_DIR)/$(TARGET).axf
	$(GDB) -x $(AMEBAZ2_GCCTOOLDIR)/rtl_gdb_debug.txt

.PHONY: dump
dump:
	chmod +rx $(AMEBAZ2_GCCTOOLDIR)/dump.sh
	$(AMEBAZ2_GCCTOOLDIR)/dump.sh $(BIN_DIR)/flash_is_dump.bin $(DUMP_START_ADDRESS) $(DUMP_END_ADDRESS)
	$(GDB) -x $(AMEBAZ2_GCCTOOLDIR)/rtl_gdb_dump_jlink.txt

.PHONY: setup
setup:
	@echo "----------------"
	@echo Setup $(GDB_SERVER)
	@echo "----------------"
ifeq ($(GDB_SERVER), pyocd)
	cp -p $(AMEBAZ2_GCCTOOLDIR)/rtl_gdb_debug_pyocd.txt $(AMEBAZ2_GCCTOOLDIR)/rtl_gdb_debug.txt
	cp -p $(AMEBAZ2_GCCTOOLDIR)/rtl_gdb_flashloader_pyocd.txt $(AMEBAZ2_GCCTOOLDIR)/rtl_gdb_flashloader.txt
else ifeq ($(GDB_SERVER), openocd)
	cp -p $(AMEBAZ2_GCCTOOLDIR)/rtl_gdb_debug_openocd.txt $(AMEBAZ2_GCCTOOLDIR)/rtl_gdb_debug.txt
	cp -p $(AMEBAZ2_GCCTOOLDIR)/rtl_gdb_flashloader_openocd.txt $(AMEBAZ2_GCCTOOLDIR)/rtl_gdb_flashloader.txt
else
	cp -p $(AMEBAZ2_GCCTOOLDIR)/rtl_gdb_debug_jlink.txt $(AMEBAZ2_GCCTOOLDIR)/rtl_gdb_debug.txt
	cp -p $(AMEBAZ2_GCCTOOLDIR)/rtl_gdb_flashloader_jlink.txt $(AMEBAZ2_GCCTOOLDIR)/rtl_gdb_flashloader.txt
endif

.PHONY: clean
clean:
	rm -rf $(TARGET)
	rm -f $(SRC_O) $(SRAM_O) $(ERAM_O) $(SRC_OO)
	rm -f $(patsubst %.o,%.d,$(SRC_O)) $(patsubst %.o,%.d,$(SRAM_O)) $(patsubst %.o,%.d,$(ERAM_O)) $(patsubst %.oo,%.d,$(SRC_OO))
	rm -f $(patsubst %.o,%.su,$(SRC_O)) $(patsubst %.o,%.su,$(SRAM_O)) $(patsubst %.o,%.su,$(ERAM_O)) $(patsubst %.oo,%.su,$(SRC_OO))
	rm -f *.i
	rm -f *.s