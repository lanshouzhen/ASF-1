/**
 * \file perf_api_serial_handler.h
 *
 * \brief This file contains macros and function prototypes for SIO handling.
 * - Performance Analyzer application.
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

/* Prevent double inclusion */
#ifndef PERF_API_SERIAL_HANDLER_H
#define PERF_API_SERIAL_HANDLER_H

/**
 * \ingroup group_perf_analyzer
 * \defgroup group_serial_parser Serial Parser
 * Handles all the serial input and output commands.
 * It de-serializes the incoming Performance Analyzer Req Msg packets and calls
 * the corresponding Performance Analyzer APIs to process the request.
 * It also serializes the Performance Analyzer confirmation and indication Msgs
 * which are generated by the application.
 * It expects the packet to be received or send the packets through serial
 * interface in the following format.\n\r
 * ------------------------------------------------------------------\n\r
 * | SOT  | Length | Protocol Id | Msg Id | Msg Payload | EOT |\n\r
 * ------------------------------------------------------------------\n\r
 * \{
 */
/* === Includes ============================================================= */

/* === Types ================================================================ */


/* === Externals ============================================================ */


/* === Prototypes =========================================================== */

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * \brief This function does the initialization of the Serial handler state Machine.
     */
    void init_sio(void);

    /**
     *   \brief Function to handle the state machine serial data exchange.
     */
    void serial_data_handler(void);
    /**
     * \brief Function to write messages into UART/USB, used to send assert
     *  messages
     */
    void sio_write(uint8_t *message);

    //! \}
#ifdef __cplusplus
} /* extern "C" */
#endif

# endif
/* EOF */
