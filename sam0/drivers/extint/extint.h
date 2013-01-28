/**
 * \file
 *
 * \brief SAMD20 External Interrupt Driver
 *
 * Copyright (C) 2012-2013 Atmel Corporation. All rights reserved.
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
#ifndef EXTINT_H_INCLUDED
#define EXTINT_H_INCLUDED

/**
 * \defgroup asfdoc_sam0_extint SAMD20 External Interrupt Driver (EXTINT)
 *
 * Driver for the SAMD20 architecture devices. This driver provides a unified
 * interface for the configuration and management of external interrupts
 * generated by the physical device pins, including edge detection. This driver
 * encompasses the following modules within the device to provide a consistent
 * interface:
 *
 * \li \b EIC (External Interrupt Controller)
 *
 * Physically, the modules are interconnected within the device as shown in the
 * following diagram:
 *
 * \dot
 * digraph overview {
 *   node [label="Port Pad" shape=square] pad;
 *
 *   subgraph driver {
 *     node [label="Peripheral Mux" shape=trapezium] pinmux;
 *     node [label="EIC Module" shape=ellipse] eic;
 *     node [label="Other Peripheral Modules" shape=ellipse style=filled fillcolor=lightgray] peripherals;
 *   }
 *
 *   pinmux -> eic;
 *   pad    -> pinmux;
 *   pinmux -> peripherals;
 * }
 * \enddot
 *
 * \section asfdoc_sam0_extint_introduction Introduction
 * The External Interrupt (EXTINT) module provides a method of asynchronously
 * detecting rising edge, falling edge or specific level detection on individual
 * I/O pins of a device. This detection can then be used to trigger a software
 * interrupt or event, or polled for later use if required. External interrupts
 * can also optionally be used to automatically wake up the device from sleep
 * mode, allowing the device to conserve power while still being able to react
 * to an external stimulus in a timely manner.
 *
 * \subsection asfdoc_sam0_extint_logical_channels Logical Channels
 * The External Interrupt module contains a number of logical channels, each of
 * which is capable of being individually configured for a given pin routing,
 * detection mode and filtering/wake up characteristics.
 *
 * Each individual logical external interrupt channel may be routed to a single
 * physical device I/O pin in order to detect a particular edge or level of the
 * incoming signal.
 *
 * \subsection asfdoc_sam0_extint_nmi_chanel NMI Channel(s)
 * One or more Non Maskable Interrupt (NMI) channels are provided within each
 * physical External Interrupt Controller module, allowing a single physical pin
 * of the device to fire a single NMI interrupt in response to a particular
 * edge or level stimulus. A NMI cannot, as the name suggests, be disabled in
 * firmware and will take precedence over any in-progress interrupt sources.
 *
 * NMIs can be used to implement critical device features such as forced
 * software reset or other functionality where the action should be executed in
 * preference to all other running code with a minimum amount of latency.
 *
 * \subsection asfdoc_sam0_extint_filtering_and_detection Input Filtering and Detection
 * To reduce the possibility of noise or other transient signals causing
 * unwanted device wake-ups, interrupts and/or events via an external interrupt
 * channel, a hardware signal filter can be enabled on individual channels. This
 * filter provides a Majority-of-Three voter filter on the incoming signal, so
 * that the input state is considered to be the majority vote of three
 * subsequent samples of the pin input buffer:
 *
 * <table>
 *	<tr>
 *		<th>Input Sample 1</th>
 *		<th>Input Sample 2</th>
 *		<th>Input Sample 3</th>
 *		<th>Filtered Output</th>
 *	</tr>
 *	<tr>
 *		<td>0</td> <td>0</td> <td>0</td> <td>0</td>
 *	</tr>
 *	<tr>
 *		<td>0</td> <td>0</td> <td>1</td> <td>0</td>
 *	</tr>
 *	<tr>
 *		<td>0</td> <td>1</td> <td>0</td> <td>0</td>
 *	</tr>
 *	<tr>
 *		<td>0</td> <td>1</td> <td>1</td> <td>1</td>
 *	</tr>
 *	<tr>
 *		<td>1</td> <td>0</td> <td>0</td> <td>0</td>
 *	</tr>
 *	<tr>
 *		<td>1</td> <td>0</td> <td>1</td> <td>1</td>
 *	</tr>
 *	<tr>
 *		<td>1</td> <td>1</td> <td>0</td> <td>1</td>
 *	</tr>
 *	<tr>
 *		<td>1</td> <td>1</td> <td>1</td> <td>1</td>
 *	</tr>
 * </table>
 *
 * \subsection asfdoc_sam0_extint_events_and_interrupts Events and Interrupts
 * Channel detection states may be polled inside the application for synchronous
 * detection, or events and interrupts may be used for asynchronous behavior.
 * Each channel can be configured to give an asynchronous hardware event (which
 * may in turn trigger actions in other hardware modules) or an asynchronous
 * software interrupt.
 *
 * \section asfdoc_sam0_extint_module_dependencies Dependencies
 * The external interrupt driver has the following dependencies:
 *
 * \li \ref sam0_pinmux_group "System Pin Multiplexer Driver"
 *
 * \section asfdoc_sam0_extint_special_considerations Special Considerations
 * Not all devices support disabling of the NMI channel(s) detection mode - see
 * your device datasheet.
 *
 * \section asfdoc_sam0_extint_extra_info Extra Information
 * For extra information see \ref asfdoc_sam0_extint_extra_info.
 *
 * \section asfdoc_sam0_extint_examples Examples
 * - \ref asfdoc_sam0_extint_quickstart
 *
 * \section asfdoc_sam0_extint_api_overview API Overview
 * @{
 */

#include <compiler.h>
#include <pinmux.h>

#if EXTINT_ASYNC == true
#  include "extint_async.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief External interrupt edge detection configuration enum.
 *
 * Enum for the possible signal edge detection modes of the External
 * Interrupt Controller module.
 */
enum extint_detect {
	/** No edge detection. Not allowed as a NMI detection mode on some
	 *  devices. */
	EXTINT_DETECT_NONE    = 0,
	/** Detect rising signal edges. */
	EXTINT_DETECT_RISING  = 1,
	/** Detect falling signal edges. */
	EXTINT_DETECT_FALLING = 2,
	/** Detect both signal edges. */
	EXTINT_DETECT_BOTH    = 3,
	/** Detect high signal levels. */
	EXTINT_DETECT_HIGH    = 4,
	/** Detect low signal levels. */
	EXTINT_DETECT_LOW     = 5,
};

/**
 * \brief External Interrupt Controller channel configuration structure.
 *
 *  Configuration structure for the edge detection mode of an external
 *  interrupt channel.
 */
struct extint_ch_conf {
	/** GPIO pin the NMI should be connected to. */
	uint32_t gpio_pin;
	/** MUX position the GPIO pin should be configured to. */
	uint32_t gpio_pin_mux;
	/** Wake up the device if the channel interrupt fires during sleep mode. */
	bool wake_if_sleeping;
	/** Filter the raw input signal to prevent noise from triggering an
	 *  interrupt accidentally, using a 3 sample majority filter. */
	bool filter_input_signal;
	/** Edge detection mode to use. */
	enum extint_detect detect;
};

/**
 * \brief External Interrupt event enable/disable structure.
 *
 * Event flags for the External Interrupt) and \ref extint_disable_events().
 */
struct extint_events {
	/** If \c true, an event will be generated when an external interrupt
	 *  channel detection state changes. */
	bool output_on_detect[32 * EIC_INST_NUM];
};

/**
 * \brief External Interrupt Controller NMI configuration structure.
 *
 *  Configuration structure for the edge detection mode of an external
 *  interrupt NMI channel.
 */
struct extint_nmi_conf {
	/** GPIO pin the NMI should be connected to. */
	uint32_t gpio_pin;
	/** MUX position the GPIO pin should be configured to. */
	uint32_t gpio_pin_mux;
	/** Filter the raw input signal to prevent noise from triggering an
	 *  interrupt accidentally, using a 3 sample majority filter. */
	bool filter_input_signal;
	/** Edge detection mode to use. Not all devices support all possible
	 *  detection modes for NMIs.
	 */
	enum extint_detect detect;
};

#if !defined(__DOXYGEN__)
/** \internal
 *  Internal EXTINT module device instance structure definition.
 */
struct _extint_device
{
#  if EXTINT_ASYNC == true
	/** Asynchronous channel callback table, for user-registered handlers. */
	extint_async_callback_t callbacks[EXTINT_CALLBACKS_MAX];
#  endif
};

/**
 * \brief Retrieves the base EIC module address from a given channel number.
 *
 * Retrieves the base address of a EIC hardware module associated with the
 * given external interrupt channel.
 *
 * \param[in] channel  External interrupt channel index to convert.
 *
 * \return Base address of the associated EIC module.
 */
static inline Eic *const _extint_get_eic_from_channel(
		const uint8_t channel)
{
	uint8_t eic_index = (channel / 32);

	/* Array of available EICs. */
	Eic *const eics[EIC_INST_NUM] = EIC_INSTS;

	if (eic_index < EIC_INST_NUM) {
		return eics[eic_index];
	} else {
		Assert(false);
		return NULL;
	}
}

/**
 * \brief Retrieves the base EIC module address from a given NMI channel number.
 *
 * Retrieves the base address of a EIC hardware module associated with the
 * given non-maskable external interrupt channel.
 *
 * \param[in] nmi_channel  Non-Maskable interrupt channel index to convert.
 *
 * \return Base address of the associated EIC module.
 */
static inline Eic *const _extint_get_eic_from_nmi(
		const uint8_t nmi_channel)
{
	uint8_t eic_index = nmi_channel;

	/* Array of available EICs. */
	Eic *const eics[EIC_INST_NUM] = EIC_INSTS;

	if (eic_index < EIC_INST_NUM) {
		return eics[eic_index];
	} else {
		Assert(false);
		return NULL;
	}
}
#endif

/** \name Configuration and initialization
 * @{
 */

void extint_reset(void);
void extint_enable(void);
void extint_disable(void);

/**
 * \brief Enables an External Interrupt event output.
 *
 *  Enables one or more output events from the External Interrupt module. See
 *  \ref extint_events "here" for a list of events this module supports.
 *
 *  \note Events cannot be altered while the module is enabled.
 *
 *  \param[in] events    Struct containing flags of events to enable
 */
static inline void extint_enable_events(
		struct extint_events *const events)
{
	/* Sanity check arguments */
	Assert(events);

	/* Array of available EICs. */
	Eic *const eics[EIC_INST_NUM] = EIC_INSTS;

	/* Update the event control register for each physical EIC instance */
	for (uint32_t i = 0; i < EIC_INST_NUM; i++) {
		uint32_t event_mask = 0;

		/* Create an enable mask for the current EIC module */
		for (uint32_t j = 0; j < 32; j++) {
			if (events->output_on_detect[(32 * i) + j]) {
				event_mask |= (1UL << j);
			}
		}

		/* Enable the masked events */
		eics[i]->EVCTRL.reg |= event_mask;
	}
}

/**
 * \brief Disables an External Interrupt event output.
 *
 *  Disables one or more output events from the External Interrupt module. See
 *  \ref extint_events "here" for a list of events this module supports.
 *
 *  \note Events cannot be altered while the module is enabled.
 *
 *  \param[in] events    Struct containing flags of events to disable
 */
static inline void extint_disable_events(
		struct extint_events *const events)
{
	/* Sanity check arguments */
	Assert(events);

	/* Array of available EICs. */
	Eic *const eics[EIC_INST_NUM] = EIC_INSTS;

	/* Update the event control register for each physical EIC instance */
	for (uint32_t i = 0; i < EIC_INST_NUM; i++) {
		uint32_t event_mask = 0;

		/* Create a disable mask for the current EIC module */
		for (uint32_t j = 0; j < 32; j++) {
			if (events->output_on_detect[(32 * i) + j]) {
				event_mask |= (1UL << j);
			}
		}

		/* Disable the masked events */
		eics[i]->EVCTRL.reg &= ~event_mask;
	}
}

/** @} */

/** \name Configuration and initialization (channel)
 * @{
 */

/**
 * \brief Initializes an External Interrupt channel configuration structure to defaults.
 *
 * Initializes a given External Interrupt channel configuration structure to a
 * set of known default values. This function should be called on all new
 * instances of these configuration structures before being modified by the
 * user application.
 *
 * The default configuration is as follows:
 * \li Wake the device if an edge detection occurs whilst in sleep
 * \li Input filtering disabled
 * \li Detect falling edges of a signal
 *
 * \param[out] config  Configuration structure to initialize to default values
 */
static inline void extint_ch_get_config_defaults(
		struct extint_ch_conf *const config)
{
	/* Sanity check arguments */
	Assert(config);

	/* Default configuration values */
	config->gpio_pin            = 0;
	config->gpio_pin_mux        = 0;
	config->wake_if_sleeping    = true;
	config->filter_input_signal = false;
	config->detect              = EXTINT_DETECT_FALLING;
}

void extint_ch_set_config(
		const uint8_t channel,
		const struct extint_ch_conf *const config);

/** @} */

/** \name Configuration and initialization (NMI)
 * @{
 */

/**
 * \brief Initializes an External Interrupt NMI channel configuration structure to defaults.
 *
 * Initializes a given External Interrupt NMI channel configuration structure
 * to a set of known default values. This function should be called on all new
 * instances of these configuration structures before being modified by the
 * user application.
 *
 * The default configuration is as follows:
 * \li Input filtering disabled
 * \li Detect falling edges of a signal
 *
 * \param[out] config  Configuration structure to initialize to default values
 */
static inline void extint_nmi_get_config_defaults(
		struct extint_nmi_conf *const config)
{
	/* Sanity check arguments */
	Assert(config);

	/* Default configuration values */
	config->gpio_pin            = 0;
	config->gpio_pin_mux        = 0;
	config->filter_input_signal = false;
	config->detect              = EXTINT_DETECT_FALLING;
}

enum status_code extint_nmi_set_config(
		const uint8_t nmi_channel,
		const struct extint_nmi_conf *const config);

/** @} */

/** \name Detection testing and clearing (channel)
 * @{
 */

/**
 * \brief Retrieves the edge detection state of a configured channel.
 *
 *  Reads the current state of a configured channel, and determines if
 *  if the detection criteria of the channel has been met.
 *
 *  \param[in] channel  External Interrupt channel index to check.
 *
 *  \return Status of the requested channel's edge detection state.
 *  \retval true  If the channel's edge/level detection criteria was met
 *  \retval false  If the channel has not detected its configured criteria
 */
static inline bool extint_ch_is_detected(
		const uint8_t channel)
{
	Eic *const eic_module = _extint_get_eic_from_channel(channel);
	uint32_t eic_mask   = (1UL << (channel % 32));

	return (eic_module->INTFLAG.reg & eic_mask);
}

/**
 * \brief Clears the edge detection state of a configured channel.
 *
 *  Clears the current state of a configured channel, readying it for
 *  the next level or edge detection.
 *
 *  \param[in] channel  External Interrupt channel index to check.
 */
static inline void extint_ch_clear_detected(
		const uint8_t channel)
{
	Eic *const eic_module = _extint_get_eic_from_channel(channel);
	uint32_t eic_mask   = (1UL << (channel % 32));

	eic_module->INTFLAG.reg = eic_mask;
}

/** @} */

/** \name Detection testing and clearing (NMI)
 * @{
 */

/**
 * \brief Retrieves the edge detection state of a configured NMI channel.
 *
 *  Reads the current state of a configured NMI channel, and determines if
 *  if the detection criteria of the NMI channel has been met.
 *
 *  \param[in] nmi_channel  External Interrupt NMI channel index to check.
 *
 *  \return Status of the requested NMI channel's edge detection state.
 *  \retval true  If the NMI channel's edge/level detection criteria was met
 *  \retval false  If the NMI channel has not detected its configured criteria
 */
static inline bool extint_nmi_is_detected(
		const uint8_t nmi_channel)
{
	Eic *const eic_module = _extint_get_eic_from_nmi(nmi_channel);

	return (eic_module->NMIFLAG.reg & EIC_NMIFLAG_NMI);
}

/**
 * \brief Clears the edge detection state of a configured NMI channel.
 *
 *  Clears the current state of a configured NMI channel, readying it for
 *  the next level or edge detection.
 *
 *  \param[in] nmi_channel  External Interrupt NMI channel index to check.
 */
static inline void extint_nmi_clear_detected(
		const uint8_t nmi_channel)
{
	Eic *const eic_module = _extint_get_eic_from_nmi(nmi_channel);

	eic_module->NMIFLAG.reg = EIC_NMIFLAG_NMI;
}

/** @} */

#ifdef __cplusplus
}
#endif

/** @} */

/**
 * \page asfdoc_sam0_extint_extra_info Extra Information
 *
 * \section asfdoc_sam0_extint_acronyms Acronyms
 * Below is a table listing the acronyms used in this module, along with their
 * intended meanings.
 *
 * <table>
 *	<tr>
 *		<th>Acronym</th>
 *		<th>Description</th>
 *	</tr>
 *	<tr>
 *		<td>EIC</td>
 *		<td>External Interrupt Controller</td>
 *	</tr>
 *	<tr>
 *		<td>MUX</td>
 *		<td>Multiplexer</td>
 *	</tr>
 *	<tr>
 *		<td>NMI</td>
 *		<td>Non-Maskable Interrupt</td>
 *	</tr>
 * </table>
 *
 * \section asfdoc_sam0_extint_fixed_errata Errata fixed by driver
 * No errata workarounds in driver.
 *
 * \section asfdoc_sam0_extint_module_history Module History
 * Below is an overview of the module history, detailing enhancements and fixes
 * made to the module since its first release. The current version of this
 * corresponds to the newest version listed in the table below.
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
 * \page asfdoc_sam0_extint_quickstart Quick Start Guides for the EXTINT module
 *
 * This is the quick start guide list for the \ref asfdoc_sam0_extint,
 * with step-by-step instructions on how to configure and use the driver in a
 * selection of use cases.
 *
 * The use cases contain several code fragments. The code fragments in the
 * steps for setup can be copied into a custom initialization function of the
 * user application and run at system startup, while the steps for usage can be
 * copied into the normal user application program flow.
 *
 * \see General list of module \ref asfdoc_sam0_extint_examples "examples".
 *
 * \section asfdoc_sam0_extint_use_cases EXTINT module use cases
 * - \subpage asfdoc_sam0_extint_basic_use_case
 */

#endif
