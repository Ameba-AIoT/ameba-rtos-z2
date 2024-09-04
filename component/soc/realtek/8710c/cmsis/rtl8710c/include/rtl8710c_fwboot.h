/**************************************************************************//**
 * @file     rtl8710c_fwboot.h
 * @brief    The data type definition related load firmware image info which can be used for RAM code .
 * @version  V1.00
 * @date     2020-07-23
 *
 * @note
 *
 ******************************************************************************
 *
 * Copyright(c) 2007 - 2016 Realtek Corporation. All rights reserved.
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
 *
 ******************************************************************************/

#ifndef _RTL8710C_FWBOOT_H_
#define _RTL8710C_FWBOOT_H_

#ifdef  __cplusplus
 extern "C"
 {
#endif

typedef struct load_fw_fail_info_s {
    uint32_t fw1_sn;                // the FW1 SN
    uint32_t fw2_sn;                // the FW2 SN
    uint8_t fw1_valid;              // is FW1 valid (OTA signature valid)
    uint8_t fw2_valid;              // is FW2 valid (OTA signature valid)
    uint8_t loaded_fw_idx;          // select loaded fw index
    uint8_t reserved;               // select loaded fw index
} load_fw_fail_info_t;

#ifdef  __cplusplus
}
#endif

#endif  // end of "#define _RTL8710C_FWBOOT_H_"
