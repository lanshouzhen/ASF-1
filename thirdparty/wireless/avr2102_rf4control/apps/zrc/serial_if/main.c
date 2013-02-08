/**
 * @file main.c
 *
 * @brief RF4CE Serial Interface Application
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
 *
 */
/*
 * Copyright (c) 2013, Atmel Corporation All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/* === Includes ============================================================ */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <asf.h>
#include "conf_board.h"
#include "common_sw_timer.h"
#include "serial_interface.h"
#include "app_config.h"
#include "pb_pairing.h"


/* === Macros ============================================================== */

/* === Globals ============================================================= */

/* Write application specific values into flash memory */
FLASH_DECLARE(uint16_t VendorIdentifier) = (uint16_t)NWKC_VENDOR_IDENTIFIER;
FLASH_DECLARE(uint8_t vendor_string[7]) = NWKC_VENDOR_STRING;
FLASH_DECLARE(uint8_t app_user_string[15]) = APP_USER_STRING;
#ifdef ZRC_CMD_DISCOVERY
FLASH_DECLARE(uint8_t supported_cec_cmds[32]) = SUPPORTED_CEC_CMDS;
#endif

/* === EXTERNALS =========================================================== */

extern void stack_indication_callback_init(void);



/* === Prototypes ========================================================== */
static void app_alert(void);
/* === Implementation ====================================================== */

/**
 * Main function, initialization and main message loop
 *
 * @return error code
 */
int main (void)
{
    irq_initialize_vectors();

	/* Initialize the board.
	 * The board-specific conf_board.h file contains the configuration of
	 * the board initialization.
	 */
	board_init();
	sysclk_init();

	sw_timer_init();
        
    if (nwk_init() != NWK_SUCCESS)
    {
        app_alert();
    }


    stack_indication_callback_init();


#ifdef FLASH_NVRAM
    pal_ps_set(EE_IEEE_ADDR, 8, &tal_pib.IeeeAddress);
#endif
    /* Initialize LEDs */
    //pal_led_init();
    cpu_irq_enable();

    /*
     * The global interrupt has to be enabled here as TAL uses the timer
     * delay which in turn requires interrupt to be enabled
     */
    //pal_global_irq_enable();

    serial_interface_init();

    /* Loop forever, the interrupts are doing the rest */
    while (1)
    {
        nwk_task();
        serial_data_handler();
    }
    /* No return statement here, because this code is unreachable */
}

#ifdef PBP_REC
bool pbp_allow_pairing(nwk_enum_t Status, uint64_t SrcIEEEAddr, uint16_t OrgVendorId,
                       uint8_t OrgVendorString[7], uint8_t OrgUserString[15],
                       uint8_t KeyExTransferCount)
{
    /* Keep compiler happy */
    Status = Status;
    SrcIEEEAddr = SrcIEEEAddr;
    OrgVendorId = OrgVendorId;
    OrgVendorString[0] = OrgVendorString[0];
    OrgUserString[0] = OrgUserString[0];
    KeyExTransferCount = KeyExTransferCount;

    return true;
}
#endif
static void app_alert(void)
{
    while (1)
    {
     
		#if LED_COUNT > 0
		LED_Toggle(LED0);
		#endif

		#if LED_COUNT > 1
		LED_Toggle(LED1);
		#endif

		#if LED_COUNT > 2
		LED_Toggle(LED2);
		#endif

		#if LED_COUNT > 3
		LED_Toggle(LED3);
		#endif

		#if LED_COUNT > 4
		LED_Toggle(LED4);
		#endif

		#if LED_COUNT > 5
		LED_Toggle(LED5);
		#endif

		#if LED_COUNT > 6
		LED_Toggle(LED6);
		#endif

		#if LED_COUNT > 7
		LED_Toggle(LED7);
		#endif
		delay_us(0xFFFF);
	}
}
/* EOF */
