/**
 * \file
 *
 * \brief Instance description for TCC1
 *
 * Copyright (c) 2014 Atmel Corporation. All rights reserved.
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

#ifndef _SAMC21_TCC1_INSTANCE_
#define _SAMC21_TCC1_INSTANCE_

/* ========== Register definition for TCC1 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_TCC1_CTRLA             (0x42002800U) /**< \brief (TCC1) Control A */
#define REG_TCC1_CTRLBCLR          (0x42002804U) /**< \brief (TCC1) Control B Clear */
#define REG_TCC1_CTRLBSET          (0x42002805U) /**< \brief (TCC1) Control B Set */
#define REG_TCC1_SYNCBUSY          (0x42002808U) /**< \brief (TCC1) Synchronization Busy */
#define REG_TCC1_FCTRLA            (0x4200280CU) /**< \brief (TCC1) Recoverable FaultA Configuration */
#define REG_TCC1_FCTRLB            (0x42002810U) /**< \brief (TCC1) Recoverable FaultB Configuration */
#define REG_TCC1_DRVCTRL           (0x42002818U) /**< \brief (TCC1) Driver Configuration */
#define REG_TCC1_DBGCTRL           (0x4200281EU) /**< \brief (TCC1) Debug Control */
#define REG_TCC1_EVCTRL            (0x42002820U) /**< \brief (TCC1) Event Control */
#define REG_TCC1_INTENCLR          (0x42002824U) /**< \brief (TCC1) Interrupt Enable Clear */
#define REG_TCC1_INTENSET          (0x42002828U) /**< \brief (TCC1) Interrupt Enable Set */
#define REG_TCC1_INTFLAG           (0x4200282CU) /**< \brief (TCC1) Interrupt Flag Status and Clear */
#define REG_TCC1_STATUS            (0x42002830U) /**< \brief (TCC1) Status */
#define REG_TCC1_COUNT             (0x42002834U) /**< \brief (TCC1) Count */
#define REG_TCC1_PATT              (0x42002838U) /**< \brief (TCC1) Pattern */
#define REG_TCC1_WAVE              (0x4200283CU) /**< \brief (TCC1) Waveform Control */
#define REG_TCC1_PER               (0x42002840U) /**< \brief (TCC1) Period */
#define REG_TCC1_CC0               (0x42002844U) /**< \brief (TCC1) Compare and Capture 0 */
#define REG_TCC1_CC1               (0x42002848U) /**< \brief (TCC1) Compare and Capture 1 */
#define REG_TCC1_PATTBUF           (0x42002864U) /**< \brief (TCC1) Pattern Buffer */
#define REG_TCC1_WAVEBUF           (0x42002868U) /**< \brief (TCC1) Waveform Control Buffer */
#define REG_TCC1_PERBUF            (0x4200286CU) /**< \brief (TCC1) Period Buffer */
#define REG_TCC1_CCBUF0            (0x42002870U) /**< \brief (TCC1) Compare and Capture Buffer 0 */
#define REG_TCC1_CCBUF1            (0x42002874U) /**< \brief (TCC1) Compare and Capture Buffer 1 */
#else
#define REG_TCC1_CTRLA             (*(RwReg  *)0x42002800U) /**< \brief (TCC1) Control A */
#define REG_TCC1_CTRLBCLR          (*(RwReg8 *)0x42002804U) /**< \brief (TCC1) Control B Clear */
#define REG_TCC1_CTRLBSET          (*(RwReg8 *)0x42002805U) /**< \brief (TCC1) Control B Set */
#define REG_TCC1_SYNCBUSY          (*(RoReg  *)0x42002808U) /**< \brief (TCC1) Synchronization Busy */
#define REG_TCC1_FCTRLA            (*(RwReg  *)0x4200280CU) /**< \brief (TCC1) Recoverable FaultA Configuration */
#define REG_TCC1_FCTRLB            (*(RwReg  *)0x42002810U) /**< \brief (TCC1) Recoverable FaultB Configuration */
#define REG_TCC1_DRVCTRL           (*(RwReg  *)0x42002818U) /**< \brief (TCC1) Driver Configuration */
#define REG_TCC1_DBGCTRL           (*(RwReg8 *)0x4200281EU) /**< \brief (TCC1) Debug Control */
#define REG_TCC1_EVCTRL            (*(RwReg  *)0x42002820U) /**< \brief (TCC1) Event Control */
#define REG_TCC1_INTENCLR          (*(RwReg  *)0x42002824U) /**< \brief (TCC1) Interrupt Enable Clear */
#define REG_TCC1_INTENSET          (*(RwReg  *)0x42002828U) /**< \brief (TCC1) Interrupt Enable Set */
#define REG_TCC1_INTFLAG           (*(RwReg  *)0x4200282CU) /**< \brief (TCC1) Interrupt Flag Status and Clear */
#define REG_TCC1_STATUS            (*(RwReg  *)0x42002830U) /**< \brief (TCC1) Status */
#define REG_TCC1_COUNT             (*(RwReg  *)0x42002834U) /**< \brief (TCC1) Count */
#define REG_TCC1_PATT              (*(RwReg16*)0x42002838U) /**< \brief (TCC1) Pattern */
#define REG_TCC1_WAVE              (*(RwReg  *)0x4200283CU) /**< \brief (TCC1) Waveform Control */
#define REG_TCC1_PER               (*(RwReg  *)0x42002840U) /**< \brief (TCC1) Period */
#define REG_TCC1_CC0               (*(RwReg  *)0x42002844U) /**< \brief (TCC1) Compare and Capture 0 */
#define REG_TCC1_CC1               (*(RwReg  *)0x42002848U) /**< \brief (TCC1) Compare and Capture 1 */
#define REG_TCC1_PATTBUF           (*(RwReg16*)0x42002864U) /**< \brief (TCC1) Pattern Buffer */
#define REG_TCC1_WAVEBUF           (*(RwReg  *)0x42002868U) /**< \brief (TCC1) Waveform Control Buffer */
#define REG_TCC1_PERBUF            (*(RwReg  *)0x4200286CU) /**< \brief (TCC1) Period Buffer */
#define REG_TCC1_CCBUF0            (*(RwReg  *)0x42002870U) /**< \brief (TCC1) Compare and Capture Buffer 0 */
#define REG_TCC1_CCBUF1            (*(RwReg  *)0x42002874U) /**< \brief (TCC1) Compare and Capture Buffer 1 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for TCC1 peripheral ========== */
#define TCC1_CC_NUM                 2        // Number of Compare/Capture units
#define TCC1_DITHERING              1        // Dithering feature implemented
#define TCC1_DMAC_ID_MC_0           22
#define TCC1_DMAC_ID_MC_1           23
#define TCC1_DMAC_ID_MC_LSB         22
#define TCC1_DMAC_ID_MC_MSB         23
#define TCC1_DMAC_ID_MC_SIZE        2
#define TCC1_DMAC_ID_OVF            21       // DMA overflow/underflow/retrigger trigger
#define TCC1_DTI                    0        // Dead-Time-Insertion feature implemented
#define TCC1_EXT                    24       // Coding of implemented extended features
#define TCC1_GCLK_ID                28       // Index of Generic Clock
#define TCC1_OTMX                   0        // Output Matrix feature implemented
#define TCC1_OW_NUM                 4        // Number of Output Waveforms
#define TCC1_PG                     1        // Pattern Generation feature implemented
#define TCC1_SIZE                   24      
#define TCC1_SWAP                   0        // DTI outputs swap feature implemented
#define TCC1_TYPE                   2        // TCC type 0 : NA, 1 : Master, 2 : Slave

#endif /* _SAMC21_TCC1_INSTANCE_ */
