/** mbed Microcontroller Library
  ******************************************************************************
  * @file    external_flash_api.h
  * @author 
  * @version V1.0.0
  * @brief   This file provides following mbed EXTERNAL_FLASH API
  * @note    Only applicable to flash type of BH25D16A, which device identification is (68 40 15).
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2015, Realtek Semiconductor Corp.
  * All rights reserved.
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  ****************************************************************************** 
  */
#ifndef MBED_EXT_EXTERNAL_FLASH_API_EXT_H
#define MBED_EXT_EXTERNAL_FLASH_API_EXT_H

#ifdef __cplusplus
extern "C" {
#endif

#define CMD_WRITE_ENABLE        0x06
#define CMD_WRITE_DISABLE       0x04
#define CMD_READ_STATUS         0x05
#define CMD_READ_DEVICE_ID      0x9F
#define CMD_READ_DATA           0x03
#define CMD_FAST_READ_DATA      0x0B
#define CMD_PAGE_PROGRAM        0x02
#define CMD_FAST_PAGE_PROGRAM   0xF2
#define CMD_SECTOR_ERASE        0x20
#define CMD_BLOCK_ERASE_32K     0x52
#define CMD_BLOCK_ERASE_64K     0xD8

#define SECTOR_SIZE             4096
#define PAGE_SIZE               256

#define WIP_BIT                 (1 << 0)
#define WEL_BIT                 (1 << 1)

#define MAX_RX_SIZE             PAGE_SIZE
#define MAX_TX_SIZE             PAGE_SIZE

#define CMD_LENGTH_ONE          1           // Command length is 1 byte
#define CMD_LENGTH_TWO          2           // Command length is 2 byte
#define CMD_LENGTH_FOUR         4           // Command length is 4 byte
#define CMD_LENGTH_FIVE         5           // Command length is 5 byte

#ifdef __cplusplus
}
#endif


#endif

