/**
 * \file
 *
 * \brief AST driver for SAM.
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

#ifndef _AST_H_
#define _AST_H_

/**
 * \defgroup group_sam_drivers_ast AST - Asynchronous Timer
 *
 * Driver for the AST (Asynchronous Timer).
 * Provides functions for configuring and operating the AST in the calendar or
 * timer/counter modes.
 *
 * \{
 */

#include "compiler.h"

/*! Timeout to prevent code hang in clock initialization */
#define AST_POLL_TIMEOUT 1000

/**
 * \name Oscillator Types
 */
/*! @{ */
/*! 1KHz clock from 32KHz oscillator (CLK_1K) */
#define AST_OSC_1KHZ     4
/*! Generic clock (GCLK) */
#define AST_OSC_GCLK     3
/*! Peripheral Bus Clock */
#define AST_OSC_PB       2
/*! 32KHz oscillator (OSC32K) */
#define AST_OSC_32KHZ    1
/*! System RC oscillator (RCSYS) */
#define AST_OSC_RC       0
/*! @} */

/** \name Predefined PSEL Values
 */
/*! @{ */

/*
 * The PSEL value to set the AST source clock (after the prescaler) to 1 Hz,
 * when using an external 32-kHz crystal.
 */
#define AST_PSEL_32KHZ_1HZ    14

/*
 * The PSEL value to set the AST source clock (after the prescaler)
 * to 1.76 Hz when using the internal RC oscillator (~ 115 kHz)
 */
#define AST_PSEL_RC_1_76HZ    15

/*! @} */


/*! Roundup operation for Digital Tuner in AST */
#define ROUNDUP_DIV(x, y) ((x % y) ? ((x / y) + 1) : (x / y))

/*
 * Description for Calendar Field:
 */
typedef struct ast_calv
{
	uint32_t sec : 6;
	uint32_t min : 6;
	uint32_t hour : 5;
	uint32_t day : 5;
	uint32_t month : 4;
	uint32_t year : 6;
} ast_calv_t;

/*! Input when initializing AST in calendar mode. */
typedef struct ast_calendar {
	union {
		uint32_t field;
		ast_calv_t FIELD;
	};
} ast_calendar_t;

/* Function Declarations */
void ast_enable(volatile Ast *ast);
uint32_t ast_init_counter(volatile Ast *ast, uint8_t osc_type,
		uint8_t psel, uint32_t ast_counter);
void ast_set_counter_value(volatile Ast *ast,
		uint32_t ast_counter);
void ast_set_periodic0_value(volatile Ast *ast, uint32_t pir);
void ast_enable_periodic0(volatile Ast *ast);
uint32_t ast_is_clkbusy(volatile Ast *ast);
uint32_t ast_is_busy(volatile Ast *ast);
void ast_clear_status_flag(volatile Ast *ast,
		uint32_t status_mask);
void ast_clear_periodic_status_flag(volatile Ast *ast,
		uint32_t periodic_channel);
void ast_enable_interrupt(volatile Ast *ast,
		uint32_t interrupt_mask);
void ast_enable_periodic_interrupt(volatile Ast *ast,
		uint8_t periodic_channel);
void ast_enable_async_wakeup(volatile Ast *ast,
		uint32_t wakeup_mask);
void ast_enable_periodic_async_wakeup(volatile Ast *ast,
		uint8_t periodic_channel);

/**
 * \}
 */

#endif  /* _AST_H_ */