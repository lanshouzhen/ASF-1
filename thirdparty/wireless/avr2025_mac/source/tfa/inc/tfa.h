/**
 * @file tfa.h
 *
 * @brief This file is the interface for Transceiver Feature Access (TFA)
 * functionality.
 *
 * $Id: tfa.h 36214 2014-07-22 14:28:43Z sschneid $
 *
 * @author    Atmel Corporation: http://www.atmel.com
 * @author    Support email: avr@atmel.com
 */
/*
 * Copyright (c) 2009, Atmel Corporation All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/* Prevent double inclusion */
#ifndef TFA_H
#define TFA_H

#if (defined ENABLE_TFA) || (defined TFA_BAT_MON) || (defined DOXYGEN) || \
    (defined TFA_BAT_MON_READ) || (defined TFA_BAT_MON_IRQ)

/* === INCLUDES ============================================================ */

#include "return_val.h"
#include "ieee_const.h"

/* === TYPES =============================================================== */

/** Transceiver commands */
typedef enum tfa_pib_t
{
    TFA_PIB_RX_SENS         = 0
} SHORTENUM tfa_pib_t;

/** Continuous Transmission modes */
// RF212B has a 2nd CW mode, described as "Additional CW Mode" in the datasheet
typedef enum continuous_tx_mode_tag
{
    CW_MODE = 0,
    PRBS_MODE = 1
#if (TAL_TYPE == AT86RF212B)
                ,
    CW_MODE_2 = 2,
    CW_ALL_ZEROS_MODE = 3
#endif /* (TAL_TYPE == AT86RF212B) */
} SHORTENUM continuous_tx_mode_t;

/* === MACROS ============================================================== */

/**
 * Default value of TFA PIB attribute to reduce the Rx sensitivity.
 * By default, reduced sensitivity is disabled.
 */
#if (TAL_TYPE == AT86RF212)
#define TFA_PIB_RX_SENS_DEF                 (RSSI_BASE_VAL_BPSK_40_DBM)
#elif (TAL_TYPE == AT86RF212B)
#define TFA_PIB_RX_SENS_DEF                 (RSSI_BASE_VAL_BPSK_600_DBM)
#else
#define TFA_PIB_RX_SENS_DEF                 (RSSI_BASE_VAL_DBM)
#endif

/**
 * Supply voltage above upper limit.
 */
#define SUPPLY_VOLTAGE_ABOVE_UPPER_LIMIT    (0xFFFF)

/**
 * Supply voltage below lower limit.
 */
#define SUPPLY_VOLTAGE_BELOW_LOWER_LIMIT    (0)

/* === GLOBALS ============================================================= */


/* === PROTOTYPES ========================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#if (defined ENABLE_TFA) || (defined DOXYGEN)
    /**
     * @brief Initializes the TFA
     *
     * This function is called to initialize the TFA.
     *
     * @return MAC_SUCCESS if everything went correct;
     *         FAILURE otherwise
     *
     * @ingroup apiTfaApi
     */
    retval_t tfa_init(void);
#endif

#if (defined ENABLE_TFA) || (defined DOXYGEN)
    /**
     * @brief Reset the TFA
     *
     * This function is called to reset the TFA.
     *
     * @param set_default_pib Defines whether PIB values need to be set
     *                        to its default values
     *
     * @ingroup apiTfaApi
     */
    void tfa_reset(bool set_default_pib);
#endif

#if (defined ENABLE_TFA) || (defined DOXYGEN)
    /**
     * @brief Gets a TFA PIB attribute
     *
     * This function is called to retrieve the transceiver information base
     * attributes.
     *
     * @param[in] tfa_pib_attribute TAL infobase attribute ID
     * @param[out] value TFA infobase attribute value
     *
     * @return MAC_UNSUPPORTED_ATTRIBUTE if the TFA infobase attribute is not found
     *         MAC_SUCCESS otherwise
     *
     * @ingroup apiTfaApi
     */
    retval_t tfa_pib_get(tfa_pib_t tfa_pib_attribute, void *value);
#endif

#if (defined ENABLE_TFA) || (defined DOXYGEN)
    /**
     * @brief Sets a TFA PIB attribute
     *
     * This function is called to set the transceiver information base
     * attributes.
     *
     * @param[in] tfa_pib_attribute TFA infobase attribute ID
     * @param[in] value TFA infobase attribute value to be set
     *
     * @return MAC_UNSUPPORTED_ATTRIBUTE if the TFA info base attribute is not found
     *         TAL_BUSY if the TAL is not in TAL_IDLE state.
     *         MAC_SUCCESS if the attempt to set the PIB attribute was successful
     *
     * @ingroup apiTfaApi
     */
    retval_t tfa_pib_set(tfa_pib_t tfa_pib_attribute, void *value);
#endif

#if (defined ENABLE_TFA) || (defined TFA_CCA) || (defined DOXYGEN)
#ifdef MULTI_TRX_SUPPORT
    /**
     * @brief Perform a CCA
     *
     * This function performs a CCA request.
     *
     * @param trx_id Transceiver identifier
     *
     * @return phy_enum_t PHY_IDLE or PHY_BUSY
     *
     * @ingroup apiTfaApi
     */
    phy_enum_t tfa_cca_perform(trx_id_t trx_id);
#else
    /**
     * @brief Perform a CCA
     *
     * This function performs a CCA request.
     *
     * @return phy_enum_t PHY_IDLE or PHY_BUSY
     *
     * @ingroup apiTfaApi
     */
    phy_enum_t tfa_cca_perform(void);
#endif /* #ifdef MULTI_TRX_SUPPORT */
#endif

#if (defined ENABLE_TFA) || (defined TFA_ED_SAMPLE) || (defined DOXYGEN)
#ifdef MULTI_TRX_SUPPORT
    /**
     * @brief Perform a single ED measurement
     *
     * @param trx_id Identifier of the transceiver
     *
     * @return ed_value Result of the measurement (transceiver's register value)
     *         If the build switch TRX_REG_RAW_VALUE is defined, the transceiver's
     *         register value is returned.
     *
     * @ingroup apiTfaApi
     */
    uint8_t tfa_ed_sample(trx_id_t trx_id);
#else
    /**
     * @brief Perform a single ED measurement
     *
     * @return ed_value Result of the measurement (transceiver's register value)
     *         If the build switch TRX_REG_RAW_VALUE is defined, the transceiver's
     *         register value is returned.
     *
     * @ingroup apiTfaApi
     */
    uint8_t tfa_ed_sample(void);
#endif /* #ifdef MULTI_TRX_SUPPORT */
#endif

#if (defined ENABLE_TFA) || (defined TFA_BAT_MON) || (defined DOXYGEN) || \
    (defined TFA_BAT_MON_READ)
    /**
     * @brief Gets the transceiver's supply voltage
     *
     * @return mv Milli Volt; 0 if below threshold, 0xFFFF if above threshold
     *
     * @ingroup apiTfaApi
     */
    uint16_t tfa_get_batmon_voltage(void);
#endif

#if (defined ENABLE_TFA) || (defined TFA_BAT_MON) || (defined DOXYGEN) || \
    (defined TFA_BAT_MON_IRQ)
    /**
     * @brief Setups the battery monitor interrupt
     *
     * @param   batmon_irq_cb Callback function for the batmon interrupt
     * @param   vth Threshold value in millivolt
     *
     * @return MAC_SUCCESS if provided voltage is within range, else
     *         MAC_INVALID_PARAMETER
     *
     * @ingroup apiTfaApi
     */
    retval_t tfa_batmon_irq_init(FUNC_PTR(batmon_irq_cb), uint16_t vth);
#endif

#if (defined ENABLE_TFA) || (PAL_GENERIC_TYPE == MEGA_RF) || (defined DOXYGEN)
    /**
     * @brief Get the temperature value from the integrated sensor
     *
     * @return temperature value in degree Celsius
     *
     * @ingroup apiTfaApi
     */
    double tfa_get_temperature(void);
#endif

#if ((defined ENABLE_TFA) || defined(DOXYGEN))
#ifdef MULTI_TRX_SUPPORT
    /**
     * @brief Starts continuous transmission on current channel
     *
     * @param trx_if Transceiver identifier
     * @param tx_mode Transmission mode
     * @param random_content Use random content if true
     */
    void tfa_continuous_tx_start(trx_id_t trx_id, continuous_tx_mode_t tx_mode);
#else
    /**
     * @brief Starts continuous transmission on current channel
     *
     * @param trx_id Identifier of the transceiver
     * @param tx_mode Transmission mode
     * @param random_content Use random content if true
     */
    void tfa_continuous_tx_start(continuous_tx_mode_t tx_mode, bool random_content);
#endif /* #ifdef MULTI_TRX_SUPPORT */
#endif

#if ((defined ENABLE_TFA) || defined(DOXYGEN))
#ifdef MULTI_TRX_SUPPORT
    /**
     * @brief Stops CW transmission
     *
     * @param trx_if Transceiver identifier
     */
    void tfa_continuous_tx_stop(trx_id_t trx_id);
#else
    /**
     * @brief Stops CW transmission
     */
    void tfa_continuous_tx_stop(void);
#endif /* #ifdef MULTI_TRX_SUPPORT */
#endif


#ifdef __cplusplus
} /* extern "C" */
#endif


#endif /* #ifdef ENABLE_TFA */

#endif /* TFA_H */
/* EOF */
