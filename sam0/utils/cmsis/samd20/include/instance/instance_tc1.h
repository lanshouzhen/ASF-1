/**
 * \file
 *
 * Copyright (c) 2012 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#ifndef _SAMD20_TC1_INSTANCE_
#define _SAMD20_TC1_INSTANCE_

/* ========== Register definition for TC1 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_TC1_CTRLA              (0x42002400U) /**< \brief (TC1) Control A Register */
#define REG_TC1_READREQ            (0x42002402U) /**< \brief (TC1) Read Request Register */
#define REG_TC1_CTRLBCLR           (0x42002404U) /**< \brief (TC1) Control B Clear Register */
#define REG_TC1_CTRLBSET           (0x42002405U) /**< \brief (TC1) Control B Set Register */
#define REG_TC1_CTRLC              (0x42002406U) /**< \brief (TC1) Control C Register */
#define REG_TC1_DEBUG              (0x42002408U) /**< \brief (TC1) Debug Register */
#define REG_TC1_EVCTRL             (0x4200240AU) /**< \brief (TC1) Event Control Register */
#define REG_TC1_INTENCLR           (0x4200240CU) /**< \brief (TC1) Interrupt Enable Clear Register */
#define REG_TC1_INTENSET           (0x4200240DU) /**< \brief (TC1) Interrupt Enable Set Register */
#define REG_TC1_INTFLAG            (0x4200240EU) /**< \brief (TC1) Interrupt Flag Status and Clear Register */
#define REG_TC1_STATUS             (0x4200240FU) /**< \brief (TC1) Status Register */
#define REG_TC1_COUNT16_COUNT      (0x42002410U) /**< \brief (TC1) Count16 COUNT16 Count Register */
#define REG_TC1_COUNT32_COUNT      (0x42002410U) /**< \brief (TC1) Count32 COUNT32 Count Register */
#define REG_TC1_COUNT8_COUNT       (0x42002410U) /**< \brief (TC1) Count8 COUNT8 Count Register */
#define REG_TC1_COUNT8_PER         (0x42002414U) /**< \brief (TC1) Count8 COUNT8 Period Register */
#define REG_TC1_COUNT16_CC         (0x42002418U) /**< \brief (TC1) Count16 COUNT16 Compare and Capture Register */
#define REG_TC1_COUNT32_CC         (0x42002418U) /**< \brief (TC1) Count32 COUNT32 Compare and Capture Register */
#define REG_TC1_COUNT8_CC          (0x42002418U) /**< \brief (TC1) Count8 COUNT8 Compare and Capture Register */
#else
#define REG_TC1_CTRLA              (*(RwReg16*)0x42002400U) /**< \brief (TC1) Control A Register */
#define REG_TC1_READREQ            (*(RwReg16*)0x42002402U) /**< \brief (TC1) Read Request Register */
#define REG_TC1_CTRLBCLR           (*(RwReg8 *)0x42002404U) /**< \brief (TC1) Control B Clear Register */
#define REG_TC1_CTRLBSET           (*(RwReg8 *)0x42002405U) /**< \brief (TC1) Control B Set Register */
#define REG_TC1_CTRLC              (*(RwReg8 *)0x42002406U) /**< \brief (TC1) Control C Register */
#define REG_TC1_DEBUG              (*(RwReg8 *)0x42002408U) /**< \brief (TC1) Debug Register */
#define REG_TC1_EVCTRL             (*(RwReg16*)0x4200240AU) /**< \brief (TC1) Event Control Register */
#define REG_TC1_INTENCLR           (*(RwReg8 *)0x4200240CU) /**< \brief (TC1) Interrupt Enable Clear Register */
#define REG_TC1_INTENSET           (*(RwReg8 *)0x4200240DU) /**< \brief (TC1) Interrupt Enable Set Register */
#define REG_TC1_INTFLAG            (*(RwReg8 *)0x4200240EU) /**< \brief (TC1) Interrupt Flag Status and Clear Register */
#define REG_TC1_STATUS             (*(RoReg8 *)0x4200240FU) /**< \brief (TC1) Status Register */
#define REG_TC1_COUNT              (*(RwReg16*)0x42002410U) /**< \brief (TC1) COUNT16 Count Register */
#define REG_TC1_COUNT              (*(RwReg  *)0x42002410U) /**< \brief (TC1) COUNT32 Count Register */
#define REG_TC1_COUNT              (*(RwReg8 *)0x42002410U) /**< \brief (TC1) COUNT8 Count Register */
#define REG_TC1_PER                (*(RwReg8 *)0x42002414U) /**< \brief (TC1) COUNT8 Period Register */
#define REG_TC1_CC                 (*(RwReg16*)0x42002418U) /**< \brief (TC1) COUNT16 Compare and Capture Register */
#define REG_TC1_CC                 (*(RwReg  *)0x42002418U) /**< \brief (TC1) COUNT32 Compare and Capture Register */
#define REG_TC1_CC                 (*(RwReg8 *)0x42002418U) /**< \brief (TC1) COUNT8 Compare and Capture Register */
#endif /* (defined(__ASSEMBLY__) || defines(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for TC1 peripheral ========== */
#define TC1_CC8_NUM                 2
#define TC1_CC16_NUM                2
#define TC1_CC32_NUM                2
#define TC1_GCLK_ID                 19
#define TC1_OW_NUM                  2

#endif /* _SAMD20_TC1_INSTANCE_ */
