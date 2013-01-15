/**
 * \file
 *
 * \brief AVR MEGARF MACSC Driver Example 2
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

/**
 * \mainpage
 *
 * \section intro Introduction
 * This simple example shows how to use the \ref macsc_group to use the MAC
 * Symbol counter to implement a sleep-wakeup scheme
 * suitable for beacon-enabled IEEE802.15.4 networks.
 *
 * \section files Main files:
 *  - macsc_megarf.c MAC Symbol Counter driver implementation
 *  - macsc_megarf.h MAC Symbol Counter driver definitions
 *  - macsc_megarf_example2.c example application
 *  - conf_example.h: configuration of the example
 *
 * \section driverinfo MACSC Driver
 * The MACSC driver can be found \ref macsc_group "here".
 *
 * \section deviceinfo Device Info
 * All megaRF devices with an MACSC can be used.
 *
 * \section exampledescription Description of the example
 * This example will prepare the MACSC to use the relative compare mode to
 * remain awake during the
 * Contention Access Period(CAP) and required GTS of the superframe and sleep
 * during the sleep period.
 * All 3 compare modules are used along with Beacon Timestamp register.
 * Ideally,Beacon Timestamp register is updated with SCCNT value when an
 * EEE802.15.4 frame is received(automatic Time stamping feature).
 * However,this application simulates the scenario by enabling manual write of
 * the Beacon Timestamp register to demonstrate relative compare.
 *
 * Power-save sleep mode of the MCU is used in the application.
 * The beacon time-stamp is captured whenever a compare 3 interrupt occurs.This
 * simulates the Beacon Interval(BEACON_INTERVAL in conf_example.h)
 * Compare 1 interrupt occurs with respect to the value in Beacon timestamp
 * register- relative compare mode.This simulates the active period of the
 * superframe(ACTIVE_PERIOD in conf_example.h)
 * Compare 2 interrupt occurs with respect to the value in Beacon timestamp
 * register- relative compare mode.This simulates the sleep period of the
 * superframe(SLEEP_PERIOD in conf_example.h)
 *
 * Action taken on occurence of interrupt callbacks
 *  - Compare 3 callback:LED3 toggle.End of Beacon Interval.Relative compare for
 * active period on compare 1 is started
 *  - Compare 1 callback:LED1 toggle.End of active period of superframe.Relative
 * compare for sleep period on compare 2 is started.Put system to sleep.
 *  - Compare 2 callback:LED2 toggle.End of sleep period of superframe.Sleep is
 * disabled.
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC and IAR for AVR.
 * Other compilers may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/">Atmel</A>.\n
 */
#include <conf_example.h>
#include <string.h>
#include <asf.h>

volatile static bool sleep;

/**
 * \brief Symbol Counter Compare 1 interrupt callback function
 *
 * This function is called when a compare match has occured on channel 1 of
 *  symbol counter.
 * Compare match occurs on end of sleep period
 */
static void example_cmp1_int_cb (void)
{
	sleep = 1;
	macsc_enable_cmp_int(MACSC_CC2);
	macsc_use_cmp(COMPARE_MODE, SLEEP_PERIOD, MACSC_CC2);
}

/**
 * \brief Symbol CounterCompare 2 interrupt callback function
 *
 * This function is called when a compare match has occured on channel 2 has
 * occurred
 *  Symbol Counter and switches off LED2.
 * Compare match occurs on end of active period
 */
static void example_cmp2_int_cb(void)
{
	sleep_disable();
	LED_Off(LED2_GPIO);
	sleep = 0;
}

/**
 * \brief Symbol Counter Compare 3 interrupt callback function
 *
 * This function is called when a compare match has occured on channel 3 of
 *  symbol counter  and toggles LED3.LED2 is turned on.
 *  Compare match occurs at the end of every beacon interval.
 */
static void example_cmp3_int_cb(void)
{
	LED_Toggle(LED3_GPIO);
	LED_On(LED2_GPIO);
	macsc_enable_manual_bts();
	macsc_enable_cmp_int(MACSC_CC1);
	macsc_use_cmp(COMPARE_MODE, ACTIVE_PERIOD, MACSC_CC1);
}

int main(void)
{
	board_init();
	sysclk_init();
	ioport_init();

	sysclk_enable_peripheral_clock(&TRX_CTRL_0);

	sleep_set_mode(SLEEP_SMODE_PSAVE);

	cpu_irq_enable();

	/*
	 * Enable Symbol Counter
	 */
	macsc_enable();
	macsc_write_clock_source(SYS_CLK_SRC);
	macsc_sleep_clk_enable();

	/*
	 * Configure interrupts callback functions
	 * overflow interrupt, compare 1,2,3 interrupts
	 */
	macsc_set_cmp1_int_cb(example_cmp1_int_cb);
	macsc_set_cmp2_int_cb(example_cmp2_int_cb);
	macsc_set_cmp3_int_cb(example_cmp3_int_cb);

	/*
	 * Configure MACSC to generate compare interrupts from channels 1,2,3
	 * Set compare mode to absolute,set compare value.
	 */
	macsc_enable_manual_bts();
	macsc_enable_cmp_int(MACSC_CC3);
	macsc_use_cmp(COMPARE_MODE, BEACON_INTERVAL, MACSC_CC3);

	do {
		/* Go to sleep, everything is handled by interrupts. */
		if (sleep) {
			sleep_enable();
			sleep_enter();
		}
	} while (1);
}
