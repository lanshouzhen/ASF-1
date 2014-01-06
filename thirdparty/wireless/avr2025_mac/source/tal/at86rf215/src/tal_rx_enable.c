/**
 * @file tal_rx_enable.c
 *
 * @brief File provides functionality supporting RX-Enable feature.
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
 *
 */

/*
 * Copyright (c) 2013, Atmel Corporation All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/* === INCLUDES ============================================================ */

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "pal.h"
#include "return_val.h"
#include "tal.h"
#include "ieee_const.h"
#include "tal_config.h"
#include "bmm.h"
#include "qmm.h"
#include "tal_internal.h"


/* === TYPES =============================================================== */

/* === MACROS ============================================================== */

/* === GLOBALS ============================================================= */

/* === PROTOTYPES ========================================================== */

/* === IMPLEMENTATION ====================================================== */

/**
 * @brief Switches receiver on or off
 *
 * This function switches the receiver on (PHY_RX_ON) or off (PHY_TRX_OFF).
 *
 * @param trx_id Transceiver identifier
 * @param state New state of receiver
 *
 * @return TAL_BUSY if the TAL state machine cannot switch receiver on or off,
 *         PHY_TRX_OFF if receiver has been switched off, or
 *         PHY_RX_ON otherwise.
 *
 * @ingroup apiTalApi
 */
uint8_t tal_rx_enable(trx_id_t trx_id, uint8_t state)
{
    uint8_t ret_val;

    //debug_text_val(PSTR("tal_rx_enable, trx_id ="), trx_id);

    if (tal_state[trx_id] == TAL_SLEEP)
    {
        //debug_text(PSTR("TAL_TRX_ASLEEP"));
        return TAL_TRX_ASLEEP;
    }

    /*
     * Trx can only be enabled if TAL is not busy;
     * i.e. if TAL is IDLE.
     */
    if (tal_state[trx_id] != TAL_IDLE)
    {
        //debug_text(PSTR("TAL is busy"));
        return TAL_BUSY;
    }

    if (state == PHY_TRX_OFF)
    {
        //debug_text(PSTR("Switch to PHY_TRX_OFF"));
        /*
            * If the rx needs to be switched off,
            * we are not interested in a frame that is currently being received.
            */
        uint16_t rf_reg_offset = RF_BASE_ADDR_OFFSET * trx_id;
        pal_trx_reg_write(rf_reg_offset + RG_RF09_CMD, RF_TRXOFF);
        trx_state[trx_id] = RF_TRXOFF;
        tal_buf_shortage[trx_id] = false;
        ret_val = PHY_TRX_OFF;
#ifdef ENABLE_FTN_PLL_CALIBRATION
        stop_ftn_timer(trx_id);
#endif  /* ENABLE_FTN_PLL_CALIBRATION */
    }
    else
    {
        //debug_text(PSTR("Switch to PHY_RX_ON"));
        switch_to_txprep(trx_id);
        switch_to_rx(trx_id);
        ret_val = PHY_RX_ON;
#ifdef ENABLE_FTN_PLL_CALIBRATION
        start_ftn_timer(trx_id);
#endif  /* ENABLE_FTN_PLL_CALIBRATION */
    }

    return ret_val;
}

/* EOF */
