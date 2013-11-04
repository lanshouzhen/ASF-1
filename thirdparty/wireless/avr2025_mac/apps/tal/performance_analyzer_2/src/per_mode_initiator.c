/**
 * \file per_mode_initiator.c
 *
 * \brief Initiator/Transmitter functionalities in PER Measurement mode -
 * Performance Analyzer application
 * This is the source code of a Packet Error Rate Measurement mode as Initiator.
 *
 *
 * Copyright (c) 2013 Atmel Corporation. All rights reserved.
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
 */

/*
 * Copyright(c) 2012, Atmel Corporation All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/* === INCLUDES ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "app_init.h"
#include "tfa.h"
#include "tal_internal.h"
#include "tal_helper.h"
#include "tal_pib.h"
#include "app_frame_format.h"
#include "sio2host.h"
#include "sio2ncp.h"
#include "perf_api.h"
#include "conf_board.h"
/* === TYPES =============================================================== */

/**
 * \brief Structure used for updating the parameters after sending them to peer
 *
 */
typedef struct {
	uint8_t param_type;
	uint8_t param_value;
} set_param_cb_t;

/*=====EXTERBNALS============================================================*/

/* === MACROS ============================================================== */

/**
 * \addtogroup group_per_mode_init
 * \{
 */

/* Defines used for transceiver operation mode */
#define TX_OP_MODE                              (0x01)
#define TEST_FRAMES_SENT                        (0x02)
#define WAIT_FOR_TEST_RES                       (0x03)
#define SET_PARAMETER                           (0x04)
#if (ANTENNA_DIVERSITY == 1)
#define DIVERSITY_STATUS_REQ                    (0x05)
#define DIVERSITY_SET_REQ                       (0x06)
#endif /* #if (ANTENNA_DIVERSITY == 1) */
#define CONTINUOUS_TX_MODE                      (0x07)
#define IDENTIFY_PEER                           (0x08)
#ifdef CRC_SETTING_ON_REMOTE_NODE
#define CRC_STATUS_REQ_WAIT                     (0x09)
#define CRC_SET_REQ_WAIT                        (0x0A)
#endif /* #ifdef CRC_SETTING_ON_REMOTE_NODE */
#define PEER_INFO_RSP_WAIT                      (0x0B)
#define DISCONNECT_PEER                         (0x0C)
#define SET_DEFAULT_CONFIG_PEER                 (0x0D)
#define PER_TEST_START                          (0x0E)
#define RANGE_TEST_START                        (0x0F)
#define RANGE_TEST_TX                           (0x10)
#define RANGE_TEST_STOP                         (0x11)

#define RANGE_TST_PKT_SEQ_POS                    (11)


#define MAX_REG_ADDRESS                         (0x3ffe)
#define MIN_REG_ADDRESS                         (0x0000)


#define RX_DESENSITIZE_LEVEL                    (0x08)
#define NO_RX_DESENSITIZE_LEVEL                 (0x00)
#define INVALID_VALUE                           (0xff)

#if (ANTENNA_DIVERSITY == 1)
#define ENABLE_ANT_DIVERSITY                      (0)
#define ENABLE_ANTENNA_1                          (1)
#define ENABLE_ANTENNA_2                          (2)
#endif /* End of ANETENNA_DIVERSITY */

/* To handle Transceiver reset */
#define TRX_RESET                               (0)
#define DEFAULT_NO_OF_TEST_FRAMES               (100)
#define DEFAULT_FRAME_LENGTH                    (20)
#define BIT_COUNT                               (32)
#define MAX_SCAN_DURATION                       (14)
#define NO_OF_REGISTERS                         (256)

#define TIMEOUT_FOR_RESPONSE_IN_MICRO_SEC       (200000)
#define RANGE_TX_BEACON_INTERVAL                (3000000)
#define RANGE_TX_BEACON_START_INTERVAL          (100000)
#define PULSE_CW_TX_TIME_IN_MICRO_SEC           (50000)
#define MICRO_SEC_MULTIPLIER                    (1.0 / 1000000)
#define MILLI_VOLT_MULTIPLIER                   (1.0 / 1000)

/* === PROTOTYPES ========================================================== */
static void configure_frame_sending(trx_id_t trx);
static void send_parameters_changed(trx_id_t trx,uint8_t param, uint8_t val);
static bool send_result_req(trx_id_t trx);
static bool send_peer_info_req(trx_id_t trx);
static void wait_for_reply_timer_handler_cb(void *parameter);
static bool send_identify_command(trx_id_t trx);
static bool send_disconnect_command(trx_id_t trx);
static void set_parameter_on_transmitter_node(trx_id_t trx, retval_t status);
static void start_test(trx_id_t trx);
static void start_range_test(trx_id_t trx);

#ifdef CRC_SETTING_ON_REMOTE_NODE
static bool send_crc_set_req(trx_id_t trx, crc_set_req_t crc_msg);
static bool send_crc_status_req(trx_id_t trx);
static void get_crc_settings_peer_node(trx_id_t trx);
static void config_crc_peer_node(trx_id_t trx,bool config_value);
#endif /* End of CRC_SETTING_ON_REMOTE_NODE */

#if (ANTENNA_DIVERSITY == 1)
static bool send_diversity_set_req(div_set_req_t div_msg);
static bool send_diversity_status_req(trx_id_t trx);
static void get_diversity_settings_peer_node(trx_id_t trx);

#endif /* End of #if (ANTENNA_DIVERSITY == 1) */

static void stop_pulse_cb(void *callback_parameter);
static void save_all_settings(trx_id_t trx);
static void recover_all_settings(trx_id_t trx);


static void toggle_trx_sleep(trx_id_t trx);

static float calculate_time_duration(trx_id_t trx);
static float calculate_net_data_rate(trx_id_t trx,float per_test_duration_sec);
static void config_per_test_parameters(trx_id_t trx);
static void set_channel(trx_id_t trx, uint8_t channel);
static void set_channel_page(trx_id_t trx, uint8_t channel_page);
static void set_tx_power(trx_id_t trx, uint8_t tx_power_format, int8_t power_value);
static void config_ack_request(trx_id_t trx, bool config_value);
static void config_csma(trx_id_t trx, bool config_value);
static void config_frame_retry(trx_id_t trx, bool config_value);

#if (ANTENNA_DIVERSITY == 1)
static void set_antenna_diversity_settings(trx_id_t trx,uint8_t config_value);
static void config_antenna_diversity_peer_node(trx_id_t trx,uint8_t config_value);

#endif /* #if (ANTENNA_DIVERSITY == 1) */


static void config_rx_desensitization(trx_id_t trx,bool config_value);

static void set_transceiver_state(trx_id_t trx, uint8_t trx_state);

static void set_phy_frame_length(trx_id_t trx, uint16_t frame_len);
static bool send_set_default_config_command(trx_id_t trx);
static bool send_per_test_start_cmd(trx_id_t trx);
static float reverse_float( const float float_val );

static bool send_range_test_start_cmd(trx_id_t trx);
static bool send_range_test_stop_cmd(trx_id_t trx);

static float reverse_float( const float float_val );

/* === GLOBALS ============================================================= */

static bool range_test_in_progress[NO_TRX] = {false,false};
static bool scanning[NO_TRX] = {false,false};
static bool trx_sleep_status[NO_TRX] = {false, false};
static bool peer_found[NO_TRX] = {false,false};
static uint8_t scan_duration[NO_TRX];
static uint8_t seq_num_initiator[NO_TRX];
static uint16_t channel_before_scan[NO_TRX];
static uint8_t op_mode[NO_TRX] = {TX_OP_MODE,TX_OP_MODE};
static uint16_t no_of_roll_overs[NO_TRX];
static uint32_t start_time[NO_TRX];
static uint32_t end_time[NO_TRX];
static uint32_t restart_time[NO_TRX];
static uint32_t scan_channel_mask[NO_TRX];
static uint32_t frame_no_ack[NO_TRX];
static uint32_t frame_access_failure[NO_TRX];
static uint32_t frame_failure[NO_TRX];
static uint32_t frames_to_transmit[NO_TRX];
static set_param_cb_t set_param_cb[NO_TRX];
static uint8_t num_channels[NO_TRX];
static uint8_t last_tx_power_format_set[NO_TRX];
static void configure_range_test_frame_sending(trx_id_t trx);
static bool send_range_test_marker_rsp(trx_id_t trx);



/**
 * This is variable is to keep track of the specific features supported
 */
static uint32_t fw_feature_mask = 0;

static uint8_t phy_tx_power[NO_TRX];

#if (ANTENNA_DIVERSITY == 1)
static uint8_t ant_sel_before_ct[NO_TRX];
static uint8_t ant_div_before_ct[NO_TRX];
#endif /* End of #if (ANTENNA_DIVERSITY == 1) */


static uint8_t last_tx_power_format_se[NO_TRX];

static uint32_t range_test_frame_cnt[NO_TRX] = {0,0};

/**
 * \brief The reverse_float is used for reversing a float variable for
 * supporting BIG ENDIAN systems
 * \param float_val Float variable to be reversed
 */
static float reverse_float( const float float_val )
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

/* Size constants for PERformance configuration parameters */
FLASH_DECLARE(uint8_t perf_config_param_size[]) = {
	sizeof(uint8_t),            /* channel */
	sizeof(uint8_t),            /* channel page */
	sizeof(uint8_t),            /* TX power_reg */
	sizeof(int8_t),             /* TX_power_dBm */
	sizeof(uint8_t),            /* csma */
	sizeof(uint8_t),            /* frame retry */
	sizeof(uint8_t),            /* Ack request */
	sizeof(uint8_t),            /* Antenna diversity */
	sizeof(uint8_t),            /* Antenna diversity on remote node */
	sizeof(uint8_t),            /* desensitization */
	sizeof(uint8_t),            /* Transceiver state */
	sizeof(uint8_t),            /* CRC on remote node */
	sizeof(uint32_t),           /* No. of test frames */
	sizeof(uint16_t),            /* Physical frame length */ //sriram
	sizeof(uint8_t),            /* RPC */
	sizeof(float),              /* ISM frequency */
};

/* Database to maintain the default settings of the configurable parameter */
trx_config_params_t default_trx_config_params[NO_TRX];

/* Database to maintain the updated/latest settings of the configurable
 * parameters */
trx_config_params_t curr_trx_config_params[NO_TRX];

/* ! \} */
/* === IMPLEMENTATION ====================================================== */

/*
 * \brief Initialize the application in PER Measurement mode as Initiator
 * \param parameter Pointer to the parameter to be carried , if any
 */
void per_mode_initiator_init(trx_id_t trx,void *parameter)
{
  
#ifdef EXT_RF_FRONT_END_CTRL
    uint8_t config_tx_pwr[NO_TRX];
#endif
    
	/* PER TEST Initiator sequence number */
	seq_num_initiator[trx] = rand();

	/* Node connection status */
	peer_found[trx] = node_info[trx].peer_found;

	config_per_test_parameters(trx);

	if (PER_TEST_INITIATOR == node_info[trx].main_state) {
		/* Get Peer device information */
		if (send_peer_info_req(trx)) {
			sw_timer_start(APP_TIMER_TO_TX,
					TIMEOUT_FOR_RESPONSE_IN_MICRO_SEC,
					SW_TIMEOUT_RELATIVE,
					(FUNC_PTR)wait_for_reply_timer_handler_cb,
					NULL);
			op_mode[trx] = PEER_INFO_RSP_WAIT;
		}
	} else {
		app_led_event(LED_EVENT_POWER_ON);

		/* Put the transceiver in TRX OFF state- default state for
		 * Single node tests */
		set_trx_state(trx,RF_TXPREP);

		/* Send the confirmation to the PC application via Serial
		 * interface */
		usr_perf_start_confirm(trx,MAC_SUCCESS,
				START_MODE_SINGLE_NODE,
				&default_trx_config_params[trx],
				NUL_VAL,
				NULL,
				NULL,
				NULL,
				NUL_VAL);
	}
#ifdef EXT_RF_FRONT_END_CTRL
    /* Enable RF front end control in PER Measurement mode*/
    tal_ext_pa_ctrl(PA_EXT_ENABLE);
    /* set the TX power to default level */
    config_tx_pwr = TAL_TRANSMIT_POWER_DEFAULT;
    tal_pib_set(phyTransmitPower, (pib_value_t *)&config_tx_pwr);
#endif /* End of EXT_RF_FRONT_END_CTRL */
    
	/* keep the compiler happy */
	parameter = parameter;
}

/*
 * \brief Application task for PER Measurement mode as initiator
 *
 * This function
 * - Implements the PER Measurement mode state machine.
 *
 * - On user inputs through serial app executes various tests
 */
#include "led.h"
void per_mode_initiator_task(trx_id_t trx)
{
	/* If any packets need to be transferred */
	if (frames_to_transmit[trx] > 0) {
		/* PER measurement test is on as more frames has to
		 * be transmitted. On completion of the test result also has to
		 * be
		 * queried from the remote node */
		if (!node_info[trx].transmitting) {
			//LED_Off(LED0);
			//pal_get_current_time(&tstamp);
			//printf("\n\rTime During Trx Start %ld\n\r",tstamp);
			//delay_ms(50);
			node_info[trx].transmitting = true;
			node_info[trx].tx_frame_info->mpdu[PL_POS_SEQ_NUM-1]++; //sriram
			if (curr_trx_config_params[trx].csma_enabled) {
				tal_tx_frame(trx,node_info[trx].tx_frame_info,
						CSMA_UNSLOTTED,
						curr_trx_config_params[trx].retry_enabled );
						//LED_On(LED0);
			} else {
				tal_tx_frame(trx,node_info[trx].tx_frame_info,
						NO_CSMA_NO_IFS,//sriram
						curr_trx_config_params[trx].retry_enabled );
						//LED_On(LED0);
			}
		}
	} else {
		switch (op_mode[trx]) {
		case CONTINUOUS_TX_MODE:

		/* While CW transmission wait for stop cmd to stop transmitting
		**/
		case SET_PARAMETER:

			/* While setting parameter on remote node wait for call
			 * back
			 *  from the TAL specifying the result
			 */
#if (ANTENNA_DIVERSITY == 1)
		case DIVERSITY_STATUS_REQ:

		/* While querying the diversity settings on remote node wait for
		 * result */
		case DIVERSITY_SET_REQ:

			/* While changing the diversity setting on remote node
			 * wait
			 * for call back from the TAL specifying the result
			 */
#endif /* #if (ANTENNA_DIVERSITY == 1) */
#ifdef CRC_SETTING_ON_REMOTE_NODE
		case CRC_STATUS_REQ_WAIT:

		/* While querying the CRC settings on remote node wait for
		 * result */
		case CRC_SET_REQ_WAIT:

			/* While changing the CRC setting on remote node wait
			 * for
			 * call back from the TAL specifying the result
			 */
#endif /* #ifdef CRC_SETTING_ON_REMOTE_NODE */
		case IDENTIFY_PEER:
		/* Wait for result while identify peer command is sent */
		case DISCONNECT_PEER:
		/* Wait for result while disconnect peer command is sent */
		case WAIT_FOR_TEST_RES:
			/* Wait for PER test result */
			break;

		case TEST_FRAMES_SENT:
		{
			/* Test frames has been sent, now ask for the result */
			if (send_result_req(trx)) {
				sw_timer_start(APP_TIMER_TO_TX,
						TIMEOUT_FOR_RESPONSE_IN_MICRO_SEC,
						SW_TIMEOUT_RELATIVE,
						(FUNC_PTR)wait_for_reply_timer_handler_cb,
						NULL);
				op_mode[trx] = WAIT_FOR_TEST_RES;
			} else {
				op_mode[trx] = TX_OP_MODE;
			}
		}
		break;

		default:
			break;
		}
	}
}

/** \brief This function is called periodically by the range test
 * timer to initiate the transmission of range test packets to the receptor
 * \param parameter pass parameters to timer handler
 */
static void  range_test_timer_handler_rf24_cb(void *parameter)
{
	trx_id_t trx = RF24;
	if (!node_info[trx].transmitting) {
		/* Update the FCF and payload before transmission */
		configure_range_test_frame_sending(trx);

		/* Transmit the Range Test Packet */
		if (curr_trx_config_params[trx].csma_enabled) {
			tal_tx_frame(trx,node_info[trx].tx_frame_info,
					CSMA_UNSLOTTED,
					curr_trx_config_params[trx].retry_enabled );
		} else {
			tal_tx_frame(trx,node_info[trx].tx_frame_info,
					NO_CSMA_NO_IFS,
					curr_trx_config_params[trx].retry_enabled );
		}

		node_info[trx].transmitting = true;

	}
    		sw_timer_start(T_APP_TIMER_RANGE_RF24,
				RANGE_TX_BEACON_INTERVAL,
				SW_TIMEOUT_RELATIVE,
				(FUNC_PTR)range_test_timer_handler_rf24_cb,
				NULL);
}

/** \brief This function is called periodically by the range test
 * timer to initiate the transmission of range test packets to the receptor
 * \param parameter pass parameters to timer handler
 */
static void  range_test_timer_handler_rf09_cb(void *parameter)
{
	trx_id_t trx = RF09;
	if (!node_info[trx].transmitting) {
		/* Update the FCF and payload before transmission */
		configure_range_test_frame_sending(trx);

		/* Transmit the Range Test Packet */
		if (curr_trx_config_params[trx].csma_enabled) {
			tal_tx_frame(trx,node_info[trx].tx_frame_info,
					CSMA_UNSLOTTED,
					curr_trx_config_params[trx].retry_enabled );
		} else {
			tal_tx_frame(trx,node_info[trx].tx_frame_info,
					NO_CSMA_NO_IFS,
					curr_trx_config_params[trx].retry_enabled );
		}

		node_info[trx].transmitting = true;

	}
    		sw_timer_start(T_APP_TIMER_RANGE_RF09,
				RANGE_TX_BEACON_INTERVAL,
				SW_TIMEOUT_RELATIVE,
				(FUNC_PTR)range_test_timer_handler_rf09_cb,
				NULL);
}

/**
 * \brief Wait for reply timer handler is called if any command sent on air
 * times out before any response message is received.
 * This function  handles all the scenarios if the remote node is not accessible
 * \param parameter pass parameters to timer handler
 */
static void wait_for_reply_timer_handler_cb(void *parameter)
{
	trx_id_t trx = (trx_id_t)parameter;
	switch (op_mode[trx])
	{
	case WAIT_FOR_TEST_RES:
	{
		/* Send the PER test Indication as failure as it is not able
		 * to contact peer node after completion of the test
		 */
		usr_per_test_end_indication(trx,UNABLE_TO_CONTACT_PEER,
				NUL_VAL, NUL_VAL, NUL_VAL, NUL_VAL,
				NUL_VAL, NUL_VAL, NUL_VAL, NUL_VAL,
				NUL_VAL, NUL_VAL);
		break;
	}

#if (ANTENNA_DIVERSITY == 1)
	case DIVERSITY_STATUS_REQ:
	{
		uint8_t ant_div_settings = NUL_VAL;

		/* Send Get confirmation with status UNABLE_TO_CONTACT_PEER */
		usr_perf_get_confirm(trx,UNABLE_TO_CONTACT_PEER,
				PARAM_ANT_DIVERSITY_ON_PEER,
				(param_value_t *)&ant_div_settings);
		break;
	}

#endif /* #if (ANTENNA_DIVERSITY == 1) */
#ifdef CRC_SETTING_ON_REMOTE_NODE
	case CRC_STATUS_REQ_WAIT:
	{
		bool crc_settings = false;
		/* Send Get confirmation with status UNABLE_TO_CONTACT_PEER */
		usr_perf_get_confirm(trx,UNABLE_TO_CONTACT_PEER,
				PARAM_CRC_ON_PEER,
				(param_value_t *)&crc_settings);
		break;
	}

#endif /* #ifdef CRC_SETTING_ON_REMOTE_NODE */
	case PEER_INFO_RSP_WAIT:
	{
		/* Send the confirmation with status as SUCCESS */
		usr_perf_start_confirm(trx,NO_PEER_FOUND,
				START_MODE_PER,
				&default_trx_config_params[trx],
				NUL_VAL,
				NULL,
				NULL,
				NULL,
				NUL_VAL );
	}
		/* As the peer did not responds with its details */
		app_reset(trx);
		break;

	default:
		break;
	}
	/* notifies remote node has problems */
	op_mode[trx] = TX_OP_MODE;
	parameter = parameter;
}

/*
 * \brief Callback that is called once tx is done in the
 * PER_FOUND_PER_INITIATOR state .
 *
 * \param status    Status of the transmission procedure
 * \param frame     Pointer to the transmitted frame structure
 */
void per_mode_initiator_tx_done_cb(trx_id_t trx,retval_t status, frame_info_t *frame)
{
	static uint8_t tx_count;
	switch (op_mode[trx]) {
	case SET_PARAMETER:
	{
		/* After successful transmission, set the params on Initiator
		 * node */
		set_parameter_on_transmitter_node(trx,status);
		op_mode[trx] = TX_OP_MODE;
	}
	break;

	case WAIT_FOR_TEST_RES:
	{
		/* If no ack received from remote for the send_result_req sent
		**/
		if (MAC_SUCCESS != status) {
			/* if PER test result request fails it is enunciated to
			 * the user and waits for inputs from user
			 */
			sw_timer_stop(APP_TIMER_TO_TX);

			/* Send the PER test Indication as failure as it is not
			 * able
			 * to contact peer node after completion of the test
			 */
			usr_per_test_end_indication(trx,UNABLE_TO_CONTACT_PEER,
					NUL_VAL,
					NUL_VAL, NUL_VAL, NUL_VAL, NUL_VAL,
					NUL_VAL, NUL_VAL, NUL_VAL, NUL_VAL,
					NUL_VAL);

			op_mode[trx] = TX_OP_MODE;
		}

		break;
	}

#if (ANTENNA_DIVERSITY == 1)
	case DIVERSITY_SET_REQ:
	{
		app_payload_t *msg;

		op_mode[trx] = TX_OP_MODE;

		/* Point to the message : 1 =>size is first byte and 2=>FCS*/
		msg
			= (app_payload_t *)(frame->mpdu+FRAME_OVERHEAD);

		div_set_req_t *ant_div_set_req
			= (div_set_req_t *)&(msg->payload.
				div_set_req_data);

		if (MAC_SUCCESS == status) {
			/* Update the database with this change  */
			curr_trx_config_params[trx].antenna_diversity_on_peer
				= ant_div_set_req->status;
			curr_trx_config_params[trx].antenna_selected_on_peer
				= ant_div_set_req->ant_sel;
			/* Send Confirmation with status SUCCESS */
			usr_perf_set_confirm(trx,status,
					PARAM_ANT_DIVERSITY_ON_PEER,
					(param_value_t *)&curr_trx_config_params[trx].antenna_selected_on_peer);
		} else {
			/* if Antenna diversity set request fails it is
			 * enunciated
			 * to the user and waits for inputs from user
			 */
			sw_timer_stop(APP_TIMER_TO_TX);

			/* Send Get confirmation with status
			 * UNABLE_TO_CONTACT_PEER */
			usr_perf_set_confirm(trx,UNABLE_TO_CONTACT_PEER,
					PARAM_ANT_DIVERSITY_ON_PEER,
					(param_value_t *)&curr_trx_config_params[trx].antenna_selected_on_peer);
		}

		break;
	}

	case DIVERSITY_STATUS_REQ:
		/* Do nothing */
		break;

#endif /* #if (ANTENNA_DIVERSITY == 1) */
#ifdef CRC_SETTING_ON_REMOTE_NODE
	case CRC_SET_REQ_WAIT:
	{
		app_payload_t *msg;
		op_mode[trx] = TX_OP_MODE;

		/* Point to the message : 1 =>size is first byte and 2=>FCS*/
		msg
			= (app_payload_t *)(frame->mpdu +FRAME_OVERHEAD);

		crc_set_req_t *crc_set_req
			= (crc_set_req_t *)&(msg->payload.crc_set_req_data);

		if (MAC_SUCCESS == status) {
			/* Update the database with this change  */
			curr_trx_config_params[trx].crc_settings_on_peer
				= crc_set_req->status;
			/* Send Confirmation with status SUCCESS */
			usr_perf_set_confirm(trx,status, PARAM_CRC_ON_PEER,
					(param_value_t *)&curr_trx_config_params[trx].crc_settings_on_peer);
		} else {
			/* if set request fails it is enunciated to
			 * the user and waits for inputs from user
			 */
			sw_timer_stop(APP_TIMER_TO_TX);

			/* Send Get confirmation with status
			 * UNABLE_TO_CONTACT_PEER */
			usr_perf_set_confirm(trx,UNABLE_TO_CONTACT_PEER,
					PARAM_CRC_ON_PEER,
					(param_value_t *)&curr_trx_config_params[trx].crc_settings_on_peer);
		}

		break;
	}

	case CRC_STATUS_REQ_WAIT:
		/* Do nothing */
		break;

#endif /* #ifdef CRC_SETTING_ON_REMOTE_NODE */
	case IDENTIFY_PEER:
	{
		op_mode[trx] = TX_OP_MODE;
		if (MAC_SUCCESS == status) {
			usr_identify_peer_node_confirm(trx,MAC_SUCCESS);
		} else {
			/* if Identify peer request fails it is enunciated to
			 * the user and waits for inputs from user
			 */
			usr_identify_peer_node_confirm(trx,UNABLE_TO_CONTACT_PEER);
		}

		break;
	}

	case DISCONNECT_PEER:
	{
		op_mode[trx] = TX_OP_MODE;
		if (MAC_SUCCESS == status) {
			usr_peer_disconnect_confirm(trx,MAC_SUCCESS);

			/* app reset on source node as the disconnect peer is
			 * successful */

			/* This is to make the node to restart as boot up and
			 * open for fresh peer search
			 */
			app_reset(trx);
		} else {
			/* if Disconnect peer request fails it is enunciated to
			 * the user and waits for inputs from user
			 */
			usr_peer_disconnect_confirm(trx,UNABLE_TO_CONTACT_PEER);
		}

		break;
	}

	case SET_DEFAULT_CONFIG_PEER:
	{
		op_mode[trx] = TX_OP_MODE;
		if (MAC_SUCCESS == status) {
			/* Set the default values for all configurable
			 * parameters */
			config_per_test_parameters(trx);

			/* Send the confirmation with the status as SUCCESS */
			usr_set_default_config_confirm(trx,MAC_SUCCESS,
					&default_trx_config_params[trx]);
		} else { /* Failure */
			 /* Send the confirmation with the status as
			  * UNABLE_TO_CONTACT_PEER */
			usr_set_default_config_confirm(trx,UNABLE_TO_CONTACT_PEER,
					&default_trx_config_params[trx]);
		}

		break;
	}

	case PER_TEST_START:
	{
		op_mode[trx] = TX_OP_MODE;

		/* As start indication is successful start the actual
		 * PER Test*/
		start_test(trx);
	}
	break;

	case RANGE_TEST_START:
	{
		if (MAC_SUCCESS == status) {
			/* As start indication is successful start the actual
			 * RANGE Test in PER Mode*/
			start_range_test(trx);
		} else {
			op_mode[trx] = TX_OP_MODE;
			usr_range_test_start_confirm(trx,UNABLE_TO_CONTACT_PEER);
		}
	}
	break;

	case RANGE_TEST_STOP:
	{

if(trx==RF24)
{
			/* Stop the Range Test Timer */
			sw_timer_stop(T_APP_TIMER_RANGE_RF24);
}
else
{
			/* Stop the Range Test Timer */
			sw_timer_stop(T_APP_TIMER_RANGE_RF09);
}
			/* Set the falg to default */
			range_test_in_progress[trx] = false;
			/* reset the frame sount */
			range_test_frame_cnt[trx] = 0;
			/* Send Stop Confirmation to Host */
			usr_range_test_stop_confirm(trx,MAC_SUCCESS);
			/* Reset the OPMODE */
			op_mode[trx] = TX_OP_MODE;
			app_led_event(LED_EVENT_PEER_SEARCH_DONE);
	
	}
	break;

	case RANGE_TEST_TX:
	{
		app_payload_t *msg;
		/* Point to the message : 1 =>size is first byte and 2=>FCS*/
		msg
			= (app_payload_t *)(frame->mpdu + FRAME_OVERHEAD);
		/* Check whether the tx frame was a range test packet*/
		if (msg->cmd_id == RANGE_TEST_PKT) {
			app_led_event(LED_EVENT_TX_FRAME);

			/* Send the transmitted OTA frame to Host UI for disply
			**/
			usr_range_test_beacon_tx(trx,node_info[trx].tx_frame_info);
		}
	}
	break;

	case TX_OP_MODE:
	{
		if (MAC_SUCCESS != status) {
			if (status == MAC_NO_ACK) {
				frame_no_ack[trx]++;
			} else if (status == MAC_CHANNEL_ACCESS_FAILURE) {
				frame_access_failure[trx]++;
			} else {
				frame_failure[trx]++;
			}
		}

		/* PER test is ON after sending each frame the number of
		 * frames to be sent is decremented
		 */

		/* Added to handle the the peer search case abort case */
		if (frames_to_transmit[trx] > 0) {
			frames_to_transmit[trx]--;
		}

		/* Count when the roll-over happens */
		end_time[trx] = sw_timer_get_time();
		end_time[trx] = (uint32_t)(end_time[trx] * MICRO_SEC_MULTIPLIER);

		if (end_time[trx] == (restart_time[trx] - 1)) {
			no_of_roll_overs[trx] += 1;
			restart_time[trx]--;
		}

		/* Led is toggled indicating the test in progress at the count
		 * of
		 * LED_TOGGLE_COUNT_FOR_PER
		 */
		if (tx_count >= LED_TOGGLE_COUNT_FOR_PER) {
			/* Indicate a TX */
			app_led_event(LED_EVENT_TX_FRAME);
			tx_count = 0;
		} else {
			tx_count++;
		}

		/* If no frames to be transmitted */
		if (frames_to_transmit[trx] == 0) {
			end_time[trx] = sw_timer_get_time();
			op_mode[trx] = TEST_FRAMES_SENT;
			tx_count = 0;
			app_led_event(LED_EVENT_PEER_SEARCH_DONE);

			/* For Boards with two leds LED0 is used as both status
			 * and TX LED,hence the timer may switch off TX_LED
			 * after per test
			 * hence this timer should be stopped */
			sw_timer_stop(APP_TIMER_TO_TX_LED_OFF);
		}

		break;
	}

	case TEST_FRAMES_SENT:
	case CONTINUOUS_TX_MODE:
		/* Do nothing */
		break;

	default:
		break;
	} /* End of  switch */

	/* After transmission is completed, allow next transmission. */
	node_info[trx].transmitting = false;

	/* Keep compiler happy. */
	frame = frame;
}

/**
 * \brief Set the parameters like channel,tx power etc on transmitter node
 * \param status Transmission Status of the node.
 */
static void set_parameter_on_transmitter_node(trx_id_t trx,retval_t status) 
{
	   uint8_t temp_var;

    /* set the parameter on this node */
    if (MAC_SUCCESS != status)
    {
        usr_perf_set_confirm(trx, UNABLE_TO_CONTACT_PEER,
                             set_param_cb[trx].param_type,
                             (param_value_t *)&set_param_cb[trx].param_value);
        return;
    }

    switch (set_param_cb[trx].param_type)
    {
        case CHANNEL:
            {
				
				int8_t dbm_val =0;
				uint8_t tx_pwr =0;		
				uint16_t channel_to_set = (uint16_t)set_param_cb[trx].param_value;
			
			    /* Set back the Tx power to default value when
                 * the channel changed from 26 to other channel
                 */
          
					tal_pib_set(trx,phyCurrentChannel, (pib_value_t *)&channel_to_set);

					/* update the data base with this value */
					curr_trx_config_params[trx].channel = set_param_cb[trx].param_value;
					tal_pib_get(trx,phyTransmitPower,&dbm_val);
					//dbm_val = CONV_phyTransmitPower_TO_DBM(tx_pwr);sriram
					curr_trx_config_params[trx].tx_power_dbm = dbm_val;

                /* Send the confirmation for Set request as Success */
                usr_perf_set_confirm(trx, MAC_SUCCESS,
                                     PARAM_CHANNEL,
                                     (param_value_t *)&set_param_cb[trx].param_value);
                break;
            }
       
            /* Handle changing of channel page */
        case CHANNEL_PAGE:
            {
				
				uint16_t channel;
				int8_t dbm_val =0; 
				uint8_t tx_pwr =0;

					tal_pib_set(trx,phyCurrentPage, (pib_value_t *)&set_param_cb[trx].param_value);


                /* update the data base with this value */
                curr_trx_config_params[trx].channel_page = set_param_cb[trx].param_value;
if((curr_trx_config_params[trx].phy_frame_length > aMaxPHYPacketSize) && (curr_trx_config_params[trx].channel_page !=9) )
{

	curr_trx_config_params[trx].phy_frame_length = aMaxPHYPacketSize; //sriram
	configure_frame_sending(trx);
}
					 tal_pib_get(trx,phyCurrentChannel, &channel);

					 curr_trx_config_params[trx].channel = channel;
					 tal_pib_set(trx,phyTransmitPower,curr_trx_config_params[trx].tx_power_dbm);
					// tal_pib_get(trx,phyTransmitPower, &dbm_val);
					 //dbm_val = CONV_phyTransmitPower_TO_DBM(tx_pwr);sriram
					// curr_trx_config_params[trx].tx_power_dbm = dbm_val;


                /*Send the confirmation with status as SUCCESS */
                usr_perf_set_confirm(trx, MAC_SUCCESS,
                                     PARAM_CHANNEL_PAGE,
                                     (param_value_t *)&set_param_cb[trx].param_value);
                break;
            }
            /* Handle tx power in dBm */
        case TX_POWER_DBM:
            {
                int8_t tx_pwr_dbm;
			
                tx_pwr_dbm = (int8_t) set_param_cb[trx].param_value;
               // temp_var = CONV_DBM_TO_phyTransmitPower(tx_pwr_dbm);
               	tal_pib_set(trx,phyTransmitPower, (pib_value_t *)&tx_pwr_dbm);

                /* update the data base with this value */
                curr_trx_config_params[trx].tx_power_dbm = tx_pwr_dbm;

					/*Tx power in dBm also need to be updated as it changes with reg value */
					tal_get_curr_trx_config(trx,TX_PWR,&(curr_trx_config_params[trx].tx_power_reg));

                /* Send Set confirmation with status SUCCESS */
                usr_perf_set_confirm(trx, MAC_SUCCESS,
                                     PARAM_TX_POWER_DBM,
                                     (param_value_t *)&tx_pwr_dbm);
                break;
            }

            /* Handle tx power in reg value */
        case TX_POWER_REG:
            {

                uint8_t tx_pwr_reg;
                int8_t tx_pwr_dbm;
                tx_pwr_reg = set_param_cb[trx].param_value;
                if (MAC_SUCCESS == tal_convert_reg_value_to_dBm(tx_pwr_reg, &tx_pwr_dbm))
                {
                    //temp_var = CONV_DBM_TO_phyTransmitPower(tx_pwr_dbm);sriram
                    tal_pib_set(trx,phyTransmitPower, (pib_value_t *)&tx_pwr_dbm);
                    /* To make sure that TX_PWR register is updated with the
                     * value whatever user povided.Otherwise lowest dBm power
                     * (highest reg value will be taken)
                     */
                  //  tal_set_tx_pwr(trx,REGISTER_VALUE,tx_pwr_reg);sriram
                    
					/* update the data base with this value */
                    curr_trx_config_params[trx].tx_power_reg = tx_pwr_reg;

                    /*Tx power in dBm also need to be updated as it changes with reg value */
                    curr_trx_config_params[trx].tx_power_dbm = tx_pwr_dbm;

                    /* Send Set confirmation with status */
                    usr_perf_set_confirm(trx, MAC_SUCCESS,
                                         PARAM_TX_POWER_REG,
                                         (param_value_t *)&tx_pwr_reg);
                }
                break;
            }
        default:
            break;
    }
	}

/*
 * \brief Callback that is called if data has been received by trx
 * in the PER_FOUND_PER_INITIATOR state
 *
 * \param mac_frame_info  Frame Pointer to received frame
 */
void per_mode_initiator_rx_cb(trx_id_t trx, frame_info_t *mac_frame_info)
{
	static uint8_t range_test_seq_num[NO_TRX];
    app_payload_t *msg;
	
	/* To keep compiler happy */
	trx = trx;
    
	/* Point to the message : 1 =>size is first byte and 2=>FCS*/
    msg = (app_payload_t *)(mac_frame_info->mpdu + FRAME_OVERHEAD);
	uint16_t lqi_pos = mac_frame_info->length + tal_pib[trx].FCSLen;
	uint16_t ed_pos = lqi_pos+1;
    switch ((msg->cmd_id))
    {
        case RESULT_RSP:
            {
                if (op_mode[trx] == WAIT_FOR_TEST_RES)
                {
                    if ((mac_frame_info->length) == ( FRAME_OVERHEAD +
                                                     ((sizeof(app_payload_t) -
                                                       sizeof(general_pkt_t)) +
                                                      sizeof(result_rsp_t))))
                    {
                        uint32_t aver_lqi;
                        uint32_t aver_rssi;
                        uint32_t number_rx_frames;
                        float per_test_duration_sec;
                        float net_data_rate;

                        uint32_t frames_with_wrong_crc;

                        int8_t rssi_val =CCPU_ENDIAN_TO_LE32(msg->payload.test_result_rsp_data.rssi_avrg_rx);//sriram
						rssi_val = scale_reg_value_to_ed((uint8_t)rssi_val);
                        sw_timer_stop(APP_TIMER_TO_TX);
                        number_rx_frames = (msg->payload.test_result_rsp_data.num_of_frames_rx);
                        aver_lqi = CCPU_ENDIAN_TO_LE32(msg->payload.test_result_rsp_data.lqi_avrg_rx);

#ifdef CRC_SETTING_ON_REMOTE_NODE
                        frames_with_wrong_crc = (msg->payload.test_result_rsp_data.frames_with_wrong_crc);
                        /* Value of 0xffffffff means that CRC errors were not counted */
                        
#else
                        frames_with_wrong_crc = 0XFFFFFFFF;
#endif /* #ifdef CRC_SETTING_ON_REMOTE_NODE */
                        /* get the per test duration */
                        per_test_duration_sec = calculate_time_duration(trx);
                        /* Calculate the net data rate */
                        net_data_rate = (calculate_net_data_rate(trx,per_test_duration_sec));
                        
                        net_data_rate = reverse_float(net_data_rate);
                        per_test_duration_sec = reverse_float(per_test_duration_sec);
                        usr_per_test_end_indication(trx,
													MAC_SUCCESS,
                                                    rssi_val,
                                                    aver_lqi,
                                                    CCPU_ENDIAN_TO_LE32(curr_trx_config_params[trx].number_test_frames),
                                                    number_rx_frames,
                                                    CCPU_ENDIAN_TO_LE32(frame_failure[trx]),
                                                    CCPU_ENDIAN_TO_LE32(frame_no_ack[trx]),
                                                    CCPU_ENDIAN_TO_LE32(frame_access_failure[trx]),
                                                    frames_with_wrong_crc,
                                                    per_test_duration_sec,
                                                    net_data_rate
                                                   );
                        op_mode[trx] = TX_OP_MODE;
                    }
                }
                break;
            }
#if (ANTENNA_DIVERSITY == 1)
        case DIV_STAT_RSP:
            {
                uint8_t ant_div_settings;
                if (op_mode[trx] == DIVERSITY_STATUS_REQ)
                {
                    if ((mac_frame_info->length) == ( FRAME_OVERHEAD +
                                                     ((sizeof(app_payload_t) -
                                                       sizeof(general_pkt_t)) +
                                                      sizeof(div_stat_rsp_t))))
                    {
                        sw_timer_stop(APP_TIMER_TO_TX);
                        /* Antenna diversity is enabled on peer node */
                        if (msg->payload.div_stat_rsp_data.status)
                        {
                            ant_div_settings = ANT_CTRL_0;
                        }
                        else /* Antenna diversity is disabled on peer node */
                        {

                            if (ENABLE_ANTENNA_1 == msg->payload.div_stat_rsp_data.ant_sel)
                            {
                                ant_div_settings = ANT_CTRL_1; /* Antenna diversity Disabled on remote node, ant1 is selected */
                            }
                            else if (ENABLE_ANTENNA_2 == msg->payload.div_stat_rsp_data.ant_sel)
                            {
                                ant_div_settings = ANT_CTRL_2; /* Antenna diversity Disabled on remote node, ant2 is selected */
                            }
                            else
                            {
                                ant_div_settings = INVALID_VALUE; /* Invalid settings */
                            }
                        }
                        /* Send Get confirmation with status SUCCESS */
                        usr_perf_get_confirm(trx, MAC_SUCCESS,
                                             PARAM_ANT_DIVERSITY_ON_PEER,
                                             (param_value_t *)&ant_div_settings);
                    }
                    op_mode[trx] = TX_OP_MODE;
                }
            }
            break;
#endif /* #if (ANTENNA_DIVERSITY == 1) */
#ifdef CRC_SETTING_ON_REMOTE_NODE
        case CRC_STAT_RSP:
            {
                bool crc_settings;
                if (op_mode[trx] == CRC_STATUS_REQ_WAIT)
                {
                    if ((mac_frame_info->length) == ( FRAME_OVERHEAD +
                                                     ((sizeof(app_payload_t) -
                                                       sizeof(general_pkt_t)) +
                                                      sizeof(crc_stat_rsp_t))))
                    {
                        sw_timer_stop(APP_TIMER_TO_TX);
                        if (msg->payload.crc_stat_rsp_data.status)
                        {
                            crc_settings = true;
                        }
                        else
                        {
                            crc_settings = false;
                        }
                        /* Send Get confirmation with status SUCCESS */
                        usr_perf_get_confirm(trx, MAC_SUCCESS,
                                             PARAM_CRC_ON_PEER,
                                             (param_value_t *)&crc_settings);
                    }
                    op_mode[trx] = TX_OP_MODE;
                }
            }
            break;

#endif /* #ifdef CRC_SETTING_ON_REMOTE_NODE */
        case PEER_INFO_RSP:
            {
                if (op_mode[trx] == PEER_INFO_RSP_WAIT)
                {
                    if ((mac_frame_info->length) == ( FRAME_OVERHEAD +
                                                     ((sizeof(app_payload_t) -
                                                       sizeof(general_pkt_t)) +
                                                      sizeof(peer_info_rsp_t))))
                    {
                        sw_timer_stop(APP_TIMER_TO_TX);
                        /* Send the confirmation with status as SUCCESS */
                        usr_perf_start_confirm(trx,
												MAC_SUCCESS,
                                               START_MODE_PER,
                                               &default_trx_config_params[trx],
                                               msg->payload.peer_info_rsp_data.ic_type,
                                               msg->payload.peer_info_rsp_data.soc_mcu_name,
                                               msg->payload.peer_info_rsp_data.trx_name,
                                               msg->payload.peer_info_rsp_data.board_name,
                                               msg->payload.peer_info_rsp_data.mac_address );
                    }
                    op_mode[trx] = TX_OP_MODE;
                }
            }
            break;
	case RANGE_TEST_RSP:
	{

		/*Verify if the response is recieved in the correct Operating
		 * mode*/
		if (op_mode[trx] == RANGE_TEST_TX) {
			/* Verify if the frame was already processed*/
			if (range_test_seq_num[trx] == msg->seq_num) {
				return;
			}

			/* Calculate the ED value and LQI for the received frame
			 * and
			 * also derrive the LQI and ED values sent by the
			 * receptor from the received payload */
			uint8_t phy_frame_len = (uint8_t)mac_frame_info->length; //sriram - > range test pklt length not more than 127

			app_led_event(LED_EVENT_RX_FRAME);

			/* Send the range test rsp indication to Host UI with
			 * the two set of ED and LQI values */

			usr_range_test_beacon_rsp(trx,mac_frame_info,
					mac_frame_info->mpdu[lqi_pos],
					mac_frame_info->mpdu[ed_pos],
					msg->payload.range_tx_data.lqi,
					msg->payload.range_tx_data.ed);
			range_test_seq_num[trx] = msg->seq_num;
		}
	}
	break;

	case RANGE_TEST_MARKER_CMD:
	{
		if (op_mode[trx] == RANGE_TEST_TX) {

			/* Timer to Perform LED indication for received Marker
			 * indication */
			sw_timer_start(T_APP_TIMER,
					LED_BLINK_RATE_IN_MICRO_SEC,
					SW_TIMEOUT_RELATIVE,
					(FUNC_PTR)marker_rsp_timer_handler_cb,
					NULL);

			/* Send response to the receptor on receiving the marker
			 * packet */
			send_range_test_marker_rsp(trx);
			/*send marker indication to Host UI */
			usr_range_test_marker_ind(trx,mac_frame_info,
					mac_frame_info->mpdu[lqi_pos],
					mac_frame_info->mpdu[ed_pos]);
		}
	}
	break;
        default:
            break;
    }
}




/**
 * \brief Function to set trx configure parameters
 *
 */
static void config_per_test_parameters(trx_id_t trx)
{
    uint8_t temp;

    /* Set the default values */
    curr_trx_config_params[trx].ack_request = default_trx_config_params[trx].ack_request = true;
    curr_trx_config_params[trx].csma_enabled = default_trx_config_params[trx].csma_enabled = true;
    curr_trx_config_params[trx].retry_enabled = default_trx_config_params[trx].retry_enabled = false;

#if (ANTENNA_DIVERSITY == 1)

	 /* Disable antenna diversity by default. */
	  curr_trx_config_params[trx].antenna_diversity = default_trx_config_params[trx].antenna_diversity = false;
	  curr_trx_config_params[trx].antenna_selected = default_trx_config_params[trx].antenna_selected = ANT_CTRL_1;
	  /* This is required for set default config request command to set the config parameters to their defaults */
	 /* Disable antenna diversity by default */
	 /* Enable A1/X2 */
	   tal_ant_div_config(trx, ANT_DIVERSITY_DISABLE,ANT_CTRL_1);/* Enable A1/X2 */

#endif


    /* Disable desensitization by default */
    curr_trx_config_params[trx].rx_desensitize = default_trx_config_params[trx].rx_desensitize = false;
    /* Disable Rx desensitization */

    //tal_set_rx_sensitivity_level(trx,NO_RX_DESENSITIZE_LEVEL);


    if (peer_found[trx] == true)
    {
        curr_trx_config_params[trx].trx_state = default_trx_config_params[trx].trx_state = RX_ON;
    }
    else
    {
        curr_trx_config_params[trx].trx_state = default_trx_config_params[trx].trx_state = TRX_OFF;
    }

    curr_trx_config_params[trx].number_test_frames = default_trx_config_params[trx].number_test_frames = DEFAULT_NO_OF_TEST_FRAMES;
    curr_trx_config_params[trx].phy_frame_length = default_trx_config_params[trx].phy_frame_length = DEFAULT_FRAME_LENGTH;

    /* As channel & channel page are already configured during
     * the application initialization so get it
     */
	if(trx == RF24)
	{
		curr_trx_config_params[trx].channel = default_trx_config_params[trx].channel = DEFAULT_CHANNEL_RF24;
		tal_pib_set(trx,phyCurrentChannel, (pib_value_t *)&default_trx_config_params[trx].channel);

		
		/* Make the ISM frequency as null as IEEE channel is set in default case */
		curr_trx_config_params[trx].channel_page = default_trx_config_params[trx].channel_page = TAL_CURRENT_PAGE_DEFAULT_RF24;
		tal_pib_set(trx,phyCurrentPage, (pib_value_t *)&default_trx_config_params[trx].channel_page);
		/* As tx power is already configure by TAL in tal_pib.c get it for application*/
		temp = TAL_TRANSMIT_POWER_DEFAULT;
		tal_pib_set(trx,phyTransmitPower, (pib_value_t *)&temp);
	}
	else
	{
		curr_trx_config_params[trx].channel = default_trx_config_params[trx].channel = DEFAULT_CHANNEL_RF09;
		tal_pib_set(trx,phyCurrentChannel, (pib_value_t *)&default_trx_config_params[trx].channel);//sriram
		curr_trx_config_params[trx].channel_page = default_trx_config_params[trx].channel_page = TAL_CURRENT_PAGE_DEFAULT_RF09;
		tal_pib_set(trx,phyCurrentPage, (pib_value_t *)&default_trx_config_params[trx].channel_page);
		/* As tx power is already configure by TAL in tal_pib.c get it for application*/
		int8_t temp_dbm = TAL_TRANSMIT_POWER_DEFAULT;
		tal_pib_set(trx,phyTransmitPower, (pib_value_t *)&temp_dbm);
	}
 
    
    curr_trx_config_params[trx].tx_power_dbm = default_trx_config_params[trx].tx_power_dbm = TAL_TRANSMIT_POWER_DEFAULT;

		tal_get_curr_trx_config(trx,TX_PWR,&(curr_trx_config_params[trx].tx_power_reg));
		tal_get_curr_trx_config(trx,TX_PWR,&(default_trx_config_params[trx].tx_power_reg));

    /* The following fields has no meaning if there is no peer */
    if (true == peer_found[trx])
    {
#ifdef CRC_SETTING_ON_REMOTE_NODE
        curr_trx_config_params[trx].crc_settings_on_peer = default_trx_config_params[trx].crc_settings_on_peer = false;
#endif

        /* Set the config parameters on peer node */
#if (ANTENNA_DIVERSITY == 1)

			curr_trx_config_params[trx].antenna_diversity_on_peer = default_trx_config_params[trx].antenna_diversity_on_peer = false;
			curr_trx_config_params[trx].antenna_selected_on_peer = default_trx_config_params[trx].antenna_selected_on_peer = ANT_CTRL_1;

#endif /* End of #if (ANTENNA_DIVERSITY == 1) */
    }

}

/*
 * *\brief prints the hardware details
 */

void get_board_details(trx_id_t trx)
{

	/* Enable the mask bit for single/multi channel selection
	 * feature is available in the firmware
	 */
	fw_feature_mask |= MULTI_CHANNEL_SELECT;
	fw_feature_mask |= PER_RANGE_TEST_MODE;
	
	float fw_version = reverse_float(2.0);
	/* Send the Confirmation with the status as SUCCESS */
	if(trx == RF24)
	{
		usr_identify_board_confirm(trx,MAC_SUCCESS,
		IC_TYPE,
		MCU_SOC_NAME,
		"AT86RF215-RF24",
		BOARD_NAME,
		(uint64_t)tal_pib[trx].IeeeAddress,
		fw_version,
		fw_feature_mask);
	}
	else
	{
		usr_identify_board_confirm(trx,MAC_SUCCESS,
		IC_TYPE,
		MCU_SOC_NAME,
		"AT86RF215-RF09",
		BOARD_NAME,
		(uint64_t)tal_pib[trx].IeeeAddress,
		fw_version,
		fw_feature_mask);
		
	}
}

/**
 * \brief Set Transceiver state as per the state given
 * \param trx_state Transceiver state to be set
 */
static void set_transceiver_state(trx_id_t trx, uint8_t trx_state)
{
	retval_t retval;
    /* Check whether transceiver is in sleep state */
    if (true == trx_sleep_status[trx])
    {
        switch (trx_state)
        {
                /* Wake up the transceiver as Toggle sleep cmd is received */
            case TRX_SLEEP:
                {
                    toggle_trx_sleep(trx);
                }
                break;

                /* For any cmd other than toggle sleep, return with error code */
            default:
                {
                    /* Send confirmation with status Failure because the transceiver is in sleep */
                    usr_perf_set_confirm(trx, TRANSCEIVER_IN_SLEEP, PARAM_TRX_STATE, (param_value_t *)&trx_state);
                    return;
                }
                break;
        }

        /* Send Set confirmation with status SUCCESS */
        trx_state = tal_get_trx_status(trx) ; 
        curr_trx_config_params[trx].trx_state = trx_state;
        usr_perf_set_confirm(trx, MAC_SUCCESS,
                             PARAM_TRX_STATE,
                             (param_value_t *)&trx_state);
        return;
    }

    switch (trx_state)
    {
        case TRX_RESET:  /* RESET */
            {
                /* save user setting before reset */
                save_all_settings(trx);
				retval = tal_reset(trx,false);	
				
                if (MAC_SUCCESS == retval)
                {
                    curr_trx_config_params[trx].trx_state = TRX_OFF;
                    recover_all_settings(trx);
                }
            }
            break;
        case TRX_OFF: /* TRX_OFF */
            {

			/* Put the transceiver in TRX OFF state- default state for Single node tests */
			tal_rx_enable(trx,PHY_TRX_OFF);

            curr_trx_config_params[trx].trx_state = TRX_OFF;
            }
            break;
        case PLL_ON: /*trx_prep*/
            {
			
			switch_to_txprep(trx);

            curr_trx_config_params[trx].trx_state = PLL_ON;
            }
            break;
        case RX_ON: /*RX_ON*/
            {

	        /* Put the transceiver in TRX OFF state- default state for Single node tests */
	        tal_rx_enable(trx,PHY_RX_ON);

            curr_trx_config_params[trx].trx_state = RX_ON;
            }
            break;

        case TRX_SLEEP: /* SLEEP */
            {
                /* Put the transceiver in sleep */
                toggle_trx_sleep(trx);
                curr_trx_config_params[trx].trx_state = TRX_SLEEP;
            }
            break;


        default: /* Do nothing */
            break;
    }
    /* Send Set confirmation with status SUCCESS & and current trx state*/
    usr_perf_set_confirm(trx,MAC_SUCCESS,
                         PARAM_TRX_STATE,
                         (param_value_t *)&curr_trx_config_params[trx].trx_state);
}

/**
 *  \brief To toggle between sleep /deep state
 *
 *  \param sleep_mode Type of sleep mode as input in case of RF233
 */

static void toggle_trx_sleep(trx_id_t trx)

{
	if (false == trx_sleep_status[trx])
	{
		
			/* Sleep cmd is successful */
			if (MAC_SUCCESS == tal_trx_sleep(trx))
			{
				trx_sleep_status[trx] = true;
			}

	}
	else
	{

			/* Wakeup from sleep or deep sleep is successful */
			if (MAC_SUCCESS == tal_trx_wakeup(trx) )
			{
				trx_sleep_status[trx] = false;
			}

	}
}


/**
 * \brief Save all user settings before Start of CW transmission
 */
static void save_all_settings(trx_id_t trx)
{
#if (ANTENNA_DIVERSITY == 1)
	tal_get_curr_trx_config(trx,ANT_DIVERSITY, &ant_div_before_ct[trx]);

	if (ANT_DIV_DISABLE == ant_div_before_ct[trx]) {
		tal_get_curr_trx_config(trx,ANT_CTRL, &ant_sel_before_ct[trx]);
	}

#endif
}

/**
 * \brief Recover all user settings before Start of CW transmission
 */
static void recover_all_settings(trx_id_t trx)
{
	//int8_t tx_pwr_dbm;
	//uint8_t temp_var;

#if( ANTENNA_DIVERSITY == 1)
if ( ANT_DIV_DISABLE == ant_div_before_ct[trx])
{
	tal_ant_div_config(trx, ANT_DIVERSITY_DISABLE,ant_sel_before_ct[trx]);

}
#endif


#if(TAL_TYPE != AT86RF230B)
/* set the desensitization settings back */
if (true == curr_trx_config_params[trx].rx_desensitize)
{
	tal_set_rx_sensitivity_level(trx,RX_DESENSITIZE_LEVEL);
}
#endif
//sriram decide whther this can be removed
/*
		if(last_tx_power_format_set[trx] == 0)
		{
			uint8_t tx_pwr_reg = curr_trx_config_params[trx].tx_power_reg;
			tal_set_tx_pwr(trx,REGISTER_VALUE,tx_pwr_reg);
		}
		else
		{
			tx_pwr_dbm = curr_trx_config_params[trx].tx_power_dbm;
			
			temp_var = CONV_DBM_TO_phyTransmitPower(tx_pwr_dbm);
			tal_pib_set(trx,phyTransmitPower, (pib_value_t *)&temp_var);
			
		}*/
tal_pib_set(trx,phyTransmitPower, (pib_value_t *)&curr_trx_config_params[trx].tx_power_dbm);
		
}


/*
 * \brief Send an energy pulse on current channel page
 */
#if ((TAL_TYPE != AT86RF230B) || ((TAL_TYPE == AT86RF230B) && (defined CW_SUPPORTED)))
void pulse_cw_transmission(trx_id_t trx)
{
    uint8_t channel;
	
    op_mode[trx] = CONTINUOUS_TX_MODE;

		tal_pib_get(trx,phyCurrentChannel, &channel);

    /* Save all user settings before continuous tx */
    save_all_settings(trx);

	//	tal_reset(trx,false);

 

    /* Start  the Continuous Wave transmission */
    tfa_continuous_tx_start(trx,CW_MODE);
    /* Start the timer to stop the Continuous transmission */
    sw_timer_start(T_APP_TIMER,
                    PULSE_CW_TX_TIME_IN_MICRO_SEC,
                    SW_TIMEOUT_RELATIVE,
                    (FUNC_PTR)stop_pulse_cb,
                    (void*)trx);
}
#endif


/**
 * \brief Stop sending a CW signal on current channel page
 * \param callback_parameter Pointer to callback, not used here
 */
#if ((TAL_TYPE != AT86RF230B) || ((TAL_TYPE == AT86RF230B) && (defined CW_SUPPORTED)))
static void stop_pulse_cb(void *callback_parameter)
{
	trx_id_t trx = ((trx_id_t) callback_parameter);
    /* Stop CW transmission again */
    tfa_continuous_tx_stop(trx);

    op_mode[trx] = TX_OP_MODE;
    /* recover all user setting set before continuous tx */
    recover_all_settings(trx);

    /* Send the confirmation with status as SUCCESS */
    usr_cont_pulse_tx_confirm(trx,MAC_SUCCESS);

}
#endif

/*
 * \brief Start CW transmission on current channel page
 * \param tx_mode  Continuous transmission mode
 */
#if ((TAL_TYPE != AT86RF230B) || ((TAL_TYPE == AT86RF230B) && (defined CW_SUPPORTED)))
void start_cw_transmission(trx_id_t trx, uint8_t tx_mode)
{

    /* Save all user settings before continuous tx */
    save_all_settings(trx);

    /* Added to ensure CW transmission happen in every attempt */

		//tal_reset(trx,false);

    
#if( ANTENNA_DIVERSITY == 1)
    if ( ANT_DIV_DISABLE == ant_div_before_ct[trx])
    {
            tal_ant_div_config(trx, ANT_DIVERSITY_DISABLE,ant_sel_before_ct[trx]);
    
    }
#endif
    switch (tx_mode)
    {
        case CW_MODE: /* CW mode*/
            {
                /* In CW_MODE the parameter random_content is obsolete. */
                tfa_continuous_tx_start(trx,CW_MODE);
            }
            break;

        case PRBS_MODE: /* PRBS mode*/
            {
                /* Start PRBS_MODE mode using random content. */
                tfa_continuous_tx_start(trx,PRBS_MODE);
            }
            break;
        default:
            {
                usr_cont_wave_tx_confirm(trx, INVALID_ARGUMENT, 0x01, tx_mode);
                return;
            }
    }

    op_mode[trx] = CONTINUOUS_TX_MODE;
    /* Send Set confirmation with status SUCCESS */
    usr_cont_wave_tx_confirm(trx, MAC_SUCCESS, START_CWT, tx_mode);

}
/*
 * \brief Stop CW transmission on current channel page
 * \param tx_mode  Continuous transmission mode
 */
void stop_cw_transmission(trx_id_t trx, uint8_t tx_mode)
{

    /* Stop CW transmission again */
    tfa_continuous_tx_stop(trx);
    /* recover all user setting which were set before continuous tx */
    recover_all_settings(trx);
    op_mode[trx] = TX_OP_MODE;
    usr_cont_wave_tx_confirm(trx, MAC_SUCCESS, STOP_CWT/*stop*/, tx_mode);

}
#endif

/*
 * \brief Read transceiver register
 * \param reg_addr  The address of the register to be read
 */
void read_trx_registers(trx_id_t trx, uint16_t reg_addr)
{
    uint8_t reg_val = INVALID_VALUE;


    if (reg_addr > MAX_REG_ADDRESS)


    {
        /* Send the confirmation with status as OUT_OF_RANGE register address */
        usr_register_read_confirm(trx, VALUE_OUT_OF_RANGE, reg_addr, reg_val);
        return;
    }

	tal_trx_reg_read(trx,reg_addr,&reg_val);

    /* Send the confirmation with status as SUCCESS */
    usr_register_read_confirm(trx, MAC_SUCCESS, reg_addr, reg_val);
}

/*
 * \brief Write Transceiver registers
 * \param reg_addr  The address of the register to be written
 * \param reg_val   The value of the register to be written
 */
void write_trx_registers(trx_id_t trx, uint16_t reg_addr, uint8_t reg_val)
{

    if (reg_addr > MAX_REG_ADDRESS)

    {
        /* Send the confirmation with status as OUT_OF_RANGE register address */
        usr_register_write_confirm(trx, VALUE_OUT_OF_RANGE, reg_addr, reg_val);
        return;
    }

    tal_trx_reg_write(trx,reg_addr, reg_val);

    /* Send the confirmation with status as SUCCESS */
    usr_register_write_confirm(trx, MAC_SUCCESS, reg_addr, reg_val);
}

/*
 * \brief Read a set of registers
 * \param start_reg_addr  The start address of the group of registers to be read
 * \param end_reg_addr    The end register of the group of registers to be read
 */
void dump_trx_register_values(trx_id_t trx, uint16_t start_reg_addr, uint16_t end_reg_addr)
{
  
  uint8_t status;
   int16_t num_of_reg_to_read;
   
  uint8_t reg_val[NO_OF_REGISTERS] = {NUL_VAL};
  num_of_reg_to_read = ((end_reg_addr - start_reg_addr));
  if (num_of_reg_to_read < 0)
    {
        /* Send the confirmation with status as OUT_OF_RANGE register address */
        usr_register_dump_confirm(trx, INVALID_REGISTER_ORDER,
                                  start_reg_addr,
                                  end_reg_addr,
                                  reg_val);
        return;
    }
  
  status = tal_dump_registers(trx,start_reg_addr,end_reg_addr,(reg_val+1));
  
  reg_val[0] = num_of_reg_to_read+1;
  
    if(status==MAC_SUCCESS)
    {
    /* Send the confirmation with status as MAC_SUCCESS register address */
    usr_register_dump_confirm(trx, MAC_SUCCESS,
                              start_reg_addr,
                              end_reg_addr,
                              reg_val);
    
    
    }
  else
  {
   usr_register_dump_confirm(trx, VALUE_OUT_OF_RANGE,
                                  start_reg_addr,
                                  end_reg_addr,
                                  reg_val);
  
  }

}


/*
 * \brief User call back function after ED Scan completion
 * \param energy_level  Energy detected in a channel
 */
void per_mode_initiator_ed_end_cb(trx_id_t trx, uint8_t energy_level)
{
    uint8_t ch_cnt;
	
	uint8_t min_ch;
	uint8_t max_ch;
    uint16_t channel;
    static uint8_t p_in_index[NO_TRX];
	int8_t p_in;

    uint8_t page;
    static ed_scan_result_t ed_scan_result[NO_TRX][16];

    /* Print result */

	tal_pib_get(trx,phyCurrentChannel, &channel);

if(trx==0)
{
	min_ch = 0;
	max_ch =10;

}
else
{
	min_ch = 11;
	max_ch =26;

}
p_in = (int8_t)energy_level ;

    /* get the channel number and its corresponding Pin value */
    ed_scan_result[trx][p_in_index[trx]].channel_no = channel;
    ed_scan_result[trx][p_in_index[trx]].p_in = p_in;

    /* Increment the index  */
    p_in_index[trx]++;

    /* Check for next channel */
    channel = INVALID_VALUE;
    for (ch_cnt = min_ch;
         ch_cnt <= max_ch;
         ch_cnt++)
    {
        if ((scan_channel_mask[trx] & ((uint32_t)1 << ch_cnt)) > 0)
        {
            channel = ch_cnt;
            scan_channel_mask[trx] &= ~((uint32_t)1 << ch_cnt);
            break;
        }
		if (trx == RF09)
		{
			tal_pib_get(trx,phyCurrentPage, &page);
			if (5 == page)
			{
				if (4 == ch_cnt)
				{
					channel = INVALID_VALUE;
				}
			}
		}
    }
    /* Check if all channels were scanned. */
    if (INVALID_VALUE == channel)
    {
        p_in_index[trx] = 0;
        
        scanning[trx] = false;

			/* Set original channel. */
			tal_pib_set(trx,phyCurrentChannel, (pib_value_t *)&channel_before_scan[trx]);
	
        /**
         * Send the ED_END_INDICATION with the no. of channels and
         * the list with channel no and pin values
         */
        usr_ed_scan_end_indication(trx,num_channels[trx], ed_scan_result[trx]);

    }
    else
    {
        /* Scan next channel */

			tal_pib_set(trx,phyCurrentChannel, (pib_value_t *)&channel);
			tal_ed_start(trx,scan_duration[trx]);

    }
}

/*
 *\brief Function to set the various configuration parameters for PER Test
 *
 *\param param_type   Type of the parameter to be set
 *\param param_value  Pointer to the value to be set
 */

void perf_set_req(trx_id_t trx, uint8_t param_type, param_value_t *param_value)
{

    switch (param_type) /* parameter type */
    {
        case PARAM_CHANNEL: /* Channel Set request */
            {
                set_channel(trx, param_value->param_value_8bit);
            }
            break;
        case PARAM_CHANNEL_PAGE: /* Channel Page Set request */
            {
                set_channel_page(trx, param_value->param_value_8bit);
            }
            break;
        case PARAM_TX_POWER_REG: /* TX power in Register value Set request  */
            {

	                set_tx_power(trx, 0, param_value->param_value_8bit);
		            last_tx_power_format_set[trx] = 0;

            }
            break;
        case PARAM_TX_POWER_DBM: /* TX power in dBm value Set request  */
            {
                set_tx_power(trx, 1, (int8_t)param_value->param_value_8bit);
            
					last_tx_power_format_set[trx] = 1;

            }
            break;
        case PARAM_CSMA: /* CSMA configuration request- Enable/Disable */
            {
                config_csma(trx, param_value->param_value_bool);
            }
            break;
        case PARAM_FRAME_RETRY: /* Frame Retry configuration request- Enable/Disable */
            {
                config_frame_retry(trx, param_value->param_value_bool);
            }
            break;
        case PARAM_ACK_REQUEST:/* Auto Ack Request configuration request- Enable/Disable */
            {
                config_ack_request(trx,param_value->param_value_bool);
            }
            break;
#if (ANTENNA_DIVERSITY == 1)
        case PARAM_ANT_DIVERSITY: /* Antenna Diversity Configuration request */
            {
                set_antenna_diversity_settings(trx,param_value->param_value_8bit);
            }
            break;
        case PARAM_ANT_DIVERSITY_ON_PEER:/* Antenna Diversity Configuration on remote request */
            {
                config_antenna_diversity_peer_node(trx,param_value->param_value_8bit);
            }
            break;
#endif /* #if (ANTENNA_DIVERSITY ==1) */
#if (TAL_TYPE != AT86RF230B)
        case PARAM_DESENSITIZATION: /* Configure Rx Desensitization request- Enable /Disable*/
            {
                config_rx_desensitization(trx,param_value->param_value_bool);
            }
            break;
#endif //(TAL_TYPE != AT86RF230B)      
#ifdef CRC_SETTING_ON_REMOTE_NODE           
        case PARAM_CRC_ON_PEER: /* Set CRC settings on Peer node request */
            {
                config_crc_peer_node(trx, param_value->param_value_bool);
            }
            break;            
#endif
        case PARAM_TRX_STATE: /* Set Transceiver state request */
            {
                set_transceiver_state(trx,param_value->param_value_8bit);
            }
            break;

        case PARAM_NO_OF_TEST_FRAMES: /* Set No.Of test Frames for PER test request */
            {

                uint32_t no_of_test_frames ;
                MEMCPY_ENDIAN(&(no_of_test_frames),&(param_value->param_value_32bit),4);
               
                if((no_of_test_frames)== NUL_VAL)
                {
                   usr_perf_set_confirm(trx, VALUE_OUT_OF_RANGE,
                             PARAM_NO_OF_TEST_FRAMES,
                             (param_value_t*)(&no_of_test_frames));
                 
                }
                else
                {
               curr_trx_config_params[trx].number_test_frames = no_of_test_frames;
                /* Send Set confirmation with status SUCCESS */
                usr_perf_set_confirm(trx,MAC_SUCCESS, PARAM_NO_OF_TEST_FRAMES, (param_value_t*)&curr_trx_config_params[trx].number_test_frames);
                }
            }
            break;
        case PARAM_PHY_FRAME_LENGTH:  /* Set PHY frame length for PER test request */
            {
              
                set_phy_frame_length(trx, param_value->param_value_16bit);
            }
            break;
        case PARAM_RPC:
            {

            }
            break;
        case PARAM_ISM_FREQUENCY:
            {

            }
            break;
        default:
            {
                /* Send Set confirmation with status INVALID ARGUMENT */
                usr_perf_set_confirm(trx,INVALID_ARGUMENT, param_type, param_value);
            }
            break;

    }
}


/*
 *\brief Function to get the various configuration parameters for PER Test
 *
 *\param param_type Parameter type to be read
 */
void perf_get_req(trx_id_t trx, uint8_t param_type)
{
    switch (param_type) /* parameter type */
    {
        case PARAM_CHANNEL: /* Channel Get request */
            {
                uint8_t current_channel = 0;

					tal_pib_get(trx,phyCurrentChannel, &current_channel);


                /* Send Get confirmation with status SUCCESS */
                usr_perf_get_confirm(trx, MAC_SUCCESS, PARAM_CHANNEL,  (param_value_t *)&current_channel);
            }
            break;
        case PARAM_CHANNEL_PAGE: /* Channel Page Get request */
            {
                uint8_t ch_page = 0;

					tal_pib_get(trx,phyCurrentPage, &ch_page);


                /* Send Get confirmation with status SUCCESS */
                usr_perf_get_confirm(trx, MAC_SUCCESS, PARAM_CHANNEL_PAGE,  (param_value_t *)&ch_page);

            }
            break;

        case PARAM_TX_POWER_REG: /* TX power in Register value Set request  */
            {
                uint8_t tx_pwr_reg = 0;

                tal_get_curr_trx_config(trx,TX_PWR,&(tx_pwr_reg));

                /* Send Get confirmation with status SUCCESS */
                usr_perf_get_confirm(trx, MAC_SUCCESS, PARAM_TX_POWER_REG,  (param_value_t *)&tx_pwr_reg);
            }
            break;
        case PARAM_TX_POWER_DBM: /* TX power in dBm value Set request  */
            {
                uint8_t tx_pwr = 0;
                int8_t  tx_pwr_dbm = 0;

				tal_pib_get(trx,phyTransmitPower, &tx_pwr_dbm);

               // tx_pwr_dbm = CONV_phyTransmitPower_TO_DBM(tx_pwr);sriram

                /* Send Get confirmation with status SUCCESS */
                usr_perf_get_confirm(trx, MAC_SUCCESS,
                                     PARAM_TX_POWER_DBM,
                                     (param_value_t *)&tx_pwr_dbm);
            }
            break;
        case PARAM_CSMA: /* CSMA configuration request- Enable/Disable */
            {
                /* Send Get confirmation with status SUCCESS */
                usr_perf_get_confirm(trx, MAC_SUCCESS,
                                     PARAM_CSMA,
                                     (param_value_t *)&curr_trx_config_params[trx].csma_enabled);
            }
            break;
        case PARAM_FRAME_RETRY: /* Frame Retry configuration request- Enable/Disable */
            {
                /* Send Get confirmation with status SUCCESS */
                usr_perf_get_confirm(trx, MAC_SUCCESS,
                                     PARAM_FRAME_RETRY,
                                     (param_value_t *)&curr_trx_config_params[trx].retry_enabled);

            }
            break;
        case PARAM_ACK_REQUEST:/* Auto Ack Request configuration request- Enable/Disable */
            {
                /* Send Get confirmation with status SUCCESS */
                usr_perf_get_confirm(trx, MAC_SUCCESS,
                                     PARAM_ACK_REQUEST,
                                     (param_value_t *)&curr_trx_config_params[trx].ack_request);
            }
            break;
#if (ANTENNA_DIVERSITY == 1)
        case PARAM_ANT_DIVERSITY: /* Antenna Diversity Configuration request */
            {
                /* Send Get confirmation with status SUCCESS */
                usr_perf_get_confirm(trx, MAC_SUCCESS,
                                     PARAM_ANT_DIVERSITY,
                                     (param_value_t *)&curr_trx_config_params[trx].antenna_selected);
            }
            break;
        case PARAM_ANT_DIVERSITY_ON_PEER:/* Antenna Diversity Configuration on remote request */
            {
                get_diversity_settings_peer_node(trx);
            }
            break;
#endif /* #if (ANTENNA_DIVERSITY == 1) */
#if (TAL_TYPE != AT86RF230B)
        case PARAM_DESENSITIZATION: /* Configure Rx Desensitization request- Enable /Disable*/
            {
                /* Send Get confirmation with status SUCCESS */
                usr_perf_get_confirm(trx, MAC_SUCCESS,
                                     PARAM_DESENSITIZATION,
                                     (param_value_t *)&curr_trx_config_params[trx].rx_desensitize);
            }
            break;

#endif //(TAL_TYPE != AT86RF230B)
#ifdef CRC_SETTING_ON_REMOTE_NODE
        case PARAM_CRC_ON_PEER: /* Set CRC settings on Peer node request */
            {
                get_crc_settings_peer_node(trx);
            }
            break;
#endif 
        case PARAM_TRX_STATE: /* Set Transceiver state request */
            {
                uint8_t trx_state;
                trx_state = tal_get_trx_status(trx) ; 

                /* Send Get confirmation with status SUCCESS */
                usr_perf_get_confirm(trx, MAC_SUCCESS,
                                     PARAM_TRX_STATE,
                                     (param_value_t *)&trx_state);
            }
            break;
        case PARAM_NO_OF_TEST_FRAMES: /* Set No.Of test Frames for PER test request */
            {
                /* Send get confirmation with status SUCCESS */
                usr_perf_get_confirm(trx, MAC_SUCCESS,
                                     PARAM_NO_OF_TEST_FRAMES,
                                     (param_value_t *)&curr_trx_config_params[trx].number_test_frames);
            }
            break;

        case PARAM_PHY_FRAME_LENGTH:  /* Set PHY frame length for PER test request */
            {
                /* Send get confirmation with status SUCCESS */
                usr_perf_get_confirm(trx, MAC_SUCCESS,
                                     PARAM_PHY_FRAME_LENGTH,
                                     (param_value_t *)&curr_trx_config_params[trx].phy_frame_length);

            }
            break;

        case PARAM_RPC:
            {

					uint8_t dummy_val = 0;
					/* Send Get confirmation with status INVALID ARGUMENT */
					usr_perf_get_confirm(trx,INVALID_ARGUMENT, param_type, (param_value_t *)&dummy_val);	
					break;

            }
            break;
        case PARAM_ISM_FREQUENCY:
            {

					uint8_t dummy_val = 0;
					/* Send Get confirmation with status INVALID ARGUMENT */
					usr_perf_get_confirm(trx, INVALID_ARGUMENT, param_type, (param_value_t *)&dummy_val);
					break;

            }
            break;

        default:
            {
                uint8_t dummy_val = 0;
                /* Send Get confirmation with status INVALID ARGUMENT */
                usr_perf_get_confirm(trx, INVALID_ARGUMENT, param_type, (param_value_t *)&dummy_val);
            }
            break;
    }

}


/**
 * \brief To set PER Test frame length
 *
 * \param frame_len Length of the frame to be set
 */
static void set_phy_frame_length(trx_id_t trx, uint16_t frame_len)
{
	if(tal_pib[trx].phy.modulation == LEG_OQPSK)
	{
    /* Check for maximum allowed IEEE 802.15.4 frame length. */
    if (frame_len > aMaxPHYPacketSize) //sriram
    {
        curr_trx_config_params[trx].phy_frame_length = aMaxPHYPacketSize;
    }
	else if (frame_len < (FRAME_OVERHEAD + 1)) /* 1=> cmdID*/
	{
		curr_trx_config_params[trx].phy_frame_length = (FRAME_OVERHEAD + 1);
	}
	else
	{
		curr_trx_config_params[trx].phy_frame_length = frame_len;
	}
	}
	else 
	{
    if (frame_len > aMaxPHYPacketSize_4g) //sriram
    {
	    curr_trx_config_params[trx].phy_frame_length = aMaxPHYPacketSize_4g;
    }	
   else if (frame_len < (FRAME_OVERHEAD + 1)) /* 1=> cmdID*/
    {
	    curr_trx_config_params[trx].phy_frame_length = (FRAME_OVERHEAD + 1);
    }
    else
    {
	    curr_trx_config_params[trx].phy_frame_length = frame_len;
    }	
	
	}


    /* The FCF has to be updated. */
    configure_frame_sending(trx);

    /* Send Set confirmation with status SUCCESS */
    usr_perf_set_confirm(trx,MAC_SUCCESS,
                         PARAM_PHY_FRAME_LENGTH,
                         (param_value_t *)&curr_trx_config_params[trx].phy_frame_length);
}

/**
 * \brief To set the Channel
 *
 * \param channel channel number to be set
 */
static void set_channel(trx_id_t trx, uint8_t channel)
{
    uint32_t supported_channels;
uint16_t channel_to_set = (uint16_t)channel;
#ifdef SUPPORT_LEGACY_OQPSK
		tal_pib_get(trx,phyChannelsSupported, (uint8_t *)&supported_channels);
#endif

    /* Check the channel is a valid one and is a supported channel */
    if ( (channel < BIT_COUNT) && (supported_channels & ((uint32_t)0x01) << channel) )
    {
        if (true == peer_found[trx])
        {
            send_parameters_changed(trx, CHANNEL, channel);
        }
        else
        {

				int8_t dbm_val =0;
				uint8_t tx_pwr =0;
				tal_pib_set(trx,phyCurrentChannel, (pib_value_t *)&channel_to_set);

				/* Update the database */
				curr_trx_config_params[trx].channel = channel;
				
				tal_pib_get(trx,phyTransmitPower, &dbm_val);
				//dbm_val = CONV_phyTransmitPower_TO_DBM(tx_pwr);
				curr_trx_config_params[trx].tx_power_dbm = dbm_val;
			

            /* Send Set confirmation with status SUCCESS */
            usr_perf_set_confirm(trx,MAC_SUCCESS,
                                 PARAM_CHANNEL,
                                 (param_value_t *)&channel);
        }
    }
    else
    {
        /* Send Set confirmation with status MAC_INVALID_PARAMETER */
        usr_perf_set_confirm(trx,VALUE_OUT_OF_RANGE,
                             PARAM_CHANNEL,
                             (param_value_t *)&channel);

    }
}

/**
 * \brief To set the Channel Page
 *
 * \param channel_page  Channel page to be set
 */
static void set_channel_page(trx_id_t trx, uint8_t channel_page)
{

#if((TAL_TYPE == AT86RF230B) )

    if (channel_page != 0)
    {
        /* Send the confirmation with status as VALUE_OUT_OF_RANGE */
        usr_perf_set_confirm(trx,VALUE_OUT_OF_RANGE,
                             PARAM_CHANNEL_PAGE,
                             (param_value_t *)&channel_page);
        return;
    }
#endif /* End of #if((TAL_TYPE == AT86RF230B) ) */

    switch (channel_page)
    {
        case 0:
		case 9:
        case 2:
        case 16:
        case 17:
        case 5:
        case 18:
        case 19:
            {
                if (true == peer_found[trx])
                {
                    send_parameters_changed(trx,CHANNEL_PAGE, channel_page);
                }
                else
                {


						tal_pib_set(trx,phyCurrentPage, (pib_value_t *)&channel_page);

						/* update the data base with this value */
						curr_trx_config_params[trx].channel_page = channel_page;

if((curr_trx_config_params[trx].phy_frame_length > aMaxPHYPacketSize) && (channel_page!=9) )
{

curr_trx_config_params[trx].phy_frame_length = aMaxPHYPacketSize; //sriram
configure_frame_sending(trx);
}

                    /* Send the confirmation with status as SUCCESS */
                    usr_perf_set_confirm(trx,MAC_SUCCESS,
                                         PARAM_CHANNEL_PAGE,
                                         (param_value_t *)&channel_page);
                }
            }
            break;
        default:
            {
                /* Send the confirmation with status as VALUE_OUT_OF_RANGE */
                usr_perf_set_confirm(trx,VALUE_OUT_OF_RANGE,
                                     PARAM_CHANNEL_PAGE,
                                     (param_value_t *)&channel_page);
            }
            break;
    }
}

/**
 * \brief To set the Tx power of the Transceiver
 *
 * \param tx_power_format Format in which input is given whether in dBm or reg
 * \param power_value     Tx power value in the given format
 */
static void set_tx_power(trx_id_t trx, uint8_t tx_power_format, int8_t power_value)
{
    int8_t tx_pwr_dbm;
    uint8_t temp_var;

    switch (tx_power_format)
    {
            /* To handle TX_PWR reg value input */
        case TX_POWER_FORMAT_REG:/* Input is in the Register value */
            {

                uint8_t tx_pwr_reg = (uint8_t)power_value;

                if (tx_pwr_reg <= MAX_TX_PWR_REG_VAL)

                {
                    if (true == peer_found[trx])
                    {
                        /* send the tx power in Reg value to remote node */
                        send_parameters_changed(trx, TX_POWER_REG, (uint8_t)tx_pwr_reg);
                    }
                    else
                    {
                        /* set the Tx power on source node in case of no peer */
                        if (MAC_SUCCESS == tal_convert_reg_value_to_dBm(tx_pwr_reg, &tx_pwr_dbm))
                        {
                          //  temp_var = CONV_DBM_TO_phyTransmitPower(tx_pwr_dbm);sriram
                            tal_pib_set(trx,phyTransmitPower, (pib_value_t *)&tx_pwr_dbm);
                            /* To make sure that TX_PWR register is updated with the
                             * value whatever user povided.Otherwise lowest dBm power
                             * (highest reg value will be taken)
                             */
                        //    tal_set_tx_pwr(trx,REGISTER_VALUE,tx_pwr_reg);


                            /* update the data base with this value */
                            curr_trx_config_params[trx].tx_power_reg = tx_pwr_reg;

                            /*Tx power in dBm also need to be updated as it changes with reg value */
                            curr_trx_config_params[trx].tx_power_dbm = tx_pwr_dbm;

                            /* Send Set confirmation with status SUCCESS */
                            usr_perf_set_confirm(trx,MAC_SUCCESS,
                                                 PARAM_TX_POWER_REG,
                                                 (param_value_t *)&tx_pwr_reg);
                        }
                    }
                }
                else
                {
                    /* Send confirmation as VALUE_OUT_OF_RANGE */
                    usr_perf_set_confirm(trx,VALUE_OUT_OF_RANGE,
                                         PARAM_TX_POWER_REG,
                                         (param_value_t *)&tx_pwr_reg);
                }
            }
            break;
            /* To handle Absolute power(dBm) input */
        case TX_POWER_FORMAT_DBM: /* Input is in dBm values */
            {
                tx_pwr_dbm = power_value;


					int8_t min_dbm_val;
					int8_t max_dbm_val;
					/* Check for the valid range of tx power in dBm */
					tal_convert_reg_value_to_dBm(0X00, &min_dbm_val);

					/* get max tx power in dbM allowed */
					tal_convert_reg_value_to_dBm(MAX_TX_PWR_REG_VAL, &max_dbm_val);

					if ( (tx_pwr_dbm >= min_dbm_val) && (tx_pwr_dbm <= max_dbm_val) )

					{
						if (true == peer_found[trx])
						{
							/*send the tx power in dBm to remote node */
							send_parameters_changed(trx,TX_POWER_DBM, (uint8_t) tx_pwr_dbm);
						}
						else
						{
							/* set the Tx power on source node in case of no peer */
							//temp_var = CONV_DBM_TO_phyTransmitPower(tx_pwr_dbm);sriram
							tal_pib_set(trx,phyTransmitPower, (pib_value_t *)&tx_pwr_dbm);

							/* update the data base with this value */
							curr_trx_config_params[trx].tx_power_dbm = tx_pwr_dbm;

							/*Tx power in Reg also need to be updated as it changes with dBm value */
							tal_get_curr_trx_config(trx,TX_PWR,&(curr_trx_config_params[trx].tx_power_reg));

							/* Send Set confirmation with status SUCCESS */
							usr_perf_set_confirm(trx,MAC_SUCCESS,
												 PARAM_TX_POWER_DBM,
												 (param_value_t *)&tx_pwr_dbm);

						}
					}
					else
					{
						/* Send Set confirmation with status VALUE_OUT_OF_RANGE */
						usr_perf_set_confirm(trx,VALUE_OUT_OF_RANGE,
											 PARAM_TX_POWER_DBM,
											 (param_value_t *)&tx_pwr_dbm);
					}
	
            } /* case end */
            break;
        default:
            break;
    }
}

/**
 * \brief To start the ED scan
 *
 * \param ed_scan_duration  Scan duration parameter which is used to calculate
 *                          the scan time on each channel
 */
void start_ed_scan(trx_id_t trx,uint8_t ed_scan_duration, uint32_t channel_sel_mask)
{
	uint16_t first_channel;
	uint8_t ch_cnt;
	uint32_t supported_channels;
	float scan_time;
	/* Initialize the no. of channels to 0 */
	num_channels[trx] = 0;
    tal_pib_get(trx,phyChannelsSupported, (uint8_t *)&supported_channels);
	scan_duration[trx] = ed_scan_duration;
	scan_channel_mask[trx] = (channel_sel_mask & supported_channels);//TRX_SUPPORTED_CHANNELS_LEG(trx));

	/* Check the range for the scan duration */
	if (scan_duration[trx] > MAX_SCAN_DURATION) {
		/* Send confirmation with the error code - invalid parameter */
		usr_ed_scan_start_confirm(trx,VALUE_OUT_OF_RANGE, NUL_VAL, NUL_VAL);
		return;
	}
uint8_t min_ch,max_ch;
if(trx==0)
{
min_ch = 0;
max_ch =10;

}
else
{
	min_ch = 11;
	max_ch =26;

	}
	scanning[trx] = true;
	for (ch_cnt = min_ch; ch_cnt <= max_ch; ch_cnt++) {
		num_channels[trx]
			+= (scan_channel_mask[trx] &
				((uint32_t)1 << ch_cnt)) ? 1 : 0;
	}

	scan_time = (float)aBaseSuperframeDuration *
			((1 << scan_duration[trx]) + 1) *
			(TAL_CONVERT_SYMBOLS_TO_US(1)) * num_channels[trx] / (1e6);

	if (scan_time >= 60) {
		uint8_t scan_time_min = (uint8_t)(scan_time / 60);
		float scan_time_sec = (scan_time -  (scan_time_min * 60));

		/* Send confirm with the status as SUCCESS and scan time in
		 * minutes and secs */
		usr_ed_scan_start_confirm(trx,MAC_SUCCESS, scan_time_min, reverse_float(
				scan_time_sec));
	} else {
		/* Send conform with the status as SUCCESS and scan time in secs
		 *& minutes = 0 */
		usr_ed_scan_start_confirm(trx,MAC_SUCCESS, NUL_VAL,
				reverse_float(scan_time));
	}

	tal_pib_get(trx,phyCurrentChannel, &channel_before_scan[trx]);

	/* Identify first channel */
	for (ch_cnt = min_ch; ch_cnt <= max_ch; ch_cnt++) {
		if ((scan_channel_mask[trx] & ((uint32_t)1 << ch_cnt)) > 0) {
			first_channel = ch_cnt;
			scan_channel_mask[trx] &= ~((uint32_t)1 << ch_cnt);
			break;
		}
	}
	tal_pib_set(trx,phyCurrentChannel, (pib_value_t *)&first_channel);
	tal_ed_start(trx,scan_duration);
}

/**
 * \brief To get the Sensor data like Battery voltage
 */
void get_sensor_data(trx_id_t trx)
{
    uint16_t bat_mon;
    double bat_voltage;
    double temperature;

    bat_mon = tfa_get_batmon_voltage();
    bat_voltage = reverse_float((double)(bat_mon * MILLI_VOLT_MULTIPLIER));
    

#if (TAL_TYPE == ATMEGARFR2)
    temperature = tfa_get_temperature();
temperature = reverse_float(temperature);
#else
    temperature = reverse_float(0.0); 
    
#endif /* End of #if (TAL_TYPE == ATMEGARFR2) */
    /* Send the confirmation with status as SUCCESS */
    usr_sensor_data_get_confirm(trx, MAC_SUCCESS, bat_voltage, temperature);
}

/**
 * \brief To set the default values of  all configurable
 * parameters on source and peer node
 */
void set_default_configuration(trx_id_t trx)
{
    trx_config_params_t dummy_params = {0};
    /* Send set_default_config_req command if the peer device is connected */
    if (true == peer_found[trx])
    {
        /* CRC set request sent successfully */
        if (send_set_default_config_command(trx))
        {
            op_mode[trx] = SET_DEFAULT_CONFIG_PEER;
        }
        else
        {
            /* Send confirmation with TRANSMISSION_FAILURE */
            usr_set_default_config_confirm(trx,TRANSMISSION_FAILURE, &dummy_params);
            op_mode[trx] = TX_OP_MODE;
        }

    }
    else /* Single node Tests case,set default values only on source node */
    {
        /* Set the default values for all configurable parameters */
        config_per_test_parameters(trx);

        /* Send the confirmation with the status as SUCCESS */
        usr_set_default_config_confirm(trx,MAC_SUCCESS, &default_trx_config_params[trx]);
    }
}
/**
 * \brief To Get the updated/latest values of  all configurable
 * parameters on source and peer node
 */
void get_current_configuration(trx_id_t trx)
{

    uint8_t temp;
    /* Make sure the Register values are in sync with database values
     * as there are chances of the same because of the User register writes
     */
	uint16_t temp_channel = (uint16_t)curr_trx_config_params[trx].channel;
		/* If the transceiver currently not set in ism frequencies, set the IEEE channel */
		if (temp_channel != INVALID_VALUE)
		{
			/* Channel configuration */
			tal_pib_set(trx,phyCurrentChannel, (pib_value_t *)&temp_channel);

		}
	

		/* Channel page configuration */
		tal_pib_set(trx,phyCurrentPage, (pib_value_t *)&curr_trx_config_params[trx].channel_page);

		/* Tx_power configurations */
		//temp = CONV_DBM_TO_phyTransmitPower(curr_trx_config_params[trx].tx_power_dbm);
		tal_pib_set(trx,phyTransmitPower, (pib_value_t *)&curr_trx_config_params[trx].tx_power_dbm);
		//tal_set_tx_pwr(trx,REGISTER_VALUE,curr_trx_config_params[trx].tx_power_reg);

    /* trx state configuration */
    if (RX_ON == curr_trx_config_params[trx].trx_state)
    {

	        	/* Put the transceiver in TRX OFF state- default state for Single node tests */
	        	set_trx_state(trx,RF_RX);

    }
    else if (TRX_OFF == curr_trx_config_params[trx].trx_state)
    {

	       	/* Put the transceiver in TRX OFF state- default state for Single node tests */
	       	set_trx_state(trx,RF_TXPREP);

    }
    else if (PLL_ON == curr_trx_config_params[trx].trx_state)
    {

	        	/* Put the transceiver in TRX OFF state- default state for Single node tests */
	        	set_trx_state(trx,RF_TXPREP);

    }
#if(TAL_TYPE != AT86RF230B)
    /* Rx desensitization configuration */
    if (true == curr_trx_config_params[trx].rx_desensitize)
    {

      tal_set_rx_sensitivity_level(trx,RX_DESENSITIZE_LEVEL);
    }
    else
    {

      tal_set_rx_sensitivity_level(trx,NO_RX_DESENSITIZE_LEVEL);
    }
#endif
    /* Send the confirmation with the status as SUCCESS */
    usr_get_current_config_confirm(trx,MAC_SUCCESS, &curr_trx_config_params[trx]);
}

/**
 * \brief To set or reset ACK request value
 *
 * \param config_value value to be configured
 */
static void config_ack_request(trx_id_t trx, bool config_value)
{
    /* Set the ack request configuration as per the config value */
    if (true == config_value)
    {
        curr_trx_config_params[trx].ack_request = true;
    }
    else
    {
        curr_trx_config_params[trx].ack_request = false;
    }
    /* Send Set confirmation with status SUCCESS */
    usr_perf_set_confirm(trx, MAC_SUCCESS,
                         PARAM_ACK_REQUEST,
                         (param_value_t *)&curr_trx_config_params[trx].ack_request);
}

/**
 * \brief To configure CSMA Setting in the transceiver
 *
 * \param config_value value to be configured
 */
static void config_csma(trx_id_t trx, bool config_value)
{
    /* Set the csma configuration as per the config value */
    if (true == config_value)
    {
        curr_trx_config_params[trx].csma_enabled = true;
    }
    else
    {
        curr_trx_config_params[trx].csma_enabled = false;
    }

    /* Send Set confirmation with status SUCCESS */
    usr_perf_set_confirm(trx, MAC_SUCCESS,
                         PARAM_CSMA,
                         (param_value_t *)&curr_trx_config_params[trx].csma_enabled);
}

/**
 * \brief To configure frame retry Setting
 *
 * \param config_value value to be configured
 */
static void config_frame_retry(trx_id_t trx, bool config_value)
{
    /* Set the auto transmission configuration as per the config value */
    if (true == config_value)
    {
        curr_trx_config_params[trx].retry_enabled = true;
    }
    else
    {
        curr_trx_config_params[trx].retry_enabled = false;
    }

    /* Send Set confirmation with status SUCCESS */
    usr_perf_set_confirm(trx, MAC_SUCCESS,
                         PARAM_FRAME_RETRY,
                         (param_value_t *)&curr_trx_config_params[trx].retry_enabled);
}

#if (ANTENNA_DIVERSITY == 1)
/**
 * \brief To configure antenna diversity setting
 * \param config_value Value to be configured for Antenna diversity
 */
static void set_antenna_diversity_settings(trx_id_t trx, uint8_t config_value)
{
    uint8_t curr_state = INVALID_VALUE;

    if (ENABLE_ANT_DIVERSITY == config_value) /* Enable antenna diversity request */
    {
        /* Update the data base with these values*/
        curr_trx_config_params[trx].antenna_diversity = true;
        curr_trx_config_params[trx].antenna_selected = ANT_CTRL_0;

        tal_ant_div_config(trx,ANT_DIVERSITY_ENABLE,ANTENNA_DEFAULT);

    }
    else /* Disable antenna diversity request */
    {
        /* To avoid the transition time from antenna diversity enabled to disabled
         * during receiver on state,switch off the transceiver and restore the
         * state after the antenna diversity settings are done */
        curr_state = tal_get_trx_status(trx) ; 

        if ( (RX_ON == curr_state) || (RX_AACK_ON == curr_state) )
        {

	            	/* Put the transceiver in TRX OFF state- default state for Single node tests */
	            	set_trx_state(CMD_TRX_OFF);

        }

        switch (config_value)
        {

            case ENABLE_ANTENNA_1: /* Disable antenna Diversity request & Enable ANT1 */
                {

                    /* Update the data base with these values*/
                    curr_trx_config_params[trx].antenna_diversity = false;
                    curr_trx_config_params[trx].antenna_selected = ANT_CTRL_1;

                }
                break;

            case ENABLE_ANTENNA_2: /* Disable antenna Diversity request & Enable ANT2 */
                {

                    /* Update the data base with these values*/
                    curr_trx_config_params[trx].antenna_diversity = false;
                    curr_trx_config_params[trx].antenna_selected = ANT_CTRL_2;

                }
                break;
            default:
                {
                    /* Send Set confirmation with status VALUE_OUT_OF_RANGE */
                    usr_perf_set_confirm(trx, VALUE_OUT_OF_RANGE,
                                         PARAM_ANT_DIVERSITY,
                                         (param_value_t *)&config_value);
                    return;

                }
        }
        tal_ant_div_config(trx,ANT_DIVERSITY_DISABLE,config_value); 


        /* Restore the current state back again */
        if (RX_ON == curr_state)
        {

	            	/* Put the transceiver in TRX OFF state- default state for Single node tests */
	            	set_trx_state(CMD_RX_ON);

        }
        else if (RX_AACK_ON == curr_state)
        {

	                        	/* Put the transceiver in TRX OFF state- default state for Single node tests */
	                        	set_trx_state(CMD_RX_AACK_ON);

        }
        else
        {
            /* No need to to anything */
        }
    }


    /* Send Set confirmation with status SUCCESS */
    usr_perf_set_confirm(trx, MAC_SUCCESS,
                         PARAM_ANT_DIVERSITY,
                         (param_value_t *)&config_value);

}

/**
 * \brief To configure antenna diversity on peer node
 * \param config_value  Value used to configure Antenna diversity
 */
static void config_antenna_diversity_peer_node(trx_id_t trx, uint8_t config_value)
{
    /* This cmd is valid only if the peer device is present */
    if (true == peer_found[trx])
    {
        div_set_req_t div_msg;
        switch (config_value)
        {
            case ENABLE_ANT_DIVERSITY: /* Enable Antenna diversity on peer node request */
                {
                    div_msg.status = ANT_DIV_ENABLE;
                    div_msg.ant_sel = ANT_CTRL_0; /* No antenna selected */
                }
                break;
            case ENABLE_ANTENNA_1: /* Disable Antenna diversity on peer node request & select ANT1*/
                {
                    div_msg.status = ANT_DIV_DISABLE;
                    div_msg.ant_sel = ANT_CTRL_1;
                }
                break;
            case ENABLE_ANTENNA_2:/* Disable Antenna diversity on peer node request & select ANT2*/
                {
                    div_msg.status = ANT_DIV_DISABLE;
                    div_msg.ant_sel = ANT_CTRL_2;
                }
                break;
            default:
                {
                    op_mode[trx] = TX_OP_MODE;
                    /* Send Confirmation with VALUE_OUT_OF_RANGE */
                    usr_perf_set_confirm(trx,VALUE_OUT_OF_RANGE,
                                         PARAM_ANT_DIVERSITY_ON_PEER,
                                         (param_value_t *)&config_value);
                    return;
                }
        }
        /* Diversity set request sent successfully  */
        if (send_diversity_set_req(trx, div_msg))
        {
            op_mode[trx] = DIVERSITY_SET_REQ;
        }
        else
        {
            /* Send confirmation with TRANSMISSION_FAILURE */
            usr_perf_set_confirm(trx, TRANSMISSION_FAILURE,
                                 PARAM_ANT_DIVERSITY_ON_PEER,
                                 (param_value_t *)&config_value);
            op_mode[trx] = TX_OP_MODE;
        }
    }
    else /* The node is operating in the SINGLE NODE TESTS mode*/
    {
        usr_perf_set_confirm(trx, INVALID_CMD,
                             PARAM_ANT_DIVERSITY_ON_PEER,
                             (param_value_t *)&config_value);
    }
}
#endif /* End of #if (ANTENNA_DIVERSITY == 1) */

#ifdef CRC_SETTING_ON_REMOTE_NODE
/**
 * \brief To configure CRC settings on peer node
 * \param config_value Value used to configure CRC on peer
 */
static void config_crc_peer_node(trx_id_t trx, bool config_value)
{
    /* This cmd is valid only if the peer device is present */
    if (true == peer_found[trx])
    {
        crc_set_req_t crc_msg;

        crc_msg.status = config_value;

        /* CRC set request sent successfully */
        if (send_crc_set_req(trx, crc_msg))
        {
            op_mode[trx] = CRC_SET_REQ_WAIT;
        }
        else
        {
            /* Send confirmation with TRANSMISSION_FAILURE */
            usr_perf_set_confirm(trx,TRANSMISSION_FAILURE,
                                 PARAM_CRC_ON_PEER,
                                 (param_value_t *)&config_value);
            op_mode[trx] = TX_OP_MODE;
        }
    }
    else /* The node is operating in the SINGLE NODE TESTS mode*/
    {
        /* Send the confirmation with status INVALID_CMD */
        usr_perf_set_confirm(trx, INVALID_CMD,
                             PARAM_CRC_ON_PEER,
                             (param_value_t *)&config_value);
    }
}
#endif

#if (TAL_TYPE != AT86RF230B)
/**
 * \brief To configure rx desensitization level in the transceiver.
 * \param config_value Value used to configure Rx desensitization
 */
static void config_rx_desensitization(trx_id_t trx, bool config_value)
{
    if (false == config_value)
    {
        curr_trx_config_params[trx].rx_desensitize = false;
        tal_set_rx_sensitivity_level(trx,NO_RX_DESENSITIZE_LEVEL);
       
    }
    /* Receiver desensitization is disabled, enable it */
    else
    {
        curr_trx_config_params[trx].rx_desensitize = true;
        tal_set_rx_sensitivity_level(trx,RX_DESENSITIZE_LEVEL);
   
    }
    /* Send confirmation with status MAC_SUCCESS */
    usr_perf_set_confirm(trx, MAC_SUCCESS,
                         PARAM_DESENSITIZATION,
                         (param_value_t *)&curr_trx_config_params[trx].rx_desensitize);

}
#endif //(TAL_TYPE != AT86RF230B)
/*
 * \brief Function to send Identify command to peer node
 */
void identify_peer_node(trx_id_t trx)
{
    /* Send the Identify command to the peer node */
    if (send_identify_command(trx))
    {
        op_mode[trx] = IDENTIFY_PEER;
    }
    else
    {
        op_mode[trx] = TX_OP_MODE;

        /* Send confirmation with status as TRANSMISSION_FAILURE */
        usr_identify_peer_node_confirm(trx, TRANSMISSION_FAILURE);
    }

}
/*
 * \brief Function to request the peer to get disconnected from source node
 */
void disconnect_peer_node(trx_id_t trx)
{
    /* check whether the disconnection requested when peer is present */
    if (true == node_info[trx].peer_found)
    {
        /* Send Disconnect command to the peer node */
        if (send_disconnect_command(trx))
        {
            op_mode[trx] = DISCONNECT_PEER;
        }
        else
        {
            op_mode[trx] = TX_OP_MODE;

            /* Send confirmation with status as TRANSMISSION_FAILURE */
            usr_peer_disconnect_confirm(trx, TRANSMISSION_FAILURE);
        }
    }
    else /* non PER mode case */
    {
        usr_peer_disconnect_confirm(trx, MAC_SUCCESS);

        /* app reset on source node in single node tests mode */
        /* This is to make the node to restart as boot up and
         * open for fresh peer search
         */
        app_reset(trx);
    }

}
/*
 * \brief To Initiate the PER test
 */
void initiate_per_test(trx_id_t trx)
{
    if (TX_OP_MODE == op_mode[trx])
    {
        /* Initiate a packet to tell the receptor that a new PER test is going to be started */
        if (send_per_test_start_cmd(trx))
        {
            op_mode[trx] = PER_TEST_START;
        }
    }
}

/*
 * \brief To start the PER test
 */
static void start_test(trx_id_t trx)
{
    /* Check for the current operating mode */
    if (TX_OP_MODE == op_mode[trx])
    {
        frames_to_transmit[trx] = curr_trx_config_params[trx].number_test_frames;
        if (true == curr_trx_config_params[trx].ack_request)
        {
            frame_no_ack[trx] = 0;
        }
        else
        {
            frame_no_ack[trx] = CCPU_ENDIAN_TO_LE32((uint32_t)(-1));
        }
 //CSMA/CA is enabled by default in 230B       
        
#if (TAL_TYPE != AT86RF230B)
        if (true == curr_trx_config_params[trx].csma_enabled ) 
        {
            frame_access_failure[trx] = 0;
        }
        else
        {
            frame_access_failure[trx] = CCPU_ENDIAN_TO_LE32((uint32_t)(-1));
        }
#else
        frame_access_failure[trx] = 0;
#endif
        frame_failure[trx] = 0;
        configure_frame_sending(trx);
         start_time[trx] = sw_timer_get_time();
        restart_time[trx] = (uint32_t)(start_time[trx] * MICRO_SEC_MULTIPLIER);
        no_of_roll_overs[trx] = 0;

        /* Send the confirmation with the status as SUCCESS */
        usr_per_test_start_confirm(trx,MAC_SUCCESS);
    }
    else
    {
        /* Send the confirmation with the status as INVALID_CMD
         * as the state is not correct
         */
        usr_per_test_start_confirm(trx,INVALID_CMD);
    }

}

/**
 * \brief To Configure the frame sending
 */
static void configure_frame_sending(trx_id_t trx)
{
    uint8_t index;
    uint16_t app_frame_length;
    uint8_t *frame_ptr;
    uint8_t *temp_frame_ptr;
    uint16_t fcf = 0;
    uint16_t temp_value;
    app_payload_t *tmp;

    /*
     * Fill in PHY frame.
     */

    /* Get length of current frame. */
    app_frame_length = (curr_trx_config_params[trx].phy_frame_length  - FRAME_OVERHEAD - tal_pib[trx].FCSLen);

    /* Set payload pointer. */
    frame_ptr = temp_frame_ptr =
                    (uint8_t *)node_info[trx].tx_frame_info +
						LARGE_BUFFER_SIZE -
                    app_frame_length - tal_pib[trx].FCSLen; /* Add 2 octets for FCS. */

    tmp = (app_payload_t *) temp_frame_ptr;

    (tmp->cmd_id) = PER_TEST_PKT;

    temp_frame_ptr++;

    /*
     * Assign dummy payload values.
     * Payload is stored to the end of the buffer avoiding payload copying by TAL.
     */
    for (uint16_t index_t = 0; index_t < (app_frame_length - 1); index_t++) /* 1=> cmd ID */
    {
        *temp_frame_ptr++ = index_t; /* dummy values */
    }

    /* Source Address */

		temp_value =  tal_pib[trx].ShortAddress;

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
    temp_value = node_info[trx].peer_short_addr;
    frame_ptr -= SHORT_ADDR_LEN;
    convert_16_bit_to_byte_array(temp_value, frame_ptr);

    /* Destination PAN-Id */
    temp_value = CCPU_ENDIAN_TO_LE16(DST_PAN_ID);
    frame_ptr -= PAN_ID_LEN;
    convert_16_bit_to_byte_array(temp_value, frame_ptr);

    /* Set DSN. */
    frame_ptr--;
    *frame_ptr = (uint8_t)rand();

    /* Set the FCF. */
    fcf |= FCF_FRAMETYPE_DATA | FCF_SET_SOURCE_ADDR_MODE(FCF_SHORT_ADDR) |
           FCF_SET_DEST_ADDR_MODE(FCF_SHORT_ADDR);
    if (curr_trx_config_params[trx].ack_request)
    {
        fcf |= FCF_ACK_REQUEST;
    }
    frame_ptr -= FCF_LEN;
    convert_16_bit_to_byte_array(CCPU_ENDIAN_TO_LE16(fcf), frame_ptr);

    /* First element shall be length of PHY frame. */ //sriram
    //frame_ptr--;
    //*frame_ptr = curr_trx_config_params[trx].phy_frame_length;
	node_info[trx].tx_frame_info->length = (curr_trx_config_params[trx].phy_frame_length )-tal_pib[trx].FCSLen ;//+ 1500; //sriram->fcs length added at the end in tal
    /* Finished building of frame. */
    node_info[trx].tx_frame_info->mpdu = frame_ptr;
}


/**
 * \brief Function to send the parameters which has been updated to
 * the receptor node.
 * \param param  Parameters being modified i.e. Channel,channel page,etc
 * \param val    Value of the parameter being modified
 *
 */
static void send_parameters_changed(trx_id_t trx, uint8_t param, uint8_t val)
{
    uint16_t payload_length;
    app_payload_t msg;
    set_parm_req_t *data;

    /* maintain a copy of the parameter to be used to set on this node */
    set_param_cb[trx].param_type = param;
    set_param_cb[trx].param_value = val;

    /* Create the payload */
    msg.cmd_id = SET_PARAM;
    seq_num_initiator[trx]++;
    msg.seq_num = seq_num_initiator[trx];
    data = (set_parm_req_t *)&msg.payload;
    data->param_type = param;
    data->param_value = val;

    payload_length = ((sizeof(app_payload_t) -
                       sizeof(general_pkt_t)) +
                      sizeof(set_parm_req_t));

    /* Send the frame to Peer node */
    if (MAC_SUCCESS == transmit_frame(trx, FCF_SHORT_ADDR,
                                      (uint8_t *) & (node_info[trx].peer_short_addr),
                                      FCF_SHORT_ADDR,
                                      seq_num_initiator[trx],
                                      (uint8_t *) &msg,
                                      payload_length,
                                      true)
       )
    {
        op_mode[trx] = SET_PARAMETER;
    }
}

static bool send_per_test_start_cmd(trx_id_t trx)
{
    uint16_t payload_length;
    app_payload_t msg;
    result_req_t *data;

    /* Create the payload */
    msg.cmd_id = PER_TEST_START_PKT;
    seq_num_initiator[trx]++;
    msg.seq_num = seq_num_initiator[trx];
    data = (result_req_t *)&msg.payload;
    /* Just a dummy value */
    data->cmd = DUMMY_PAYLOAD;

    payload_length = ((sizeof(app_payload_t) -
                       sizeof(general_pkt_t)) +
                      sizeof(result_req_t));

    /* Send the frame to Peer node */
    if (MAC_SUCCESS == transmit_frame(trx, FCF_SHORT_ADDR,
                                      (uint8_t *) & (node_info[trx].peer_short_addr),
                                      FCF_SHORT_ADDR,
                                      seq_num_initiator[trx],
                                      (uint8_t *) &msg,
                                      payload_length,
                                      true)
       )
    {
        return(true);
    }
    return(false);

}
/**
 * \brief Function used to request PER test result.
 * \return true if request was sent successfully, false if not.
 */
static bool send_result_req(trx_id_t trx)
{
    uint16_t payload_length;
    app_payload_t msg;
    result_req_t *data;

    /* Create the payload */
    msg.cmd_id = RESULT_REQ;
    seq_num_initiator[trx]++;
    msg.seq_num = seq_num_initiator[trx];
    data = (result_req_t *)&msg.payload;
    /* Just a dummy value */
    data->cmd = DUMMY_PAYLOAD;

    payload_length = ((sizeof(app_payload_t) -
                       sizeof(general_pkt_t)) +
                      sizeof(result_req_t));

    /* Send the frame to Peer node */
    if (MAC_SUCCESS == transmit_frame(trx, FCF_SHORT_ADDR,
                                      (uint8_t *) & (node_info[trx].peer_short_addr),
                                      FCF_SHORT_ADDR,
                                      seq_num_initiator[trx],
                                      (uint8_t *) &msg,
                                      payload_length,
                                      true)
       )
    {
        return(true);
    }
    return(false);

}

#if (ANTENNA_DIVERSITY == 1)
/**
 * \brief Function used to query diversity settings in remote node
 */
static void get_diversity_settings_peer_node(trx_id_t trx)
{

    /* Send Diversity settings query request to remote node*/
    if (send_diversity_status_req(trx))
    {
        /* start the timer to wait for the reply*/
        sw_timer_start(APP_TIMER_TO_TX,
                        TIMEOUT_FOR_RESPONSE_IN_MICRO_SEC,
                        SW_TIMEOUT_RELATIVE,
                        (FUNC_PTR)wait_for_reply_timer_handler_cb,
                        (void*) trx);
        op_mode[trx] = DIVERSITY_STATUS_REQ;
    }
    else
    {
        op_mode[trx] = TX_OP_MODE;
    }
}


/**
 * \brief Function used to send the diversity status request to get the
 * diversity settings on peer node
 *
 *  \return    Trasmission status - success/failure
 */
static bool send_diversity_status_req(trx_id_t trx)
{

    uint16_t payload_length;
    app_payload_t msg;
    div_stat_req_t *data;

    /* Create the payload */
    msg.cmd_id = DIV_STAT_REQ;
    seq_num_initiator[trx]++;
    msg.seq_num = seq_num_initiator[trx];
    data = (div_stat_req_t *)&msg.payload;
    /* Dummy value */
    data->status = DUMMY_PAYLOAD;

    payload_length = ((sizeof(app_payload_t) -
                       sizeof(general_pkt_t)) +
                      sizeof(div_stat_req_t));

    /* Send the frame to Peer node */
    if (MAC_SUCCESS == transmit_frame(trx, FCF_SHORT_ADDR,
                                      (uint8_t *) & (node_info[trx].peer_short_addr),
                                      FCF_SHORT_ADDR,
                                      seq_num_initiator[trx],
                                      (uint8_t *) &msg,
                                      payload_length,
                                      true)
       )
    {
        return(true);
    }
    return (false);
}


/**
 * \brief Function used to change diversity settings on remote node
 *
 * \param div_msg   Diversity settings for the remote node
 *
 * \return          Trasmission status - success/failure
 */
static bool send_diversity_set_req(trx_id_t trx, div_set_req_t div_msg)
{

    uint16_t payload_length;
    app_payload_t msg;

    /* Create the payload */
    msg.cmd_id = DIV_SET_REQ;
    seq_num_initiator[trx]++;
    msg.seq_num = seq_num_initiator[trx];
    msg.payload.div_set_req_data = div_msg;

    payload_length = ((sizeof(app_payload_t) -
                       sizeof(general_pkt_t)) +
                      sizeof(div_set_req_t));

    /* Send the frame to Peer node */
    if (MAC_SUCCESS == transmit_frame(trx, FCF_SHORT_ADDR,
                                      (uint8_t *) & (node_info[trx].peer_short_addr),
                                      FCF_SHORT_ADDR,
                                      seq_num_initiator[trx],
                                      (uint8_t *) &msg,
                                      payload_length,
                                      true)
       )
    {
        return(true);
    }
    return(false);
}
#endif /* #if (ANTENNA_DIVERSITY == 1) */


#ifdef CRC_SETTING_ON_REMOTE_NODE
/**
 * \brief Function used to query CRC settings in remote node
 */
static void get_crc_settings_peer_node(trx_id_t trx)
{
    if (send_crc_status_req(trx))
    {
        sw_timer_start(APP_TIMER_TO_TX,
                        TIMEOUT_FOR_RESPONSE_IN_MICRO_SEC,
                        SW_TIMEOUT_RELATIVE,
                        (FUNC_PTR)wait_for_reply_timer_handler_cb,
                        (void*) trx);
        op_mode[trx] = CRC_STATUS_REQ_WAIT;
    }
    else
    {
        op_mode[trx] = TX_OP_MODE;
    }
}


/**
 * \brief Function used to send crc status request to get
 * CRC settings on the peer node
 * \return    Trasmission status - success/failure
 */
static bool send_crc_status_req(trx_id_t trx)
{
    uint16_t payload_length;
    app_payload_t msg;
    crc_stat_req_t *data;

    /* Create the payload */
    msg.cmd_id = CRC_STAT_REQ;
    seq_num_initiator[trx]++;
    msg.seq_num = seq_num_initiator[trx];
    data = (crc_stat_req_t *)&msg.payload;

    /* Dummy value */
    data->status = DUMMY_PAYLOAD;

    payload_length = ((sizeof(app_payload_t) -
                       sizeof(general_pkt_t)) +
                      sizeof(div_stat_req_t));

    /* Send the frame to Peer node */
    if (MAC_SUCCESS == transmit_frame(trx, FCF_SHORT_ADDR,
                                      (uint8_t *) & (node_info[trx].peer_short_addr),
                                      FCF_SHORT_ADDR,
                                      seq_num_initiator[trx],
                                      (uint8_t *) &msg,
                                      payload_length,
                                      true)
       )
    {
        return(true);
    }
    return(false);
}


/**
 * \brief Function used to change CRC settings on peer node
 *
 * \param crc_msg   CRC settings for peer node
 *
 * \return    Trasmission status - success/failure
 */
static bool send_crc_set_req(trx_id_t trx, crc_set_req_t crc_msg)
{

    uint16_t payload_length;
    app_payload_t msg;

    /* Create the payload */
    msg.cmd_id = CRC_SET_REQ;
    seq_num_initiator[trx]++;
    msg.seq_num = seq_num_initiator[trx];
    msg.payload.crc_set_req_data = crc_msg;

    payload_length = ((sizeof(app_payload_t) -
                       sizeof(general_pkt_t)) +
                      sizeof(crc_set_req_t));

    /* Send the frame to Peer node */
    if (MAC_SUCCESS == transmit_frame(trx, FCF_SHORT_ADDR,
                                      (uint8_t *) & (node_info[trx].peer_short_addr),
                                      FCF_SHORT_ADDR,
                                      seq_num_initiator[trx],
                                      (uint8_t *) &msg,
                                      payload_length,
                                      true)
       )
    {
        return(true);
    }
    return(false);
}
#endif /* #ifdef CRC_SETTING_ON_REMOTE_NODE */


/**
 * \brief Function used to send identify peer node command
 *
 * \return    Trasmission status - success/failure
 */
static bool send_identify_command(trx_id_t trx)
{
    uint16_t payload_length;
    app_payload_t msg;

    /* Create the payload */
    msg.cmd_id = IDENTIFY_NODE;
    seq_num_initiator[trx]++;
    msg.seq_num = seq_num_initiator[trx];

    payload_length = ((sizeof(app_payload_t) -
                       sizeof(general_pkt_t)));

    /* Send the frame to Peer node */
    if (MAC_SUCCESS == transmit_frame(trx, FCF_SHORT_ADDR,
                                      (uint8_t *) & (node_info[trx].peer_short_addr),
                                      FCF_SHORT_ADDR,
                                      seq_num_initiator[trx],
                                      (uint8_t *) &msg,
                                      payload_length,
                                      true)
       )
    {
        return(true);
    }
    return(false);
}

/**
 * \brief Function used to send disconnect peer node command
 *
 * \return    Trasmission status - success/failure
 */
static bool send_disconnect_command(trx_id_t trx)
{
    uint16_t payload_length;
    app_payload_t msg;

    /* Create the payload */
    msg.cmd_id = DISCONNECT_NODE;
    seq_num_initiator[trx]++;
    msg.seq_num = seq_num_initiator[trx];

    payload_length = ((sizeof(app_payload_t) -
                       sizeof(general_pkt_t)));

    /* Send the frame to Peer node */
    if (MAC_SUCCESS == transmit_frame(trx, FCF_SHORT_ADDR,
                                      (uint8_t *) & (node_info[trx].peer_short_addr),
                                      FCF_SHORT_ADDR,
                                      seq_num_initiator[trx],
                                      (uint8_t *) &msg,
                                      payload_length,
                                      true)
       )
    {
        return(true);
    }
    return(false);
}
/**
 * \brief Function to configure and send the peer node info request.
 *
 * \return    Trasmission status - success/failure
 */
static bool send_peer_info_req(trx_id_t trx)
{
    uint16_t payload_length;
    app_payload_t msg;

    /* Create the payload */
    msg.cmd_id = PEER_INFO_REQ;
    seq_num_initiator[trx]++;
    msg.seq_num = seq_num_initiator[trx];

    payload_length = ((sizeof(app_payload_t) -
                       sizeof(general_pkt_t)));

    /* Send the frame to Peer node */
    if (MAC_SUCCESS == transmit_frame(trx, FCF_SHORT_ADDR,
                                      (uint8_t *) & (node_info[trx].peer_short_addr),
                                      FCF_SHORT_ADDR,
                                      seq_num_initiator[trx],
                                      (uint8_t *) &msg,
                                      payload_length,
                                      true)
       )
    {
        return(true);
    }

    return(false);
}

/**
 * \brief Function used to send set default config peer command
 *
 * \return    Trasmission status - success/failure
 */
static bool send_set_default_config_command(trx_id_t trx)
{
    uint16_t payload_length;
    app_payload_t msg;

    /* Create the payload */
    msg.cmd_id = SET_DEFAULT_REQ;
    seq_num_initiator[trx]++;
    msg.seq_num = seq_num_initiator[trx];

    payload_length = ((sizeof(app_payload_t) -
                       sizeof(general_pkt_t)));

    /* Send the frame to Peer node */
    if (MAC_SUCCESS == transmit_frame(trx, FCF_SHORT_ADDR,
                                      (uint8_t *) & (node_info[trx].peer_short_addr),
                                      FCF_SHORT_ADDR,
                                      seq_num_initiator[trx],
                                      (uint8_t *) &msg,
                                      payload_length,
                                      true)
       )
    {
        return(true);
    }
    return(false);
}

/**
 * \brief Function to Calculate the time taken for
 * PER Test
 *
 * \return Time duration as float value
 */
static float calculate_time_duration(trx_id_t trx)
{

    uint32_t duration;
    float duration_s;

    if (0 == no_of_roll_overs[trx])
    {
      duration = (SUB_TIME(end_time[trx], start_time[trx]));
        duration_s  = (float)duration * MICRO_SEC_MULTIPLIER;
    }

    else
    {
        if (end_time[trx] >= start_time[trx])
        {
            uint64_t total_duration = ((( no_of_roll_overs[trx] ) * ((1LL) << 32)) +
                                       ((end_time[trx]) - (start_time[trx])) +
                                       (no_of_roll_overs[trx] * (1000000)));

            duration_s  = (float)total_duration * MICRO_SEC_MULTIPLIER;

        }
        else
        {
            uint64_t total_duration = ((( no_of_roll_overs[trx] ) * ((1LL) << 32))  +
                                       ((((1LL) <<  32) - start_time[trx]) + (end_time[trx]))
                                       + (no_of_roll_overs[trx] * (1000000)));

            duration_s  = (float)total_duration * MICRO_SEC_MULTIPLIER;
        }
    }
    return(duration_s);
}

/**
 * \brief Function to calculate net data rate for the current PER test.
 *
 * \param per_test_duration_sec   Time duration for PER test
 * \return net data rate as float value
 */
static float calculate_net_data_rate(trx_id_t trx,float per_test_duration_sec)
{
    float data_volume;
    float data_rate;

    /* Data volume i.e total no.of bits transmitted */
    data_volume = curr_trx_config_params[trx].phy_frame_length  * curr_trx_config_params[trx].number_test_frames * 8; //1500 sriram
    /* Net data rate in Kbps*/
    data_rate = (data_volume / per_test_duration_sec) / 1000;

    return data_rate;
}

/**
 * \brief Function  to check the error conditions before
 *
 * processing the received command
 *
 * \return the error code based on the currently ongoing operation,if any
 */
uint8_t check_error_conditions(trx_id_t trx)
{
    uint8_t error_code;

    /* Check whether transceiver is in sleep */
    if (true == trx_sleep_status[trx])
    {
        error_code = TRANSCEIVER_IN_SLEEP;
    }
    else if (true == scanning[trx]) /* Check whether ED scan is in progress */
    {
        error_code = ED_SCAN_UNDER_PROGRESS;
    }
    else if (CONTINUOUS_TX_MODE == op_mode[trx] ) /* Check CW transmission is going on */
    {
        error_code = CW_TRANSMISSION_UNDER_PROGRESS;
    }
    else if (frames_to_transmit[trx] > 0) /* Check currently Transmission is initiated */
    {
        error_code = TRANSMISSION_UNDER_PROGRESS;
    }
	else if (true == range_test_in_progress[trx]) 
	{ 
		                                  /* Check if Range Mode is  initiated */
		error_code = RANGE_TEST_IN_PROGRESS;
	}
    else
    {
        error_code = MAC_SUCCESS;
    }
    return error_code;

}



/*
 * \brief get the parameter length based on the parameter type
 *
 * \param param_type    parameter type
 */
uint8_t get_param_length(uint8_t param_type)
{
    return (uint8_t)PGM_READ_BYTE(&perf_config_param_size[param_type]);
}

/**
 * \brief validating the tx power input based on the current
 * channel and page configuration
 *
 * \param dbm_value   Tx power in dBm as input to be validated
 */
static bool validate_tx_power(trx_id_t trx, int8_t dbm_value)
{

     uint8_t ch_page ;
	 tal_pib_t tal_pib_trx;

		tal_pib_trx = tal_pib[trx];
		tal_pib_get(trx,phyCurrentPage, &ch_page);	 
	 
    /* Check for MIN Tx power for any case */
    if (dbm_value < -25)
    {
        return(false);
    }
    else
    {
        switch (ch_page)
        {

            case 0: /* BPSK */
                {
                    if (0 == tal_pib_trx.CurrentChannel)
                    {
                        if (dbm_value > 6) /*MAX_TX_PWR_BPSK_20*/
                        {
                            return (false);
                        }
                    }
                    else    /* channels 1-10 */
                    {
                        if (dbm_value > 11 )  /* MAX_TX_PWR */
                        {
                            return (false);
                        }
                    }
                }
                break;
            case 2: /* O-QPSK */
                {
                    if (0 == tal_pib_trx.CurrentChannel)
                    {
                        if (dbm_value > 3 ) /* MAX_TX_PWR_OQPSK_100 */
                        {
                            return (false);
                        }
                    }
                    else    /* channels 1-10 */
                    {
                        if (dbm_value > 11 ) /* MAX_TX_PWR */
                        {
                            return (false);
                        }
                    }
                }
                break;
            case 5:/* China, O-QPSK: only channels 0 to 3 allowed */
                {
                    if (dbm_value > 10)
                    {
                        return(false);
                    }
                }
                break;
#ifdef HIGH_DATA_RATE_SUPPORT
            case 18: /* Chinese O-QPSK 500 */
                {
                    if (dbm_value > 10)
                    {
                        return(false);
                    }
                }
                break;
            case 19: /* Chinese O-QPSK 1000 */
                {
                    if (dbm_value > 10 ) /* MAX_TX_PWR_CHINA */
                    {
                        return (false);
                    }
                }
                break;
            case 16: /* O-QPSK 200, 500 */
                {
                    if (0 == tal_pib_trx.CurrentChannel)
                    {
                        if (dbm_value >  3 ) /* MAX_TX_PWR_OQPSK_200 */
                        {
                            return (false);
                        }
                    }
                    else    /* channels 1-10 */
                    {
                        if (dbm_value > 11 ) /* MAX_TX_PWR_OQPSK_500 */
                        {
                            return (false);
                        }
                    }
                }
                break;
            case 17: /* O-QPSK 400, 1000 */
                {
                    if (0 == tal_pib_trx.CurrentChannel)
                    {
                        if (dbm_value >  3) /* MAX_TX_PWR_OQPSK_400 */
                        {
                            return (false);
                        }
                    }
                    else    /* channels 1-10 */
                    {
                        if (dbm_value > 11 ) /* MAX_TX_PWR_OQPSK_1000 */
                        {
                            return (false);
                        }
                    }
                }
                break;
            default: /* Do nothing */
                break;
#endif  /* #ifdef HIGH_DATA_RATE_SUPPORT */
        }
    }
    return true;

}


/*
 * \brief To Initiate the Range  test
 */
void initiate_range_test(trx_id_t trx)
{
	if (TX_OP_MODE == op_mode[trx]) {
		/* Initiate a packet to tell the receptor that a new Range test
		 * is going to be started */
		if (send_range_test_start_cmd(trx)) {
			op_mode[trx] = RANGE_TEST_START;
		}
	} else {
		/* Send the confirmation with the status as INVALID_CMD
		 * as the state is not correct
		 */
		usr_range_test_start_confirm(trx,INVALID_CMD);
	}
}


/*
 * \brief Function to Start the Range test
 */
static void start_range_test(trx_id_t trx)
{
	/* set the range_test_in_progress flag to true to indicate range test is
	 * in progress */
	range_test_in_progress[trx] = true;

	/* Send the confirmation with the status as SUCCESS */
	usr_range_test_start_confirm(trx,MAC_SUCCESS);

	/* Change the OPMODE to Range Test TX */
	op_mode[trx] = RANGE_TEST_TX;

if(RF24 == trx)
{
	/* Start a Range test timer  to start the  transmission of range test
	 * packets periodically*/
	sw_timer_start(T_APP_TIMER_RANGE_RF24,
			RANGE_TX_BEACON_START_INTERVAL,
			SW_TIMEOUT_RELATIVE,
			(FUNC_PTR)range_test_timer_handler_rf24_cb,
			NULL);
}
else
{
	/* Start a Range test timer  to start the  transmission of range test
	 * packets periodically*/
	sw_timer_start(T_APP_TIMER_RANGE_RF09,
			RANGE_TX_BEACON_START_INTERVAL,
			SW_TIMEOUT_RELATIVE,
			(FUNC_PTR)range_test_timer_handler_rf09_cb,
			NULL);	
}
}

/*
 * \brief To stop the Range test
 */
void stop_range_test(trx_id_t trx)
{
	/* Check for the current operating mode and send stop range test command
	 * to the receptor*/
	if ((RANGE_TEST_TX == op_mode[trx]) && (true == range_test_in_progress[trx]) &&
			send_range_test_stop_cmd(trx)) {
		/* Change the opmode to RANGE_TEST_STOP so that once the cmd is
		 * sent succesfully
		 * the mode can be changed back to TX_OP_MODE and flags can be
		 * cleared and send confirmation to Host*/
		op_mode[trx] = RANGE_TEST_STOP;
	} else {
		/* Send the confirmation with the status as INVALID_CMD
		 * as the state is not correct
		 */
		usr_range_test_start_confirm(trx,INVALID_CMD);
	}
}



/**
 * \brief To Configure the frame sending
 */
static void configure_range_test_frame_sending(trx_id_t trx)
{
	uint8_t index;
	uint8_t app_frame_length;
	uint8_t *frame_ptr;
	uint8_t *temp_frame_ptr;
	uint16_t fcf = 0;
	uint16_t temp_value;
	app_payload_t *tmp;
	range_tx_t *data;

	/* Increment the seq_num of the initiator node */
	seq_num_initiator[trx]++;

	/*
	 * Fill in PHY frame.
	 */

	/* Get length of current frame. */
	app_frame_length = (RANGE_TEST_PKT_LENGTH - FRAME_OVERHEAD -tal_pib[trx].FCSLen ); /* to be
	                                                              * changed
	                                                               **///sriram 

	/* Set payload pointer. */
	frame_ptr = temp_frame_ptr
				= (uint8_t *)node_info[trx].tx_frame_info +
					LARGE_BUFFER_SIZE -
					app_frame_length - tal_pib[trx].FCSLen; /* Add 2
	                                                            * octets for
	                                                            * FCS. */

	tmp = (app_payload_t *)temp_frame_ptr;

	data = (range_tx_t *)&tmp->payload;

	(tmp->cmd_id) = RANGE_TEST_PKT;
	temp_frame_ptr++;
	(tmp->seq_num) = seq_num_initiator[trx]; /* to be incremented for every frame
	                                     **/
	temp_frame_ptr++;
	range_test_frame_cnt[trx]++;
	data->frame_count = (CCPU_ENDIAN_TO_LE32(range_test_frame_cnt[trx])); /* to
	                                                                  * be
	                                                                  *
	                                                                  *checked
	                                                                  **/
	temp_frame_ptr += 4;
	data->ed = 0;
	temp_frame_ptr++;
	data->lqi = 0;
	temp_frame_ptr++;

	/*
	 * Assign dummy payload values.
	 * Payload is stored to the end of the buffer avoiding payload copying
	 * by TAL.
	 */
	for (index = 0; index < (app_frame_length - 8); index++) { /* 1=> cmd ID
		                                                   **/
		*temp_frame_ptr++ = index; /* dummy values */
	}

	/* Source Address */
	temp_value =  tal_pib[trx].ShortAddress;
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
	temp_value = node_info[trx].peer_short_addr;
	frame_ptr -= SHORT_ADDR_LEN;
	convert_16_bit_to_byte_array(temp_value, frame_ptr);

	/* Destination PAN-Id */
	temp_value = CCPU_ENDIAN_TO_LE16(DST_PAN_ID);
	frame_ptr -= PAN_ID_LEN;
	convert_16_bit_to_byte_array(temp_value, frame_ptr);

	/* Set DSN. */
	frame_ptr--;
	*frame_ptr = seq_num_initiator[trx];

	/* Set the FCF. */
	fcf |= FCF_FRAMETYPE_DATA | FCF_SET_SOURCE_ADDR_MODE(FCF_SHORT_ADDR) |
			FCF_SET_DEST_ADDR_MODE(FCF_SHORT_ADDR);
	if (curr_trx_config_params[trx].ack_request) {
		fcf |= FCF_ACK_REQUEST;
	}

	frame_ptr -= FCF_LEN;
	
	convert_16_bit_to_byte_array(CCPU_ENDIAN_TO_LE16(fcf), frame_ptr);

    /* First element shall be length of PHY frame. */ //sriram
    //frame_ptr--;
    //*frame_ptr = curr_trx_config_params[trx].phy_frame_length;
	node_info[trx].tx_frame_info->length = (RANGE_TEST_PKT_LENGTH -tal_pib[trx].FCSLen) ;
	
	 /* Finished building of frame. */
    node_info[trx].tx_frame_info->mpdu = frame_ptr;

}



/**
 * \brief Function to send the range test start command to the receptor to start
 * the mode in the receptor
 */
static bool send_range_test_start_cmd(trx_id_t trx)
{
	uint16_t payload_length;
	app_payload_t msg;
	result_req_t *data;

	/* Create the payload */
	msg.cmd_id = RANGE_TEST_START_PKT;
	seq_num_initiator[trx]++;
	msg.seq_num = seq_num_initiator[trx];
	data = (result_req_t *)&msg.payload;
	/* Just a dummy value */
	data->cmd = DUMMY_PAYLOAD;

	payload_length = ((sizeof(app_payload_t) -
			sizeof(general_pkt_t)) +
			sizeof(result_req_t));

	/* Send the frame to Peer node */
	if (MAC_SUCCESS == transmit_frame(trx,FCF_SHORT_ADDR,
			(uint8_t *)&(node_info[trx].peer_short_addr),
			FCF_SHORT_ADDR,
			seq_num_initiator[trx],
			(uint8_t *)&msg,
			payload_length,
			true)
			) {
		return(true);
	}

	return(false);
}

/**
 * \brief Function to send the range test stop command to the receptor to stop
 * the mode in the receptor
 */
static bool send_range_test_stop_cmd(trx_id_t trx)
{
	uint16_t payload_length;
	app_payload_t msg;
	result_req_t *data;

	/* Create the payload */
	msg.cmd_id = RANGE_TEST_STOP_PKT;
	seq_num_initiator[trx]++;
	msg.seq_num = seq_num_initiator[trx];
	data = (result_req_t *)&msg.payload;
	/* Just a dummy value */
	data->cmd = DUMMY_PAYLOAD;

	payload_length = ((sizeof(app_payload_t) -
			sizeof(general_pkt_t)) +
			sizeof(result_req_t));

	/* Send the frame to Peer node */
	if (MAC_SUCCESS == transmit_frame(trx,FCF_SHORT_ADDR,
			(uint8_t *)&(node_info[trx].peer_short_addr),
			FCF_SHORT_ADDR,
			seq_num_initiator[trx],
			(uint8_t *)&msg,
			payload_length,
			true)
			) {
		return(true);
	}

	return(false);
}

/**
 * \brief Function to send the response packet for the marker sent from the
 * receptor
 */
static bool send_range_test_marker_rsp(trx_id_t trx)
{
	static uint8_t marker_seq_num[NO_TRX];
	uint16_t payload_length;
	app_payload_t msg;
	result_req_t *data;

	/* Create the payload */
	msg.cmd_id = RANGE_TEST_MARKER_RSP;
	seq_num_initiator[trx]++;
	msg.seq_num = marker_seq_num[trx]++;
	data = (result_req_t *)&msg.payload;
	/* Just a dummy value */
	data->cmd = DUMMY_PAYLOAD;

	payload_length = ((sizeof(app_payload_t) -
			sizeof(general_pkt_t)) +
			sizeof(result_req_t));

	/* Send the frame to Peer node */
	if (MAC_SUCCESS == transmit_frame(trx,FCF_SHORT_ADDR,
			(uint8_t *)&(node_info[trx].peer_short_addr),
			FCF_SHORT_ADDR,
			seq_num_initiator[trx],
			(uint8_t *)&msg,
			payload_length,
			true)
			) {
		sw_timer_start(APP_TIMER_TO_TX,
				LED_BLINK_RATE_IN_MICRO_SEC,
				SW_TIMEOUT_RELATIVE,
				(FUNC_PTR)marker_tx_timer_handler_cb,
				NULL);
		return(true);
	}

	return(false);
}



/* EOF */
