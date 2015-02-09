/**
 * \file
 *
 * \brief TWI Temperature Sensor Example
 *
 * Copyright (c) 2014 Atmel Corporation. All rights reserved.
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

/**
 * \mainpage TWI Temperature Sensor Example
 *
 * \par Purpose
 *
 * This example program demonstrates how to use MCP980X module to control the
 * MCP9800/1/2/3 and retrieve ambient temperature.
 *
 * \par Usage
 *
 * -# Enable and initialize the TWI peripheral which connect with the sensor.
 * -# Set MCP980X configuration.
 * -# Set temperature limit and then read it.
 * -# Set temperature hysteresis and then read it.
 * -# Perform a single temperature measure by enabling One-Shot mode every
 *    second and retrieve ambient temperature.
 *
 * \note
 * The MCP9800/1/2/3 must be connected to the TWI bus of the microcontroller
 * and there must be pull-up on the TWI bus.
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "asf.h"

#define TEMP_LIMIT_MAX (100)  /* +100 degree Celsius */
#define TEMP_LIMIT_DEC (5000) /* 0.5 degree Celsius */
#define TEMP_LIMIT_MIN (-30)  /* -30 degree Celsius */

#define STRING_EOL    "\r"
#define STRING_HEADER "-- TWI Temperature Sensor Example (Celsius) --\n\r" \
		"-- "BOARD_NAME" --\r\n" \
		"-- Compiled: "__DATE__" "__TIME__" --"STRING_EOL

/** Global timestamp in milliseconds since start of application */
volatile uint32_t ul_ms_ticks = 0;

/**
 * \brief Handler for System Tick interrupt.
 *
 * Process System Tick Event increments the timestamp counter.
 */
void SysTick_Handler(void)
{
	ul_ms_ticks++;
}

/**
 * \brief Wait for the given number of milliseconds (using the ul_ms_ticks generated by the SAM microcontrollers' system tick).
 * \param ul_dly_ticks  Delay to wait for, in milliseconds.
 */
static void mdelay(uint32_t ul_dly_ticks)
{
	uint32_t ul_cur_ticks;

	ul_cur_ticks = ul_ms_ticks;
	while ((ul_ms_ticks - ul_cur_ticks) < ul_dly_ticks);
}

/**
 * \brief Configure the Console UART.
 */
static void configure_console(void)
{
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
		.paritytype = CONF_UART_PARITY
	};
	
	/* Configure console UART. */
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONF_UART, &uart_serial_options);
}

/**
 * \brief Application entry point for TWI temperature sensor example.
 *
 * \return Unused (ANSI-C compatibility).
 */
int main(void)
{
	int8_t temperature_int = 0;
	uint32_t temperature_dec = 0;

	/* Initialize the SAM system */
	sysclk_init();
	board_init();

	/* Initialize the console UART */
	configure_console();

	/* Output example information */
	puts(STRING_HEADER);

	/* Configure systick for 1 ms */
	printf("Configure system tick to get 1ms tick period.\n\r");
	if (SysTick_Config(sysclk_get_cpu_hz() / 1000)) {
		printf("-E- Systick configuration error\n\r");
		while (1) {
			/* Capture error */
		}
	}

	/* Initialize MCP980X driver. */
	mcp980x_init();

	/* Set MCP980X configuration.
	 * Disable ONE-SHOT mode;
	 * 12-bit ADC resolution;
	 * 2 fault queue cycles;
	 * Active-high alert polarity;
	 * Alert output in interrupt mode;
	 * Disable shutdown mode.
	 */
	mcp980x_set_configuration(
			MCP980X_CONFIG_RESOLUTION_12_BIT          |
			MCP980X_CONFIG_FAULT_QUEUE_2              |
			MCP980X_CONFIG_ALERT_POLARITY_ACTIVE_HIGH |
			MCP980X_CONFIG_INTERRUPT_MODE);

	/* Set temperature limit and then get it. */
	mcp980x_set_temperature_limit(TEMP_LIMIT_MAX, TEMP_LIMIT_DEC);
	mcp980x_get_temperature_limit(&temperature_int, &temperature_dec);
	printf("Temperature Limit:      %d.%04lu\r\n", temperature_int,
			(unsigned long)temperature_dec);

	/* Set temperature hysteresis and then get it. */
	mcp980x_set_temperature_hysteresis(TEMP_LIMIT_MIN, TEMP_LIMIT_DEC);
	mcp980x_get_temperature_hysteresis(&temperature_int, &temperature_dec);
	printf("Temperature Hysteresis: %d.%04lu\r\n", temperature_int,
			(unsigned long)temperature_dec);

	while (1) {
		/* Enable One-Shot mode to perform a single temperature measurement. */
		if (TWI_SUCCESS != mcp980x_one_shot_mode()) {
			break;
		}

		/* Retrieve ambient temperature every second and print it. */
		mdelay(1000);
		if (TWI_SUCCESS !=
				mcp980x_get_temperature(&temperature_int,
				&temperature_dec)) {
			break;
		}
		printf("Ambient Temperature:    %d.%04lu\r\n", temperature_int,
				(unsigned long)temperature_dec);
	}

	printf("TWI bus operation error!\r\n");
	while (1) {
		/* Do nothing */
	}
}
