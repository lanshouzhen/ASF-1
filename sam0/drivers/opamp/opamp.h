/**
 * \file
 *
 * \brief SAM0 Operational Amplifier Controller (OPAMP) Driver
 *
 * Copyright (C) 2014 Atmel Corporation. All rights reserved.
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

#ifndef OPAMP_H_INCLUDED
#define OPAMP_H_INCLUDED

/**
 * \defgroup asfdoc_sam0_opamp_group SAM0 Operational Amplifier Controller (OPAMP) Driver
 *
 * This driver for SAM0 devices provides an interface for the configuration
 * and management of the device's Operational Amplifier Controller functionality.
 *
 * The following peripherals are used by this module:
 *  - OPAMP (Operational Amplifier Controller)
 *
 * The following devices can use this module:
 *  - SAM L21
 *
 * The outline of this documentation is as follows:
 *  - \ref asfdoc_sam0_opamp_prerequisites
 *  - \ref asfdoc_sam0_opamp_module_overview
 *  - \ref asfdoc_sam0_opamp_special_considerations
 *  - \ref asfdoc_sam0_opamp_extra_info
 *  - \ref asfdoc_sam0_opamp_examples
 *  - \ref asfdoc_sam0_opamp_api_overview
 *
 *
 * \section asfdoc_sam0_opamp_prerequisites Prerequisites
 *
 * There are no prerequisites for this module.
 *
 *
 * \section asfdoc_sam0_opamp_module_overview Module Overview
 *
 * This driver provides an interface for the Operational Amplifier Controller
 * functions on the device.
 *
 * For the detail configuration reference, you can refer to the datasheet
 * "Build-in Modes" section.
 *
 * \section asfdoc_sam0_opamp_special_considerations Special Considerations
 *
 * There are no special considerations for this module.
 *
 *
 * \section asfdoc_sam0_opamp_extra_info Extra Information
 *
 * For extra information see \ref asfdoc_sam0_opamp_extra. This includes:
 *  - \ref asfdoc_sam0_opamp_extra_acronyms
 *  - \ref asfdoc_sam0_opamp_extra_dependencies
 *  - \ref asfdoc_sam0_opamp_extra_errata
 *  - \ref asfdoc_sam0_opamp_extra_history
 *
 *
 * \section asfdoc_sam0_opamp_examples Examples
 *
 * For a list of examples related to this driver, see
 * \ref asfdoc_sam0_opamp_exqsg.
 *
 *
 * \section asfdoc_sam0_opamp_api_overview API Overview
 * @{
 */

#include <compiler.h>
#include <system.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief OPAMP number enum.
 */
enum opamp_number {
	/** OPAMP 0  */
	OPAMP_0 = 0,
	/** OPAMP 1  */
	OPAMP_1 = 1,
	/** OPAMP 2  */
	OPAMP_2 = 2,
	/** OPAMP number  */
	OPAMP_NUM,
};

/**
 * \brief Negative Input Mux Selection configuration enum.
 *
 * Enum for the negative input of OPAMP0.
 */
enum opamp0_neg_mux {
	/** Negative I/O pin 0. */
	OPAMP0_NEG_MUX_PIN0 = OPAMP_OPAMPCTRL0_MUXNEG(0),
	/** Resistor ladder 0 taps. */
	OPAMP0_NEG_MUX_TAP0 = OPAMP_OPAMPCTRL0_MUXNEG(1),
	/** OPAMP output. */
	OPAMP0_NEG_MUX_OUT0 = OPAMP_OPAMPCTRL0_MUXNEG(2),
	/** DAC output. */
	OPAMP0_NEG_MUX_DAC = OPAMP_OPAMPCTRL0_MUXNEG(3),
};

/**
 * \brief Negative Input Mux Selection configuration enum.
 *
 * Enum for the negative input of OPAMP1.
 */
enum opamp1_neg_mux {
	/** Negative I/O pin 1. */
	OPAMP1_NEG_MUX_PIN1 = OPAMP_OPAMPCTRL1_MUXNEG(0),
	/** Resistor ladder 1 taps. */
	OPAMP1_NEG_MUX_TAP1 = OPAMP_OPAMPCTRL1_MUXNEG(1),
	/** OPAMP output. */
	OPAMP1_NEG_MUX_OUT1 = OPAMP_OPAMPCTRL1_MUXNEG(2),
	/** DAC output. */
	OPAMP1_NEG_MUX_DAC = OPAMP_OPAMPCTRL1_MUXNEG(3),
};

/**
 * \brief Negative Input Mux Selection configuration enum.
 *
 * Enum for the negative input of OPAMP2.
 */
enum opamp2_neg_mux {
	/** Negative I/O pin 2. */
	OPAMP2_NEG_MUX_PIN2 = OPAMP_OPAMPCTRL2_MUXNEG(0),
	/** Resistor ladder 2 taps. */
	OPAMP2_NEG_MUX_TAP2 = OPAMP_OPAMPCTRL2_MUXNEG(1),
	/** OPAMP output. */
	OPAMP2_NEG_MUX_OUT2 = OPAMP_OPAMPCTRL2_MUXNEG(2),
	/** Negative I/O pin 0. */
	OPAMP2_NEG_MUX_PIN0 = OPAMP_OPAMPCTRL2_MUXNEG(3),
	/** Negative I/O pin 1. */
	OPAMP2_NEG_MUX_PIN1 = OPAMP_OPAMPCTRL2_MUXNEG(4),
	/** DAC output. */
	OPAMP2_NEG_MUX_DAC = OPAMP_OPAMPCTRL2_MUXNEG(5),
};

/**
 * \brief Positive Input Mux Selection configuration enum.
 *
 * Enum for the negative input of OPAMP0.
 */
enum opamp0_pos_mux {
	/** Positive I/O pin 0. */
	OPAMP0_POS_MUX_PIN0 = OPAMP_OPAMPCTRL0_MUXPOS(0),
	/** Resistor ladder 0 taps. */
	OPAMP0_POS_MUX_TAP0 = OPAMP_OPAMPCTRL0_MUXPOS(1),
	/** DAC output. */
	OPAMP0_POS_MUX_DAC = OPAMP_OPAMPCTRL0_MUXPOS(2),
	/** Ground. */
	OPAMP0_POS_MUX_GND = OPAMP_OPAMPCTRL0_MUXPOS(3),
};

/**
 * \brief Positive Input Mux Selection configuration enum.
 *
 * Enum for the negative input of OPAMP1.
 */
enum opamp1_pos_mux {
	/** Positive I/O pin 1. */
	OPAMP1_POS_MUX_PIN1 = OPAMP_OPAMPCTRL0_MUXPOS(0),
	/** Resistor ladder 1 taps. */
	OPAMP1_POS_MUX_TAP1 = OPAMP_OPAMPCTRL0_MUXPOS(1),
	/** OPAMP0 output. */
	OPAMP1_POS_MUX_OUT0 = OPAMP_OPAMPCTRL0_MUXPOS(2),
	/** Ground. */
	OPAMP1_POS_MUX_GND = OPAMP_OPAMPCTRL0_MUXPOS(3),
};

/**
 * \brief Positive Input Mux Selection configuration enum.
 *
 * Enum for the negative input of OPAMP2.
 */
enum opamp2_pos_mux {
	/** Positive I/O pin 2. */
	OPAMP2_POS_MUX_PIN2 = OPAMP_OPAMPCTRL0_MUXPOS(0),
	/** Resistor ladder 2 taps. */
	OPAMP2_POS_MUX_TAP2 = OPAMP_OPAMPCTRL0_MUXPOS(1),
	/** OPAMP1 output. */
	OPAMP2_POS_MUX_OUT1 = OPAMP_OPAMPCTRL0_MUXPOS(2),
	/** Ground. */
	OPAMP2_POS_MUX_GND = OPAMP_OPAMPCTRL0_MUXPOS(3),
	/** Positive I/O pin 0. */
	OPAMP2_POS_MUX_PIN0 = OPAMP_OPAMPCTRL0_MUXPOS(4),
	/** Positive I/O pin 1. */
	OPAMP2_POS_MUX_PIN1 = OPAMP_OPAMPCTRL0_MUXPOS(5),
	/** Resistor ladder 0 taps. */
	OPAMP2_POS_MUX_TAP0 = OPAMP_OPAMPCTRL0_MUXPOS(6),
};

/**
 * \brief Potentimeter selection Mux configuration enum.
 *
 * Enum for the Potentimeter selection of OPAMP 0 to 2.
 */
enum opamp_pot_mux {
	/** Gain = R2/R1 = 1/7. */
	OPAMP_POT_MUX_14R_2R = OPAMP_OPAMPCTRL0_POTMUX(0),
	/** Gain = R2/R1 = 1/3. */
	OPAMP_POT_MUX_12R_4R = OPAMP_OPAMPCTRL0_POTMUX(1),
	/** Gain = R2/R1 = 1. */
	OPAMP_POT_MUX_8R_8R = OPAMP_OPAMPCTRL0_POTMUX(2),
	/** Gain = R2/R1 = 1 + 2/3. */
	OPAMP_POT_MUX_6R_10R = OPAMP_OPAMPCTRL0_POTMUX(3),
	/** Gain = R2/R1 = 3. */
	OPAMP_POT_MUX_4R_12R = OPAMP_OPAMPCTRL0_POTMUX(4),
	/** Gain = R2/R1 = 4 + 1/3. */
	OPAMP_POT_MUX_3R_13R = OPAMP_OPAMPCTRL0_POTMUX(5),
	/** Gain = R2/R1 = 7. */
	OPAMP_POT_MUX_2R_14R = OPAMP_OPAMPCTRL0_POTMUX(6),
	/** Gain = R2/R1 = 15. */
	OPAMP_POT_MUX_R_15R = OPAMP_OPAMPCTRL0_POTMUX(7),
};

/**
 * \brief Resistor 1 Mux Selection configuration enum.
 *
 * Enum for the Resistor 1 of OPAMP0.
 */
enum opamp0_res1_mux {
	/** Positive input of OPAMP0. */
	OPAMP0_RES1_MUX_POS_PIN0 = OPAMP_OPAMPCTRL0_RES1MUX(0),
	/** Negative input of OPAMP0. */
	OPAMP0_RES1_MUX_NEG_PIN0 = OPAMP_OPAMPCTRL0_RES1MUX(1),
	/** DAC output. */
	OPAMP0_RES1_MUX_DAC = OPAMP_OPAMPCTRL0_RES1MUX(2),
	/** Ground. */
	OPAMP0_RES1_MUX_GND = OPAMP_OPAMPCTRL0_RES1MUX(3),
};

/**
 * \brief Resistor 1 Mux Selection configuration enum.
 *
 * Enum for the Resistor 1 of OPAMP1.
 */
enum opamp1_res1_mux {
	/** Positive input of OPAMP1. */
	OPAMP1_RES1_MUX_POS_PIN0 = OPAMP_OPAMPCTRL0_RES1MUX(0),
	/** Negative input of OPAMP1. */
	OPAMP1_RES1_MUX_NEG_PIN0 = OPAMP_OPAMPCTRL0_RES1MUX(1),
	/** OPAMP0 output. */
	OPAMP1_RES1_MUX_OUT0 = OPAMP_OPAMPCTRL0_RES1MUX(2),
	/** Ground. */
	OPAMP1_RES1_MUX_GND = OPAMP_OPAMPCTRL0_RES1MUX(3),
};

/**
 * \brief Resistor 1 Mux Selection configuration enum.
 *
 * Enum for the Resistor 1 of OPAMP2.
 */
enum opamp2_res1_mux {
	/** Positive input of OPAMP2. */
	OPAMP2_RES1_MUX_POS_PIN0 = OPAMP_OPAMPCTRL0_RES1MUX(0),
	/** Negative input of OPAMP2. */
	OPAMP2_RES1_MUX_NEG_PIN0 = OPAMP_OPAMPCTRL0_RES1MUX(1),
	/** OPAMP1 output. */
	OPAMP2_RES1_MUX_OUT1 = OPAMP_OPAMPCTRL0_RES1MUX(2),
	/** Ground. */
	OPAMP2_RES1_MUX_GND = OPAMP_OPAMPCTRL0_RES1MUX(3),
};

/**
 * \brief Bias mode selection Mux configuration enum.
 *
 * Enum for the Bias mode selection of OPAMP 0 to 2.
 */
enum opamp_bias_selection {
	/** Minimum current consumption but the slowest mode. */
	OPAMP_BIAS_MODE_0 = OPAMP_OPAMPCTRL0_BIAS(0),
	/** Low current consumption, slow speed. */
	OPAMP_BIAS_MODE_1 = OPAMP_OPAMPCTRL0_BIAS(1),
	/** High current consumption, fast speed. */
	OPAMP_BIAS_MODE_2 = OPAMP_OPAMPCTRL0_BIAS(2),
	/** Maximum current consumption but the fastest mode. */
	OPAMP_BIAS_MODE_3 = OPAMP_OPAMPCTRL0_BIAS(3),
};

/**
 * \brief OPAMP 0 to 2 common configuration structure.
 *
 *  Common configuration structure for OPAMP 0 to 2.
 */
struct opamp_config_common {
	/**  Potentimeter selection. */
	enum opamp_pot_mux potentiometer_selection;
	/** If \c true, R1 connected to RES1MUX. */
	bool r1_enable;
	/** If \c true, resistor ladder To VCC. */
	bool r2_vcc;
	/** If \c true, resistor ladder To Output. */
	bool r2_out;
	/** If \c true, the OPAMPx is enabled when a peripheral is requesting
	 * the OPAMPx to be used as an input. The OPAMPx is
        * disabled if no peripheral is requesting it as an input. */
	bool on_demand;
	/** If \c true, the OPAMPx is not stopped during sleep
	 *  mode when triggered. */
	bool run_in_standby;
	/**  Bias mode selection. */
	enum opamp_bias_selection bias_value;
	/** If \c true, OPAMP output is connected to the ADC or AC input. */
	bool analog_out;
};

/**
 * \brief OPAMP 0 configuration structure.
 *
 *  Configuration structure for OPAMP 0.
 */
struct opamp0_config {
	/** Negative Input Mux Selection. */
	enum opamp0_neg_mux negative_input;
	/** Positive Input Mux Selection. */
	enum opamp0_pos_mux positive_input;
	/** Resistor 1 Mux Selection. */
	enum opamp0_res1_mux r1_connection;
	/** If \c true, the comparator will continue to sample during sleep
	 *  mode when triggered. */
	struct opamp_config_common config_common;
};

/**
 * \brief OPAMP 1 configuration structure.
 *
 *  Configuration structure for OPAMP 0.
 */
struct opamp1_config {
	/** Negative Input Mux Selection. */
	enum opamp1_neg_mux negative_input;
	/** Positive Input Mux Selection. */
	enum opamp1_pos_mux positive_input;
	/** Resistor 1 Mux Selection. */
	enum opamp1_res1_mux r1_connection;
	/** If \c true, the comparator will continue to sample during sleep
	 *  mode when triggered. */
	struct opamp_config_common config_common;
};

/**
 * \brief OPAMP 2 configuration structure.
 *
 *  Configuration structure for OPAMP 0.
 */
struct opamp2_config {
	/** Negative Input Mux Selection. */
	enum opamp2_neg_mux negative_input;
	/** Positive Input Mux Selection. */
	enum opamp2_pos_mux positive_input;
	/** Resistor 1 Mux Selection. */
	enum opamp2_res1_mux r1_connection;
	/** If \c true, the comparator will continue to sample during sleep
	 *  mode when triggered. */
	struct opamp_config_common config_common;
};

/**
 * \brief Resets OPAMP module.
 *
 * Resets all registers in the MODULE to their initial state,
 * and the OPAMP will be disabled.
 */
static inline void opamp_module_reset(void)
{
	/* Reset OPAMP */
	OPAMP->CTRLA.reg |= OPAMP_CTRLA_SWRST;
}

/**
 * \brief Enables OPAMP module.
 *
 * Enable the peripheral. Each OPAMP must also be enabled individually
 * by the Enable bit in the corresponding OPAMP Control register.
 *
 */
static inline void opamp_module_enable(void)
{
	/* Enable OPAMP */
	OPAMP->CTRLA.reg |= OPAMP_CTRLA_ENABLE;
}

/**
 * \brief Disables OPAMP module.
 *
 * Disables the peripheral.
 */
static inline void opamp_module_disable(void)
{
	/* Disable OPAMP */
	OPAMP->CTRLA.reg &= ~OPAMP_CTRLA_ENABLE;
}

/**
 * \brief Initializes all members of OPAMP0 configuration
 * structure to safe defaults.
 *
 *  Initializes all members of OPAMP0 configuration
 *  structure to safe defaults. This function should be called on all new
 *  instances of these configuration structures before being modified by the
 *  user application.
 *
 *  The default configuration is setting OPAMP0 as "Voltage Follower",
 *  Please refer to the first mode in datasheet "Build-in Modes" section.
 *
 *  \param[out] config  OPAMP0 configuration structure to initialize to
 *                       default values.
 */
void opamp0_get_config_defaults(struct opamp0_config *const config);

/**
 * \brief Initializes all members of OPAMP1 configuration
 * structure to safe defaults.
 *
 *  Initializes all members of OPAMP1 configuration
 *  structure to safe defaults. This function should be called on all new
 *  instances of these configuration structures before being modified by the
 *  user application.
 *
 *  The default configuration is setting OPAMP1 as "Voltage Follower",
 *  Please refer to the first mode in datasheet "Build-in Modes" section.
 *
 *  \param[out] config  OPAMP1 configuration structure to initialize to
 *                       default values.
 */
void opamp1_get_config_defaults(struct opamp1_config *const config);

/**
 * \brief Initializes all members of OPAMP2 configuration
 * structure to safe defaults.
 *
 *  Initializes all members of OPAMP2 configuration
 *  structure to safe defaults. This function should be called on all new
 *  instances of these configuration structures before being modified by the
 *  user application.
 *
 *  The default configuration is setting OPAMP2 as "Voltage Follower",
 *  Please refer to the first mode in datasheet "Build-in Modes" section.
 *
 *  \param[out] config  OPAMP2 configuration structure to initialize to
 *                       default values.
 */
void opamp2_get_config_defaults(struct opamp2_config *const config);

/** \brief Writes OPAMP0 configuration to the hardware module.
 *
 *  Writes a given OPAMP0 configuration to the hardware  module.
 *
 *  \param[in] config       Pointer to the OPAMP0 configuration struct.
 */
void opamp0_set_config(struct opamp0_config *const config);

/** \brief Writes OPAMP1 configuration to the hardware module.
 *
 *  Writes a given OPAMP1 configuration to the hardware  module.
 *
 *  \param[in] config       Pointer to the OPAMP1 configuration struct.
 */
void opamp1_set_config(struct opamp1_config *const config);

/** \brief Writes OPAMP2 configuration to the hardware module.
 *
 *  Writes a given OPAMP2 configuration to the hardware  module.
 *
 *  \param[in] config       Pointer to the OPAMP2 configuration struct.
 */
void opamp2_set_config(struct opamp2_config *const config);

/**
 * \brief Enables an OPAMP that was previously configured.
 *
 *  Enables an OPAMP that was previously  configured via a call to
 *  the set configuration function.
 *
 *  \param[in] number      OPAMP number to enable.
 */
void opamp_enable(const enum opamp_number number);

/**
 * \brief Disables an OPAMP that was previously enabled.
 *
 *  Disables an OPAMP that was previously enabled via a call to
 *  \ref opamp_enable().
 *
 *  \param[in] number      OPAMP number to enable.
 */
void opamp_disable(const enum opamp_number number);

/**
 * \brief Checks an OPAMP output ready status.
 *
 *  Checks if an OPAMP output is ready.
 *
 *  \param[in] number      OPAMP number to check.
 *
 * \return Ready status of the select OPAMP.
 *
 * \retval false if the select OPAMP output is not ready
 * \retval ture if the select OPAMP output is ready

 */
bool opamp_is_ready(const enum opamp_number number);


#ifdef __cplusplus
}
#endif

/** @} */

/**
 * \page asfdoc_sam0_opamp_extra Extra Information for OPAMP Driver
 *
 * \section asfdoc_sam0_opamp_extra_acronyms Acronyms
 * Below is a table listing the acronyms used in this module, along with their
 * intended meanings.
 *
 * <table>
 *	<tr>
 *		<th>Acronym</th>
 *		<th>Description</th>
 *	</tr>
 *  <tr>
 *		<td>OPAMP</td>
 *		<td>Operational Amplifier Controller</td>
 *	</tr>
 * </table>
 *
 *
 * \section asfdoc_sam0_opamp_extra_dependencies Dependencies
 * This driver has no dependencies.
 *
 *
 * \section asfdoc_sam0_opamp_extra_errata Errata
 * There are no errata related to this driver.
 *
 *
 * \section asfdoc_sam0_opamp_extra_history Module History
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
 * \page asfdoc_sam0_opamp_exqsg Examples for OPAMP Driver
 *
 * This is a list of the available Quick Start guides (QSGs) and example
 * applications for \ref asfdoc_sam0_opamp_group. QSGs are simple examples with
 * step-by-step instructions to configure and use this driver in a selection of
 * use cases. Note that QSGs can be compiled as a standalone application or be
 * added to the user application.
 *
 *  - \subpage asfdoc_sam0_opamp_basic_use_case
 *
 * \page asfdoc_sam0_opamp_document_revision_history Document Revision History
 *
 * <table>
 *	<tr>
 *		<th>Doc. Rev.</td>
 *		<th>Date</td>
 *		<th>Comments</td>
 *	</tr>
 *	<tr>
 *		<td>A</td>
 *		<td>09/2014</td>
 *		<td>Initial release</td>
 *	</tr>
 * </table>
 */

#endif /* OPAMP_H_INCLUDED */

