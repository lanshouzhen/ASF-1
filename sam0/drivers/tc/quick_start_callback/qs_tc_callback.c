/**
 * \file
 *
 * \brief SAM D20 TC - Timer Counter Callback Driver Quick Start
 *
 * Copyright (C) 2013 Atmel Corporation. All rights reserved.
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
#include <conf_quick_start_callback.h>
#include <asf.h>

void configure_tc(void);
void configure_tc_callbacks(void);
void tc_callback_to_change_duty_cycle(
		struct tc_module *const module_inst);


//! [module_inst]
struct tc_module pwm_tc_module;
//! [module_inst]

//! [callback_funcs]
void tc_callback_to_change_duty_cycle(
		struct tc_module *const module_inst)
{
	static uint16_t i = 0;

	i += 128;
	tc_set_compare_value(module_inst, TC_COMPARE_CAPTURE_CHANNEL_0, i + 1);
}
//! [callback_funcs]

//! [setup]
void configure_tc(void)
{
	//! [setup_config]
	struct tc_config config;
	//! [setup_config]
	//! [setup_config_defaults]
	tc_get_config_defaults(&config);
	//! [setup_config_defaults]

	//! [setup_change_config]
	config.counter_size    = TC_COUNTER_SIZE_16BIT;
	config.wave_generation = TC_WAVE_GENERATION_NORMAL_PWM;
	config.size_specific.size_16_bit.compare_capture_channel[0] = 0xFFFF;
	//! [setup_change_config]

	//! [setup_change_config_pwm]
	config.channel_pwm_out_enabled[0] = true;
	config.channel_pwm_out_pin[0]     = PWM_OUT_PIN;
	config.channel_pwm_out_mux[0]     = PWM_OUT_MUX;
	//! [setup_change_config_pwm]

	//! [setup_set_config]
	tc_init(&pwm_tc_module, PWM_MODULE, &config);
	//! [setup_set_config]

	//! [setup_enable]
	tc_enable(&pwm_tc_module);
	//! [setup_enable]
}

void configure_tc_callbacks(void)
{
	//! [setup_register_callback]
	tc_register_callback(
			&pwm_tc_module,
			tc_callback_to_change_duty_cycle,
			TC_CALLBACK_CC_CHANNEL0);
	//! [setup_register_callback]

	//! [setup_enable_callback]
	tc_enable_callback(&pwm_tc_module, TC_CALLBACK_CC_CHANNEL0);
	//! [setup_enable_callback]
}
//! [setup]

int main(void)
{
	system_init();

//! [setup_init]
	configure_tc();
	configure_tc_callbacks();
//! [setup_init]

//! [main]
	//! [enable_global_interrupts]
	system_interrupt_enable_global();
	//! [enable_global_interrupts]

	//! [main_loop]
	while (true) {
	}
	//! [main_loop]
//! [main]
}
