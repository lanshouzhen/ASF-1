/**
 * \file
 *
 * \brief SAMD20 System Interrupt Driver
 *
 * Copyright (C) 2013 Atmel Corporation. All rights reserved.
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
#ifndef SYSTEM_INTERRUPT_H_INCLUDED
#define SYSTEM_INTERRUPT_H_INCLUDED

/**
 * \defgroup asfdoc_samd20_system_interrupt_group SAMD20 System Interrupt Driver
 *
 * This driver for SAMD20 devices provides an interface for the configuration
 * and management of internal interrupts.
 *
 * The following peripherals are used by this module:
 *
 *  - NVIC (Nested Vector Interrupt Controller)
 *
 * The outline of this documentation is as follows:
 *  - \ref asfdoc_samd20_system_interrupt_prerequisites
 *  - \ref asfdoc_samd20_system_interrupt_module_overview
 *  - \ref asfdoc_samd20_system_interrupt_special_considerations
 *  - \ref asfdoc_samd20_system_interrupt_extra_info
 *  - \ref asfdoc_samd20_system_interrupt_examples
 *  - \ref asfdoc_samd20_system_interrupt_api_overview
 *
 *
 * \section asfdoc_samd20_system_interrupt_prerequisites Prerequisites
 *
 * There are no prerequisites for this module.
 *
 *
 * \section asfdoc_samd20_system_interrupt_module_overview Module Overview
 *
 * TODO
 *
 * \section asfdoc_samd20_system_interrupt_special_considerations Special Considerations
 *
 * TODO
 *
 *
 * \section asfdoc_samd20_system_interrupt_extra_info Extra Information for System Interrupt
 *
 * For extra information see \ref asfdoc_samd20_system_interrupt_extra. This includes:
 *  - \ref asfdoc_samd20_system_interrupt_extra_acronyms
 *  - \ref asfdoc_samd20_system_interrupt_extra_dependencies
 *  - \ref asfdoc_samd20_system_interrupt_extra_errata
 *  - \ref asfdoc_samd20_system_interrupt_extra_history
 *
 *
 * \section asfdoc_samd20_system_interrupt_examples Examples
 *
 * The following Quick Start guides and application examples are available for this driver:
 * - \ref asfdoc_samd20_system_interrupt_basic_use_case
 *
 *
 * \section asfdoc_samd20_system_interrupt_api_overview API Overview
 * @{
 */

#include <compiler.h>
#include <core_cm0plus.h>

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
#  define _SYSTEM_INTERRUPT_PRIORITY_MASK          0x00000007

#  define _SYSTEM_INTERRUPT_EXTERNAL_VECTOR_START  0

#  define _SYSTEM_INTERRUPT_SYSTICK_PRI_POS        29
#endif

/**
 * \brief Table of possible system interrupt/exception vector numbers.
 *
 * Table of all possible interrupt and exception vector indexes within the
 * device.
 */
enum system_interrupt_vector {
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

	/** Interrupt vector index for a Power Manager peripheral interrupt. */
	SYSTEM_INTERRUPT_MODULE_PM         = PM_IRQn,
	/** Interrupt vector index for a System Control peripheral interrupt. */
	SYSTEM_INTERRUPT_MODULE_SYSCTRL    = SYSCTRL_IRQn,
	/** Interrupt vector index for a Watch Dog peripheral interrupt. */
	SYSTEM_INTERRUPT_MODULE_WDT        = WDT_IRQn,
	/** Interrupt vector index for a Real Time Clock peripheral interrupt. */
	SYSTEM_INTERRUPT_MODULE_RTC        = RTC_IRQn,
	/** Interrupt vector index for an External Interrupt peripheral interrupt. */
	SYSTEM_INTERRUPT_MODULE_EIC        = EIC_IRQn,
	/** Interrupt vector index for a Non Volatile Memory Controller interrupt. */
	SYSTEM_INTERRUPT_MODULE_NVMCTRL    = NVMCTRL_IRQn,
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
	/** Interrupt vector index for a Timer/Counter peripheral interrupt.
	 *
	 *  Each specific device may contain several TC peripherals; each module
	 *  instance will have its own entry in the table, with the instance number
	 *  substituted for "n" in the entry name (e.g.
	 *  \c SYSTEM_INTERRUPT_MODULE_TC0).
	 */
	SYSTEM_INTERRUPT_MODULE_TCn        = TCn_IRQn,
#else
	_SYSTEM_INTERRUPT_MODULES(SERCOM)
	_SYSTEM_INTERRUPT_MODULES(TC)
#endif
	/** Interrupt vector index for an Analog Comparator peripheral interrupt. */
	SYSTEM_INTERRUPT_MODULE_AC         = AC_IRQn,
	/** Interrupt vector index for an Analog-to-Digital peripheral interrupt. */
	SYSTEM_INTERRUPT_MODULE_ADC        = ADC_IRQn,
	/** Interrupt vector index for a Digital-to-Analog peripheral interrupt. */
	SYSTEM_INTERRUPT_MODULE_DAC        = DAC_IRQn,
};

/**
 * \brief Table of possible system interrupt/exception vector priorities.
 *
 * Table of all possible interrupt and exception vector priorities within the
 * device.
 */
enum system_interrupt_priority_level {
	SYSTEM_INTERRUPT_PRIORITY_LEVEL_0  = 0,
	SYSTEM_INTERRUPT_PRIORITY_LEVEL_1  = 1,
	SYSTEM_INTERRUPT_PRIORITY_LEVEL_2  = 2,
	SYSTEM_INTERRUPT_PRIORITY_LEVEL_3  = 3,
};

/**
 * \brief Enters a critical section
 *
 * Disables global interrupts. To support nested critical sections, an internal
 * count of the critical section nesting will be kept, so that global interrupts
 * are only re-enabled upon leaving the outermost nested critical section.
 *
 */
static inline void system_interrupt_enter_critical_section(void)
{
	cpu_irq_enter_critical();
}

/**
 * \brief Leaves a critical section
 *
 * Enables global interrupts. To support nested critical sections, an internal
 * count of the critical section nesting will be kept, so that global interrupts
 * are only re-enabled upon leaving the outermost nested critical section.
 *
 */
static inline void system_interrupt_leave_critical_section(void)
{
	cpu_irq_leave_critical();
}

/**
 * \brief Check if global interrupts are enabled
 *
 * Checks if global interrupts are currently enabled.
 *
 * \returns A boolean that identifies if the global interrupts are enabled or not.
 *
 * \retval true   Global interrupts are currently enabled
 * \retval false  Global interrupts are currently disabled
 *
 */
static inline bool system_interrupt_is_global_enabled(void)
{
	return cpu_irq_is_enabled();
}

/**
 * \brief Checks if an interrupt vector is enabled or not
 *
 * Checks if a specific interrupt vector is currently enabled.
 *
 * \param[in] vector  Interrupt vector number to check
 *
 * \returns A variable identifying if the requested interrupt vector is enabled
 *
 * \retval true   Specified interrupt vector is currently enabled
 * \retval false  Specified interrupt vector is currently disabled
 *
 */
static inline bool system_interrupt_is_enabled(
		const enum system_interrupt_vector vector)
{
	return (bool)((NVIC->ISER[0] >> (uint32_t)vector) & 0x00000001);
}

/**
 * \brief Enable interrupt vector
 *
 * Enables execution of the software handler for the requested interrupt vector.
 *
 * \param[in] vector Interrupt vector to enable
 */
static inline void system_interrupt_enable(
		const enum system_interrupt_vector vector)
{
	NVIC->ISER[0] = (uint32_t)(1 << ((uint32_t)vector & 0x0000001f));
}

/**
 * \brief Disable interrupt vector
 *
 * Disables execution of the software handler for the requested interrupt vector.
 *
 * \param[in] vector  Interrupt vector to disable
 */
static inline void system_interrupt_disable(
		const enum system_interrupt_vector vector)
{
	NVIC->ICER[0] = (uint32_t)(1 << ((uint32_t)vector & 0x0000001f));
}

/**
 * \brief Get active interrupt (if any)
 *
 * Return the vector number for the current executing software handler, if any.
 *
 * \return Interrupt number that is currently executing.
 */
static inline enum system_interrupt_vector system_interrupt_get_active(void)
{
	uint32_t IPSR = __get_IPSR();

	return (enum system_interrupt_vector)(IPSR & _SYSTEM_INTERRUPT_IPSR_MASK);
}

/**
 * \brief Check if a interrupt line is pending
 *
 * Checks if the requested interrupt vector is pending.
 *
 * \param[in] vector  Interrupt vector number to check
 *
 * \returns A boolean identifying if the requested interrupt vector is pending.
 *
 * \retval true   Specified interrupt vector is pending
 * \retval false  Specified interrupt vector is not pending
 *
 */
bool system_interrupt_is_pending(
		const enum system_interrupt_vector vector);

/**
 * \brief Set a interrupt vector as pending
 *
 * Set the requested interrupt vector as pending (i.e issues a software
 * interrupt request for the specified vector). The software handler will be
 * handled (if enabled) in a priority order based on vector number and
 * configured priority settings.
 *
 * \param[in] vector  Interrupt vector number which is set as pending
 *
 * \returns Status code identifying if the vector was successfully set as
 *          pending.
 *
 * \retval STATUS_OK           If no error was detected
 * \retval STATUS_INVALID_ARG  If an unsupported interrupt vector number was given
 */
enum status_code system_interrupt_set_pending(
		const enum system_interrupt_vector vector);

/**
 * \brief Clear pending interrupt vector
 *
 * Clear a pending interrupt vector, so the software handler is not executed.
 *
 * \param[in] vector  Interrupt vector number to clear
 *
 * \returns A status code identifying if the interrupt pending state was
 *          successfully cleared.
 *
 * \retval STATUS_OK           If no error was detected
 * \retval STATUS_INVALID_ARG  If an unsupported interrupt vector number was given
 */
enum status_code system_interrupt_clear_pending(
		const enum system_interrupt_vector vector);

/**
 * \brief Set interrupt vector priority level
 *
 * Set the priority level of an external interrupt or exception.
 *
 * \param[in] vector  Interrupt vector to which the priority level will be set
 *
 * \returns Status code indicating if the priority level of the interrupt was
 *          successfully set.
 *
 * \retval STATUS_OK           If no error was detected
 * \retval STATUS_INVALID_ARG  If an unsupported interrupt vector number was given
 */
enum status_code system_interrupt_set_priority(
		const enum system_interrupt_vector vector,
		const enum system_interrupt_priority_level priority_level);

/**
 * \brief Get interrupt vector priority level
 *
 * Retrieves the priority level of the requested external interrupt or exception.
 *
 * \param[in] vector  Interrupt vector of which the priority level will be read
 *
 * \return Currently configured interrupt priority level of the given interrupt
 *         vector.
 */
enum system_interrupt_priority_level system_interrupt_get_priority(
		const enum system_interrupt_vector vector);

/** @} */

/**
 * \page asfdoc_samd20_system_interrupt_extra Extra Information for System Interrupt Driver
 *
 * \section asfdoc_samd20_system_interrupt_extra_acronyms Acronyms
 * The table below presents the acronyms used in this module:
 *
 * <table>
 *	<tr>
 *		<th>Acronym</th>
 *		<th>Description</th>
 *	</tr>
 *	<tr>
 *		<td>ISR</td>
 *		<td>Interrupt Service Routine</td>
 *	</tr>
 * </table>
 *
 *
 * \section asfdoc_samd20_system_interrupt_extra_dependencies Dependencies
 * This driver has the following dependencies:
 *
 *  - None
 *
 *
 * \section asfdoc_samd20_system_interrupt_extra_errata Errata
 * There are no errata related to this driver.
 *
 *
 * \section asfdoc_samd20_system_interrupt_extra_history Module History
 * An overview of the module history is presented in the table below, with
 * details on the enhancements and fixes made to the module since its first
 * release. The current version of this corresponds to the newest version in
 * the table.
 *
 * <table>
 *	<tr>
 *		<th>Changelog</th>
 *	</tr>
 *	<tr>
 *		<td>Initial Release</td>
 *	</tr>
 * </table>
 */

/**
 * \page asfdoc_samd20_system_interrupt_exqsg Examples for System Interrupt Driver
 *
 * This is a list of the available Quick Start guides (QSGs) and example
 * applications for \ref asfdoc_samd20_system_interrupt_group. QSGs are simple examples with
 * step-by-step instructions to configure and use this driver in a selection of
 * use cases. Note that QSGs can be compiled as a standalone application or be
 * added to the user application.
 *
 *  - \subpage asfdoc_samd20_system_interrupt_basic_use_case
 */

#endif
