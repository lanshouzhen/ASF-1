/**
 * \file
 *
 * \brief SAM D20 Clock Driver
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
#include <clock.h>
#include <conf_clocks.h>

/** \internal
 *  Internal variable to cache XOSC frequency.
 */
static uint32_t xosc_frequency = 0;

/** \internal
 *  Internal variable to cache XOSC32 frequency.
 */
static uint32_t xosc32k_frequency = 0;

/**
 * \internal
 * \brief Wait for sync to the DFLL control registers
 */
static inline void _system_dfll_wait_for_sync(void)
{
	while (!(SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_DFLLRDY)) {
		/* Wait for DFLL sync */
	}
}

/**
 * \internal
 * \brief Wait for sync to the OSC32K control registers
 */
static inline void _system_osc32k_wait_for_sync(void)
{
	while (!(SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_OSC32KRDY)) {
		/* Wait for OSC32K sync */
	}
}

/**
 * \brief Retrieve the frequency of a clock source
 *
 * Determines the current operating frequency of a given clock source.
 *
 * \param[in] clock_source  Clock source to get the frequency of
 *
 * \returns Frequency of the given clock source, in Hz
 */
uint32_t system_clock_source_get_hz(
		const enum system_clock_source clock_source)
{
	uint32_t temp = 0;

	switch (clock_source) {
		case SYSTEM_CLOCK_SOURCE_XOSC:
			return xosc_frequency;

		case SYSTEM_CLOCK_SOURCE_OSC8M:
			temp = (SYSCTRL->OSC8M.reg & SYSCTRL_OSC8M_PRESC_Msk) >> SYSCTRL_OSC8M_PRESC_Pos;
			if (temp) {
				return 8000000 / (1 << temp);
			} else {
				return 8000000;
			}

		case SYSTEM_CLOCK_SOURCE_OSC32K:
			return 32768UL;

		case SYSTEM_CLOCK_SOURCE_ULP32K:
			return 32768UL;

		case SYSTEM_CLOCK_SOURCE_XOSC32K:
			return xosc32k_frequency;

		case SYSTEM_CLOCK_SOURCE_DFLL:
			_system_dfll_wait_for_sync();

			/* Check if operating in closed loop mode */
			if (SYSCTRL->DFLLCTRL.reg & SYSCTRL_DFLLCTRL_MODE) {
				SYSCTRL->DFLLSYNC.bit.READREQ = 1;
				_system_dfll_wait_for_sync();
				temp = SYSCTRL->DFLLMUL.bit.MUL;
				return system_gclk_chan_get_hz(SYSCTRL_GCLK_ID_DFLL48) * temp;
			}

			return 48000000UL;

		default:
			return 0;
	}
}

/**
 * \brief Configure the internal OSC8M oscillator clock source
 *
 * Configures the 8MHz (nominal) internal RC oscillator with the given
 * configuration settings.
 *
 * \param[in] config  OSC8M configuration structure containing the new config
 */
void system_clock_source_osc8m_set_config(
		struct system_clock_source_osc8m_config *const config)
{
	SYSCTRL_OSC8M_Type temp = SYSCTRL->OSC8M;

	/* Use temporary struct to reduce register access */
	temp.bit.PRESC = config->prescaler;
	temp.bit.ONDEMAND = config->on_demand;
	temp.bit.RUNSTDBY = config->run_in_standby;

	SYSCTRL->OSC8M = temp;
}

/**
 * \brief Configure the internal OSC32K oscillator clock source
 *
 * Configures the 32KHz (nominal) internal RC oscillator with the given
 * configuration settings.
 *
 * \param[in] config  OSC32K configuration structure containing the new config
 */
void system_clock_source_osc32k_set_config(
		struct system_clock_source_osc32k_config *const config)
{
	SYSCTRL_OSC32K_Type temp = SYSCTRL->OSC32K;

	/* Update settings via a temporary struct to reduce register access */
	temp.bit.EN1K     = config->enable_1khz_output;
	temp.bit.EN32K    = config->enable_32khz_output;
	temp.bit.STARTUP  = config->startup_time;
	temp.bit.ONDEMAND = config->on_demand;
	temp.bit.RUNSTDBY = config->run_in_standby;

	SYSCTRL->OSC32K  = temp;
}

/**
 * \brief Configure the external oscillator clock source
 *
 * Configures the external oscillator clock source with the given configuration
 * settings.
 *
 * \param[in] config  External oscillator configuration structure containing
 *                    the new config
 */
void system_clock_source_xosc_set_config(
		struct system_clock_source_xosc_config *const config)
{
	SYSCTRL_XOSC_Type temp = SYSCTRL->XOSC;

	temp.bit.STARTUP = config->startup_time;

	if (config->external_clock == SYSTEM_CLOCK_EXTERNAL_CRYSTAL) {
		temp.bit.XTALEN = 1;
	} else {
		temp.bit.XTALEN = 0;
	}

	temp.bit.AMPGC = config->auto_gain_control;

	/* Set gain if automatic gain control is not selected */
	if (!config->auto_gain_control) {
		if (config->frequency <= 2000000) {
			temp.bit.GAIN = 0;
		} else if (config->frequency <= 4000000) {
			temp.bit.GAIN = 1;
		} else if (config->frequency <= 8000000) {
			temp.bit.GAIN = 2;
		} else if (config->frequency <= 16000000) {
			temp.bit.GAIN = 3;
		} else if (config->frequency <= 30000000) {
			temp.bit.GAIN = 4;
		}

	}

	temp.bit.ONDEMAND = config->on_demand;
	temp.bit.RUNSTDBY = config->run_in_standby;

	/* Store XOSC frequency for internal use */
	xosc_frequency = config->frequency;

	SYSCTRL->XOSC = temp;
}

/**
 * \brief Configure the XOSC32K external 32KHz oscillator clock source
 *
 * Configures the external 32KHz oscillator clock source with the given
 * configuration settings.
 *
 * \param[in] config  XOSC32K configuration structure containing the new config
 */
void system_clock_source_xosc32k_set_config(
		struct system_clock_source_xosc32k_config *const config)
{

	SYSCTRL_XOSC32K_Type temp = SYSCTRL->XOSC32K;

	temp.bit.STARTUP = config->startup_time;

	if (config->external_clock == SYSTEM_CLOCK_EXTERNAL_CRYSTAL) {
		temp.bit.XTALEN = 1;
	} else {
		temp.bit.XTALEN = 0;
	}

	temp.bit.AAMPEN = config->auto_gain_control;
	temp.bit.EN1K = config->enable_1khz_output;
	temp.bit.EN32K = config->enable_32khz_output;

	temp.bit.ONDEMAND = config->on_demand;
	temp.bit.RUNSTDBY = config->run_in_standby;

	/* Cache the new frequency in case the user needs to check the current
	 * operating frequency later */
	xosc32k_frequency = config->frequency;

	SYSCTRL->XOSC32K = temp;
}

/**
 * \brief Configure the DFLL clock source
 *
 * Configures the Digital Frequency Locked Loop clock source with the given
 * configuration settings.
 *
 * \note The DFLL will be running when this function returns, as the DFLL module
 *       needs to be enabled in order to perform the module configuration.
 *
 * \param[in] config  DFLL configuration structure containing the new config
 */
void system_clock_source_dfll_set_config(
		struct system_clock_source_dfll_config *const config)
{
	uint32_t temp;

	/* Enable the DFLL, as all the DFLL core registers are clocked
	 * by the DFLL output */
	system_clock_source_enable(SYSTEM_CLOCK_SOURCE_DFLL);

	/* Write Fine and Coarse values for open loop mode */
	_system_dfll_wait_for_sync();
	SYSCTRL->DFLLVAL.reg =
			SYSCTRL_DFLLVAL_COARSE(config->coarse_value) |
			SYSCTRL_DFLLVAL_FINE(config->fine_value);

	temp =
			(uint32_t)config->wakeup_lock     |
			(uint32_t)config->stable_tracking |
			(uint32_t)config->quick_lock      |
			(uint32_t)config->chill_cycle     |
			(uint32_t)config->run_in_standby << SYSCTRL_DFLLCTRL_RUNSTDBY_Pos |
			(uint32_t)config->on_demand << SYSCTRL_DFLLCTRL_ONDEMAND_Pos;

	if (config->loop_mode == SYSTEM_CLOCK_DFLL_LOOP_MODE_CLOSED) {
		_system_dfll_wait_for_sync();
		SYSCTRL->DFLLMUL.reg =
				SYSCTRL_DFLLMUL_CSTEP(config->coarse_max_step) |
				SYSCTRL_DFLLMUL_FSTEP(config->fine_max_step)   |
				SYSCTRL_DFLLMUL_MUL(config->multiply_factor);

		/* Enable the closed loop mode */
		temp |= config->loop_mode;
	}

	/* Set configuration to DFLL */
	_system_dfll_wait_for_sync();
	SYSCTRL->DFLLCTRL.reg |= temp;
}

/**
 * \brief Writes the calibration values for a given oscillator clock source
 *
 * Writes an oscillator calibration value to the given oscillator control
 * registers. The acceptable ranges are:
 *
 * For OSC32K:
 *  - 7 bits (max value 128)
 * For OSC8MHZ:
 *  - 8 bits (Max value 255)
 * For OSCULP:
 *  - 5 bits (Max value 32)
 *
 * \note The frequency range parameter applies only when configuring the 8MHz
 *       oscillator and will be ignored for the other oscillators.
 *
 * \param[in] clock_source       Clock source to calibrate
 * \param[in] calibration_value  Calibration value to write
 * \param[in] freq_range         Frequency range (8MHz oscillator only)
 *
 * \retval STATUS_ERR_INVALID_ARG  The selected clock source is not available
 */
enum status_code system_clock_source_write_calibration(
		const enum system_clock_source clock_source,
		const uint16_t calibration_value,
		const uint8_t freq_range)
{

	switch (clock_source) {
		case SYSTEM_CLOCK_SOURCE_OSC8M:

			if (calibration_value > 0xfff || freq_range > 4) {
				return STATUS_ERR_INVALID_ARG;
			}

			SYSCTRL->OSC8M.bit.CALIB = calibration_value;
			SYSCTRL->OSC8M.bit.FRANGE = freq_range;
			break;

		case SYSTEM_CLOCK_SOURCE_OSC32K:

			if (calibration_value > 128) {
				return STATUS_ERR_INVALID_ARG;
			}

			_system_osc32k_wait_for_sync();
			SYSCTRL->OSC32K.bit.CALIB = calibration_value;
			break;

		case SYSTEM_CLOCK_SOURCE_ULP32K:

			if (calibration_value > 32) {
				return STATUS_ERR_INVALID_ARG;
			}
			SYSCTRL->OSCULP32K.bit.CALIB = calibration_value;
			break;

		default:
			Assert(!"Invalid clock source provided");
			return STATUS_ERR_INVALID_ARG;
			break;
	}

	return STATUS_OK;
}

/**
 * \brief Enables a clock source
 *
 * Enables a clock source which has been previously configured.
 *
 * \param[in] clock_source       Clock source to enable
 *
 * \retval STATUS_OK               Clock source was enabled successfully and
 *                                 is ready
 * \retval STATUS_ERR_INVALID_ARG  The clock source is not available on this
 *                                 device
 */
enum status_code system_clock_source_enable(
		const enum system_clock_source clock_source)
{
	switch (clock_source) {
		case SYSTEM_CLOCK_SOURCE_OSC8M:
			SYSCTRL->OSC8M.reg |= SYSCTRL_OSC8M_ENABLE;
			return STATUS_OK;

		case SYSTEM_CLOCK_SOURCE_OSC32K:
			SYSCTRL->OSC32K.reg |= SYSCTRL_OSC32K_ENABLE;
			break;

		case SYSTEM_CLOCK_SOURCE_XOSC:
			SYSCTRL->XOSC.reg |= SYSCTRL_XOSC_ENABLE;
			break;

		case SYSTEM_CLOCK_SOURCE_XOSC32K:
			SYSCTRL->XOSC32K.reg |= SYSCTRL_XOSC32K_ENABLE;
			break;

		case SYSTEM_CLOCK_SOURCE_DFLL:
			_system_dfll_wait_for_sync();

			/* Will erase current config as read-modify-write is not possible
			   while DFLL is not running */
			SYSCTRL->DFLLCTRL.reg = SYSCTRL_DFLLCTRL_ENABLE;
			break;
		case SYSTEM_CLOCK_SOURCE_ULP32K:
			/* Always enabled */
			return STATUS_OK;

		default:
			Assert(!"Invalid clock source supplied");
			return STATUS_ERR_INVALID_ARG;
	}

	return STATUS_OK;
}

/**
 * \brief Disables a clock source
 *
 * Disables a clock source that was previously enabled.
 *
 * \param[in] clock_source  Clock source to disable
 *
 * \retval STATUS_OK               Clock source was disabled successfully
 * \retval STATUS_ERR_INVALID_ARG  An invalid or unavailable clock source was
 *                                 given
 */
enum status_code system_clock_source_disable(
		const enum system_clock_source clock_source)
{
	switch (clock_source) {
		case SYSTEM_CLOCK_SOURCE_OSC8M:
			SYSCTRL->OSC8M.reg &= ~SYSCTRL_OSC8M_ENABLE;
			break;

		case SYSTEM_CLOCK_SOURCE_OSC32K:
			SYSCTRL->OSC32K.reg &= ~SYSCTRL_OSC32K_ENABLE;
			break;

		case SYSTEM_CLOCK_SOURCE_XOSC:
			SYSCTRL->XOSC.reg &= ~SYSCTRL_XOSC_ENABLE;
			break;

		case SYSTEM_CLOCK_SOURCE_XOSC32K:
			SYSCTRL->XOSC32K.reg &= ~SYSCTRL_XOSC32K_ENABLE;
			break;

		case SYSTEM_CLOCK_SOURCE_DFLL:
			SYSCTRL->DFLLCTRL.reg &= ~SYSCTRL_DFLLCTRL_ENABLE;
			break;

		case SYSTEM_CLOCK_SOURCE_ULP32K:
			/* Not possible to disable */
			return STATUS_ERR_INVALID_ARG;

		default:
			return STATUS_ERR_INVALID_ARG;
	}

	return STATUS_OK;
}

/**
 * \brief Checks if a clock source is ready
 *
 * Checks if a given clock source is ready to be used.
 *
 * \param[in] clock_source  Clock source to check if ready
 *
 * \returns Ready state of the given clock source.
 *
 * \retval true   Clock source is enabled and ready
 * \retval false  Clock source is disabled or not yet ready
 */
bool system_clock_source_is_ready(
		const enum system_clock_source clock_source)
{
	uint32_t mask;

	switch (clock_source) {
		case SYSTEM_CLOCK_SOURCE_OSC8M:
			return true;

		case SYSTEM_CLOCK_SOURCE_OSC32K:
			mask = SYSCTRL_PCLKSR_OSC32KRDY;
			break;

		case SYSTEM_CLOCK_SOURCE_XOSC:
			mask = SYSCTRL_PCLKSR_XOSCRDY;
			break;

		case SYSTEM_CLOCK_SOURCE_XOSC32K:
			mask = SYSCTRL_PCLKSR_XOSC32KRDY;
			break;

		case SYSTEM_CLOCK_SOURCE_DFLL:
			mask = SYSCTRL_PCLKSR_DFLLRDY;
			break;

		case SYSTEM_CLOCK_SOURCE_ULP32K:
			/* Not possible to disable */
			return true;

		default:
			return false;
		}
	if ((SYSCTRL->PCLKSR.reg & mask) != 0) {
		return true;
	} else {
		return false;
	}
}

/**
 * \brief Initialize clock system based on the configuration in conf_clocks.h
 *
 * This function will apply the settings in conf_clock.h when run from the user
 * application. All clock sources and GCLK generators are running when this function
 * returns.
 */
void system_clock_init(void)
{
	system_flash_set_waitstates(CONF_CLOCK_FLASH_WAIT_STATES);

	/* XOSC */
#if CONF_CLOCK_XOSC_ENABLE == true
	struct system_clock_source_xosc_config xosc_conf;

	system_clock_source_xosc_get_default_config(&xosc_conf);
	xosc_conf.external_clock       = CONF_CLOCK_XOSC_EXTERNAL_CRYSTAL;
	xosc_conf.startup_time         = CONF_CLOCK_XOSC_STARTUP_TIME;
	xosc_conf.auto_gain_control    = CONF_CLOCK_XOSC_AUTO_GAIN_CONTROL;
	xosc_conf.frequency            = CONF_CLOCK_XOSC_EXTERNAL_FREQUENCY;
	xosc_conf.on_demand            = CONF_CLOCK_XOSC_ON_DEMAND;
	xosc_conf.run_in_standby       = CONF_CLOCK_XOSC_RUN_IN_STANDBY;

	system_clock_source_xosc_set_config(&xosc_conf);
#endif


	/* XOSC32K */
#if CONF_CLOCK_XOSC32K_ENABLE == true
	struct system_clock_source_xosc32k_config xosc32k_conf;
	xosc32k_conf.frequency             = 32768UL;
	xosc32k_conf.external_clock        = CONF_CLOCK_XOSC32K_EXTERNAL_CRYSTAL;
	xosc32k_conf.startup_time          = CONF_CLOCK_XOSC32K_STARTUP_TIME;
	xosc32k_conf.auto_gain_control     = CONF_CLOCK_XOSC32K_AUTO_AMPLITUDE_CONTROL;
	xosc32k_conf.enable_1khz_output    = CONF_CLOCK_XOSC32K_ENABLE_1KHZ_OUPUT;
	xosc32k_conf.enable_32khz_output   = CONF_CLOCK_XOSC32K_ENABLE_32KHZ_OUTPUT;
	xosc32k_conf.on_demand             = CONF_CLOCK_XOSC32K_ON_DEMAND;
	xosc32k_conf.run_in_standby        = CONF_CLOCK_XOSC32K_RUN_IN_STANDBY;

	system_clock_source_xosc32k_set_config(&xosc32k_conf);
	system_clock_source_enable(SYSTEM_CLOCK_SOURCE_XOSC32K);
#endif


	/* OSCK32K */
#if CONF_CLOCK_OSC32K_ENABLE == true
	struct system_clock_source_osc32k_config osc32k_conf;

	osc32k_conf.startup_time        = CONF_CLOCK_OSC32K_STARTUP_TIME;
	osc32k_conf.enable_1khz_output  = CONF_CLOCK_OSC32K_ENABLE_1KHZ_OUTPUT;
	osc32k_conf.enable_32khz_output = CONF_CLOCK_OSC32K_ENABLE_32KHZ_OUTPUT;
	osc32k_conf.on_demand           = CONF_CLOCK_OSC32K_ON_DEMAND;
	osc32k_conf.run_in_standby      = CONF_CLOCK_OSC32K_RUN_IN_STANDBY;

	system_clock_source_osc32k_set_config(&osc32k_conf);
	system_clock_source_enable(SYSTEM_CLOCK_SOURCE_OSC32K);
#endif


	/* DFLL (Open and Closed Loop) */
#if CONF_CLOCK_DFLL_ENABLE == true
	struct system_clock_source_dfll_config dfll_conf;
	system_clock_source_dfll_get_default_config(&dfll_conf);

	dfll_conf.loop_mode            = CONF_CLOCK_DFLL_LOOP_MODE;
	dfll_conf.on_demand            = CONF_CLOCK_DFLL_ON_DEMAND;
	dfll_conf.run_in_standby       = CONF_CLOCK_DFLL_RUN_IN_STANDBY;

	if (CONF_CLOCK_DFLL_LOOP_MODE == SYSTEM_CLOCK_DFLL_LOOP_MODE_OPEN) {
		dfll_conf.coarse_value = CONF_CLOCK_DFLL_COARSE_VALUE;
		dfll_conf.fine_value   = CONF_CLOCK_DFLL_FINE_VALUE;
	}

#  if CONF_CLOCK_DFLL_QUICK_LOCK == true
	dfll_conf.quick_lock = SYSTEM_CLOCK_DFLL_QUICK_LOCK_ENABLE;
#  else
	dfll_conf.quick_lock = SYSTEM_CLOCK_DFLL_QUICK_LOCK_DISABLE;
#  endif

#  if CONF_CLOCK_DFLL_TRACK_AFTER_FINE_LOCK == true
	dfll_conf.stable_tracking = SYSTEM_CLOCK_DFLL_STABLE_TRACKING_TRACK_AFTER_LOCK;
#  else
	dfll_conf.stable_tracking = SYSTEM_CLOCK_DFLL_STABLE_TRACKING_FIX_AFTER_LOCK;
#  endif

#  if CONF_CLOCK_DFLL_KEEP_LOCK_ON_WAKEUP == true
	dfll_conf.wakeup_lock = SYSTEM_CLOCK_DFLL_WAKEUP_LOCK_KEEP;
#  else
	dfll_conf.wakeup_lock = SYSTEM_CLOCK_DFLL_WAKEUP_LOCK_LOSE;
#  endif

#  if CONF_CLOCK_DFLL_ENABLE_CHILL_CYCLE == true
	dfll_conf.chill_cycle = SYSTEM_CLOCK_DFLL_CHILL_CYCLE_ENABLE;
#  else
	dfll_conf.chill_cycle = SYSTEM_CLOCK_DFLL_CHILL_CYCLE_DISABLE;
#  endif

	if (CONF_CLOCK_DFLL_LOOP_MODE == SYSTEM_CLOCK_DFLL_LOOP_MODE_CLOSED) {
		dfll_conf.multiply_factor = CONF_CLOCK_DFLL_MULTIPLY_FACTOR;
	}

	dfll_conf.coarse_max_step = CONF_CLOCK_DFLL_MAX_COARSE_STEP_SIZE;
	dfll_conf.fine_max_step   = CONF_CLOCK_DFLL_MAX_FINE_STEP_SIZE;

	system_clock_source_dfll_set_config(&dfll_conf);
#endif


	/* OSC8M */
	struct system_clock_source_osc8m_config osc8m_conf;

	osc8m_conf.prescaler            = CONF_CLOCK_OSC8M_PRESCALER;
	osc8m_conf.on_demand            = CONF_CLOCK_OSC8M_ON_DEMAND;
	osc8m_conf.run_in_standby       = CONF_CLOCK_OSC8M_RUN_IN_STANDBY;

	system_clock_source_osc8m_set_config(&osc8m_conf);
	system_clock_source_enable(SYSTEM_CLOCK_SOURCE_OSC8M);

	/* GCLK */
#if CONF_CLOCK_CONFIGURE_GCLK == true
	system_gclk_init();

	struct system_gclk_gen_config gclk_generator_conf;
	system_gclk_gen_get_config_defaults(&gclk_generator_conf);

#  if CONF_CLOCK_GCLK_1_ENABLE == true
	gclk_generator_conf.source_clock    = CONF_CLOCK_GCLK_1_CLOCK_SOURCE;
	gclk_generator_conf.division_factor = CONF_CLOCK_GCLK_1_PRESCALER;
	gclk_generator_conf.run_in_standby  = CONF_CLOCK_GCLK_1_RUN_IN_STANDBY;
	gclk_generator_conf.output_enable   = CONF_CLOCK_GCLK_1_OUTPUT_ENABLE;
	system_gclk_gen_set_config(1, &gclk_generator_conf);
	system_gclk_gen_enable(1);
#  endif

#  if CONF_CLOCK_GCLK_2_ENABLE == true
	gclk_generator_conf.source_clock    = CONF_CLOCK_GCLK_2_CLOCK_SOURCE;
	gclk_generator_conf.division_factor = CONF_CLOCK_GCLK_2_PRESCALER;
	gclk_generator_conf.run_in_standby  = CONF_CLOCK_GCLK_2_RUN_IN_STANDBY;
	gclk_generator_conf.output_enable   = CONF_CLOCK_GCLK_2_OUTPUT_ENABLE;
	system_gclk_gen_set_config(2, &gclk_generator_conf);
	system_gclk_gen_enable(2);
#  endif

#  if CONF_CLOCK_GCLK_3_ENABLE == true
	gclk_generator_conf.source_clock    = CONF_CLOCK_GCLK_3_CLOCK_SOURCE;
	gclk_generator_conf.division_factor = CONF_CLOCK_GCLK_3_PRESCALER;
	gclk_generator_conf.run_in_standby  = CONF_CLOCK_GCLK_3_RUN_IN_STANDBY;
	gclk_generator_conf.output_enable   = CONF_CLOCK_GCLK_3_OUTPUT_ENABLE;
	system_gclk_gen_set_config(3, &gclk_generator_conf);
	system_gclk_gen_enable(3);
#  endif

#  if CONF_CLOCK_GCLK_4_ENABLE == true
	gclk_generator_conf.source_clock    = CONF_CLOCK_GCLK_4_CLOCK_SOURCE;
	gclk_generator_conf.division_factor = CONF_CLOCK_GCLK_4_PRESCALER;
	gclk_generator_conf.run_in_standby  = CONF_CLOCK_GCLK_4_RUN_IN_STANDBY;
	gclk_generator_conf.output_enable   = CONF_CLOCK_GCLK_4_OUTPUT_ENABLE;
	system_gclk_gen_set_config(4, &gclk_generator_conf);
	system_gclk_gen_enable(4);
#  endif

#  if CONF_CLOCK_GCLK_5_ENABLE == true
	gclk_generator_conf.source_clock    = CONF_CLOCK_GCLK_5_CLOCK_SOURCE;
	gclk_generator_conf.division_factor = CONF_CLOCK_GCLK_5_PRESCALER;
	gclk_generator_conf.run_in_standby  = CONF_CLOCK_GCLK_5_RUN_IN_STANDBY;
	gclk_generator_conf.output_enable   = CONF_CLOCK_GCLK_5_OUTPUT_ENABLE;
	system_gclk_gen_set_config(5, &gclk_generator_conf);
	system_gclk_gen_enable(5);
#  endif

#  if CONF_CLOCK_GCLK_6_ENABLE == true
	gclk_generator_conf.source_clock    = CONF_CLOCK_GCLK_6_CLOCK_SOURCE;
	gclk_generator_conf.division_factor = CONF_CLOCK_GCLK_6_PRESCALER;
	gclk_generator_conf.run_in_standby  = CONF_CLOCK_GCLK_6_RUN_IN_STANDBY;
	gclk_generator_conf.output_enable   = CONF_CLOCK_GCLK_6_OUTPUT_ENABLE;
	system_gclk_gen_set_config(6, &gclk_generator_conf);
	system_gclk_gen_enable(6);
#  endif

#  if CONF_CLOCK_GCLK_7_ENABLE == true
	gclk_generator_conf.source_clock    = CONF_CLOCK_GCLK_7_CLOCK_SOURCE;
	gclk_generator_conf.division_factor = CONF_CLOCK_GCLK_7_PRESCALER;
	gclk_generator_conf.run_in_standby  = CONF_CLOCK_GCLK_7_RUN_IN_STANDBY;
	gclk_generator_conf.output_enable   = CONF_CLOCK_GCLK_7_OUTPUT_ENABLE;
	system_gclk_gen_set_config(7, &gclk_generator_conf);
	system_gclk_gen_enable(7);
#  endif

/* Enable DFLL reference clock if in closed loop mode */
#  if (CONF_CLOCK_DFLL_ENABLE)
	if (CONF_CLOCK_DFLL_LOOP_MODE == SYSTEM_CLOCK_DFLL_LOOP_MODE_CLOSED) {
		struct system_gclk_chan_config dfll_gclk_chan_conf;

		system_gclk_chan_get_config_defaults(&dfll_gclk_chan_conf);
		dfll_gclk_chan_conf.source_generator = CONF_CLOCK_DFLL_SOURCE_GCLK_GENERATOR;
		system_gclk_chan_set_config(0, &dfll_gclk_chan_conf);
		system_gclk_chan_enable(0);
	}
#  endif

	/* Configured last as it might depend on other generators */
#  if CONF_CLOCK_GCLK_0_ENABLE == true
	gclk_generator_conf.source_clock    = CONF_CLOCK_GCLK_0_CLOCK_SOURCE;
	gclk_generator_conf.division_factor = CONF_CLOCK_GCLK_0_PRESCALER;
	gclk_generator_conf.run_in_standby  = CONF_CLOCK_GCLK_0_RUN_IN_STANDBY;
	gclk_generator_conf.output_enable   = CONF_CLOCK_GCLK_0_OUTPUT_ENABLE;
	system_gclk_gen_set_config(0, &gclk_generator_conf);
	system_gclk_gen_enable(0);
#  endif
#endif

	/* CPU and BUS clocks */
	system_cpu_clock_set_divider(CONF_CLOCK_CPU_DIVIDER);
#if CONF_CLOCK_CPU_CLOCK_FAILURE_DETECT == true
	system_main_clock_set_failure_detect(true);
#else
	system_main_clock_set_failure_detect(false);
#endif
	system_apb_clock_set_divider(SYSTEM_CLOCK_APB_APBA, CONF_CLOCK_APBA_DIVIDER);
	system_apb_clock_set_divider(SYSTEM_CLOCK_APB_APBB, CONF_CLOCK_APBB_DIVIDER);
}

