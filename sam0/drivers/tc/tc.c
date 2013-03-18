/**
 * \file
 *
 * \brief SAM0+ TC Driver
 *
 * Copyright (C) 2012 Atmel Corporation. All rights reserved.
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

#include "tc.h"

#if !defined(__DOXYGEN__)
#  define _TC_GCLK_ID(n, unused)       TC##n##_GCLK_ID   ,
#  define _TC_PM_APBCMASK(n, unused)   PM_APBCMASK_TC##n ,

/** TODO: Remove once present in device header file */
#  define TC_INST_GCLK_ID              { MREPEAT(TC_INST_NUM, _TC_GCLK_ID    , ~) }

/** TODO: Remove once present in device header file */
#  define TC_INST_PM_APBCMASK          { MREPEAT(TC_INST_NUM, _TC_PM_APBCMASK, ~) }
#endif

/**
 * \internal Find the index of given TC module instance.
 *
 * \param[in] TC module instance pointer.
 *
 * \return Index of the given TC module instance.
 */
static uint8_t _tc_get_inst_index(
		Tc *const hw)
{
	/* List of available TC modules. */
	Tc *const tc_modules[TC_INST_NUM] = TC_INSTS;

	/* Find index for TC instance. */
	for (uint32_t i = 0; i < TC_INST_NUM; i++) {
		if (hw == tc_modules[i]) {
			return i;
		}
	}

	/* Invalid data given. */
	Assert(false);
	return 0;
}

/**
 * \brief Initializes a hardware TC module instance.
 *
 * Enables the clock and initializes the TC module, based on the given
 * configuration values.
 *
 * \param[in,out] module_inst  Pointer to the software module instance struct
 * \param[in]     hw           Pointer to the TC hardware module
 * \param[in]     config       Pointer to the TC configuration options struct
 *
 * \return Status of the initialization procedure.
 *
 * \retval STATUS_OK           The module was initialized successfully
 * \retval STATUS_BUSY         Hardware module was busy when the
 *                             initialization procedure was attempted
 * \retval STATUS_INVALID_ARG  An invalid configuration option or argument
 *                             was supplied
 * \retval STATUS_ERR_DENIED   Hardware module was already enabled, or the
 *                             hardware module is configured in 32 bit
 *                             slave mode
 */
enum status_code tc_init(
		struct tc_module *const module_inst,
		Tc *const hw,
		const struct tc_config *const config)
{
	/* Sanity check arguments */
	Assert(hw);
	Assert(module_inst);
	Assert(config);

	/* Temporary variable to hold all updates to the CTRLA
	 * register before they are written to it */
	uint16_t ctrla_tmp = 0;
	/* Temporary variable to hold all updates to the CTRLBSET
	 * register before they are written to it */
	uint8_t ctrlbset_tmp = 0;
	/* Temporary variable to hold all updates to the EVCTRL
	 * register before they are written to it */
	uint8_t evctrl_tmp = 0;
	/* Temporary variable to hold all updates to the CTRLC
	 * register before they are written to it */
	uint8_t ctrlc_tmp = 0;
	/* Temporary variable to hold TC instance number */
	uint8_t instance = _tc_get_inst_index(hw);

	/* Array of GLCK ID for different TC instances */
	uint8_t inst_gclk_id[] = TC_INST_GCLK_ID;
	/* Array of PM APBC mask bit position for different TC instances */
	uint16_t inst_pm_apbmask[] = TC_INST_PM_APBCMASK;

	struct system_pinmux_config pin_config;
	struct system_gclk_chan_config gclk_chan_config;

	/* Associate the given device instance with the hardware module */
	module_inst->hw = hw;

	/* Check if odd numbered TC modules are being configured in 32-bit
	 * counter size. Only even numbered counters are allowed to be configured
	 * in 32-bit counter size.
	 */
	if ((config->counter_size == TC_COUNTER_SIZE_32BIT) &&
			(instance & 0x01)) {
		Assert(false);
		return STATUS_ERR_INVALID_ARG;
	}

	/* Make the counter size variable in the module_inst struct reflect
	 * the counter size in the module
	 */
	module_inst->counter_size = config->counter_size;

	if (hw->COUNT8.CTRLA.reg & TC_CTRLA_SWRST) {
		/* We are in the middle of a reset. Abort. */
		return STATUS_BUSY;
	}

	if (hw->COUNT8.STATUS.reg & TC_STATUS_SLAVE) {
		/* Module is used as a slave */
		return STATUS_ERR_DENIED;
	}

	if (hw->COUNT8.CTRLA.reg & TC_CTRLA_ENABLE) {
		/* Module must be disabled before initialization. Abort. */
		return STATUS_ERR_DENIED;
	}


	/* Set up the TC PWM out pin for channel 0 */
	if (config->channel_pwm_out_enabled[0]) {
		system_pinmux_get_config_defaults(&pin_config);
		pin_config.mux_position = config->channel_pwm_out_mux[0];
		pin_config.direction = SYSTEM_PINMUX_PIN_DIR_OUTPUT;
		system_pinmux_pin_set_config(config->channel_pwm_out_pin[0], &pin_config);
	}

	/* Set up the TC PWM out pin for channel 1 */
	if (config->channel_pwm_out_enabled[1]) {
		system_pinmux_get_config_defaults(&pin_config);
		pin_config.mux_position = config->channel_pwm_out_mux[1];
		pin_config.direction = SYSTEM_PINMUX_PIN_DIR_OUTPUT;
		system_pinmux_pin_set_config(config->channel_pwm_out_pin[1], &pin_config);
	}

	/* Enable the user interface clock in the PM */
	system_apb_clock_set_mask(SYSTEM_CLOCK_APB_APBC,
			inst_pm_apbmask[instance]);

	/* Setup clock for module */
	system_gclk_chan_get_config_defaults(&gclk_chan_config);
	gclk_chan_config.source_generator = config->clock_source;
	gclk_chan_config.run_in_standby   = config->run_in_standby;
	system_gclk_chan_set_config(inst_gclk_id[instance], &gclk_chan_config);
	system_gclk_chan_enable(inst_gclk_id[instance]);

	if (config->run_in_standby) {
		ctrla_tmp |= TC_CTRLA_RUNSTDBY;
	}

	ctrla_tmp = config->counter_size | config->wave_generation
			| config->reload_action | config->clock_prescaler;

	while (tc_is_syncing(module_inst)) {
		/* Wait for sync */
	}

	/* Set configuration to registers common for all 3 modes */
	hw->COUNT8.CTRLA.reg = ctrla_tmp;

	/* Set ctrlb register */
	if (config->oneshot) {
		ctrlbset_tmp = TC_CTRLBSET_ONESHOT;
	}

	if (config->count_direction) {
		ctrlbset_tmp |= TC_CTRLBSET_DIR;
	}

	while (tc_is_syncing(module_inst)) {
		/* Wait for sync */
	}
	hw->COUNT8.CTRLBCLR.reg = 0xFF;

	/* Check if we actually need to go into a wait state. */
	if (ctrlbset_tmp) {
		while (tc_is_syncing(module_inst)) {
			/* Wait for sync */
		}

		/* Check if we actually need to go into a wait state. */
		hw->COUNT8.CTRLBSET.reg = ctrlbset_tmp;
	}

	ctrlc_tmp = config->waveform_invert_output | config->capture_enable;

	while (tc_is_syncing(module_inst)) {
		/* Wait for sync */
	}

	hw->COUNT8.CTRLC.reg = ctrlc_tmp;

	if (config->invert_event_input) {
		evctrl_tmp |= TC_EVCTRL_TCINV;
	}

	while (tc_is_syncing(module_inst)) {
		/* Wait for sync */
	}

	hw->COUNT8.EVCTRL.reg = evctrl_tmp | config->event_action;

	/* Switch for TC counter size  */
	switch (module_inst->counter_size) {
		case TC_COUNTER_SIZE_8BIT:
			while (tc_is_syncing(module_inst)) {
				/* Wait for sync */
			}

			hw->COUNT8.COUNT.reg =
					config->size_specific.size_8_bit.count;


			while (tc_is_syncing(module_inst)) {
				/* Wait for sync */
			}

			hw->COUNT8.PER.reg =
					config->size_specific.size_8_bit.period;

			while (tc_is_syncing(module_inst)) {
				/* Wait for sync */
			}

			hw->COUNT8.CC[0].reg
				= config->size_specific.size_8_bit.compare_capture_channel[0];

			while (tc_is_syncing(module_inst)) {
				/* Wait for sync */
			}

			hw->COUNT8.CC[1].reg
				= config->size_specific.size_8_bit.compare_capture_channel[1];

			return STATUS_OK;

		case TC_COUNTER_SIZE_16BIT:
			while (tc_is_syncing(module_inst)) {
				/* Wait for sync */
			}

			hw->COUNT16.COUNT.reg
				= config->size_specific.size_16_bit.count;

			while (tc_is_syncing(module_inst)) {
				/* Wait for sync */
			}

			hw->COUNT16.CC[0].reg
				= config->size_specific.size_16_bit.compare_capture_channel[0];

			while (tc_is_syncing(module_inst)) {
				/* Wait for sync */
			}

			hw->COUNT16.CC[1].reg
				= config->size_specific.size_16_bit.compare_capture_channel[1];

			return STATUS_OK;

		case TC_COUNTER_SIZE_32BIT:
			while (tc_is_syncing(module_inst)) {
				/* Wait for sync */
			}

			hw->COUNT32.COUNT.reg
				= config->size_specific.size_32_bit.count;

			while (tc_is_syncing(module_inst)) {
				/* Wait for sync */
			}

			hw->COUNT32.CC[0].reg
				= config->size_specific.size_32_bit.compare_capture_channel[0];

			while (tc_is_syncing(module_inst)) {
				/* Wait for sync */
			}

			hw->COUNT32.CC[1].reg
				= config->size_specific.size_32_bit.compare_capture_channel[1];

			return STATUS_OK;
	}

	Assert(false);
	return STATUS_ERR_INVALID_ARG;
}

/**
 * \brief Set TC module count value.
 *
 * Sets the current timer count value of a initialized TC module. The
 * specified TC module may be started or stopped.
 *
 * \param[in] module_inst  Pointer to the software module instance struct
 * \param[in] count        New timer count value to set
 *
 * \return Status of the count update procedure.
 *
 * \retval STATUS_OK               The timer count was updated successfully
 * \retval STATUS_ERR_INVALID_ARG  An invalid timer counter size was specified
 */
enum status_code tc_set_count_value(
		const struct tc_module *const module_inst,
		const uint32_t count)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	/* Get a pointer to the module's hardware instance*/
	Tc *const tc_module = module_inst->hw;

	while (tc_is_syncing(module_inst)) {
		/* Wait for sync */
	}

	/* Write to based on the TC counter_size */
	switch (module_inst->counter_size) {
		case TC_COUNTER_SIZE_8BIT:
			tc_module->COUNT8.COUNT.reg  = (uint8_t)count;
			return STATUS_OK;

		case TC_COUNTER_SIZE_16BIT:
			tc_module->COUNT16.COUNT.reg = (uint16_t)count;
			return STATUS_OK;

		case TC_COUNTER_SIZE_32BIT:
			tc_module->COUNT32.COUNT.reg = (uint32_t)count;
			return STATUS_OK;

		default:
			return STATUS_ERR_INVALID_ARG;
	}
}

/**
 * \brief Get TC module count value.
 *
 * Retrieves the current count value of a TC module. The specified TC module
 * may be started or stopped.
 *
 * \param[in] module_inst  Pointer to the software module instance struct
 *
 * \return Count value of the specified TC module.
 */
uint32_t tc_get_count_value(
		const struct tc_module *const module_inst)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	/* Get a pointer to the module's hardware instance */
	Tc *const tc_module = module_inst->hw;

	while (tc_is_syncing(module_inst)) {
		/* Wait for sync */
	}

	/* Read from based on the TC counter size */
	switch (module_inst->counter_size) {
		case TC_COUNTER_SIZE_8BIT:
			return (uint32_t)tc_module->COUNT8.COUNT.reg;

		case TC_COUNTER_SIZE_16BIT:
			return (uint32_t)tc_module->COUNT16.COUNT.reg;

		case TC_COUNTER_SIZE_32BIT:
			return tc_module->COUNT32.COUNT.reg;
	}

	Assert(false);
	return 0;
}

/**
 * \brief Gets the TC module capture value.
 *
 * Retrieves the capture value in the indicated TC module capture channel.
 *
 * \param[in]  module_inst    Pointer to the software module instance struct
 * \param[in]  channel_index  Index of the Compare Capture channel to read
 *
 * \return Capture value stored in the specified timer channel.
 */
uint32_t tc_get_capture_value(
		const struct tc_module *const module_inst,
		const enum tc_compare_capture_channel channel_index)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	/* Get a pointer to the module's hardware instance */
	Tc *const tc_module = module_inst->hw;

	while (tc_is_syncing(module_inst)) {
		/* Wait for sync */
	}

	/* Read out based on the TC counter size */
	switch (module_inst->counter_size) {
		case TC_COUNTER_SIZE_8BIT:
			if (channel_index < 2) {
				return tc_module->COUNT8.CC[channel_index].reg;
			}

		case TC_COUNTER_SIZE_16BIT:
			if (channel_index < 2) {
				return tc_module->COUNT16.CC[channel_index].reg;
			}

		case TC_COUNTER_SIZE_32BIT:
			if (channel_index < 2) {
				return tc_module->COUNT32.CC[channel_index].reg;
			}
	}

	Assert(false);
	return 0;
}

/**
 * \brief Set a TC module compare value.
 *
 * Writes a compare value to the given TC module compare/capture channel.
 *
 * \param[in]  module_inst    Pointer to the software module instance struct
 * \param[in]  channel_index  Index of the compare channel to write to
 * \param[in]  compare        New compare value to set
 *
 * \return Status of the compare update procedure.
 *
 * \retval  STATUS_OK               The compare value was updated successfully
 * \retval  STATUS_ERR_INVALID_ARG  An invalid channel index was supplied
 */
enum status_code tc_set_compare_value(
		const struct tc_module *const module_inst,
		const enum tc_compare_capture_channel channel_index,
		const uint32_t compare)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);
	Assert(compare);

	/* Get a pointer to the module's hardware instance */
	Tc *const tc_module = module_inst->hw;

	while (tc_is_syncing(module_inst)) {
		/* Wait for sync */
	}

	/* Read out based on the TC counter size */
	switch (module_inst->counter_size) {
		case TC_COUNTER_SIZE_8BIT:
			if (channel_index < 2) {
				tc_module->COUNT8.CC[channel_index].reg  = (uint8_t)compare;
				return STATUS_OK;
			}

		case TC_COUNTER_SIZE_16BIT:
			if (channel_index < 2) {
				tc_module->COUNT16.CC[channel_index].reg = (uint16_t)compare;
				return STATUS_OK;
			}

		case TC_COUNTER_SIZE_32BIT:
			if (channel_index < 2) {
				tc_module->COUNT16.CC[channel_index].reg = (uint32_t)compare;
				return STATUS_OK;
			}
	}

	return STATUS_ERR_INVALID_ARG;
}

/**
 * \brief Reset the TC module.
 *
 * Resets the TC module, restoring all hardware module registers to their
 * default values and disabling the module. The TC module will not be
 * accessible while the reset is being performed.
 *
 * \note When resetting a 32-bit counter only the master TC module's instance
 *       structure should be used.
 *
 * \param[in]  module_inst    Pointer to the software module instance struct
 *
 * \return Status of the procedure
 * \retval STATUS_OK                   The module was reset successfully
 * \retval STATUS_ERR_UNSUPPORTED_DEV  A 32-bit slave TC module was supplied
 */
enum status_code tc_reset(
		const struct tc_module *const module_inst)
{
	/* Sanity check arguments  */
	Assert(module_inst);
	Assert(module_inst->hw);

	/* Get a pointer to the module hardware instance */
	TcCount8 *const tc_module = &(module_inst->hw->COUNT8);

	/* Disable this module */
	tc_disable(module_inst);

	if (tc_module->STATUS.reg & TC_STATUS_SLAVE) {
		return STATUS_ERR_UNSUPPORTED_DEV;
	}

	/* Reset TC slave if one exists */
	if (tc_module->CTRLA.reg & TC_COUNTER_SIZE_32BIT) {
		while (tc_is_syncing(module_inst)) {
			/* Wait for sync */
		}

		/* Reset this TC module */
		tc_module->CTRLA.reg  |= TC_CTRLA_SWRST;

		/* Get the slave hw pointer */
		Tc *const tc_modules[TC_INST_NUM] = TC_INSTS;
		Tc *const slave = tc_modules[_tc_get_inst_index(module_inst->hw) + 1];

		while (slave->COUNT8.STATUS.reg & TC_STATUS_SYNCBUSY) {
			/* Wait for sync */
		}

		/* Reset slave */
		slave->COUNT8.CTRLA.reg |= TC_CTRLA_SWRST;
	}
	else {
		while (tc_is_syncing(module_inst)) {
			/* Wait for sync */
		}

		/* Reset this TC module */
		tc_module->CTRLA.reg  |= TC_CTRLA_SWRST;
	}

	return STATUS_OK;
}

/**
 * \brief Set the timer TOP/period value.
 *
 * For 8-bit counter size this function writes the top value to the period
 * register.
 *
 * For 16- and 32-bit counter size this function writes the top value to
 * Capture Compare register 0. The value in this register can not be used for
 * any other purpose.
 *
 * \note This function is designed to be used in PWM or frequency
 *       match modes only. When the counter is set to 16- or 32-bit counter
 *       size. In 8-bit counter size it will always be possible to change the
 *       top value even in normal mode.
 *
 * \param[in]  module_inst   Pointer to the software module instance struct
 * \param[in]  top_value     New timer TOP value to set
 *
 * \return Status of the TOP set procedure.
 *
 * \retval STATUS_OK              The timer TOP value was updated successfully
 * \retval STATUS_ERR_INVALID_ARG The configured TC module counter size in the
 *                                module instance is invalid.
 */
enum status_code tc_set_top_value (
		const struct tc_module *const module_inst,
		const uint32_t top_value)
{
	Assert(module_inst);
	Assert(module_inst->hw);
	Assert(compare);

	Tc *const tc_module = module_inst->hw;

	while (tc_is_syncing(module_inst)) {
		/* Wait for sync */
	}

	switch (module_inst->counter_size) {
		case TC_COUNTER_SIZE_8BIT:
			tc_module->COUNT8.PER.reg    = (uint8_t)top_value;
			return STATUS_OK;

		case TC_COUNTER_SIZE_16BIT:
			tc_module->COUNT16.CC[0].reg = (uint16_t)top_value;
			return STATUS_OK;

		case TC_COUNTER_SIZE_32BIT:
			tc_module->COUNT32.CC[0].reg = (uint32_t)top_value;
			return STATUS_OK;

		default:
			Assert(false);
			return STATUS_ERR_INVALID_ARG;
	}
}
