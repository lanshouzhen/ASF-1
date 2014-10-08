/**
 * \file
 *
 * \brief SAM Configurable Custom Logic (CCL) Driver
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

#include "ccl.h"

void ccl_init(struct ccl_config *const config)
{
	/* Turn on the digital interface clock. */
	system_apb_clock_set_mask(SYSTEM_CLOCK_APB_APBD, MCLK_APBDMASK_CCL);

	/* Reset module. */
	ccl_module_reset();

	/* Enable module. */
	ccl_module_enable();

	/* Configure GCLK channel and enable clock */
	struct system_gclk_chan_config gclk_chan_conf;
	system_gclk_chan_get_config_defaults(&gclk_chan_conf);
	gclk_chan_conf.source_generator = config->clock_source;
	system_gclk_chan_set_config(CCL_GCLK_ID, &gclk_chan_conf);
	system_gclk_chan_enable(CCL_GCLK_ID);

	if(config->run_in_standby) {
		ccl_module_runstdby_enable();
	} else {
		ccl_module_runstdby_disable();
	}
}

void ccl_lut_get_config_defaults(struct ccl_lut_config *const config)
{
	/* Sanity check arguments */
	Assert(config);

	/* Default configuration values */
	config->truth_table_value = 0;
	config->event_output_enable = false;
	config->event_input_enable = false;
	config->event_input_inverted_enable = false;
	config->input0_src_sel = CCL_LUT_INPUT_SRC_MASK;
	config->input1_src_sel = CCL_LUT_INPUT_SRC_MASK;
	config->input2_src_sel = CCL_LUT_INPUT_SRC_MASK;
	config->edge_selection_enable = false;
	config->filter_sel = CCL_LUT_FILTER_DISABLE;
}

void ccl_lut_set_config(const enum ccl_lut_id number,
		struct ccl_lut_config *const config)
{
	/* Sanity check arguments */
	Assert(config);
	Assert(number);

	uint32_t temp = 0;

	if (config->event_output_enable) {
		temp |= CCL_LUTCTRL_LUTEO;
	}

	if (config->event_input_enable) {
		temp |= CCL_LUTCTRL_LUTEI;
	}

	if (config->event_input_inverted_enable) {
		temp |= CCL_LUTCTRL_INVEI;
	}

	if (config->edge_selection_enable) {
		temp |= CCL_LUTCTRL_EDGESEL;
	}

	CCL->LUTCTRL[number].reg = temp |
		CCL_LUTCTRL_INSEL0(config->input0_src_sel) |
		CCL_LUTCTRL_INSEL1(config->input1_src_sel) |
		CCL_LUTCTRL_INSEL2(config->input2_src_sel) |
		CCL_LUTCTRL_TRUTH(config->truth_table_value) |
		config->filter_sel;
}

void ccl_seq_config(const enum ccl_seq_id number,
		const enum ccl_seq_selection seq_selection)
{
	/* Sanity check arguments */
	Assert(seq_selection);
	Assert(number);

	CCL->SEQCTRL[number].reg |= seq_selection;
}

void ccl_lut_enable(const enum ccl_lut_id number)
{
	/* Sanity check arguments */
	Assert(number);

	/* Enable the LUTx */
	CCL->LUTCTRL[number].reg |= CCL_LUTCTRL_ENABLE;
}

void ccl_lut_disable(const enum ccl_lut_id number)
{
	/* Sanity check arguments */
	Assert(number);

	/* Disable the LUTx */
	CCL->LUTCTRL[number].reg &= ~CCL_LUTCTRL_ENABLE;
}


