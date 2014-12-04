/**
 * \file
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

#ifndef _SAM4E_USART1_INSTANCE_
#define _SAM4E_USART1_INSTANCE_

/* ========== Register definition for USART1 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
  #define REG_USART1_CR                    (0x400A4000U) /**< \brief (USART1) Control Register */
  #define REG_USART1_MR                    (0x400A4004U) /**< \brief (USART1) Mode Register */
  #define REG_USART1_IER                   (0x400A4008U) /**< \brief (USART1) Interrupt Enable Register */
  #define REG_USART1_IDR                   (0x400A400CU) /**< \brief (USART1) Interrupt Disable Register */
  #define REG_USART1_IMR                   (0x400A4010U) /**< \brief (USART1) Interrupt Mask Register */
  #define REG_USART1_CSR                   (0x400A4014U) /**< \brief (USART1) Channel Status Register */
  #define REG_USART1_RHR                   (0x400A4018U) /**< \brief (USART1) Receive Holding Register */
  #define REG_USART1_THR                   (0x400A401CU) /**< \brief (USART1) Transmit Holding Register */
  #define REG_USART1_BRGR                  (0x400A4020U) /**< \brief (USART1) Baud Rate Generator Register */
  #define REG_USART1_RTOR                  (0x400A4024U) /**< \brief (USART1) Receiver Time-out Register */
  #define REG_USART1_TTGR                  (0x400A4028U) /**< \brief (USART1) Transmitter Timeguard Register */
  #define REG_USART1_FIDI                  (0x400A4040U) /**< \brief (USART1) FI DI Ratio Register */
  #define REG_USART1_NER                   (0x400A4044U) /**< \brief (USART1) Number of Errors Register */
  #define REG_USART1_IF                    (0x400A404CU) /**< \brief (USART1) IrDA Filter Register */
  #define REG_USART1_MAN                   (0x400A4050U) /**< \brief (USART1) Manchester Configuration Register */
  #define REG_USART1_WPMR                  (0x400A40E4U) /**< \brief (USART1) Write Protection Mode Register */
  #define REG_USART1_WPSR                  (0x400A40E8U) /**< \brief (USART1) Write Protection Status Register */
  #define REG_USART1_RPR                   (0x400A4100U) /**< \brief (USART1) Receive Pointer Register */
  #define REG_USART1_RCR                   (0x400A4104U) /**< \brief (USART1) Receive Counter Register */
  #define REG_USART1_TPR                   (0x400A4108U) /**< \brief (USART1) Transmit Pointer Register */
  #define REG_USART1_TCR                   (0x400A410CU) /**< \brief (USART1) Transmit Counter Register */
  #define REG_USART1_RNPR                  (0x400A4110U) /**< \brief (USART1) Receive Next Pointer Register */
  #define REG_USART1_RNCR                  (0x400A4114U) /**< \brief (USART1) Receive Next Counter Register */
  #define REG_USART1_TNPR                  (0x400A4118U) /**< \brief (USART1) Transmit Next Pointer Register */
  #define REG_USART1_TNCR                  (0x400A411CU) /**< \brief (USART1) Transmit Next Counter Register */
  #define REG_USART1_PTCR                  (0x400A4120U) /**< \brief (USART1) Transfer Control Register */
  #define REG_USART1_PTSR                  (0x400A4124U) /**< \brief (USART1) Transfer Status Register */
#else
  #define REG_USART1_CR   (*(__O  uint32_t*)0x400A4000U) /**< \brief (USART1) Control Register */
  #define REG_USART1_MR   (*(__IO uint32_t*)0x400A4004U) /**< \brief (USART1) Mode Register */
  #define REG_USART1_IER  (*(__O  uint32_t*)0x400A4008U) /**< \brief (USART1) Interrupt Enable Register */
  #define REG_USART1_IDR  (*(__O  uint32_t*)0x400A400CU) /**< \brief (USART1) Interrupt Disable Register */
  #define REG_USART1_IMR  (*(__I  uint32_t*)0x400A4010U) /**< \brief (USART1) Interrupt Mask Register */
  #define REG_USART1_CSR  (*(__I  uint32_t*)0x400A4014U) /**< \brief (USART1) Channel Status Register */
  #define REG_USART1_RHR  (*(__I  uint32_t*)0x400A4018U) /**< \brief (USART1) Receive Holding Register */
  #define REG_USART1_THR  (*(__O  uint32_t*)0x400A401CU) /**< \brief (USART1) Transmit Holding Register */
  #define REG_USART1_BRGR (*(__IO uint32_t*)0x400A4020U) /**< \brief (USART1) Baud Rate Generator Register */
  #define REG_USART1_RTOR (*(__IO uint32_t*)0x400A4024U) /**< \brief (USART1) Receiver Time-out Register */
  #define REG_USART1_TTGR (*(__IO uint32_t*)0x400A4028U) /**< \brief (USART1) Transmitter Timeguard Register */
  #define REG_USART1_FIDI (*(__IO uint32_t*)0x400A4040U) /**< \brief (USART1) FI DI Ratio Register */
  #define REG_USART1_NER  (*(__I  uint32_t*)0x400A4044U) /**< \brief (USART1) Number of Errors Register */
  #define REG_USART1_IF   (*(__IO uint32_t*)0x400A404CU) /**< \brief (USART1) IrDA Filter Register */
  #define REG_USART1_MAN  (*(__IO uint32_t*)0x400A4050U) /**< \brief (USART1) Manchester Configuration Register */
  #define REG_USART1_WPMR (*(__IO uint32_t*)0x400A40E4U) /**< \brief (USART1) Write Protection Mode Register */
  #define REG_USART1_WPSR (*(__I  uint32_t*)0x400A40E8U) /**< \brief (USART1) Write Protection Status Register */
  #define REG_USART1_RPR  (*(__IO uint32_t*)0x400A4100U) /**< \brief (USART1) Receive Pointer Register */
  #define REG_USART1_RCR  (*(__IO uint32_t*)0x400A4104U) /**< \brief (USART1) Receive Counter Register */
  #define REG_USART1_TPR  (*(__IO uint32_t*)0x400A4108U) /**< \brief (USART1) Transmit Pointer Register */
  #define REG_USART1_TCR  (*(__IO uint32_t*)0x400A410CU) /**< \brief (USART1) Transmit Counter Register */
  #define REG_USART1_RNPR (*(__IO uint32_t*)0x400A4110U) /**< \brief (USART1) Receive Next Pointer Register */
  #define REG_USART1_RNCR (*(__IO uint32_t*)0x400A4114U) /**< \brief (USART1) Receive Next Counter Register */
  #define REG_USART1_TNPR (*(__IO uint32_t*)0x400A4118U) /**< \brief (USART1) Transmit Next Pointer Register */
  #define REG_USART1_TNCR (*(__IO uint32_t*)0x400A411CU) /**< \brief (USART1) Transmit Next Counter Register */
  #define REG_USART1_PTCR (*(__O  uint32_t*)0x400A4120U) /**< \brief (USART1) Transfer Control Register */
  #define REG_USART1_PTSR (*(__I  uint32_t*)0x400A4124U) /**< \brief (USART1) Transfer Status Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAM4E_USART1_INSTANCE_ */
