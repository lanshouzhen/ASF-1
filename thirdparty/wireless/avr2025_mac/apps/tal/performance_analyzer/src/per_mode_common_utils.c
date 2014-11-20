/**
 * \file per_mode_common_utils.c
 *
 * \brief  Common utilities for both Initiator and Receptor in PER Measurement
 * mode - Performance Analyzer application
 *
 * Copyright (c) 2013-2014 Atmel Corporation. All rights reserved.
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
 *    Atmel micro controller product.
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
 */

/*
 * Copyright(c) 2013-2014, Atmel Corporation All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/* === INCLUDES ============================================================ */
#include <stdio.h>
#include "tfa.h"
#include "tal.h"
#include "tal_internal.h"
#include "tal_constants.h"
#include "tal_helper.h"
#include "app_frame_format.h"
#include "app_init.h"
#include "perf_api.h"
#include "perf_api_serial_handler.h"
/* === TYPES =============================================================== */

/* === EXTERNALS============================================================== */

extern bool rdy_to_tx;
extern bool pkt_stream_stop;
extern bool peer_found;
extern uint8_t last_tx_power_format_set;
#if (ANTENNA_DIVERSITY == 1)
extern uint8_t ant_sel_before_ct;
extern uint8_t ant_div_before_ct;
#endif /* End of #if (ANTENNA_DIVERSITY == 1) */
extern uint8_t op_mode;

/* === GLOBALS============================================================== */

#ifdef EXT_RF_FRONT_END_CTRL
static uint8_t prev_non_26chn_tx_power;
#endif
frame_info_t *stream_pkt;
uint8_t pkt_buffer[LARGE_BUFFER_SIZE];
uint32_t pkt_stream_gap_time ;
/* Database to maintain the default settings of the configurable parameter */
trx_config_params_t default_trx_config_params;
/* Database to maintain the updated/latest settings of the configurable
 * parameters */
trx_config_params_t curr_trx_config_params;
uint16_t cw_tmr_val = 0;
bool cw_ack_sent=false,remote_cw_start=false;
uint8_t cw_start_mode;
bool pulse_mode = false;
bool rx_on_mode = false;
extern uint8_t cc_band_ct;
extern uint8_t cc_number_ct;
/* === DEFINES============================================================== */

#define DEFAULT_NO_OF_TEST_FRAMES               (100)
#define DEFAULT_FRAME_LENGTH                    (20)

/* === PROTOTYPES============================================================== */

#if ((TAL_TYPE != AT86RF230B) || ((TAL_TYPE == AT86RF230B) && \
	(defined CW_SUPPORTED)))
	
static void stop_pulse_cb(void *callback_parameter);

#endif /* End of #if ((TAL_TYPE != AT86RF230B) || ((TAL_TYPE == AT86RF230B) &&
        *(defined CW_SUPPORTED))) */

/* === IMPLEMENTATION======================================================= */

/*
 * \brief This function is called rest the application equivalent to soft reset
 *
 */
void app_reset(void)
{
	/* app reset - this is to make the node to restart application as
	 * bootup */
	set_main_state(INIT, NULL);

	/* INIT was a success - so change to WAIT_FOR_EVENT state */
	set_main_state(WAIT_FOR_EVENT, NULL);
}

/**
 * \brief Function to set trx configure parameters
 *
 */
void config_per_test_parameters(void)
{
	uint8_t temp;
	pib_value_t pib_value;

	/* Set the default values */
	curr_trx_config_params.ack_request
		= default_trx_config_params.ack_request = true;
	curr_trx_config_params.csma_enabled
		= default_trx_config_params.csma_enabled = true;
	curr_trx_config_params.retry_enabled
		= default_trx_config_params.retry_enabled = false;

#if (ANTENNA_DIVERSITY == 1)
#if (TAL_TYPE == AT86RF233)
	/* Disable antenna diversity by default. */
	curr_trx_config_params.antenna_diversity
		= default_trx_config_params.antenna_diversity = false;
	curr_trx_config_params.antenna_selected
		= default_trx_config_params.antenna_selected
				= ANT_CTRL_1;

	/* This is required for set default config request command to set the
	 * config parameters to their defaults */
	/* Disable antenna diversity by default */
	/* Enable A1/X2 */
	tal_ant_div_config(ANT_DIVERSITY_DISABLE, ANT_CTRL_1); /* Enable A1/X2
	                                                       **/

#else
	curr_trx_config_params.antenna_diversity
		= default_trx_config_params.antenna_diversity = true;
	curr_trx_config_params.antenna_selected
		= default_trx_config_params.antenna_selected
				= ANT_CTRL_0;

	/* Enable Antenna Diversity*/
	tal_ant_div_config(ANT_DIVERSITY_ENABLE, ANTENNA_DEFAULT);
#endif /* end of (TAL_TYPE == AT86RF233) */
#endif

#if (TAL_TYPE != AT86RF230B)
	/* Disable desensitization by default */
	curr_trx_config_params.rx_desensitize
		= default_trx_config_params.rx_desensitize = false;
	/* Disable Rx desensitization */

	tal_set_rx_sensitivity_level(NO_RX_DESENSITIZE_LEVEL);
#endif /* End of #if(TAL_TYPE != AT86RF230B)*/

#if ((TAL_TYPE == AT86RF233) || (TAL_TYPE == ATMEGARFR2))
	curr_trx_config_params.rpc_enable
		= default_trx_config_params.rpc_enable = false;

	/* Enable RPC feature by default */
	tal_rpc_mode_config(DISABLE_ALL_RPC_MODES);
	/* Reset RX_SAFE Mode in TRX_CTRL_2 */
	tal_trx_reg_write(RG_TRX_CTRL_2, ENABLE_RX_SAFE_MODE);
#endif
	if (peer_found == true) {
		curr_trx_config_params.trx_state
			= default_trx_config_params.trx_state
					= RX_AACK_ON;
	} else {
		curr_trx_config_params.trx_state
			= default_trx_config_params.trx_state = TRX_OFF;
	}

	curr_trx_config_params.number_test_frames
		= default_trx_config_params.number_test_frames
				= DEFAULT_NO_OF_TEST_FRAMES;
	curr_trx_config_params.phy_frame_length
		= default_trx_config_params.phy_frame_length
				= DEFAULT_FRAME_LENGTH;

	/* As channel & channel page are already configured during
	 * the application initialization so get it
	 */
	curr_trx_config_params.channel = default_trx_config_params.channel
				= DEFAULT_CHANNEL;
	pib_value.pib_value_16bit = default_trx_config_params.channel;
	tal_pib_set(phyCurrentChannel,
			&pib_value);

	/* Make the ISM frequency as null as IEEE channel is set in default case
	**/
#if (TAL_TYPE == AT86RF233)
	curr_trx_config_params.ism_frequency
		= default_trx_config_params.ism_frequency = 0.0;
#endif

	curr_trx_config_params.channel_page
		= default_trx_config_params.channel_page
				= TAL_CURRENT_PAGE_DEFAULT;
	pib_value.pib_value_8bit = default_trx_config_params.channel_page;
	tal_pib_set(phyCurrentPage, &pib_value);

	/* As tx power is already configure by TAL in tal_pib.c get it for
	 * application*/
	temp = TAL_TRANSMIT_POWER_DEFAULT;
	pib_value.pib_value_8bit = temp;
	tal_pib_set(phyTransmitPower, &pib_value);

	curr_trx_config_params.tx_power_dbm
		= default_trx_config_params.tx_power_dbm
				= CONV_phyTransmitPower_TO_DBM(
			TAL_TRANSMIT_POWER_DEFAULT);
#if ((TAL_TYPE != AT86RF212) && (TAL_TYPE != AT86RF212B))
	tal_get_curr_trx_config(TX_PWR, &(curr_trx_config_params.tx_power_reg));
	tal_get_curr_trx_config(TX_PWR,
			&(default_trx_config_params.tx_power_reg));
#endif

	/* The following fields has no meaning if there is no peer */
	if (true == peer_found) {
#ifdef CRC_SETTING_ON_REMOTE_NODE
		curr_trx_config_params.crc_settings_on_peer
			= default_trx_config_params.crc_settings_on_peer
					= false;
#endif

		/* Set the config parameters on peer node */
#if (ANTENNA_DIVERSITY == 1)
#if (TAL_TYPE == AT86RF233)
		curr_trx_config_params.antenna_diversity_on_peer
			= default_trx_config_params.
				antenna_diversity_on_peer = false;
		curr_trx_config_params.antenna_selected_on_peer
			= default_trx_config_params.
				antenna_selected_on_peer = ANT_CTRL_1;

#else
		curr_trx_config_params.antenna_diversity_on_peer
			= default_trx_config_params.
				antenna_diversity_on_peer = true;
		curr_trx_config_params.antenna_selected_on_peer
			= default_trx_config_params.
				antenna_selected_on_peer = ANT_CTRL_0;
#endif /* End of #if(TAL_TYPE == AT86RF233) */
#endif /* End of #if (ANTENNA_DIVERSITY == 1) */
	}
}


/*Start Packet Streaming Test*/

void pktstream_test(uint16_t gap_time,uint16_t timeout,bool start_stop,uint16_t frame_len)
{
	pkt_stream_gap_time = gap_time;
	if(frame_len<=127)
	{
		usr_pkt_stream_confirm(MAC_SUCCESS,start_stop);
	}
	else
	{
		usr_pkt_stream_confirm(INVALID_ARGUMENT,start_stop);
	}
	
	if((node_info.main_state == PER_TEST_RECEPTOR))
	{
		serial_data_handler();
	}
	if(start_stop)
	{
		configure_pkt_stream_frames(frame_len);
		op_mode=PKT_STREAM_MODE;
		if(pkt_stream_gap_time)
		{
			sw_timer_start(T_APP_TIMER,pkt_stream_gap_time*1E3,SW_TIMEOUT_RELATIVE,(FUNC_PTR)pkt_stream_gap_timer,NULL);
		}
		else
		{
			rdy_to_tx = true;
		}
		pkt_stream_stop = false;
		
		if((node_info.main_state == PER_TEST_RECEPTOR) && 1 <= timeout )
		{
			sw_timer_start(CW_TX_TIMER,
			(uint32_t)timeout * 1E6,
			SW_TIMEOUT_RELATIVE,
			(FUNC_PTR)stop_pkt_streaming,
			NULL);
		}
	}
	else
	{
		//stop packet streaming once the current packet transmission is completed
		pkt_stream_stop = true;
		sw_timer_stop(T_APP_TIMER);
		 op_mode=TX_OP_MODE;
	}
	
	
}

/**
 * \brief To Configure the frame sending
 */
void configure_pkt_stream_frames(uint16_t frame_len)
{
	uint8_t index;
	uint16_t app_frame_length;
	uint8_t *frame_ptr;
	uint8_t *temp_frame_ptr;
	uint16_t fcf = 0;
	uint16_t temp_value;
	app_payload_t *tmp;

	stream_pkt = (frame_info_t *)pkt_buffer;	
	/*
	 * Fill in PHY frame.
	 */

	/* Get length of current frame. */
	app_frame_length = frame_len - FRAME_OVERHEAD;

	/* Set payload pointer. */
	/* Add 2 octets for FCS. */
	frame_ptr = temp_frame_ptr
				= (uint8_t *)stream_pkt +
					LARGE_BUFFER_SIZE -
					app_frame_length - FCS_LEN; 

	tmp = (app_payload_t *)temp_frame_ptr;

	(tmp->cmd_id) = PKT_STREAM_PKT;

	temp_frame_ptr++;

	/*
	 * Assign dummy payload values.
	 * Payload is stored to the end of the buffer avoiding payload copying
	 * by TAL.
	 */
	/* 1=> cmd ID*/
	for (index = 0; index < (app_frame_length - 1); index++) { 
		*temp_frame_ptr++ = index; /* dummy values */
	}

	/* Source Address */
	temp_value =  tal_pib.ShortAddress;
	frame_ptr -= SHORT_ADDR_LEN;
	convert_16_bit_to_byte_array(temp_value, frame_ptr);

	/* Source PAN-Id */
#if (DST_PAN_ID == SRC_PAN_ID)
	/* No source PAN-Id included, but FCF updated. */
	fcf |= FCF_PAN_ID_COMPRESSION;
#else
	frame_ptr -= PAN_ID_LEN;
	temp_value = CCPU_ENDIAN_TO_LE16(SRC_PAN_ID);
	convert_16_bit_to_byte_array(temp_value, frame_ptr);
#endif

	/* Destination Address */
	temp_value = 0XFFFF;
	frame_ptr -= SHORT_ADDR_LEN;
	convert_16_bit_to_byte_array(temp_value, frame_ptr);

	/* Destination PAN-Id */
	temp_value = CCPU_ENDIAN_TO_LE16(DST_PAN_ID);
	frame_ptr -= PAN_ID_LEN;
	convert_16_bit_to_byte_array(temp_value, frame_ptr);

	/* Set DSN. */
	frame_ptr--;
	*frame_ptr = (uint8_t)rand();

	/* Set the FCF. */ // Reserved frame type so that other apps doesnot receive and process this data
	fcf |= 0x04 | FCF_SET_SOURCE_ADDR_MODE(FCF_SHORT_ADDR) |
			FCF_SET_DEST_ADDR_MODE(FCF_SHORT_ADDR);


	frame_ptr -= FCF_LEN;
	convert_16_bit_to_byte_array(CCPU_ENDIAN_TO_LE16(fcf), frame_ptr);

	/* First element shall be length of PHY frame. */
	frame_ptr--;
	*frame_ptr = frame_len; //sriram

	/* Finished building of frame. */
	stream_pkt->mpdu = frame_ptr;
}



void pkt_stream_gap_timer(void *parameter)
{
	rdy_to_tx = true;
	parameter=parameter;
}

void stop_pkt_streaming(void * parameter)
{
	pkt_stream_stop = true;
	sw_timer_stop(T_APP_TIMER);
	usr_pkt_stream_confirm(MAC_SUCCESS,false);
}

/*
 * \brief Send an energy pulse on current channel page
 */
#if ((TAL_TYPE != AT86RF230B) || ((TAL_TYPE == AT86RF230B) && \
	(defined CW_SUPPORTED)))
void pulse_cw_transmission(void)
{
	if(node_info.main_state == PER_TEST_RECEPTOR)
	{
		//give some time //sriram
		delay_ms(10);
	}
	uint16_t channel;
	op_mode = CONTINUOUS_TX_MODE;
	tal_pib_get(phyCurrentChannel,(uint8_t *)&channel);

	/* Save all user settings before continuous tx */
	save_all_settings();

	tal_reset(false);

#if (TAL_TYPE == AT86RF233)
	/* Set the frequency back to already set value after tal_reset */
	if (CC_BAND_0 != cc_band_ct) {
		tal_set_frequency_regs(cc_band_ct, cc_number_ct);
	}


#endif /* End of (TAL_TYPE == AT86RF233) */

	/* Start  the Continuous Wave transmission */
	tfa_continuous_tx_start(CW_MODE, false);
	/* Start the timer to stop the Continuous transmission */
	sw_timer_start(T_APP_TIMER,
			PULSE_CW_TX_TIME_IN_MICRO_SEC,
			SW_TIMEOUT_RELATIVE,
			(FUNC_PTR)stop_pulse_cb,
			NULL);
}

#endif

/**
 * \brief Stop sending a CW signal on current channel page
 * \param callback_parameter Pointer to callback, not used here
 */
#if ((TAL_TYPE != AT86RF230B) || ((TAL_TYPE == AT86RF230B) && \
	(defined CW_SUPPORTED)))
static void stop_pulse_cb(void *callback_parameter)
{
	/* Stop CW transmission again */
	tfa_continuous_tx_stop();

	op_mode = TX_OP_MODE;
	/* recover all user setting set before continuous tx */
	recover_all_settings();

	/* Send the confirmation with status as SUCCESS */
	usr_cont_pulse_tx_confirm(MAC_SUCCESS);

	/* Keep compiler happy. */
	callback_parameter = callback_parameter;
}

#endif

/*
 * \brief Start CW transmission on current channel page
 * \param tx_mode  Continuous transmission mode
 */
#if ((TAL_TYPE != AT86RF230B) || ((TAL_TYPE == AT86RF230B) && \
	(defined CW_SUPPORTED)))
void start_cw_transmission(uint8_t tx_mode,uint16_t tmr_val)
{
if(node_info.main_state == PER_TEST_RECEPTOR && !cw_ack_sent)
{

	if((tx_mode !=CW_MODE && tx_mode != PRBS_MODE) || (3600 < tmr_val)) /* timer value should not exceed 3600 seconds */
	{
		usr_cont_wave_tx_confirm(INVALID_ARGUMENT, 0x01, tx_mode);
		return;
	}
	else
	{
	   /* Send Set confirmation with status SUCCESS and start cw trx on successful trx of the */
	   usr_cont_wave_tx_confirm(MAC_SUCCESS, START_CWT, tx_mode); 
	   remote_cw_start = true;
	   cw_start_mode = tx_mode;
	   cw_tmr_val = tmr_val;
	   return;
	}
	
	
}
else if(node_info.main_state == PER_TEST_INITIATOR || (node_info.main_state == SINGLE_NODE_TESTS) || ((node_info.main_state == PER_TEST_RECEPTOR) && cw_ack_sent))
{
	/* Save all user settings before continuous tx */
	save_all_settings();

	/* Added to ensure CW transmission happen in every attempt */
	tal_reset(false);

#if (ANTENNA_DIVERSITY == 1)
	if (ANT_DIV_DISABLE == ant_div_before_ct) {
		tal_ant_div_config(ANT_DIVERSITY_DISABLE, ant_sel_before_ct);
	}

#endif

if((node_info.main_state == PER_TEST_RECEPTOR) && 1 <= tmr_val )
{
	sw_timer_start(CW_TX_TIMER,
	(uint32_t)tmr_val * 1E6,
	SW_TIMEOUT_RELATIVE,
	(FUNC_PTR)stop_cw_transmission,
	(void *)&tx_mode);

	
}

	switch (tx_mode) {
	case CW_MODE: /* CW mode*/
	{
		/* In CW_MODE the parameter random_content is obsolete. */
		tfa_continuous_tx_start(CW_MODE, false);
	}
	break;

	case PRBS_MODE: /* PRBS mode*/
	{
		/* Start PRBS_MODE mode using random content. */
		tfa_continuous_tx_start(PRBS_MODE, true);
	}
	break;

	default:
	{
		usr_cont_wave_tx_confirm(INVALID_ARGUMENT, 0x01, tx_mode);
		return;
	}
	}

	op_mode = CONTINUOUS_TX_MODE;
	if(node_info.main_state == PER_TEST_RECEPTOR )
	{
		cw_ack_sent=false;
	}
	else
	{
	
	/* Send Set confirmation with status SUCCESS */
	usr_cont_wave_tx_confirm(MAC_SUCCESS, START_CWT, tx_mode);
	}
}
}

/*
 * \brief Stop CW transmission on current channel page
 * \param tx_mode  Continuous transmission mode
 */
void stop_cw_transmission(void *parameter)
{
	uint8_t cw_mode;
	cw_mode = *(uint8_t *)parameter;
	/* Stop CW transmission again */
	tfa_continuous_tx_stop();
	/* recover all user setting which were set before continuous tx */
	recover_all_settings();
	op_mode = TX_OP_MODE;
	usr_cont_wave_tx_confirm(MAC_SUCCESS, STOP_CWT /*stop*/, cw_mode);
	remote_cw_start = false;
}

#endif

/*
 * \brief Read transceiver register
 * \param reg_addr  The address of the register to be read
 */
void read_trx_registers(uint16_t reg_addr)
{
	uint8_t reg_val = INVALID_VALUE;

#if (TAL_TYPE != ATMEGARFR2)
	if (reg_addr > MAX_REG_ADDRESS)
#else
	if (reg_addr < MIN_REG_ADDRESS || reg_addr > MAX_REG_ADDRESS)
#endif

	{
		/* Send the confirmation with status as OUT_OF_RANGE register
		 * address */
		usr_register_read_confirm(VALUE_OUT_OF_RANGE, reg_addr,
				reg_val);
		return;
	}

#if (TAL_TYPE != ATMEGARFR2)
	tal_trx_reg_read((uint8_t)reg_addr, &reg_val);
#else
	tal_trx_reg_read(reg_addr, &reg_val);
#endif

	/* Send the confirmation with status as SUCCESS */
	usr_register_read_confirm(MAC_SUCCESS, reg_addr, reg_val);
}

/*
 * \brief Write Transceiver registers
 * \param reg_addr  The address of the register to be written
 * \param reg_val   The value of the register to be written
 */
void write_trx_registers(uint16_t reg_addr, uint8_t reg_val)
{
#if (TAL_TYPE != ATMEGARFR2)
	if (reg_addr > MAX_REG_ADDRESS)
#else
	if (reg_addr < MIN_REG_ADDRESS || reg_addr > MAX_REG_ADDRESS)
#endif
	{
		/* Send the confirmation with status as OUT_OF_RANGE register
		 * address */
		usr_register_write_confirm(VALUE_OUT_OF_RANGE, reg_addr,
				reg_val);
		return;
	}

	tal_trx_reg_write(reg_addr, reg_val);

	/* Send the confirmation with status as SUCCESS */
	usr_register_write_confirm(MAC_SUCCESS, reg_addr, reg_val);
}

/*
 * \brief Read a set of registers
 * \param start_reg_addr  The start address of the group of registers to be read
 * \param end_reg_addr    The end register of the group of registers to be read
 */
void dump_trx_register_values(uint16_t start_reg_addr, uint16_t end_reg_addr)
{
	uint8_t status;
	int8_t num_of_reg_to_read;

	uint8_t reg_val[NO_OF_REGISTERS] = {NUL_VAL};
	num_of_reg_to_read = ((end_reg_addr - start_reg_addr));
	if (num_of_reg_to_read < 0) {
		/* Send the confirmation with status as OUT_OF_RANGE register
		 * address */
		usr_register_dump_confirm(INVALID_REGISTER_ORDER,
				start_reg_addr,
				end_reg_addr,
				reg_val);
		return;
	}

	status
		= tal_dump_registers(start_reg_addr, end_reg_addr,
			(reg_val + 1));

	reg_val[0] = num_of_reg_to_read + 1;

	if (status == MAC_SUCCESS) {
		/* Send the confirmation with status as MAC_SUCCESS register
		 * address */
		usr_register_dump_confirm(MAC_SUCCESS,
				start_reg_addr,
				end_reg_addr,
				reg_val);
	} else {
		usr_register_dump_confirm(VALUE_OUT_OF_RANGE,
				start_reg_addr,
				end_reg_addr,
				reg_val);
	}
}

void rx_on_test(bool start_stop_param)
{

	// Implementation for Rx on mode
	if(start_stop_param)
	{
		if(node_info.main_state != PER_TEST_RECEPTOR)
		{
			
			set_trx_state(CMD_RX_ON);
			curr_trx_config_params.trx_state = RX_ON ;
		}
		// handle rx on mode in rcptor tx done cb
		rx_on_mode = true;
	}
	else
	{
		set_trx_state(CMD_RX_AACK_ON);
		curr_trx_config_params.trx_state = RX_AACK_ON ;
		rx_on_mode = false;
	}
	usr_rx_on_confirm(MAC_SUCCESS,start_stop_param);
}

#if ((TAL_TYPE != AT86RF230B) || ((TAL_TYPE == AT86RF230B) && \
	(defined CW_SUPPORTED)))

/**
 * \brief Save all user settings before Start of CW transmission
 */
void save_all_settings(void)
{
#if (ANTENNA_DIVERSITY == 1)
	tal_get_curr_trx_config(ANT_DIVERSITY, &ant_div_before_ct);

	if (ANT_DIV_DISABLE == ant_div_before_ct) {
		tal_get_curr_trx_config(ANT_CTRL, &ant_sel_before_ct);
	}

#endif

#if (TAL_TYPE == AT86RF233)
	cc_band_ct = trx_bit_read(SR_CC_BAND);
	cc_number_ct = trx_bit_read(SR_CC_NUMBER);
#endif /* End of #if(TAL_TYPE == AT86RF233) */
}

/**
 * \brief Recover all user settings before Start of CW transmission
 */
void recover_all_settings(void)
{
	int8_t tx_pwr_dbm;
	uint8_t temp_var;
	pib_value_t pib_value;

#if (ANTENNA_DIVERSITY == 1)
	if (ANT_DIV_DISABLE == ant_div_before_ct) {
		tal_ant_div_config(ANT_DIVERSITY_DISABLE, ant_sel_before_ct);
	}

#endif

#if (TAL_TYPE == AT86RF233)
	/* Set the ISM frequency back   */
	if (CC_BAND_0 != cc_band_ct) {
		tal_set_frequency_regs(cc_band_ct, cc_number_ct);
	}

#endif /* End of #if(TAL_TYPE == AT86RF233) */

	/*RPC settings are reseted during tal_reset,hence reconfiguring based
	 *on old config*/
#if ((TAL_TYPE == ATMEGARFR2)||(TAL_TYPE == AT86RF233))
	if (true == curr_trx_config_params.rpc_enable) {
		tal_rpc_mode_config(ENABLE_ALL_RPC_MODES); /* RPC feature
		                                            * configuration. */
	} else {
		tal_rpc_mode_config(DISABLE_ALL_RPC_MODES);
	}

#endif

#if (TAL_TYPE != AT86RF230B)
	/* set the desensitization settings back */
	if (true == curr_trx_config_params.rx_desensitize) {
		tal_set_rx_sensitivity_level(RX_DESENSITIZE_LEVEL);
	}

#endif

#if ((TAL_TYPE != AT86RF212) && (TAL_TYPE != AT86RF212B))
	if (last_tx_power_format_set == 0) {
		uint8_t tx_pwr_reg = curr_trx_config_params.tx_power_reg;
		tal_set_tx_pwr(REGISTER_VALUE, tx_pwr_reg);
	} else
#endif
	{
		tx_pwr_dbm = curr_trx_config_params.tx_power_dbm;
		temp_var = CONV_DBM_TO_phyTransmitPower(tx_pwr_dbm);
		pib_value.pib_value_8bit = temp_var;
		tal_pib_set(phyTransmitPower, &pib_value);
	}
}

#endif /* End of #if ((TAL_TYPE != AT86RF230B) || ((TAL_TYPE == AT86RF230B) &&
        *(defined CW_SUPPORTED))) */


#ifdef EXT_RF_FRONT_END_CTRL

/*
 * \brief handle the tx power settings in case of External PA enabled,
 * and the channel changes from or to 26.This is to meet the FCC compliance
 *
 * \param Current channel and Previous channel
 */
void limit_tx_power_in_ch26(uint8_t curr_chnl, uint8_t prev_chnl)
{
	pib_value_t pib_value;

	/* If the cuurent channel set to 26*/
	if (curr_chnl == CHANNEL_26) {
		/* Get last previous non 26 channel tx power  */
		if (prev_chnl != CHANNEL_26) {
			tal_pib_get(phyTransmitPower, &prev_non_26chn_tx_power);
		}

		/* If the Tx power is more than 13dBm, i.e. TX_PWR < 0x0d */
		if (trx_bit_read(SR_TX_PWR) <= MAX_TX_PWR_REG_VAL_CH26) {
			pib_value.pib_value_8bit = DEFAULT_TX_POWER_CH26;
			tal_pib_set(phyTransmitPower, &pib_value);
			curr_trx_config_params.tx_power_reg = trx_bit_read(
					SR_TX_PWR);
			curr_trx_config_params.tx_power_dbm
				= CONV_phyTransmitPower_TO_DBM(
					pib_value.pib_value_8bit);
		}
	} else {
		/* if the channel changed from 26 to other  */
		if (prev_chnl == CHANNEL_26) {
			/* Set back the tx power to default value i.e. 20dBm,
			 *TX_PWR 0x09 */
			pib_value.pib_value_8bit = prev_non_26chn_tx_power;
			tal_pib_set(phyTransmitPower, &pib_value);
			curr_trx_config_params.tx_power_reg = trx_bit_read(
					SR_TX_PWR);
			curr_trx_config_params.tx_power_dbm
				= CONV_phyTransmitPower_TO_DBM(
					pib_value.pib_value_8bit);
		}
	}
}

#endif /* End of EXT_RF_FRONT_END_CTRL */

/**
 * \brief The reverse_float is used for reversing a float variable for
 * supporting BIG ENDIAN systems
 * \param float_val Float variable to be reversed
 */
float reverse_float( const float float_val )
{
	float retuVal;
	char *floatToConvert = (char *)&float_val;
	char *returnFloat = (char *)&retuVal;
#if UC3
	/* swap the bytes into a temporary buffer */
	returnFloat[0] = floatToConvert[3];
	returnFloat[1] = floatToConvert[2];
	returnFloat[2] = floatToConvert[1];
	returnFloat[3] = floatToConvert[0];
#else
	returnFloat[0] = floatToConvert[0];
	returnFloat[1] = floatToConvert[1];
	returnFloat[2] = floatToConvert[2];
	returnFloat[3] = floatToConvert[3];
#endif
	return retuVal;
}
/* EOF */
