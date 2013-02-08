/**
 * @file tal_irq_handler.c
 *
 * @brief This file handles the interrupts generated by the transceiver.
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
 * Copyright (c) 2013, Atmel Corporation All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/* === INCLUDES ============================================================ */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "pal.h"
#include "return_val.h"
#include "tal.h"
#include "ieee_const.h"
#include "stack_config.h"
#include "bmm.h"
#include "qmm.h"
#include "tal_irq_handler.h"
#include "tal_rx.h"
#include "atmega128rfa1.h"
#include "tal_internal.h"
#include "tal_constants.h"
#include "tal_tx.h"
#include "mac_build_config.h"

/* === TYPES =============================================================== */


/* === MACROS ============================================================== */


/* === GLOBALS ============================================================= */

/*
 * Function pointers to store the callback function of
 * the transceiver interrupt
 */
/** Function pointer to store callback for transceiver TX_END interrupt. */
static irq_handler_t irq_hdl_trx_tx_end;
/** Function pointer to store callback for transceiver RX_END interrupt. */
static irq_handler_t irq_hdl_trx_rx_end;
/** Function pointer to store callback for transceiver CCA_ED interrupt. */
static irq_handler_t irq_hdl_trx_cca_ed;
/** Function pointer to store callback for transceiver AWAKE interrupt. */
static irq_handler_t irq_hdl_trx_awake;

#if (defined BEACON_SUPPORT) || (defined ENABLE_TSTAMP) || (defined DOXYGEN)
/** Function pointer to store callback for transceiver timestamp (RX_START) interrupt. */
static irq_handler_t irq_hdl_trx_tstamp;
#endif

#if (defined ENABLE_ALL_TRX_IRQS) || (defined DOXYGEN)
/** Function pointer to store callback for transceiver AMI interrupt. */
static irq_handler_t irq_hdl_trx_ami;
/** Function pointer to store callback for transceiver BATMON interrupt. */
static irq_handler_t irq_hdl_trx_batmon;
/** Function pointer to store callback for transceiver PLL_LOCK interrupt. */
static irq_handler_t irq_hdl_trx_pll_lock;
/** Function pointer to store callback for transceiver PLL_UNLOCK interrupt. */
static irq_handler_t irq_hdl_trx_pll_unlock;
/** Function pointer to store callback for transceiver AES_READY interrupt. */
static irq_handler_t irq_hdl_trx_aes_ready;
#endif  /* ENABLE_ALL_TRX_IRQS */

/* === Prototypes ========================================================== */


/* === Implementation ====================================================== */

/*
 * \brief Initializes the transceiver TX END interrupt
 *
 * This function sets the microcontroller specific registers
 * responsible for handling the transceiver TX END interrupt
 *
 * \param trx_irq_cb Callback function for the transceiver TX END interrupt
 */
void pal_trx_irq_init_tx_end(FUNC_PTR trx_irq_cb)
{
    irq_hdl_trx_tx_end = (irq_handler_t)trx_irq_cb;
}


/*
 * \brief Initializes the transceiver RX END interrupt
 *
 * This function sets the microcontroller specific registers
 * responsible for handling the transceiver RX END interrupt
 *
 * \param trx_irq_cb Callback function for the transceiver RX END interrupt
 */
void pal_trx_irq_init_rx_end(FUNC_PTR trx_irq_cb)
{
    irq_hdl_trx_rx_end = (irq_handler_t)trx_irq_cb;
}

/*
 * \brief Initializes the transceiver CCA ED END interrupt
 *
 * This function sets the microcontroller specific registers
 * responsible for handling the transceiver CCA ED END interrupt
 *
 * \param trx_irq_cb Callback function for the transceiver CCA ED END interrupt
 */
void pal_trx_irq_init_cca_ed(FUNC_PTR trx_irq_cb)
{
    irq_hdl_trx_cca_ed = (irq_handler_t)trx_irq_cb;
}


#if (defined BEACON_SUPPORT) || (defined ENABLE_TSTAMP) || (defined DOXYGEN)
/*
 * \brief Initializes the transceiver timestamp interrupt (RX START interrupt)
 *
 * This function sets the microcontroller specific registers
 * responsible for handling the transceiver timestamp interrupt (RX START interrupt)
 *
 * \param trx_irq_cb Callback function for the transceiver
 * timestamp interrupt (RX START interrupt)
 */
void pal_trx_irq_init_tstamp(FUNC_PTR trx_irq_cb)
{
    irq_hdl_trx_tstamp = (irq_handler_t)trx_irq_cb;
}
#endif  /* #if (defined BEACON_SUPPORT) || (defined ENABLE_TSTAMP) || (defined DOXYGEN) */

/*
 * \brief Initializes the transceiver AWAKE interrupt
 *
 * This function sets the microcontroller specific registers
 * responsible for handling the transceiver AWAKE interrupt
 *
 * \param trx_irq_cb Callback function for the transceiver AWAKE interrupt
 */
void pal_trx_irq_init_awake(FUNC_PTR trx_irq_cb)
{
    irq_hdl_trx_awake = (irq_handler_t)trx_irq_cb;
}

#if defined(ENABLE_ALL_TRX_IRQS) || defined(DOXYGEN)
/*
 * \brief Initializes the transceiver AMI interrupt
 *
 * This function sets the microcontroller specific registers
 * responsible for handling the transceiver AMI interrupt
 *
 * \param trx_irq_cb Callback function for the transceiver AMI interrupt
 */
void pal_trx_irq_init_ami(FUNC_PTR trx_irq_cb)
{
    irq_hdl_trx_ami = (irq_handler_t)trx_irq_cb;
}


/*
 * \brief Initializes the transceiver BATMON interrupt
 *
 * This function sets the microcontroller specific registers
 * responsible for handling the transceiver BATMON interrupt
 *
 * \param trx_irq_cb Callback function for the transceiver BATMON interrupt
 */
void pal_trx_irq_init_batmon(FUNC_PTR trx_irq_cb)
{
    irq_hdl_trx_batmon = (irq_handler_t)trx_irq_cb;
}


/*
 * \brief Initializes the transceiver PLL_LOCK interrupt
 *
 * This function sets the microcontroller specific registers
 * responsible for handling the transceiver PLL_LOCK interrupt
 *
 * \param trx_irq_cb Callback function for the transceiver PLL_LOCK interrupt
 */
void pal_trx_irq_init_pll_lock(FUNC_PTR trx_irq_cb)
{
    irq_hdl_trx_pll_lock = (irq_handler_t)trx_irq_cb;
}


/*
 * \brief Initializes the transceiver PLL_UNLOCK interrupt
 *
 * This function sets the microcontroller specific registers
 * responsible for handling the transceiver PLL_UNLOCK interrupt
 *
 * \param trx_irq_cb Callback function for the transceiver PLL_UNLOCK interrupt
 */
void pal_trx_irq_init_pll_unlock(FUNC_PTR trx_irq_cb)
{
    irq_hdl_trx_pll_unlock = (irq_handler_t)trx_irq_cb;
}


/*
 * \brief Initializes the transceiver AES_READY interrupt
 *
 * This function sets the microcontroller specific registers
 * responsible for handling the transceiver AES_READY interrupt
 *
 * \param trx_irq_cb Callback function for the transceiver AES_READY interrupt
 */
void pal_trx_irq_init_aes_ready(FUNC_PTR trx_irq_cb)
{
    irq_hdl_trx_aes_ready = (irq_handler_t)trx_irq_cb;
}
#endif  /* ENABLE_ALL_TRX_IRQS */


/**
 * \brief ISR for transceiver's transmit end interrupt
 */
ISR(TRX24_TX_END_vect)
{
    irq_hdl_trx_tx_end();
}

/**
 * \brief ISR for transceiver's receive end interrupt
 */
ISR(TRX24_RX_END_vect)
{
    irq_hdl_trx_rx_end();
}

/**
 * \brief ISR for transceiver's CCA/ED measurement done interrupt
 */
ISR(TRX24_CCA_ED_DONE_vect)
{
    irq_hdl_trx_cca_ed();
}

/**
 * \brief ISR for transceiver's Awake interrupt
 */
ISR(TRX24_AWAKE_vect)
{
    irq_hdl_trx_awake();
}

/**
 * \brief ISR for transceiver's rx start interrupt
 *
 * By the time the SFD is detected, the hardware timestamps the
 * current frame in the SCTSR register.
 */
ISR(TRX24_RX_START_vect)
{
#if (defined BEACON_SUPPORT) || (defined ENABLE_TSTAMP)
    irq_hdl_trx_tstamp();
#endif  /* #if (defined BEACON_SUPPORT) || (defined ENABLE_TSTAMP) */
}

/**
 * \brief ISR for transceiver's PLL lock interrupt
 */
ISR(TRX24_PLL_LOCK_vect)
{
#ifdef ENABLE_ALL_TRX_IRQS
    irq_hdl_trx_pll_lock();
#endif  /*  ENABLE_ALL_TRX_IRQS*/
}

/**
 * \brief ISR for transceiver's PLL unlock interrupt
 */
ISR(TRX24_PLL_UNLOCK_vect)
{
#ifdef ENABLE_ALL_TRX_IRQS
    irq_hdl_trx_pll_unlock();
#endif  /*  ENABLE_ALL_TRX_IRQS*/
}

/**
 * \brief ISR for transceiver's address match interrupt
 */
ISR(TRX24_XAH_AMI_vect)
{
#ifdef ENABLE_ALL_TRX_IRQS
    irq_hdl_trx_ami();
#endif  /*  ENABLE_ALL_TRX_IRQS*/
}

/**
 * \brief ISR for transceiver's battery low interrupt
 */
ISR(BAT_LOW_vect)
{
#ifdef ENABLE_ALL_TRX_IRQS
    irq_hdl_trx_batmon();
#endif  /*  ENABLE_ALL_TRX_IRQS*/
}

/**
 * \brief ISR for transceiver's AES interrupt
 */
ISR(AES_READY_vect)
{
#ifdef ENABLE_ALL_TRX_IRQS
    irq_hdl_trx_aes_ready();
#endif  /*  ENABLE_ALL_TRX_IRQS*/
}

/*
 * \brief Transceiver interrupt handler
 *
 * This function handles the transceiver generated interrupts for RX end.
 */
void trx_rx_end_handler_cb(void)
{
    ENTER_CRITICAL_REGION();

    /* Handle rx interrupt. */
    handle_received_frame_irq();    // see tal_rx.c

    LEAVE_CRITICAL_REGION();
}/* trx_rx_end_handler_cb() */


/*
 * \brief Transceiver interrupt handler
 *
 * This function handles the transceiver generated interrupts for TX end.
 */
void trx_tx_end_handler_cb(void)
{
    ENTER_CRITICAL_REGION();

    /* Get the result and push it to the queue. */
    handle_tx_end_irq();            // see tal_tx.c

    LEAVE_CRITICAL_REGION();
}/* trx_tx_end_handler_cb() */


#if (defined BEACON_SUPPORT) || (defined ENABLE_TSTAMP)
/*
 * \brief Timestamp interrupt handler
 *
 * This function handles the interrupts handling the timestamp.
 * The timestamping is only required for beaconing networks
 * or if timestamping is explicitly enabled.
 */
void trx_irq_timestamp_handler_cb(void)
{
    /* The timestamping is only required for beaconing networks
     * or if timestamping is explicitly enabled.
     */
    pal_trx_read_timestamp(&tal_rx_timestamp);
}
#endif /* (defined BEACON_SUPPORT) || (defined ENABLE_TSTAMP) */

/*
 * \brief Transceiver interrupt handler for awake end IRQ
 *
 * This function handles the transceiver awake end interrupt.
 */
void trx_awake_handler_cb(void)
{
    /* Set the wake-up flag. */
    tal_awake_end_flag = true;
}
/* EOF */

