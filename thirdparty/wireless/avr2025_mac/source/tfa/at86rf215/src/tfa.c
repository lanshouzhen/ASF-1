/**
 * @file tfa.c
 *
 * @brief Implementation of Transceiver Feature Access (TFA) functionality.
 *
 * $Id: tfa.c 36425 2014-08-29 16:38:42Z uwalter $
 *
 * @author    Atmel Corporation: http://www.atmel.com
 * @author    Support email: avr@atmel.com
 */
/*
 * Copyright (c) 2012, Atmel Corporation All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

#if (defined ENABLE_TFA) || (defined CW_SUPPORTED)

/* === INCLUDES ============================================================ */

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "pal.h"
#include "return_val.h"
#include "tal.h"
#include "ieee_const.h"
#include "at86rf215.h"
#include "tal_internal.h"
#include "tfa.h"
#ifdef IQ_RADIO
#include "pal_internal.h"
#endif

/* === TYPES =============================================================== */

/* === MACROS ============================================================== */

/* === GLOBALS ============================================================= */

/* === PROTOTYPES ========================================================== */

/* === IMPLEMENTATION ====================================================== */


#if (defined ENABLE_TFA) || (defined TFA_CCA)
/**
 * @brief Perform a CCA
 *
 * This blocking function performs a CCA request.
 *
 * @return phy_enum_t PHY_IDLE or PHY_BUSY
 */
phy_enum_t tfa_cca_perform(trx_id_t trx_id)
{
    phy_enum_t ret;

    //debug_text(PSTR("tfa_cca_perform()"));

    if (tal_state[trx_id] != TAL_IDLE)
    {
        //debug_text(PSTR("TAL busy"));
        ret = PHY_BUSY;
    }
    else
    {
        rf_cmd_state_t previous_state = trx_state[trx_id];

        if (trx_state[trx_id] == RF_TRXOFF)
        {
            switch_to_txprep(trx_id);
        }
        if (trx_state[trx_id] != RF_RX)
        {
            switch_to_rx(trx_id);
            pal_timer_delay(tal_pib[trx_id].agc_settle_dur); // allow filters to settle
        }

        //debug_text_val(PSTR("CCA dur"), tal_pib[trx_id].CCADuration_us);

        /* Disable BB */
        uint16_t reg_offset = RF_BASE_ADDR_OFFSET * trx_id;
        pal_trx_bit_write(reg_offset + SR_BBC0_PC_BBEN, 0);

#ifndef BASIC_MODE
        /* Enable EDC interrupt */
        pal_trx_bit_write(reg_offset + SR_RF09_IRQM_EDC, 1);
#endif
        /* Start single ED measurement; use reg_write - it's the only subregister */
        tal_state[trx_id] = TAL_TFA_CCA;
#ifdef IQ_RADIO
        /* Enable EDC interrupt */
        pal_trx_bit_write(RF215_RF, reg_offset + SR_RF09_IRQM_EDC, 1);
        pal_trx_reg_write(RF215_RF, reg_offset + RG_RF09_EDC, RF_EDSINGLE);
#else
        pal_trx_reg_write(reg_offset + RG_RF09_EDC, RF_EDSINGLE);
#endif

        /* Wait until measurement is completed */
        while (TAL_RF_IS_IRQ_SET(trx_id, RF_IRQ_EDC) == false);
        TAL_RF_IRQ_CLR(trx_id, RF_IRQ_EDC);
        //debug_text(PSTR("Measurement completed"));
#ifndef BASIC_MODE
        /* Disable EDC interrupt again */
        pal_trx_bit_write(reg_offset + SR_RF09_IRQM_EDC, 0);
#endif
#ifdef IQ_RADIO
        pal_trx_bit_write(RF215_RF, reg_offset + SR_RF09_IRQM_EDC, 0);
#endif
        /* Since it is a blocking function, restore TAL state */
        tal_state[trx_id] = TAL_IDLE;

        switch_to_txprep(trx_id); /* Leave Rx mode */

        /* Switch BB on again */
        pal_trx_bit_write(reg_offset + SR_BBC0_PC_BBEN, 1);

        /* Capture ED value for current frame / ED scan */
#ifdef IQ_RADIO
        tal_current_ed_val[trx_id] = pal_trx_reg_read(RF215_RF, reg_offset + RG_RF09_EDV);
#else
        tal_current_ed_val[trx_id] = pal_trx_reg_read(reg_offset + RG_RF09_EDV);
#endif
        //debug_text_val(PSTR("tal_current_ed_val = "), (uint8_t)tal_current_ed_val[trx_id]);
#if (PAL_GENERIC_TYPE == MEGA_RF_SIM)
        uint16_t dbm = 256 - tal_current_ed_val[trx_id];
#endif
        //debug_text_val(PSTR("Energy (dBm) = -"), dbm);
        //debug_text_val(PSTR("tal_pib[trx_id].CCAThreshold = "), (uint8_t)tal_pib[trx_id].CCAThreshold);
        //debug_text_val(PSTR("CCAThreshold dBm = -"), (uint8_t)(~(uint8_t)tal_pib[trx_id].CCAThreshold) + 1);
        if (tal_current_ed_val[trx_id] < tal_pib[trx_id].CCAThreshold)
        {
            /* Idle */
            //debug_text(PSTR("channel idle"));
            ret = PHY_IDLE;
        }
        else
        {
            /* Busy */
            //debug_text(PSTR("channel busy"));
            ret = PHY_BUSY;
        }

        /* Restore previous trx state */
        if (previous_state == RF_RX)
        {
            switch_to_rx(trx_id);
        }
        else
        {
            //debug_text(PSTR("Switch back to TRXOFF"));
            /* Switch to TRXOFF */
            pal_trx_reg_write(reg_offset + RG_RF09_CMD, RF_TRXOFF);
#ifdef IQ_RADIO
            pal_trx_reg_write(RF215_RF, reg_offset + RG_RF09_CMD, RF_TRXOFF);
#endif
            trx_state[trx_id] = RF_TRXOFF;
        }
    }

    return ret;
}
#endif


#if (defined ENABLE_TFA) || (defined CW_SUPPORTED)
/**
 * @brief Starts continuous transmission on current channel
 *
 * @param trx_id Identifier of the transceiver
 * @param tx_mode Transmission mode
 */
void tfa_continuous_tx_start(trx_id_t trx_id, continuous_tx_mode_t tx_mode)
{
    uint16_t len;

    //debug_text(PSTR("tfa_continuous_tx_start()"));

    if (tal_state[trx_id] != TAL_IDLE)
    {
        //debug_text(PSTR("TAL not IDLE"));
        return;
    }

#ifdef IQ_RADIO
    if (trx_id == RF09)
    {
        /* Check if the other radio is currently in use */
        if (trx_state[RF24] == RF_TX)
        {
            debug_text_finish(PSTR("Radio is already in use"), DEBUG_ERROR);
            return;
        }
        else
        {
            /* Select corresponding baseband core */
            pal_trx_bit_write(RF215_BB, SR_RF_IQIFC2_CSELTX, RF09); // RF09 is selected
        }
    }
    else
    {
        /* Check if the other radio is currently in use */
        if (trx_state[RF09] == RF_TX)
        {
            debug_text_finish(PSTR("Radio is already in use"), DEBUG_ERROR);
            return;
        }
        else
        {
            /* Select corresponding baseband core */
            pal_trx_bit_write(RF215_BB, SR_RF_IQIFC2_CSELTX, RF24); // RF24 is selected
        }
    }
#endif

    if (trx_state[trx_id] == RF_RX)
    {
        tal_state[trx_id] = TAL_TFA_CW_RX;
    }
    else
    {
        tal_state[trx_id] = TAL_TFA_CW;
    }

    /* Set to TxPREP state */
    switch_to_txprep(trx_id);

    uint16_t reg_offset = RF_BASE_ADDR_OFFSET * trx_id;

    if (tx_mode == CW_MODE)
    {
        //debug_text(PSTR("CW mode"));

#ifdef IQ_RADIO
        /* Disable embedded TX control */
        pal_trx_bit_write(RF215_RF, SR_RF_IQIFC0_EEC, 0);
#else
        /* Enable baseband bypass */
        pal_trx_bit_write(SR_RF_IQIFC1_CHPM, 1);
#endif

        /* Configure DAC to generate carrier signal */
        uint8_t dac_config[2] = {(0x7E | 0x80), (0x3F | 0x80)};
#ifdef IQ_RADIO
        pal_trx_write(RF215_RF, reg_offset + RG_RF09_TXDACI, dac_config, 2);
#else
        pal_trx_write(reg_offset + RG_RF09_TXDACI, dac_config, 2);
#endif

        /* Trigger Tx start */
        //debug_text(PSTR("Start transmission"));
#ifdef IQ_RADIO
        pal_trx_reg_write(RF215_RF, reg_offset + RG_RF09_CMD, RF_TX);
#else
        pal_trx_reg_write(reg_offset + RG_RF09_CMD, RF_TX);
#endif
        trx_state[trx_id] = RF_TX;
    }
    else // PRBS mode
    {
        //debug_text(PSTR("PRBS mode"));

        /* Enable continuous transmission mode */
        pal_trx_bit_write(reg_offset + SR_BBC0_PC_CTX, 1);

        /* Fill length field */
#ifdef SUPPORT_LEGACY_OQPSK
        if (tal_pib[trx_id].phy.modulation == LEG_OQPSK)
        {
            len = 127;
        }
        else
#endif
        {
            len = 2047;
        }
        pal_trx_write(reg_offset + RG_BBC0_TXFLL, (uint8_t *)&len, 2);

        //debug_text(PSTR("Start transmission"));
        pal_trx_reg_write(reg_offset + RG_RF09_CMD, RF_TX);
        trx_state[trx_id] = RF_TX;

        /* Fill frame buffer */
        uint16_t tx_frm_buf_offset = BB_TX_FRM_BUF_OFFSET * trx_id;
        uint8_t data[10];
        for (uint16_t k = 0; k < (len / 10); k++)
        {
            for (uint16_t i = 0; i < 10; i++)
            {
                data[i] = (uint8_t)rand();
            }
            pal_trx_write(tx_frm_buf_offset + RG_BBC0_FBTXS, data, 10);
            tx_frm_buf_offset += 10;
        }
        uint16_t remaining_bytes = len % 10;
        for (uint16_t i = 0; i < remaining_bytes; i++)
        {
            data[i] = (uint8_t)rand();
        }
        pal_trx_write(tx_frm_buf_offset + RG_BBC0_FBTXS, data, remaining_bytes);
    }
}
#endif


#if (defined ENABLE_TFA) || (defined CW_SUPPORTED)
/**
 * @brief Stops CW transmission
 *
 * @param trx_id Identifier of the transceiver
 */
void tfa_continuous_tx_stop(trx_id_t trx_id)
{
    //debug_text(PSTR("tfa_continuous_tx_stop()"));

    uint16_t reg_offset = BB_BASE_ADDR_OFFSET * trx_id;
    /* Stop continuous transmission */
    pal_trx_bit_write(reg_offset + SR_BBC0_PC_CTX, 0);
#ifdef IQ_RADIO
    /* Allow command via SPI */
    pal_trx_bit_write(RF215_RF, SR_RF_IQIFC0_EEC, 0);
#endif
    switch_to_txprep(trx_id);

    /* Disable carrier transmission - even if it has not been used. */
    uint8_t dac_config[2] = {0x00, 0x00};
    uint16_t rft_reg_offset = RFT_TST_ADDR_OFFSET * trx_id;
#ifdef IQ_RADIO
    pal_trx_write(RF215_RF, rft_reg_offset + RG_RF09_TXDACI, dac_config, 2);
    /* Enable embedded TX control again */
    pal_trx_bit_write(RF215_RF, SR_RF_IQIFC0_EEC, 1);
#else
    pal_trx_write(rft_reg_offset + RG_RF09_TXDACI, dac_config, 2);
    /* Disable baseband bypass */
    pal_trx_bit_write( SR_RF_IQIFC1_CHPM, 0);
#endif

    /* Restore previous settings */
    if (tal_state[trx_id] == TAL_TFA_CW_RX)
    {
        tal_state[trx_id] = TAL_IDLE;
        switch_to_rx(trx_id);
    }
    else
    {
        tal_state[trx_id] = TAL_IDLE;
        //debug_text(PSTR("Switch back to TRXOFF"));
        /* Switch to TRXOFF */
#ifdef IQ_RADIO
        pal_trx_reg_write(RF215_RF, reg_offset + RG_RF09_CMD, RF_TRXOFF);
        pal_trx_reg_write(RF215_BB, reg_offset + RG_RF09_CMD, RF_TRXOFF);
#else
        pal_trx_reg_write(reg_offset + RG_RF09_CMD, RF_TRXOFF);
#endif
        trx_state[trx_id] = RF_TRXOFF;
    }
}
#endif


#endif /* #if (defined ENABLE_TFA) || (defined TFA_BAT_MON) */


/* EOF */
