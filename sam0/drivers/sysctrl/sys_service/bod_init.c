/**
 * \file
 *
 * \brief SAM0+ GCLK configuration
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

#include <compiler.h>
#include <system.h>
#include <conf_bod.h>
#include <bod_init.h>

/**
 * \brief Initialize BOD12 and BOD33 based on the configuration in conf_bod.h
 *
 */
void system_bod_init(void)
{
	#if (BOD33_ENABLED == true) || (BOD12_ENABLED == true)
	struct sysctrl_bod_config conf;

	#if BOD33_ENABLED == true
	conf.action = BOD33_ACTION;
	conf.sampled_mode = BOD33_MODE;
	conf.prescaler = BOD33_PRESCALER;
	conf.hysteresis = BOD33_HYSTERESIS;

	system_bod_set_config(&conf, SYSCTRL_BOD33);
	#endif

	#if BOD12_ENABLED == true
	conf.action = BOD12_ACTION;
	conf.sampled_mode = BOD12_MODE;
	conf.prescaler = BOD12_PRESCALER;
	conf.hysteresis = BOD12_HYSTERESIS;

	system_bod_set_config(&conf, SYSCTRL_BOD12);
	#endif

	#endif
}
