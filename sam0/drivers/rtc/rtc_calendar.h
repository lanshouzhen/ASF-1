/**
 * \file
 *
 * \brief SAM RTC Driver (Calendar Mode)
 *
 * Copyright (C) 2012-2014 Atmel Corporation. All rights reserved.
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
#ifndef RTC_CALENDAR_H_INCLUDED
#define RTC_CALENDAR_H_INCLUDED

/**
 * \defgroup asfdoc_sam0_rtc_calendar_group SAM RTC Calendar Driver (RTC CAL)
 *
 * This driver for Atmel | SMART SAM devices provides an interface for the configuration
 * and management of the device's Real Time Clock functionality in Calendar
 * operating mode, for the configuration and retrieval of the current time and
 * date as maintained by the RTC module. The following driver API modes are
 * covered by this manual:
 *
 *  - Polled APIs
 * \if RTC_CALENDAR_CALLBACK_MODE
 *  - Callback APIs
 * \endif
 *
 * The following peripherals are used by this module:
 *  - RTC (Real Time Clock)
 *
 * The following devices can use this module:
 *  - Atmel | SMART SAM D20/D21
 *  - Atmel | SMART SAM R21
 *  - Atmel | SMART SAM D10/D11
 *  - Atmel | SMART SAM L21
 *
 * The outline of this documentation is as follows:
 *  - \ref asfdoc_sam0_rtc_calendar_prerequisites
 *  - \ref asfdoc_sam0_rtc_calendar_module_overview
 *  - \ref asfdoc_sam0_rtc_calendar_special_considerations
 *  - \ref asfdoc_sam0_rtc_calendar_extra_info
 *  - \ref asfdoc_sam0_rtc_calendar_examples
 *  - \ref asfdoc_sam0_rtc_calendar_api_overview
 *
 *
 * \section asfdoc_sam0_rtc_calendar_prerequisites Prerequisites
 *
 * There are no prerequisites for this module.
 *
 *
 * \section asfdoc_sam0_rtc_calendar_module_overview Module Overview
 *
 * The RTC module in the SAM devices is a 32-bit counter, with a 10-bit
 * programmable prescaler. Typically, the RTC clock is run continuously,
 * including in the device's low-power sleep modes, to track the current time
 * and date information. The RTC can be used as a source to wake up the system
 * at a scheduled time or periodically using the alarm functions.
 *
 * In this driver, the RTC is operated in Calendar mode. This allows for an
 * easy integration of a real time clock and calendar into a user application
 * to track the passing of time and/or perform scheduled tasks.
 *
 * Whilst operating in Calendar mode, the RTC features:
 *  - Time tracking in seconds, minutes, and hours
 *  - 12 or 24 hour mode
 *  - Date tracking in day, month, and year
 *  - Automatic leap year correction
 *
 * \subsection asfdoc_sam0_rtc_features Driver Feature Macro Definition
 * <table>
 *  <tr>
 *    <th>Driver Feature Macro</th>
 *    <th>Supported devices</th>
 *  </tr>
 *  <tr>
 *    <td>FEATURE_RTC_PERIODIC_INT</td>
 *    <td>SAML21</td>
 *  </tr>
 *  <tr>
 *    <td>FEATURE_RTC_PRESCALER_OFF</td>
 *    <td>SAML21</td>
 *  </tr>
 *  <tr>
 *    <td>FEATURE_RTC_CLOCK_SELECTION</td>
 *    <td>SAML21</td>
 *  </tr>
 *  <tr>
 *    <td>FEATURE_RTC_GENERAL_PURPOSE_REG</td>
 *    <td>SAML21</td>
 *  </tr>
 *  <tr>
 *    <td>FEATURE_RTC_CONTINUOUSLY_UPDATED</td>
 *    <td>SAMD20,SAMD21,SAMR21,SAMD10,SAMD11</td>
 *  </tr>
 * </table>
 * \note The specific features are only available in the driver when the
 * selected device supports those features.
 *
 * \subsection asfdoc_sam0_rtc_calendar_module_overview_alarms Alarms and Overflow
 * The RTC has four independent hardware alarms that can be configured by the user
 * application. These alarms will be will triggered on match with the current
 * clock value, and can be set up to trigger an interrupt, event, or both. The
 * RTC can also be configured to clear the clock value on alarm match, resetting
 * the clock to the original start time.
 *
 * If the RTC is operated in clock-only mode (i.e. with calendar disabled), the
 * RTC counter value will instead be cleared on overflow once the maximum count
 * value has been reached:
 *
 * \f[ COUNT_{MAX} = 2^{32}-1 \f]
 *
 * When the RTC is operated with the calendar enabled and run using a nominal
 * 1Hz input clock frequency, a register overflow will occur after 64 years.
 *
 * \subsection asfdoc_sam0_rtc_calendar_module_overview_periodic Periodic Events
 * The RTC can generate events at periodic intervals, allowing for direct
 * peripheral actions without CPU intervention. The periodic events can be
 * generated on the upper eight bits of the RTC prescaler, and will be generated on
 * the rising edge transition of the specified bit. The resulting periodic
 * frequency can be calculated by the following formula:
 *
 * \f[ f_{PERIODIC}=\frac{f_{ASY}}{2^{n+3}} \f]
 *
 * Where \f$f_{ASY}\f$ refers to the \e asynchronous clock set up in the RTC
 * module configuration. For the RTC to operate correctly in calendar mode, this
 * frequency must be 1KHz, while the RTC's internal prescaler should be set to
 * divide by 1024. The \b n parameter is the event source generator index of the
 * RTC module. If the asynchronous clock is operated at the recommended 1KHz,
 * the formula results in the values shown in
 * \ref asfdoc_sam0_rtc_calendar_module_rtc_hz "the table below".
 *
 * \anchor asfdoc_sam0_rtc_calendar_module_rtc_hz
 * <table>
 *   <caption>RTC Event Frequencies for Each Prescaler Bit Using a 1KHz Clock</caption>
 *   <tr>
 *      <th>n</th> <th>Periodic event</th>
 *   </tr>
 *   <tr>
 *      <td>7</td> <td>1Hz</td>
 *   </tr>
 *   <tr>
 *      <td>6</td> <td>2Hz</td>
 *   </tr>
 *   <tr>
 *      <td>5</td> <td>4Hz</td>
 *   </tr>
 *   <tr>
 *      <td>4</td> <td>8Hz</td>
 *   </tr>
 *   <tr>
 *      <td>3</td> <td>16Hz</td>
 *   </tr>
 *   <tr>
 *      <td>2</td> <td>32Hz</td>
 *   </tr>
 *   <tr>
 *      <td>1</td> <td>64Hz</td>
 *   </tr>
 *   <tr>
 *      <td>0</td> <td>128Hz</td>
 *   </tr>
 * </table>
 *
 * \note The connection of events between modules requires the use of the
 *       \ref asfdoc_sam0_events_group "SAM Event System Driver (EVENTS)"
 *       to route output event of one module to the the input event of another.
 *       For more information on event routing, refer to the event driver
 *       documentation.
 *
 * \subsection asfdoc_sam0_rtc_calendar_module_overview_correction Digital Frequency Correction
 * The RTC module contains Digital Frequency Correction logic to compensate for
 * inaccurate source clock frequencies which would otherwise result in skewed
 * time measurements. The correction scheme requires that at least two bits
 * in the RTC module prescaler are reserved by the correction logic. As a
 * result of this implementation, frequency correction is only available when
 * the RTC is running from a 1Hz reference clock.
 *
 * The correction procedure is implemented by subtracting or adding a single
 * cycle from the RTC prescaler every 1024 RTC GCLK cycles. The adjustment is
 * applied the specified number of time (maximum 127) over 976 of these periods. The
 * corresponding correction in PPM will be given by:
 *
 * \f[ Correction(PPM) = \frac{VALUE}{999424}10^6 \f]
 *
 * The RTC clock will tick faster if provided with a positive correction value,
 * and slower when given a negative correction value.
 *
 *
 * \section asfdoc_sam0_rtc_calendar_special_considerations Special Considerations
 *
 * \subsection asfdoc_sam0_rtc_calendar_special_considerations_year Year Limit
 * The RTC module has a year range of 63 years from the starting year configured
 * when the module is initialized. Dates outside the start to end year range
 * described below will need software adjustment:
 *
 * \f[ [YEAR_{START}, YEAR_{START}+64) \f]
 *
 * \subsection asfdoc_sam0_rtc_calendar_special_considerations_clock Clock Setup
 * \subsubsection asfdoc_sam0_rtc_calendar_clock_samd_r SAM D20/D21/R21/D10/D11 Clock Setup
 * The RTC is typically clocked by a specialized GCLK generator that has a
 * smaller prescaler than the others. By default the RTC clock is on, selected
 * to use the internal 32KHz RC-oscillator with a prescaler of 32, giving a
 * resulting clock frequency of 1024Hz to the RTC. When the internal RTC
 * prescaler is set to 1024, this yields an end-frequency of 1Hz for correct
 * time keeping operations.
 *
 * The implementer also has the option to set other end-frequencies.
 * \ref asfdoc_sam0_rtc_calendar_rtc_out_freq "The table below" lists the
 * available RTC frequencies for each possible GCLK and RTC input prescaler
 * options.
 *
 * \anchor asfdoc_sam0_rtc_calendar_rtc_out_freq
 * <table>
 *   <caption>RTC Output Frequencies from Allowable Input Clocks</caption>
 *   <tr>
 *     <th>End-frequency</th>
 *     <th>GCLK prescaler</th>
 *     <th>RTC prescaler</th>
 *   </tr>
 *   <tr>
 *     <td>32KHz</td>
 *     <td>1</td>
 *     <td>1</td>
 *   </tr>
 *   <tr>
 *     <td>1KHz</td>
 *     <td>32</td>
 *     <td>1</td>
 *   </tr>
 *   <tr>
 *     <td>1Hz</td>
 *     <td>32</td>
 *     <td>1024</td>
 *   </tr>
 * </table>
 *
 * The overall RTC module clocking scheme is shown in
 * \ref asfdoc_sam0_rtc_calendar_rtc_clock_fig "the figure below".
 *
 * \anchor asfdoc_sam0_rtc_calendar_rtc_clock_fig
 * \dot
 * digraph clocking_scheme {
 *     rankdir=LR;
 *     GCLK [shape="record", label="<f0> GCLK | <f1> RTC_GCLK",
 *         bgcolor="lightgray", style="filled"];
 *     RTCPRE [shape="record" label="<f0> RTC | <f1> RTC PRESCALER"];
 *     RTC [shape="record", label="<f0> RTC | <f1> RTC CLOCK"];
 *
 *     GCLK:f1 -> RTCPRE:f1;
 *     RTCPRE:f1 -> RTC:f1;
 * }
 * \enddot
 *
 * \note For the calendar to operate correctly, an asynchronous clock of 1Hz
 *       should be used.
 *
 * \subsubsection asfdoc_sam0_rtc_calendar_clock_saml SAM L21 Clock Setup
 * The RTC clock can be selected from OSC32K,XOSC32K or OSCULP32K , and a 32KHz
 * or 1KHz oscillator clock frequency is required. This clock must be
 * configured and enabled in the 32KHz oscillator controller before using the RTC.
 *
 * The table below lists the available RTC clock \ref asfdoc_sam0_rtc_calendar_rtc_clk
 *
 * \anchor asfdoc_sam0_rtc_calendar_rtc_clk
 * <table>
 *   <caption>RTC clocks source</caption>
 *   <tr>
 *     <th>RTC clock frequency</th>
 *     <th>Clock source</th>
 *     <th>Description</th>
 *   </tr>
 *   <tr>
 *     <td>1.024KHz</td>
 *     <td>ULP1K</td>
 *     <td>1.024KHz from 32KHz internal ULP oscillator</td>
 *   </tr>
 *   <tr>
 *     <td>32.768KHz</td>
 *     <td>ULP32K</td>
 *     <td>32.768KHz from 32KHz internal ULP oscillator</td>
 *   </tr>
 *   <tr>
 *     <td>1.024KHz</td>
 *     <td>OSC1K</td>
 *     <td>1.024KHz from 32KHz internal oscillator</td>
 *   </tr>
 *   <tr>
 *     <td>32.768KHz</td>
 *     <td>OSC32K</td>
 *     <td>32.768KHz from 32KHz internal oscillator</td>
 *   </tr>
 *   <tr>
 *     <td>1.024KHz</td>
 *     <td>XOSC1K</td>
 *     <td>1.024KHz from 32KHz internal oscillator</td>
 *   </tr>
 *   <tr>
 *     <td>32.768KHz</td>
 *     <td>XOSC32K</td>
 *     <td>32.768KHz from 32KHz external crystal oscillator</td>
 *   </tr>
 * </table>
 *
 * \note For the calendar to operate correctly, an asynchronous clock of 1Hz
 *       should be used.
 *
 * \section asfdoc_sam0_rtc_calendar_extra_info Extra Information
 *
 * For extra information, see \ref asfdoc_sam0_rtc_calendar_extra. This includes:
 *  - \ref asfdoc_sam0_rtc_calendar_extra_acronyms
 *  - \ref asfdoc_sam0_rtc_calendar_extra_dependencies
 *  - \ref asfdoc_sam0_rtc_calendar_extra_errata
 *  - \ref asfdoc_sam0_rtc_calendar_extra_history
 *
 *
 * \section asfdoc_sam0_rtc_calendar_examples Examples
 *
 * For a list of examples related to this driver, see
 * \ref asfdoc_sam0_rtc_calendar_exqsg.
 *
 *
 * \section asfdoc_sam0_rtc_calendar_api_overview API Overview
 * @{
 */

#include <conf_clocks.h>

#if RTC_CALENDAR_ASYNC == true
#  include <system_interrupt.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Define port features set according to different device family
 * @{
*/
#if (SAML21) || defined(__DOXYGEN__)
/** RTC periodic interval interrupt. */
#  define FEATURE_RTC_PERIODIC_INT
/** RTC prescaler is off. */
#  define FEATURE_RTC_PRESCALER_OFF
/** RTC clock selection. */
#  define FEATURE_RTC_CLOCK_SELECTION
/** General purpose registers. */
#  define FEATURE_RTC_GENERAL_PURPOSE_REG
#else
/** RTC continuously updated. */
#  define FEATURE_RTC_CONTINUOUSLY_UPDATED
#endif
/*@}*/

#ifdef FEATURE_RTC_CLOCK_SELECTION
/**
 * \brief Available clock source for RTC.
 * RTC clock source.
 */
enum rtc_clock_sel {
	/** 1.024KHz from 32KHz internal ULP oscillator. */
	RTC_CLOCK_SELECTION_ULP1K = OSC32KCTRL_RTCCTRL_RTCSEL_ULP1K_Val,
	/** 32.768KHz from 32KHz internal ULP oscillator. */
	RTC_CLOCK_SELECTION_ULP32K = OSC32KCTRL_RTCCTRL_RTCSEL_ULP32K_Val,
	/** 1.024KHz from 32KHz internal oscillator. */
	RTC_CLOCK_SELECTION_OSC1K = OSC32KCTRL_RTCCTRL_RTCSEL_OSC1K_Val,
	/** 32.768KHz from 32KHz internal oscillator. */
	RTC_CLOCK_SELECTION_OSC32K = OSC32KCTRL_RTCCTRL_RTCSEL_OSC32K_Val,
	/** 1.024KHz from 32KHz internal oscillator. */
	RTC_CLOCK_SELECTION_XOSC1K = OSC32KCTRL_RTCCTRL_RTCSEL_XOSC1K_Val,
	/** 32.768KHz from 32.768KHz external crystal oscillator. */
	RTC_CLOCK_SELECTION_XOSC32K = OSC32KCTRL_RTCCTRL_RTCSEL_XOSC32K_Val,
};
#endif

#if !defined (RTC_NUM_OF_ALARMS) && defined(RTC_ALARM_NUM)
#define RTC_NUM_OF_ALARMS RTC_ALARM_NUM
#endif

/**
 * \brief Available alarm channels.
 *
 * Available alarm channels.
 *
 * \note Not all alarm channels are available on all devices.
 */
enum rtc_calendar_alarm {
	/** Alarm channel 0. */
	RTC_CALENDAR_ALARM_0 = 0,
#if (RTC_NUM_OF_ALARMS > 1) || defined(__DOXYGEN__)
	/** Alarm channel 1. */
	RTC_CALENDAR_ALARM_1 = 1,
#endif
#if (RTC_NUM_OF_ALARMS > 2) || defined(__DOXYGEN__)
	/** Alarm channel 2. */
	RTC_CALENDAR_ALARM_2 = 2,
#endif
#if (RTC_NUM_OF_ALARMS > 3) || defined(__DOXYGEN__)
	/** Alarm channel 3. */
	RTC_CALENDAR_ALARM_3 = 3,
#endif
};

#if RTC_CALENDAR_ASYNC == true
#ifdef FEATURE_RTC_PERIODIC_INT
/**
 * \brief Callback types.
 *
 * The available callback types for the RTC calendar module.
 */
enum rtc_calendar_callback {
	/** Callback for Periodic Interval 0 Interrupt. */
	RTC_CALENDAR_CALLBACK_PERIODIC_INTERVAL_0 = 0,
	/** Callback for Periodic Interval 1 Interrupt. */
	RTC_CALENDAR_CALLBACK_PERIODIC_INTERVAL_1,
	/** Callback for Periodic Interval 2 Interrupt. */
	RTC_CALENDAR_CALLBACK_PERIODIC_INTERVAL_2,
	/** Callback for Periodic Interval 3 Interrupt. */
	RTC_CALENDAR_CALLBACK_PERIODIC_INTERVAL_3,
	/** Callback for Periodic Interval 4 Interrupt. */
	RTC_CALENDAR_CALLBACK_PERIODIC_INTERVAL_4,
	/** Callback for Periodic Interval 5 Interrupt. */
	RTC_CALENDAR_CALLBACK_PERIODIC_INTERVAL_5,
	/** Callback for Periodic Interval 6 Interrupt. */
	RTC_CALENDAR_CALLBACK_PERIODIC_INTERVAL_6,
	/** Callback for Periodic Interval 7 Interrupt. */
	RTC_CALENDAR_CALLBACK_PERIODIC_INTERVAL_7,
	/** Callback for alarm 0. */
	RTC_CALENDAR_CALLBACK_ALARM_0,
#  if (RTC_NUM_OF_ALARMS > 1) || defined(__DOXYGEN__)
	/** Callback for alarm 1. */
	RTC_CALENDAR_CALLBACK_ALARM_1,
#  endif
#  if (RTC_NUM_OF_ALARMS > 2) || defined(__DOXYGEN__)
	/** Callback for alarm 2. */
	RTC_CALENDAR_CALLBACK_ALARM_2,
#  endif
#  if (RTC_NUM_OF_ALARMS > 3)	|| defined(__DOXYGEN__)
	/** Callback for alarm 3. */
	RTC_CALENDAR_CALLBACK_ALARM_3,
#  endif
	/** Callback for  overflow. */
	RTC_CALENDAR_CALLBACK_OVERFLOW,
#  if !defined(__DOXYGEN__)
	/** Total number of callbacks. */
	_RTC_CALENDAR_CALLBACK_N
#  endif
};
#else
/**
 * \brief Callback types.
 *
 * The available callback types for the RTC calendar module.
 */
enum rtc_calendar_callback {
	/** Callback for alarm 0. */
	RTC_CALENDAR_CALLBACK_ALARM_0 = 0,
#  if (RTC_NUM_OF_ALARMS > 1) || defined(__DOXYGEN__)
	/** Callback for alarm 1. */
	RTC_CALENDAR_CALLBACK_ALARM_1,
#  endif
#  if (RTC_NUM_OF_ALARMS > 2) || defined(__DOXYGEN__)
	/** Callback for alarm 2. */
	RTC_CALENDAR_CALLBACK_ALARM_2,
#  endif
#  if (RTC_NUM_OF_ALARMS > 3)	|| defined(__DOXYGEN__)
	/** Callback for alarm 3. */
	RTC_CALENDAR_CALLBACK_ALARM_3,
#  endif
	/** Callback for  overflow. */
	RTC_CALENDAR_CALLBACK_OVERFLOW,
#  if !defined(__DOXYGEN__)
	/** Total number of callbacks. */
	_RTC_CALENDAR_CALLBACK_N
#  endif
};
#endif

#  if !defined(__DOXYGEN__)
typedef void (*rtc_calendar_callback_t)(void);
#  endif
#endif

#ifdef FEATURE_RTC_PRESCALER_OFF
/**
 * \brief RTC input clock prescaler settings.
 *
 * The available input clock prescaler values for the RTC calendar module.
 */
enum rtc_calendar_prescaler {
	/** RTC prescaler is off, and the input clock frequency is
	prescaled by a factor of 1. */
	RTC_CALENDAR_PRESCALER_OFF      = RTC_MODE2_CTRLA_PRESCALER_OFF,
	/** RTC input clock frequency is prescaled by a factor of 1. */
	RTC_CALENDAR_PRESCALER_DIV_1    = RTC_MODE2_CTRLA_PRESCALER_DIV1,
	/** RTC input clock frequency is prescaled by a factor of 2. */
	RTC_CALENDAR_PRESCALER_DIV_2    = RTC_MODE2_CTRLA_PRESCALER_DIV2,
	/** RTC input clock frequency is prescaled by a factor of 4. */
	RTC_CALENDAR_PRESCALER_DIV_4    = RTC_MODE2_CTRLA_PRESCALER_DIV4,
	/** RTC input clock frequency is prescaled by a factor of 8. */
	RTC_CALENDAR_PRESCALER_DIV_8    = RTC_MODE2_CTRLA_PRESCALER_DIV8,
	/** RTC input clock frequency is prescaled by a factor of 16. */
	RTC_CALENDAR_PRESCALER_DIV_16   = RTC_MODE2_CTRLA_PRESCALER_DIV16,
	/** RTC input clock frequency is prescaled by a factor of 32. */
	RTC_CALENDAR_PRESCALER_DIV_32   = RTC_MODE2_CTRLA_PRESCALER_DIV32,
	/** RTC input clock frequency is prescaled by a factor of 64. */
	RTC_CALENDAR_PRESCALER_DIV_64   = RTC_MODE2_CTRLA_PRESCALER_DIV64,
	/** RTC input clock frequency is prescaled by a factor of 128. */
	RTC_CALENDAR_PRESCALER_DIV_128  = RTC_MODE2_CTRLA_PRESCALER_DIV128,
	/** RTC input clock frequency is prescaled by a factor of 256. */
	RTC_CALENDAR_PRESCALER_DIV_256  = RTC_MODE2_CTRLA_PRESCALER_DIV256,
	/** RTC input clock frequency is prescaled by a factor of 512. */
	RTC_CALENDAR_PRESCALER_DIV_512  = RTC_MODE2_CTRLA_PRESCALER_DIV512,
	/** RTC input clock frequency is prescaled by a factor of 1024. */
	RTC_CALENDAR_PRESCALER_DIV_1024 = RTC_MODE2_CTRLA_PRESCALER_DIV1024,
};

#else
/**
 * \brief RTC input clock prescaler settings.
 *
 * The available input clock prescaler values for the RTC calendar module.
 */
enum rtc_calendar_prescaler {
	/** RTC input clock frequency is prescaled by a factor of 1. */
	RTC_CALENDAR_PRESCALER_DIV_1    = RTC_MODE2_CTRL_PRESCALER_DIV1,
	/** RTC input clock frequency is prescaled by a factor of 2. */
	RTC_CALENDAR_PRESCALER_DIV_2    = RTC_MODE2_CTRL_PRESCALER_DIV2,
	/** RTC input clock frequency is prescaled by a factor of 4. */
	RTC_CALENDAR_PRESCALER_DIV_4    = RTC_MODE2_CTRL_PRESCALER_DIV4,
	/** RTC input clock frequency is prescaled by a factor of 8. */
	RTC_CALENDAR_PRESCALER_DIV_8    = RTC_MODE2_CTRL_PRESCALER_DIV8,
	/** RTC input clock frequency is prescaled by a factor of 16. */
	RTC_CALENDAR_PRESCALER_DIV_16   = RTC_MODE2_CTRL_PRESCALER_DIV16,
	/** RTC input clock frequency is prescaled by a factor of 32. */
	RTC_CALENDAR_PRESCALER_DIV_32   = RTC_MODE2_CTRL_PRESCALER_DIV32,
	/** RTC input clock frequency is prescaled by a factor of 64. */
	RTC_CALENDAR_PRESCALER_DIV_64   = RTC_MODE2_CTRL_PRESCALER_DIV64,
	/** RTC input clock frequency is prescaled by a factor of 128. */
	RTC_CALENDAR_PRESCALER_DIV_128  = RTC_MODE2_CTRL_PRESCALER_DIV128,
	/** RTC input clock frequency is prescaled by a factor of 256. */
	RTC_CALENDAR_PRESCALER_DIV_256  = RTC_MODE2_CTRL_PRESCALER_DIV256,
	/** RTC input clock frequency is prescaled by a factor of 512. */
	RTC_CALENDAR_PRESCALER_DIV_512  = RTC_MODE2_CTRL_PRESCALER_DIV512,
	/** RTC input clock frequency is prescaled by a factor of 1024. */
	RTC_CALENDAR_PRESCALER_DIV_1024 = RTC_MODE2_CTRL_PRESCALER_DIV1024,
};
#endif

#if !defined(__DOXYGEN__)
/**
 * \brief Device structure.
 */
struct rtc_module {
	/** RTC hardware module. */
	Rtc *hw;
	/** If clock mode 24h. */
	bool clock_24h;
#ifdef FEATURE_RTC_CONTINUOUSLY_UPDATED
	/** If continuously update clock register. */
	bool continuously_update;
#endif
	/** Initial year for counter value 0. */
	uint16_t year_init_value;
#  if RTC_CALENDAR_ASYNC == true
	/** Pointers to callback functions. */
	volatile rtc_calendar_callback_t callbacks[_RTC_CALENDAR_CALLBACK_N];
	/** Mask for registered callbacks. */
	volatile uint16_t registered_callback;
	/** Mask for enabled callbacks. */
	volatile uint16_t enabled_callback;
#  endif
};
#endif

/**
 * \brief Available mask options for alarms.
 *
 * Available mask options for alarms.
 */
enum rtc_calendar_alarm_mask {
	/** Alarm disabled. */
	RTC_CALENDAR_ALARM_MASK_DISABLED = RTC_MODE2_MASK_SEL_OFF,
	/** Alarm match on second. */
	RTC_CALENDAR_ALARM_MASK_SEC      = RTC_MODE2_MASK_SEL_SS,
	/** Alarm match on second and minute. */
	RTC_CALENDAR_ALARM_MASK_MIN      = RTC_MODE2_MASK_SEL_MMSS,
	/** Alarm match on second, minute, and hour. */
	RTC_CALENDAR_ALARM_MASK_HOUR     = RTC_MODE2_MASK_SEL_HHMMSS,
	/** Alarm match on second, minute, hour, and day. */
	RTC_CALENDAR_ALARM_MASK_DAY      = RTC_MODE2_MASK_SEL_DDHHMMSS,
	/** Alarm match on second, minute, hour, day, and month. */
	RTC_CALENDAR_ALARM_MASK_MONTH    = RTC_MODE2_MASK_SEL_MMDDHHMMSS,
	/** Alarm match on second, minute, hour, day, month, and year. */
	RTC_CALENDAR_ALARM_MASK_YEAR     = RTC_MODE2_MASK_SEL_YYMMDDHHMMSS,
};

/**
 * \brief RTC Calendar event enable/disable structure.
 *
 * Event flags for the \ref rtc_calendar_enable_events() and
 * \ref rtc_calendar_disable_events().
 */
struct rtc_calendar_events {
	/** Generate an output event on each overflow of the RTC count. */
	bool generate_event_on_overflow;
	/** Generate an output event on a alarm channel match against the RTC
	 *  count. */
	bool generate_event_on_alarm[RTC_NUM_OF_ALARMS];
	/** Generate an output event periodically at a binary division of the RTC
	 *  counter frequency.
	 */
	bool generate_event_on_periodic[8];
};

/**
 * \brief Time structure.
 *
 * Time structure containing the time given by or set to the RTC calendar.
 * The structure uses seven values to give second, minute, hour, PM/AM, day,
 * month, and year. It should be initialized via the
 * \ref rtc_calendar_get_time_defaults() function before use.
 */
struct rtc_calendar_time {
	/** Second value. */
	uint8_t  second;
	/** Minute value. */
	uint8_t  minute;
	/** Hour value. */
	uint8_t  hour;
	/** PM/AM value, \c true for PM, or \c false for AM. */
	bool     pm;
	/** Day value, where day 1 is the first day of the month. */
	uint8_t  day;
	/** Month value, where month 1 is January. */
	uint8_t  month;
	/** Year value.*/
	uint16_t year;
};

/**
 * \brief Alarm structure.
 *
 * Alarm structure containing time of the alarm and a mask to determine when
 * the alarm will trigger.
 */
struct rtc_calendar_alarm_time {
	/** Alarm time. */
	struct rtc_calendar_time time;
	/** Alarm mask to determine on what precision the alarm will match. */
	enum rtc_calendar_alarm_mask mask;
};

/**
 * \brief RTC configuration structure.
 *
 * Configuration structure for the RTC instance. This structure should
 * be initialized using the \ref rtc_calendar_get_config_defaults() before any
 * user configurations are set.
 */
struct rtc_calendar_config {
	/** Input clock prescaler for the RTC module. */
	enum rtc_calendar_prescaler prescaler;
	/** If \c true, clears the clock on alarm match. */
	bool clear_on_match;
#ifdef FEATURE_RTC_CONTINUOUSLY_UPDATED
	/** If \c true, the digital counter registers will be continuously updated
	 *  so that internal synchronization is not needed when reading the current
	 *  count. */
	bool continuously_update;
#endif
	/** If \c true, time is represented in 24 hour mode. */
	bool clock_24h;
	/** Initial year for counter value 0. */
	uint16_t year_init_value;
	/** Alarm values. */
	struct rtc_calendar_alarm_time alarm[RTC_NUM_OF_ALARMS];
};


/**
 * \name Configuration and Initialization
 * @{
 */

/**
 * \brief Initialize a \c time structure.
 *
 * This will initialize a given time structure to the time 00:00:00 (hh:mm:ss)
 * and date 2000-01-01 (YYYY-MM-DD).
 *
 * \param[out] time  Time structure to initialize
 */
static inline void rtc_calendar_get_time_defaults(
		struct rtc_calendar_time *const time)
{
	time->second = 0;
	time->minute = 0;
	time->hour   = 0;
	time->pm     = 0;
	time->day 	 = 1;
	time->month  = 1;
	time->year   = 2000;
}

/**
 * \brief Gets the RTC default settings.
 *
 * Initializes the configuration structure to the known default values. This
 * function should be called at the start of any RTC initiation.
 *
 * The default configuration is as follows:
 *  - Input clock divided by a factor of 1024
 *  - Clear on alarm match off
 *  - Continuously sync clock off
 *  - 12 hour calendar
 *  - Start year 2000 (Year 0 in the counter will be year 2000)
 *  - Events off
 *  - Alarms set to January 1. 2000, 00:00:00
 *  - Alarm will match on second, minute, hour, day, month, and year
 *
 *  \param[out] config  Configuration structure to be initialized to default
 *                      values.
 */
static inline void rtc_calendar_get_config_defaults(
		struct rtc_calendar_config *const config)
{
	/* Sanity check argument */
	Assert(config);

	/* Initialize and set time structure to default. */
	struct rtc_calendar_time time;
	rtc_calendar_get_time_defaults(&time);

	/* Set defaults into configuration structure */
	config->prescaler           = RTC_CALENDAR_PRESCALER_DIV_1024;
	config->clear_on_match      = false;
#ifdef FEATURE_RTC_CONTINUOUSLY_UPDATED
	config->continuously_update = false;
#endif
	config->clock_24h           = false;
	config->year_init_value     = 2000;
	for (uint8_t i = 0; i < RTC_NUM_OF_ALARMS; i++) {
		config->alarm[i].time = time;
		config->alarm[i].mask = RTC_CALENDAR_ALARM_MASK_YEAR;
	}
}

void rtc_calendar_reset(struct rtc_module *const module);
void rtc_calendar_enable(struct rtc_module *const module);
void rtc_calendar_disable(struct rtc_module *const module);

#if (RTC_INST_NUM > 1) && !defined(__DOXYGEN__)
/**
 * \internal Find the index of given RTC module instance.
 *
 * \param[in] hw  RTC module instance pointer
 *
 * \return Index of the given RTC module instance.
 */
uint8_t _rtc_get_inst_index(
		Rtc *const hw)
{
	/* List of available RTC modules. */
	static Rtc *const rtc_modules[RTC_INST_NUM] = RTC_INSTS;

	/* Find index for RTC instance. */
	for (uint32_t i = 0; i < RTC_INST_NUM; i++) {
		if (hw == rtc_modules[i]) {
			return i;
		}
	}

	/* Invalid data given. */
	Assert(false);
	return 0;
}
#endif /* (RTC_INST_NUM > 1) && !defined(__DOXYGEN__) */

void rtc_calendar_init(
		struct rtc_module *const module,
		Rtc *const hw,
		const struct rtc_calendar_config *const config);

void rtc_calendar_swap_time_mode(struct rtc_module *const module);

enum status_code rtc_calendar_frequency_correction(
		struct rtc_module *const module,
		const int8_t value);

/** @} */


/** \name Time and Alarm Management
 * @{
 */

void rtc_calendar_set_time(
		struct rtc_module *const module,
		const struct rtc_calendar_time *const time);

void rtc_calendar_get_time(
		struct rtc_module *const module,
		struct rtc_calendar_time *const time);

enum status_code rtc_calendar_set_alarm(
		struct rtc_module *const module,
		const struct rtc_calendar_alarm_time *const alarm,
		const enum rtc_calendar_alarm alarm_index);

enum status_code rtc_calendar_get_alarm(
		struct rtc_module *const module,
		struct rtc_calendar_alarm_time *const alarm,
		const enum rtc_calendar_alarm alarm_index);

/** @} */


/** \name Status Flag Management
 * @{
 */

/**
 * \brief Check if an RTC overflow has occurred.
 *
 * Checks the overflow flag in the RTC. The flag is set when there
 * is an overflow in the clock.
 *
 * \param[in,out] module  Pointer to the software instance struct
 *
 * \return Overflow state of the RTC module.
 *
 * \retval true   If the RTC count value has overflowed
 * \retval false  If the RTC count value has not overflowed
 */
static inline bool rtc_calendar_is_overflow(struct rtc_module *const module)
{
	/* Sanity check arguments */
	Assert(module);
	Assert(module->hw);

	Rtc *const rtc_module = module->hw;

	/* Return status of flag. */
	return (rtc_module->MODE2.INTFLAG.reg & RTC_MODE2_INTFLAG_OVF);
}

/**
 * \brief Clears the RTC overflow flag.
 *
 * \param[in,out] module  Pointer to the software instance struct
 *
 * Clears the RTC module counter overflow flag, so that new overflow conditions
 * can be detected.
 */
static inline void rtc_calendar_clear_overflow(struct rtc_module *const module)
{
	/* Sanity check arguments */
	Assert(module);
	Assert(module->hw);

	Rtc *const rtc_module = module->hw;

	/* Clear flag. */
	rtc_module->MODE2.INTFLAG.reg = RTC_MODE2_INTFLAG_OVF;
}

/**
 * \brief Check the RTC alarm flag.
 *
 * Check if the specified alarm flag is set. The flag is set when there
 * is an compare match between the alarm value and the clock.
 *
 * \param[in,out] module  Pointer to the software instance struct
 * \param[in] alarm_index  Index of the alarm to check
 *
 * \returns Match status of the specified alarm.
 *
 * \retval true   If the specified alarm has matched the current time
 * \retval false  If the specified alarm has not matched the current time
 */
static inline bool rtc_calendar_is_alarm_match(
		struct rtc_module *const module,
		const enum rtc_calendar_alarm alarm_index)
{
	/* Sanity check arguments */
	Assert(module);
	Assert(module->hw);

	Rtc *const rtc_module = module->hw;

	/* Sanity check. */
	if ((uint32_t)alarm_index > RTC_NUM_OF_ALARMS) {
		Assert(false);
		return false;
	}

	/* Return int flag status. */
	return (rtc_module->MODE2.INTFLAG.reg & RTC_MODE2_INTFLAG_ALARM(1 << alarm_index));
}

/**
 * \brief Clears the RTC alarm match flag.
 *
 * Clear the requested alarm match flag, so that future alarm matches can be
 * determined.
 *
 * \param[in,out] module  Pointer to the software instance struct
 * \param[in] alarm_index  The index of the alarm match to clear
 *
 * \return Status of the alarm match clear operation.
 *
 * \retval STATUS_OK               If flag was cleared correctly
 * \retval STATUS_ERR_INVALID_ARG  If invalid argument(s) were provided
 */
static inline enum status_code rtc_calendar_clear_alarm_match(
		struct rtc_module *const module,
		const enum rtc_calendar_alarm alarm_index)
{
	/* Sanity check arguments */
	Assert(module);
	Assert(module->hw);

	Rtc *const rtc_module = module->hw;

	/* Sanity check. */
	if ((uint32_t)alarm_index > RTC_NUM_OF_ALARMS) {
		Assert(false);
		return STATUS_ERR_INVALID_ARG;
	}

	/* Clear flag. */
	rtc_module->MODE2.INTFLAG.reg = RTC_MODE2_INTFLAG_ALARM(1 << alarm_index);

	return STATUS_OK;
}

/** @} */


/**
 * \name Event Management
 * @{
 */

/**
 * \brief Enables a RTC event output.
 *
 *  Enables one or more output events from the RTC module. See
 *  \ref rtc_calendar_events for a list of events this module supports.
 *
 *  \note Events cannot be altered while the module is enabled.
 *
 *  \param[in,out] module  Pointer to the software instance struct
 *  \param[in] events    Struct containing flags of events to enable
 */
static inline void rtc_calendar_enable_events(
		struct rtc_module *const module,
		struct rtc_calendar_events *const events)
{
	/* Sanity check arguments */
	Assert(module);
	Assert(module->hw);

	Rtc *const rtc_module = module->hw;

	uint32_t event_mask = 0;

	/* Check if the user has requested an overflow event. */
	if (events->generate_event_on_overflow) {
		event_mask |= RTC_MODE2_EVCTRL_OVFEO;
	}

	/* Check if the user has requested any alarm events. */
	for (uint8_t i = 0; i < RTC_NUM_OF_ALARMS; i++) {
		if (events->generate_event_on_alarm[i]) {
			event_mask |= RTC_MODE2_EVCTRL_ALARMEO(1 << i);
		}
	}

	/* Check if the user has requested any periodic events. */
	for (uint8_t i = 0; i < 8; i++) {
		if (events->generate_event_on_periodic[i]) {
			event_mask |= RTC_MODE2_EVCTRL_PEREO(1 << i);
		}
	}

	/* Enable given event(s). */
	rtc_module->MODE2.EVCTRL.reg |= event_mask;
}

/**
 * \brief Disables a RTC event output.
 *
 *  Disabled one or more output events from the RTC module. See
 *  \ref rtc_calendar_events for a list of events this module supports.
 *
 *  \note Events cannot be altered while the module is enabled.
 *
 *  \param[in,out] module  Pointer to the software instance struct
 *  \param[in] events    Struct containing flags of events to disable
 */
static inline void rtc_calendar_disable_events(
		struct rtc_module *const module,
		struct rtc_calendar_events *const events)
{
	/* Sanity check arguments */
	Assert(module);
	Assert(module->hw);

	Rtc *const rtc_module = module->hw;

	uint32_t event_mask = 0;

	/* Check if the user has requested an overflow event. */
	if (events->generate_event_on_overflow) {
		event_mask |= RTC_MODE2_EVCTRL_OVFEO;
	}

	/* Check if the user has requested any alarm events. */
	for (uint8_t i = 0; i < RTC_NUM_OF_ALARMS; i++) {
		if (events->generate_event_on_alarm[i]) {
			event_mask |= RTC_MODE2_EVCTRL_ALARMEO(1 << i);
		}
	}

	/* Check if the user has requested any periodic events. */
	for (uint8_t i = 0; i < 8; i++) {
		if (events->generate_event_on_periodic[i]) {
			event_mask |= RTC_MODE2_EVCTRL_PEREO(1 << i);
		}
	}

	/* Disable given event(s). */
	rtc_module->MODE2.EVCTRL.reg &= ~event_mask;
}

/** @} */

#ifdef FEATURE_RTC_GENERAL_PURPOSE_REG
/**
 * \name RTC General Purpose Registers
 * @{
 */

/**
 * \brief Write a value into general purpose register.
 *
 * \param[in] module  Pointer to the software instance struct
 * \param[in] n  General purpose type
 * \param[in] index General purpose register index (0..3)
 *
 */
static inline void rtc_write_general_purpose_reg(
	struct rtc_module *const module,
	const  uint8_t index,
	uint32_t value)
{
	/* Sanity check arguments */
	Assert(module);
	Assert(module->hw);
	Assert(index <= 3);

	Rtc *const rtc_module = module->hw;

	rtc_module->MODE0.GP[index].reg = value;
}

/**
 * \brief Read the value from general purpose register.
 *
 * \param[in] module  Pointer to the software instance struct
 * \param[in] index General purpose register index (0..3)
 *
 * \retval Value of general purpose register
 */
static inline uint32_t rtc_read_general_purpose_reg(
	struct rtc_module *const module,
	const  uint8_t index)
{
	/* Sanity check arguments */
	Assert(module);
	Assert(module->hw);
	Assert(index <= 3);

	Rtc *const rtc_module = module->hw;

	return rtc_module->MODE0.GP[index].reg;
}

/** @} */
#endif

/** @} */

#ifdef __cplusplus
}
#endif


/**
 * \page asfdoc_sam0_rtc_calendar_extra Extra Information for RTC (CAL) Driver
 *
 * \section asfdoc_sam0_rtc_calendar_extra_acronyms Acronyms
 * Below is a table listing the acronyms used in this module, along with their
 * intended meanings.
 *
 * <table>
 *	<tr>
 *		<th>Acronym</td>
 *		<th>Description</td>
 *	</tr>
 *	<tr>
 *		<td>RTC</td>
 *		<td>Real Time Counter</td>
 *	</tr>
 *	<tr>
 *		<td>PPM</td>
 *		<td>Part Per Million</td>
 *	</tr>
 *	<tr>
 *		<td>RC</td>
 *		<td>Resistor/Capacitor</td>
 *	</tr>
 * </table>
 *
 *
 * \section asfdoc_sam0_rtc_calendar_extra_dependencies Dependencies
 * This driver has the following dependencies:
 *
 *  - None
 *
 *
 * \section asfdoc_sam0_rtc_calendar_extra_errata Errata
 * There are no errata related to this driver.
 *
 *
 * \section asfdoc_sam0_rtc_calendar_extra_history Module History
 * An overview of the module history is presented in the table below, with
 * details on the enhancements and fixes made to the module since its first
 * release. The current version of this corresponds to the newest version in
 * the table.
 *
 * <table>
 *	<tr>
 *		<th>Changelog</th>
 *	</tr>
 *	<tr>
 *		<td>Added support for SAML21.</td>
 *	</tr>
 *	<tr>
 *		<td>
 *             Added support for SAMD21 and added driver instance parameter to all
 *             API function calls, except get_config_defaults
 *             </td>
 *	</tr>
 *	<tr>
 *		<td>Updated initialization function to also enable the digital interface
 *          clock to the module if it is disabled</td>
 *	</tr>
 *	<tr>
 *		<td>Initial Release</td>
 *	</tr>
 * </table>
 */

/**
 * \page asfdoc_sam0_rtc_calendar_exqsg Examples for RTC CAL Driver
 *
 * This is a list of the available Quick Start guides (QSGs) and example
 * applications for \ref asfdoc_sam0_rtc_calendar_group. QSGs are simple
 * examples with step-by-step instructions to configure and use this driver in a
 * selection of use cases. Note that QSGs can be compiled as a standalone
 * application or be added to the user application.
 *
 *  - \subpage asfdoc_sam0_rtc_calendar_basic_use_case
 * \if RTC_CALENDAR_CALLBACK_MODE
 *  - \subpage asfdoc_sam0_rtc_calendar_callback_use_case
 * \endif
 *
 * \page asfdoc_sam0_rtc_calendar_document_revision_history Document Revision History
 *
 * <table>
 *	<tr>
 *		<th>Doc. Rev.</td>
 *		<th>Date</td>
 *		<th>Comments</td>
 *	</tr>
 *	<tr>
 *		<td>E</td>
 *		<td>11/2014</td>
 *		<td>Added support for SAML21.</td>
 *	</tr>
 *	<tr>
 *		<td>D</td>
 *		<td>05/2014</td>
 *		<td>Added support for SAMR21 and SAMD10/D11.</td>
 *	</tr>
 *	<tr>
 *		<td>C</td>
 *		<td>01/2014</td>
 *		<td>Added support for SAMD21.</td>
 *	</tr>
 *	<tr>
 *		<td>B</td>
 *		<td>06/2013</td>
 *		<td>Added additional documentation on the event system. Corrected
 *          documentation typos.</td>
 *	</tr>
 *	<tr>
 *		<td>A</td>
 *		<td>06/2013</td>
 *		<td>Initial release</td>
 *	</tr>
 * </table>
 */

#endif /* RTC_CALENDAR_H_INCLUDED */

