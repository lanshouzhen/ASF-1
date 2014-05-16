/**
 * \file
 *
 * \brief USB OTG Driver for UHDP.
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

#ifndef UHDP_OTG_H_INCLUDED
#define UHDP_OTG_H_INCLUDED

#include "compiler.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Get USB VBus/ID pin configuration in board configuration */
#include "conf_board.h"
#include "board.h"
#include "ioport.h"
#include "pio.h"
#include "pio_handler.h"

__always_inline static void otg_io_pin_init(uint32_t pin, uint32_t flags,
    IRQn_Type port_irqn, uint8_t irq_level,
    void (*handler)(uint32_t,uint32_t), uint32_t wkup)
{
	// IOPORT must be initialized before by ioport_init(), \see ioport_group.
	pio_handler_set_pin(pin, flags, handler);
	ioport_set_pin_sense_mode(pin, IOPORT_SENSE_BOTHEDGES);
	NVIC_SetPriority(port_irqn, irq_level);
	NVIC_EnableIRQ(port_irqn);
	pio_enable_pin_interrupt(pin);
	if (wkup) {
		pmc_set_fast_startup_input(wkup);
	}
}

//! \ingroup usb_group
//! \defgroup otg_group UHDP OTG Driver
//! UHDP low-level driver for OTG features
//!
//! @{

/**
 * \brief Initialize the dual role
 * This function is implemented in uhdp_host.c file.
 *
 * \return \c true if the ID pin management has been started, otherwise \c false.
 */
bool otg_dual_enable(void);

/**
 * \brief Uninitialize the dual role
 * This function is implemented in uhdp_host.c file.
 */
void otg_dual_disable(void);


//! @name UHDP OTG ID pin management
//! The ID pin come from the USB OTG connector (A and B receptacle) and
//! allows to select the USB mode host or device.
//!
//! The ID pin can be managed through PIO pin.
//! This feature is optional, and it is enabled if USB_ID_PIN
//! is defined in board.h and CONF_BOARD_USB_ID_DETECT defined in
//! conf_board.h.
//! @{
#define OTG_ID_DETECT       (defined(CONF_BOARD_USB_ID_DETECT))
#define OTG_ID_IO           (defined(USB_ID_PIN) && OTG_ID_DETECT)

// TODO
#if OTG_ID_IO
# define pad_id_init() \
	io_pad_init(USB_ID_PIN, USB_ID_FLAGS, otg_id_handler, UHD_USB_INT_LEVEL);
# define pad_id_interrupt_disable() gpio_disable_pin_interrupt(USB_ID_PIN)
# define pad_ack_id_interrupt()     gpio_clear_pin_interrupt_flag(USB_ID_PIN)
# define Is_pad_id_device()         ioport_get_pin_level(USB_ID_PIN)
#endif

   //! Force device mode
#define otg_force_device_mode()             (Set_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_UIMOD), \
											 Clr_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_UIDE))
   //! Test if device mode is forced
#define Is_otg_device_mode_forced()         (Tst_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_UIMOD))
   //! Force host mode
#define otg_force_host_mode()               (Clr_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_UIMOD), \
											 Clr_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_UIDE))
   //! Test if host mode is forced
#define Is_otg_host_mode_forced()           (!Tst_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_UIMOD))

//! @}

//! @name Host VBus line control
//!
//! The VBus line can be monitored through a PIO pin and
//! a basic resistor voltage divider.
//! This feature is optional, and it is enabled if USB_VBUS_PIN
//! is defined in board.h and CONF_BOARD_USB_VBUS_DETECT defined in
//! conf_board.h.
//!
//! @{
#define OTG_VBUS_DETECT     (defined(CONF_BOARD_USB_VBUS_DETECT))
#define OTG_VBUS_IO         (defined(USB_VBUS_PIN) && OTG_VBUS_DETECT)

//TODO
#if OTG_VBUS_IO
# define pad_vbus_init(level) \
	io_pad_init(USB_VBUS_PIN, USB_VBUS_FLAGS, uhd_vbus_handler, level);
# define pad_vbus_interrupt_disable() gpio_disable_pin_interrupt(USB_VBUS_PIN)
# define pad_ack_vbus_interrupt()     gpio_clear_pin_interrupt_flag(USB_VBUS_PIN)
# define Is_pad_vbus_high()           ioport_get_pin_level(USB_VBUS_PIN)
#endif

//! Disables VBUS hardware control (Must be disabled as no VBUS pin controlled by UHDP)
#define uhd_disable_vbus_hw_control()         (Set_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_VBUSHWC))
//! Requests VBus activation
#define uhd_enable_vbus()                     (UOTGHS->UOTGHS_SFR = UOTGHS_SR_VBUSRQ)
//! Requests VBus deactivation
#define uhd_disable_vbus()                    (UOTGHS->UOTGHS_SCR = UOTGHS_SR_VBUSRQ)
//! Tests if VBus activation has been requested
#define Is_uhd_vbus_enabled()                 (Tst_bits(UOTGHS->UOTGHS_SR, UOTGHS_SR_VBUSRQ))
//! @}

//! @name UHDP OTG main management
//! These macros allows to enable/disable pad and UHDP hardware
//! @{
  //! Reset USB macro
#define otg_reset()                         \
	do {                                    \
		UOTGHS->UOTGHS_CTRL = UOTGHS_CTRL_FRZCLK; \
		UOTGHS->UOTGHS_SCR = 0xFFFFFFFF;    \
	} while (0)

  //! Enable USB macro
#define otg_enable()                        (Set_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_USBE))
  //! Disable USB macro
#define otg_disable()                       (Clr_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_USBE))
#define Is_otg_enabled()                    (Tst_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_USBE))

  //! Enable UHDP pad
#define otg_enable_pad()                    (Set_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_OTGPADE))
  //! Disable UHDP pad
#define otg_disable_pad()                   (Clr_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_OTGPADE))
#define Is_otg_pad_enabled()                (Tst_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_OTGPADE))

  //! Stop (freeze) internal USB clock
#define otg_freeze_clock()                  (Set_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_FRZCLK))
#define otg_unfreeze_clock()                (Clr_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_FRZCLK))
#define Is_otg_clock_frozen()               (Tst_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_FRZCLK))

  //! Configure time-out of specified OTG timer
#define otg_configure_timeout(timer, timeout) (Set_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_UNLOCK),\
		Wr_bitfield(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_TIMPAGE_Msk, timer),\
		Wr_bitfield(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_TIMVALUE_Msk, timeout),\
		Clr_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_UNLOCK))
  //! Get configured time-out of specified OTG timer
#define otg_get_timeout(timer)              (Set_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_UNLOCK),\
		Wr_bitfield(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_TIMPAGE_Msk, timer),\
		Clr_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_UNLOCK),\
		Rd_bitfield(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_TIMVALUE_Msk))


  //! Get the dual-role device state of the internal USB finite state machine of the UOTGHS controller
#define otg_get_fsm_drd_state()             (Rd_bitfield(UOTGHS->UOTGHS_FSM, UOTGHS_FSM_DRDSTATE_Msk))
#define Is_otg_a_suspend()                  (4==otg_get_fsm_drd_state())
#define Is_otg_a_wait_vrise()               (1==otg_get_fsm_drd_state())
//! @}

//! @}

#ifdef __cplusplus
}
#endif

#endif /* UHDP_OTG_H_INCLUDED */

