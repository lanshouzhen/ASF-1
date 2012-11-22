/**
 * \file
 *
 * \brief SAM0+ Serial Peripheral Interface Driver
 *
 * Copyright (C) 2012 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
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

#ifndef SERCOM_H_INCLUDED
#define SERCOM_H_INCLUDED

#include <compiler.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup sam0_sercom_group SAM0+ SERCOM
 *
 * Driver for the SAM0+ architecture devices.
 * This driver encompasses the following module within the SAM0+ devices:
 * \li \b SERCOM
 *
 * \section module_introduction Introduction
 *
 * \subsection module_overview SERCOM Overview
 *
 * \section dependencies Dependencies
 * The SERCOM driver has the following dependencies:
 * \li \ref clock_group "\b Clock" (System Clock Management)
 *
 * \section special_cons Special Considerations
 *
 * \section extra_info Extra Information
 * For extra information see \ref sercom_extra_info.
 *
 * \section module_examples Examples
 * - \ref quickstart
 *
 * \section api_overview API Overview
 * @{
 */
#include "sercom_header.h"
#include "sercom_interrupts.h"


struct i2c_master_dev_inst;

typedef (*i2c_master_callback_t)(
		const struct i2c_master_dev_inst *const dev_inst);

/**
 * \brief SERCOM I2C Master driver hardware instance.
 *
 * Device instance structure for SERCOM I2C Master instance. This structure
 * is used throughout the driver, and should be initiated using the
 * /ref i2c_master_init() function to associate the struct with a particular
 * hardware instance and configurations.
 */
struct i2c_master_dev_inst {
	/** Save sercom mode to use in interrupt handler. */
	SERCOM_MODE_t sercom_mode;
	/** Hardware instance initialized for the struct. */
	SERCOM_t *hw_dev;
	/** Holder for callback functions. */
	i2c_master_callback_t callback[3];
	/** Holder for registered callbacks. */
	uint8_t registered_callback;
	/** Holder for enabled callbacks. */
	uint8_t enabled_callback;
	/** Data buffer for packet write and read. */
	uint8_t **buffer_ptr;
	/** Counter used for bytes left to send in write and to count number of
	  * obtained bytes in read. */
	uint16_t buffer_length;
};

enum status_code sercom_get_sync_baud_val(uint32_t baudrate,
		uint32_t external_clock, uint16_t *baudval);

enum status_code sercom_get_async_baud_val(uint32_t baudrate,
		uint32_t peripheral_clock, uint16_t *baudval);


/** @} */


#ifdef __cplusplus
}
#endif

/** @} */


/**
 * \page sercom_extra_info Extra Information
 *
 * \section acronyms Acronyms
 * Below is a table listing the acronyms used in this module, along with their
 * intended meanings.
 *
 * <table>
 *	<tr>
 *		<th>Acronym</th>
 *		<th>Description</th>
 *	</tr>
 * </table>
 *
 * \section workarounds Workarounds implemented by driver
 * No workarounds in driver.
 *
 * \section module_history Module History
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
 * \page quickstart Quick Start Guides for the SERCOM module
 *
 * This is the quick start guide list for the \ref sam0_sercom_group module, with
 * step-by-step instructions on how to configure and use the driver in a
 * selection of use cases.
 *
 * The use cases contain several code fragments. The code fragments in the
 * steps for setup can be copied into a custom initialization function of the
 * user application and run at system startup, while the steps for usage can be
 * copied into the normal user application program flow.
 *
 * \see General list of module \ref module_examples "examples".
 *
 * \section sercom_use_cases SERCOM driver use cases
 * - \subpage sercom_basic_use_case
 */

#endif //__SERCOM_H_INCLUDED
