/**
 * \file
 *
 * \brief SAM C2x System Interrupt Driver
 *
 * Copyright (C) 2015 Atmel Corporation. All rights reserved.
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

#ifndef SYSTEM_INTERRUPT_FEATURES_H_INCLUDED
#define SYSTEM_INTERRUPT_FEATURES_H_INCLUDED

#if !defined(__DOXYGEN__)

/* Generates a interrupt vector table enum list entry for a given module type
   and index (e.g. "SYSTEM_INTERRUPT_MODULE_TC0 = TC0_IRQn,"). */
#  define _MODULE_IRQn(n, module) \
		SYSTEM_INTERRUPT_MODULE_##module##n = module##n##_IRQn,

/* Generates interrupt vector table enum list entries for all instances of a
   given module type on the selected device. */
#  define _SYSTEM_INTERRUPT_MODULES(name) \
		MREPEAT(name##_INST_NUM, _MODULE_IRQn, name)

#  define _SYSTEM_INTERRUPT_IPSR_MASK              0x0000003f
#  define _SYSTEM_INTERRUPT_PRIORITY_MASK          0x00000003

#  define _SYSTEM_INTERRUPT_EXTERNAL_VECTOR_START  0

#  define _SYSTEM_INTERRUPT_SYSTICK_PRI_POS        30
#endif

/**
 * \addtogroup asfdoc_sam0_system_interrupt_group
 * @{
 */

/**
 * \brief Table of possible system interrupt/exception vector numbers.
 *
 * Table of all possible interrupt and exception vector indexes within the
 * SAMC20/C21 device.
 */
#if defined(__DOXYGEN__)
/** \note The actual enumeration name is "system_interrupt_vector". */
enum system_interrupt_vector_samc21 {
#else
enum system_interrupt_vector {
#endif
	/** Interrupt vector index for a NMI interrupt. */
	SYSTEM_INTERRUPT_NON_MASKABLE      = NonMaskableInt_IRQn,
	/** Interrupt vector index for a Hard Fault memory access exception. */
	SYSTEM_INTERRUPT_HARD_FAULT        = HardFault_IRQn,
	/** Interrupt vector index for a Supervisor Call exception. */
	SYSTEM_INTERRUPT_SV_CALL           = SVCall_IRQn,
	/** Interrupt vector index for a Pending Supervisor interrupt. */
	SYSTEM_INTERRUPT_PENDING_SV        = PendSV_IRQn,
	/** Interrupt vector index for a System Tick interrupt. */
	SYSTEM_INTERRUPT_SYSTICK           = SysTick_IRQn,

	/** Interrupt vector index for MCLK, OSCCTRL, OSC32KCTRL, PAC, PM, SUPC, TAL peripheral interrupt. */
	SYSTEM_INTERRUPT_MODULE_SYSTEM     = SYSTEM_IRQn,
	/** Interrupt vector index for a Watch Dog peripheral interrupt. */
	SYSTEM_INTERRUPT_MODULE_WDT        = WDT_IRQn,
	/** Interrupt vector index for a Real Time Clock peripheral interrupt. */
	SYSTEM_INTERRUPT_MODULE_RTC        = RTC_IRQn,
	/** Interrupt vector index for an External Interrupt peripheral interrupt. */
	SYSTEM_INTERRUPT_MODULE_EIC        = EIC_IRQn,
	/** Interrupt vector index for Frequency Meter peripheral interrupt. */
	SYSTEM_INTERRUPT_MODULE_FREQM      = FREQM_IRQn,
#ifdef ID_TSENS
	/** Interrupt vector index for Temperature Sensor peripheral interrupt. */
	SYSTEM_INTERRUPT_MODULE_TSENS      = TSENS_IRQn,
#endif
	/** Interrupt vector index for a Non Volatile Memory Controller interrupt. */
	SYSTEM_INTERRUPT_MODULE_NVMCTRL    = NVMCTRL_IRQn,
	/** Interrupt vector index for a Direct Memory Access interrupt. */
	SYSTEM_INTERRUPT_MODULE_DMA        = DMAC_IRQn,
	/** Interrupt vector index for an Event System interrupt. */
	SYSTEM_INTERRUPT_MODULE_EVSYS      = EVSYS_IRQn,
#if defined(__DOXYGEN__)
	/** Interrupt vector index for a SERCOM peripheral interrupt.
	 *
	 *  Each specific device may contain several SERCOM peripherals; each module
	 *  instance will have its own entry in the table, with the instance number
	 *  substituted for "n" in the entry name (e.g.
	 *  \c SYSTEM_INTERRUPT_MODULE_SERCOM0).
	 */
	SYSTEM_INTERRUPT_MODULE_SERCOMn    = SERCOMn_IRQn,

	/** Interrupt vector index for a Timer/Counter Control peripheral interrupt.
	 *
	 *  Each specific device may contain several TCC peripherals; each module
	 *  instance will have its own entry in the table, with the instance number
	 *  substituted for "n" in the entry name (e.g.
	 *  \c SYSTEM_INTERRUPT_MODULE_TCC0).
	 */
	SYSTEM_INTERRUPT_MODULE_TCCn       = TCCn_IRQn,

	/** Interrupt vector index for a Timer/Counter peripheral interrupt.
	 *
	 *  Each specific device may contain several TC peripherals; each module
	 *  instance will have its own entry in the table, with the instance number
	 *  substituted for "n" in the entry name (e.g.
	 *  \c SYSTEM_INTERRUPT_MODULE_TC3).
	 */
	SYSTEM_INTERRUPT_MODULE_TCn        = TCn_IRQn,
#else
	SYSTEM_INTERRUPT_MODULE_SERCOM0    = SERCOM0_IRQn,
	SYSTEM_INTERRUPT_MODULE_SERCOM1    = SERCOM1_IRQn,
	SYSTEM_INTERRUPT_MODULE_SERCOM2    = SERCOM2_IRQn,
	SYSTEM_INTERRUPT_MODULE_SERCOM3    = SERCOM3_IRQn,
#ifdef ID_SERCOM4
	SYSTEM_INTERRUPT_MODULE_SERCOM4    = SERCOM4_IRQn,
#endif
#ifdef ID_SERCOM5
	SYSTEM_INTERRUPT_MODULE_SERCOM5    = SERCOM5_IRQn,
#endif
	_SYSTEM_INTERRUPT_MODULES(TCC)
	SYSTEM_INTERRUPT_MODULE_TC0        = TC0_IRQn,
	SYSTEM_INTERRUPT_MODULE_TC1        = TC1_IRQn,
#ifdef ID_TC2
	SYSTEM_INTERRUPT_MODULE_TC2        = TC2_IRQn,
#endif
#ifdef ID_TC3
	SYSTEM_INTERRUPT_MODULE_TC3        = TC3_IRQn,
#endif

	SYSTEM_INTERRUPT_MODULE_TC4        = TC4_IRQn,
#endif

#ifdef ID_CAN0
	/** Interrupt vector index for Control Area Network 0 interrupt. */
	SYSTEM_INTERRUPT_MODULE_CAN0       = CAN0_IRQn,
#endif
#ifdef ID_CAN1
	/** Interrupt vector index for Control Area Network 1 interrupt. */
	SYSTEM_INTERRUPT_MODULE_CAN1       = CAN1_IRQn,
#endif
#ifdef ID_AC
	/** Interrupt vector index for an Analog Comparator peripheral interrupt. */
	SYSTEM_INTERRUPT_MODULE_AC         = AC_IRQn,
#endif
#ifdef ID_ADC0
	/** Interrupt vector index for an Analog-to-Digital 0 peripheral interrupt. */
	SYSTEM_INTERRUPT_MODULE_ADC0       = ADC0_IRQn,
#endif
#ifdef ID_ADC1
	/** Interrupt vector index for an Analog-to-Digital 1 peripheral interrupt. */
	SYSTEM_INTERRUPT_MODULE_ADC1       = ADC1_IRQn,
#endif
#ifdef ID_DAC
	/** Interrupt vector index for a Digital-to-Analog peripheral interrupt. */
	SYSTEM_INTERRUPT_MODULE_DAC        = DAC_IRQn,
#endif
#ifdef ID_SDADC
	/** Interrupt vector index for a Sigma-Delta Analog Digital Converter peripheral interrupt. */
	SYSTEM_INTERRUPT_MODULE_SDADC      = SDADC_IRQn,
#endif
#ifdef ID_PTC
	/** Interrupt vector index for a Peripheral Touch Controller peripheral
	 *  interrupt. */
	SYSTEM_INTERRUPT_MODULE_PTC        = PTC_IRQn,
#endif
};

/** @} */

#endif
