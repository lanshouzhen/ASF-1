/**
 * @file tal_irq_handler.c
 *
 * @brief This file handles the interrupt generated by the transceiver
 *
 * Copyright (c) 2015 Atmel Corporation. All rights reserved.
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
 * Copyright (c) 2015, Atmel Corporation All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/* === INCLUDES ============================================================ */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "pal.h"
#include "tal_config.h"
#include "tal_internal.h"
#ifdef IQ_RADIO
#include "pal_internal.h"
#endif
#ifdef IRQ_DEBUGGING
#include "app_common.h"
#include <inttypes.h>
#endif
#include "ieee_const.h"
/* === TYPES =============================================================== */

/* === MACROS ============================================================== */

/* === EXTERNALS =========================================================== */

#ifdef IRQ_DEBUGGING
extern per_info_t per[];
#endif

/* === GLOBALS ============================================================= */

/* === PROTOTYPES ========================================================== */

#ifdef IQ_RADIO
static void switch_rf_to_txprep(trx_id_t trx_id);
#endif

/* === IMPLEMENTATION ====================================================== */


/**
 * @brief Transceiver interrupt handler
 *
 * This function handles the transceiver interrupt. It reads all IRQs from the
 * transceivers and stores them to a variable. If a transceiver is currently
 * sleeping, then the IRQs are not handled.
 * The actual processing of the IRQs is triggered from tal_task().
 */
void trx_irq_handler_cb(void)
{
    ////debug_text(PSTR("trx_irq_handler_cb()"));

#ifdef IRQ_DEBUGGING
    uint32_t now;
    pal_get_current_time(&now);
#endif

    /* Get all IRQS values */
    uint8_t irqs_array[4];

    trx_read( RG_RF09_IRQS, irqs_array, 4);

    /* Handle BB IRQS */
    for (trx_id_t trx_id = (trx_id_t)0; trx_id < NUM_TRX; trx_id++)
    {
        if (tal_state[trx_id] == TAL_SLEEP)
        {
            continue;
        }

        bb_irq_t irqs = (bb_irq_t)irqs_array[trx_id + 2];

        if (irqs != BB_IRQ_NO_IRQ)
        {
            ////debug_text_val(PSTR("INFO: ISR for BB "), trx_id);
            ////debug_text_val(PSTR("INFO: IRQ-flag-vector of RG_BBCx_IRQS ="), irqs);

            if (irqs & BB_IRQ_RXEM)
            {
                ////debug_text(PSTR("IRQ - BB_IRQ_RXEM"));
                irqs &= (uint8_t)(~((uint32_t)BB_IRQ_RXEM)); // avoid Pa091
            }
            if (irqs & BB_IRQ_RXAM)
            {
                ////debug_text(PSTR("IRQ - BB_IRQ_RXAM"));
                irqs &= (uint8_t)(~((uint32_t)BB_IRQ_RXAM)); // avoid Pa091
            }
            if (irqs & BB_IRQ_AGCR)
            {
                ////debug_text(PSTR("IRQ - BB_IRQ_AGCR"));
#if ((defined RF215v1) || (defined RF215v2)) && (defined SUPPORT_LEGACY_OQPSK)
                /* Workaround for errata reference #4908 */
                /* Keep flag set to trigger workaround; see tal.c */
#else
                irqs &= (uint8_t)(~((uint32_t)BB_IRQ_AGCR)); // avoid Pa091
#endif
#ifdef IRQ_DEBUGGING
                per[trx_id].agcr++;
                ////printf("AGCR %"PRIu32"\n", now);
#endif
            }
            if (irqs & BB_IRQ_AGCH)
            {
                ////debug_text(PSTR("IRQ - BB_IRQ_AGCH"));
#if ((defined RF215v1) || (defined RF215v2)) && (defined SUPPORT_LEGACY_OQPSK)
                /* Workaround for errata reference #4908 */
                /* Keep flag set to trigger workaround; see tal.c */
#else
                irqs &= (uint8_t)(~((uint32_t)BB_IRQ_AGCH)); // avoid Pa091
#endif
#ifdef IRQ_DEBUGGING
                per[trx_id].agch++;
                ////printf("AGCH %"PRIu32"\n", now);
#endif
            }
            if (irqs & BB_IRQ_RXFS)
            {
                ////debug_text(PSTR("IRQ - BB_IRQ_RXFS"));
#ifdef ENABLE_TSTAMP
                pal_get_current_time(&fs_tstamp[trx_id]);
#endif
#if ((defined RF215v1) || (defined RF215v2)) && (defined SUPPORT_LEGACY_OQPSK)
                /* Workaround for errata reference #4908 */
                /* Keep flag set to trigger workaround; see tal.c */
#else
                irqs &= (uint8_t)(~((uint32_t)BB_IRQ_RXFS)); // avoid Pa091
#endif
#ifdef IRQ_DEBUGGING
                per[trx_id].rxfs++;
                //printf("RXFS %"PRIu32"\n", now);
#endif
            }
            if (irqs & BB_IRQ_RXFE)
            {
                ////debug_text(PSTR("IRQ - BB_IRQ_RXFE"));
                pal_get_current_time(&rxe_txe_tstamp[trx_id]);
#ifdef IRQ_DEBUGGING
                per[trx_id].rxfe++;
                //printf("RXFE %"PRIu32"\n", now);
#endif
#if (defined RF215v1) && (!defined BASIC_MODE)
                /* Workaround for errata reference #4830 */
                /* Check if ACK transmission is actually requested by the received frame */
                uint16_t buf_reg_offset = BB_RX_FRM_BUF_OFFSET * trx_id;
                uint8_t fcf0 = trx_reg_read( buf_reg_offset + RG_BBC0_FBRXS);
                if ((fcf0 & FCF_ACK_REQUEST) == 0x00)
                {
                    /* Ensure ACK is not transmitted */
                    ////debug_text(PSTR("Apply workaround for #4830"));
                    uint16_t offset = RF_BASE_ADDR_OFFSET * trx_id;
                    trx_bit_write( offset + SR_BBC0_AMCS_AACK, 0);
                    trx_bit_write( offset + SR_BBC0_AMCS_AACK, 1);
                }
#endif
            }
            if (irqs & BB_IRQ_TXFE)
            {
                ////debug_text(PSTR("IRQ - BB_IRQ_TXFE"));
                /* used for IFS and for MEASURE_ON_AIR_DURATION */
                pal_get_current_time(&rxe_txe_tstamp[trx_id]);
            }

            /*
             * Store remaining flags to global TAL variable and
             * handle them within tal_task()
             */
            tal_bb_irqs[trx_id] |= irqs;
        }
    }

    /* Handle RF IRQS */
    for (trx_id_t trx_id = (trx_id_t)0; trx_id < NUM_TRX; trx_id++)
    {
        if (tal_state[trx_id] == TAL_SLEEP)
        {
            continue;
        }

        rf_irq_t irqs = (rf_irq_t)irqs_array[trx_id];

        if (irqs != RF_IRQ_NO_IRQ)
        {
            ////debug_text_val(PSTR("INFO: ISR for RF "), trx_id);
            ////debug_text_val(PSTR("INFO: IRQ-flag-vector of RG_RFxx_IRQS ="), irqs);

            if (irqs & RF_IRQ_TRXRDY)
            {
                ////debug_text(PSTR("IRQ - RF_IRQ_TRXRDY"));
                irqs &= (uint8_t)(~((uint32_t)RF_IRQ_TRXRDY)); // avoid Pa091
            }
            if (irqs & RF_IRQ_TRXERR)
            {
                printf(("\n \r IRQ - RF_IRQ_TRXERR"));
                irqs &= (uint8_t)(~((uint32_t)RF_IRQ_TRXERR)); // avoid Pa091
            }
            if (irqs & RF_IRQ_BATLOW)
            {
                ////debug_text(PSTR("IRQ - RF_IRQ_BATLOW"));
            }
            if (irqs & RF_IRQ_WAKEUP)
            {
                ////debug_text(PSTR("IRQ - RF_IRQ_WAKEUP"));
            }
            if (irqs & RF_IRQ_IQIFSF)
            {
                ////debug_text(PSTR("IRQ - RF_IRQ_IQIFSF"));
            }
            if (irqs & RF_IRQ_EDC)
            {
                ////debug_text(PSTR("IRQ - RF_IRQ_EDC"));
            }
            tal_rf_irqs[trx_id] |= irqs;
        }
    }
}/* trx_irq_handler_cb() */


#ifdef IQ_RADIO
void bb_irq_handler_cb(void)
{
    ////debug_text(PSTR("bb_irq_handler_cb()"));
#ifdef IRQ_DEBUGGING
    uint32_t now;
    pal_get_current_time(&now);
#endif

    /* Get all IRQS values */
    uint8_t irqs_array[4];

    trx_read(RF215_BB, RG_RF09_IRQS, irqs_array, 4);

    /* Handle BB IRQS */
    for (trx_id_t trx_id = (trx_id_t)0; trx_id < NUM_TRX; trx_id++)
    {
        if (tal_state[trx_id] == TAL_SLEEP)
        {
            continue;
        }

        uint8_t irqs = irqs_array[trx_id + 2];

        if (irqs != BB_IRQ_NO_IRQ)
        {
            ////debug_text_val(PSTR("INFO: ISR for BB "), trx_id);
            ////debug_text_val(PSTR("INFO: BB IRQ-flag-vector of RG_BBCx_IRQS ="), irqs);

            ////printf("BB IRQS 0x%"PRIX8"\n", irqs);

            if (irqs & BB_IRQ_RXEM)
            {
                ////debug_text(PSTR("BB IRQ - RXEM"));
                irqs &= (uint8_t)(~((uint32_t)BB_IRQ_RXEM)); // avoid Pa091
                //////debug_text(PSTR("No further processing BB_IRQ_RXEM"));
            }
            if (irqs & BB_IRQ_RXAM)
            {
                ////debug_text(PSTR("BB IRQ - RXAM"));
                irqs &= (uint8_t)(~((uint32_t)BB_IRQ_RXAM)); // avoid Pa091
                //////debug_text(PSTR("No further processing BB_IRQ_RXAM"));
            }
            if (irqs & BB_IRQ_AGCR)
            {
                ////debug_text(PSTR("BB IRQ - AGCR"));
                irqs &= (uint8_t)(~((uint32_t)BB_IRQ_AGCR)); // avoid Pa091
                CALC_REG_OFFSET(trx_id);
                /* Release AGC */
                //////debug_text(PSTR("Release AGC"));
                trx_bit_write(RF215_RF, GET_REG_ADDR(SR_RF09_AGCC_FRZC), 0);
#ifdef IRQ_DEBUGGING
                per[trx_id].agcr++;
                //printf("AGCR %"PRIu32"\n", now);
#endif
#if (defined RF215v1) && (!defined BASIC_MODE)
                /* Workaround for errata reference #4830 */
                if ((irqs & BB_IRQ_RXFE) == 0)
                {
                    ////debug_text(PSTR("Apply workaround for #4830"));
                    CALC_REG_OFFSET(trx_id);
                    trx_bit_write( GET_REG_ADDR(SR_BBC0_AMCS_AACK), 0);
                    trx_bit_write( GET_REG_ADDR(SR_BBC0_AMCS_AACK), 1);
                }
#endif
            }
            if (irqs & BB_IRQ_AGCH)
            {
                ////debug_text(PSTR("BB IRQ - AGCH"));
                irqs &= (uint8_t)(~((uint32_t)BB_IRQ_AGCH)); // avoid Pa091
                /* Hold AGC */
                CALC_REG_OFFSET(trx_id);
                //////debug_text(PSTR("Hold AGC"));
                trx_bit_write(RF215_RF, GET_REG_ADDR(SR_RF09_AGCC_FRZC), 1);
#ifdef IRQ_DEBUGGING
                per[trx_id].agch++;
                //printf("AGCH %"PRIu32"\n", now);
#endif
            }
            if (irqs & BB_IRQ_RXFS)
            {
                ////debug_text(PSTR("BB IRQ - RXFS"));
#ifdef ENABLE_TSTAMP
                pal_get_current_time(&fs_tstamp[trx_id]);
#endif
                irqs &= (uint8_t)(~((uint32_t)BB_IRQ_RXFS)); // avoid Pa091
#ifdef IRQ_DEBUGGING
                per[trx_id].rxfs++;
                //printf("RXFS %"PRIu32"\n", now);
#endif
            }
            if (irqs & BB_IRQ_RXFE)
            {
                ////debug_text(PSTR("BB IRQ - RXFE"));
#ifdef IRQ_DEBUGGING
                per[trx_id].rxfe++;
                //printf("RXFE %"PRIu32"\n", now);
#endif
                pal_get_current_time(&rxe_txe_tstamp[trx_id]);
                /* Wait for TXPREP and clear TRXRDY IRQ */
                switch_rf_to_txprep((trx_id_t)trx_id);
            }
            if (irqs & BB_IRQ_TXFE)
            {
                ////debug_text(PSTR("BB IRQ - TXFE"));
                /* used for IFS and for MEASURE_ON_AIR_DURATION */
                pal_get_current_time(&rxe_txe_tstamp[trx_id]);
                /* BB interrupt handles further processing */
            }

            /*
             * Store remaining flags to global TAL variable and
             * handle them within tal_task()
             */
            tal_bb_irqs[trx_id] |= irqs;
        }
    }

    /* Handle RF IRQS */
    for (trx_id_t trx_id = (trx_id_t)0; trx_id < NUM_TRX; trx_id++)
    {
        if (tal_state[trx_id] == TAL_SLEEP)
        {
            continue;
        }

        uint8_t irqs = irqs_array[trx_id];

        if (irqs != RF_IRQ_NO_IRQ)
        {
            ////debug_text_val(PSTR("INFO: ISR for RF "), trx_id);
            ////debug_text_val(PSTR("INFO: IRQ-flag-vector of RG_RFxx_IRQS ="), irqs);

            if (irqs & RF_IRQ_TRXRDY)
            {
                ////debug_text(PSTR("BB IRQ - RF_IRQ_TRXRDY"));
                irqs &= (uint8_t)(~((uint32_t)RF_IRQ_TRXRDY)); // avoid Pa091
            }
            if (irqs & RF_IRQ_TRXERR)
            {
                ////debug_text(PSTR("BB IRQ - RF_IRQ_TRXERR"));
            }
            if (irqs & RF_IRQ_BATLOW)
            {
                ////debug_text(PSTR("BB IRQ - RF_IRQ_BATLOW"));
                irqs &= (uint8_t)(~((uint32_t)RF_IRQ_BATLOW)); // avoid Pa091
            }
            if (irqs & RF_IRQ_WAKEUP)
            {
                ////debug_text(PSTR("BB IRQ - RF_IRQ_WAKEUP"));
                irqs &= (uint8_t)(~((uint32_t)RF_IRQ_WAKEUP)); // avoid Pa091
            }
            if (irqs & RF_IRQ_IQIFSF)
            {
                ////debug_text(PSTR("BB IRQ - RF_IRQ_IQIFSF"));
            }
            if (irqs & RF_IRQ_EDC)
            {
                ////debug_text(PSTR("BB IRQ - RF_IRQ_EDC"));
                irqs &= (uint8_t)(~((uint32_t)RF_IRQ_EDC)); // avoid Pa091
            }

            if (irqs != 0)
            {
                ////debug_text_finish(PSTR("Unexpected RF IRQS for BB device"), DEBUG_ERROR);
            }

            tal_rf_irqs[trx_id] |= irqs;
        }
    }
}/* bb_irq_handler_cb() */
#endif


#ifdef IQ_RADIO
void rf_irq_handler_cb(void)
{
    ////debug_text(PSTR("rf_irq_handler_cb()"));
    //////debug_text_val(PSTR("chip_mode ="), chip_mode);

    /* Get all IRQS values */
    uint8_t irqs_array[4];

    trx_read(RF215_RF, RG_RF09_IRQS, irqs_array, 4);

    /* Handle BB IRQS */
    for (trx_id_t trx_id = (trx_id_t)0; trx_id < NUM_TRX; trx_id++)
    {
        if (tal_state[trx_id] == TAL_SLEEP)
        {
            continue;
        }

        uint8_t irqs = irqs_array[trx_id + 2];

        if (irqs != BB_IRQ_NO_IRQ)
        {
            ////debug_text_val(PSTR("INFO: ISR for BB "), trx_id);
            ////debug_text_val(PSTR("INFO: BB IRQ-flag-vector of RG_BBCx_IRQS ="), irqs);

            if (irqs & BB_IRQ_RXEM)
            {
                ////debug_text(PSTR("RF IRQ - RXEM"));
            }
            if (irqs & BB_IRQ_RXAM)
            {
                ////debug_text(PSTR("RF IRQ - RXAM"));
            }
            if (irqs & BB_IRQ_AGCR)
            {
                ////debug_text(PSTR("RF IRQ - AGCR"));
            }
            if (irqs & BB_IRQ_AGCH)
            {
                ////debug_text(PSTR("RF IRQ - AGCH"));
            }
            if (irqs & BB_IRQ_RXFS)
            {
                ////debug_text(PSTR("RF IRQ - RXFS"));
            }
            if (irqs & BB_IRQ_RXFE)
            {
                ////debug_text(PSTR("RF IRQ - RXFE"));
            }
            if (irqs & BB_IRQ_TXFE)
            {
                ////debug_text(PSTR("RF IRQ - TXFE"));
            }

            if (irqs != 0)
            {
                ////debug_text_finish(PSTR("Unexpected BB IRQS for RF device"), DEBUG_ERROR);
            }

            /*
             * Store remaining flags to global TAL variable and
             * handle them within tal_task()
             */
            tal_bb_irqs[trx_id] |= irqs;
        }
    }

    /* Handle RF IRQS */
    for (trx_id_t trx_id = (trx_id_t)0; trx_id < NUM_TRX; trx_id++)
    {
        if (tal_state[trx_id] == TAL_SLEEP)
        {
            continue;
        }

        uint8_t irqs = irqs_array[trx_id];

        if (irqs != RF_IRQ_NO_IRQ)
        {
            ////debug_text_val(PSTR("INFO: ISR for RF "), trx_id);
            ////debug_text_val(PSTR("INFO: IRQ-flag-vector of RG_RFxx_IRQS ="), irqs);

            if (irqs & RF_IRQ_TRXRDY)
            {
                ////debug_text(PSTR("RF IRQ - RF_IRQ_TRXRDY"));
                irqs &= (uint8_t)(~((uint32_t)RF_IRQ_TRXRDY)); // avoid Pa091
            }
            if (irqs & RF_IRQ_TRXERR)
            {
                ////debug_text(PSTR("RF IRQ - RF_IRQ_TRXERR"));
            }
            if (irqs & RF_IRQ_BATLOW)
            {
                ////debug_text(PSTR("RF IRQ - RF_IRQ_BATLOW"));
                //irqs &= (uint8_t)(~((uint32_t)RF_IRQ_BATLOW)); // avoid Pa091
            }
            if (irqs & RF_IRQ_WAKEUP)
            {
                ////debug_text(PSTR("RF IRQ - RF_IRQ_WAKEUP"));
                //irqs &= (uint8_t)(~((uint32_t)RF_IRQ_WAKEUP)); // avoid Pa091
            }
            if (irqs & RF_IRQ_IQIFSF)
            {
                ////debug_text(PSTR("RF IRQ - RF_IRQ_IQIFSF"));
            }
            if (irqs & RF_IRQ_EDC)
            {
                ////debug_text(PSTR("RF IRQ - RF_IRQ_EDC"));
            }
            /*
            if (irqs != 0)
            {
                ////debug_text_finish(PSTR("Unexpected RF IRQS"), DEBUG_ERROR);
            }
            */

            tal_rf_irqs[trx_id] |= irqs;
        }
    }
}/* rf_irq_handler_cb() */
#endif /* #ifdef IQ_RADIO */


#ifdef IQ_RADIO
static void switch_rf_to_txprep(trx_id_t trx_id)
{
    ////debug_text(PSTR("switch_rf_to_txprep()"));

    CALC_REG_OFFSET(trx_id);
    trx_reg_write(RF215_RF, GET_REG_ADDR(RG_RF09_CMD), RF_TXPREP);
    /* Wait for TXPREP */
    rf_cmd_state_t state;
    do
    {
        state = (rf_cmd_state_t)trx_reg_read(RF215_RF, GET_REG_ADDR(RG_RF09_STATE));
        //////debug_text_val(PSTR("state"), state);
    }
    while (state != RF_TXPREP);
    /* Clear TRXRDY interrupt */
    uint8_t irqs = trx_reg_read(RF215_RF, trx_id + RG_RF09_IRQS);
    tal_rf_irqs[trx_id] |= irqs & ((uint8_t)(~((uint32_t)RF_IRQ_TRXRDY))); // avoid Pa091
    pal_dev_irq_flag_clr(RF215_RF);
}
#endif /* #ifdef IQ_RADIO */



/* EOF */
