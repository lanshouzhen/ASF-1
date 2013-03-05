/**
 * \file
 *
 * \brief SAMD20 I2C Serial Peripheral Interface Driver
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

#ifndef I2C_MASTER_INTERRUPT_H_INCLUDED
#define I2C_MASTER_INTERRUPT_H_INCLUDED

#include "i2c_master.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup asfdoc_sam0_i2c_master_interrupt_group I2C Master Interrupt
 * @{
 *
 * This is an overview of the API for the I2C master interrupt
 * driver.
 *
 * The user will still have to use the initialization from the basic use part
 * of the driver, which can be found \ref asfdoc_sam0_i2c_master_group "here". When the
 * interrupt driver is included in the project, the interrupt part of the basic driver
 * will be unlocked. This will allow the user to implement the driver in the same way as
 * for the basic polled driver, but will additionally get the interrupt functionality
 * documented below.
 *
 * \section asfdoc_sam0_i2c_master_interrupt_api_overview I2C Master Interrupt API Overview
 *
 */

/**
 * \name Callbacks
 * @{
 */
#if !defined(__DOXYGEN__)
void _i2c_master_interrupt_handler(uint8_t instance);
#endif

void i2c_master_register_callback(
		struct i2c_master_module *const module,
		i2c_master_callback_t callback,
		enum i2c_master_callback callback_type);

void i2c_master_unregister_callback(
		struct i2c_master_module *const module,
		enum i2c_master_callback callback_type);

/**
 * \brief Enable callback.
 *
 * Enables the callback specified by the callback_value.
 *
 * \param[in,out]  module      Pointer to the software module struct.
 * \param[in]      callback_type Callback type to enable.
 */
static inline void i2c_master_enable_callback(
		struct i2c_master_module *const module,
		enum i2c_master_callback callback_type)
{
	/* Sanity check. */
	Assert(module);
	Assert(module->hw);

	/* Mark callback as enabled. */
	module->enabled_callback |= (1 << callback_type);
}


/**
 * \brief Disable callback.
 *
 * Disables the callback specified by the callback_type.
 *
 * \param[in,out]  module      Pointer to the software module struct.
 * \param[in]      callback_type Callback type to disable.
 */
static inline void i2c_master_disable_callback(
		struct i2c_master_module *const module,
		enum i2c_master_callback callback_type)
{
	/* Sanity check. */
	Assert(module);
	Assert(module->hw);

	/* Mark callback as disabled. */
	module->enabled_callback &= ~(1 << callback_type);
}

/** @} */

/**
* \name Read and Write, Interrupt-Driven
* @{
*/

enum status_code i2c_master_read_packet_job(
		struct i2c_master_module *const module,
		struct i2c_packet *const packet);

enum status_code i2c_master_read_packet_job_no_stop(
		struct i2c_master_module *const module,
		struct i2c_packet *const packet);

enum status_code i2c_master_write_packet_job(
		struct i2c_master_module *const module,
		struct i2c_packet *const packet);

enum status_code i2c_master_write_packet_job_no_stop(
		struct i2c_master_module *const module,
		struct i2c_packet *const packet);

/**
 * \brief Cancel the currently running operation.
 *
 * This will terminate the running transfer operation.
 *
 * \param  module Pointer to software module structure.
 */
static inline void i2c_master_abort_job(
		struct i2c_master_module *const module)
{
	/* Sanity check. */
	Assert(module);
	Assert(module->hw);

	/* Set buffer to 0. */
	module->buffer_remaining = 0;
	/* Update status*/
	module->status = STATUS_ABORTED;
}

/**
 * \brief Get last error from ongoing job.
 *
 * Will return the last error that occurred in a transfer operation. The
 * status will be cleared on next operation.
 *
 * \param  module Pointer to software module structure.
 *
 * \return          Last status code from transfer operation.
 * \retval STATUS_OK No error has occurred.
 * \retval STATUS_BUSY If transfer is in progress.
 * \retval STATUS_BUSY If master module is busy.
 * \retval STATUS_ERR_DENIED If error on bus.
 * \retval STATUS_ERR_PACKET_COLLISION If arbitration is lost.
 * \retval STATUS_ERR_BAD_ADDRESS If slave is busy, or no slave acknowledged the
 *                                address.
 * \retval STATUS_ERR_TIMEOUT If timeout occurred.
 * \retval STATUS_ERR_OVERFLOW If slave did not acknowledge last sent data,
 *                             indicating that slave do not want more data.
 */
static inline enum status_code i2c_master_get_job_status(
		struct i2c_master_module *const module)
{
	/* Check sanity. */
	Assert(module);
	Assert(module->hw);

	/* Return current status code. */
	return module->status;
}

/** @} */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* I2C_MASTER_INTERRUPT_H_INCLUDED */
