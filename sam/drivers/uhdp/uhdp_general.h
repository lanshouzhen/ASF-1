/**
 * \file
 *
 * \brief USB General Driver for UHDP.
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

#ifndef UHDP_GENERAL_H_INCLUDED
#define UHDP_GENERAL_H_INCLUDED

#include "compiler.h"

#ifdef __cplusplus
extern "C" {
#endif

//! \ingroup usb_group
//! \defgroup uhdp_general_group UHDP General Driver
//! UHDP low-level driver for general features
//!
//! @{

//! @name UHDP General ID pin management
//! The ID pin come from the USB OTG connector (A and B receptable) and
//! allows to select the USB mode host or device.
//!
//! UHDP doesn't support management of ID pin.
//! @{
   //! Disable external OTG_ID pin (ignored by USB)
#define uhdp_disable_id_pin()                (Clr_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_UIDE))
   //! Disable external OTG_ID pin and force device mode
#define uhdp_force_device_mode()             (Set_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_UIMOD), uhdp_disable_id_pin())
   //! Disable external OTG_ID pin and force host mode
#define uhdp_force_host_mode()               (Clr_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_UIMOD), uhdp_disable_id_pin())
//! @}

//! @name UHDP General main management
//! These macros allows to enable/disable pad and UHDP hardware
//! @{
  //! Enable USB macro
#define uhdp_enable()                        (Set_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_USBE))
  //! Disable USB macro
#define uhdp_disable()                       (Clr_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_USBE))
#define Is_uhdp_enabled()                    (Tst_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_USBE))

  //! Enable UHDP pad
#define uhdp_enable_pad()                    (Set_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_OTGPADE))
  //! Disable UHDP pad
#define uhdp_disable_pad()                   (Clr_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_OTGPADE))
#define Is_uhdp_pad_enabled()                (Tst_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_OTGPADE))

  //! Stop (freeze) internal USB clock
#define uhdp_freeze_clock()                  (Set_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_FRZCLK))
#define uhdp_unfreeze_clock()                (Clr_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_FRZCLK))
#define Is_uhdp_clock_frozen()               (Tst_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_FRZCLK))
//! @}

//! @}

#ifdef __cplusplus
}
#endif

#endif /* UHDP_GENERAL_H_INCLUDED */

