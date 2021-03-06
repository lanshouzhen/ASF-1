/**
 * \file
 *
 * \brief QTouch component task for the FreeRTOS Web/DSP Demo.
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
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
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "task_demo.h"
#include "arm_math.h"
#include "qt_i2c.h"
#include "conf_qt_i2c.h"

#define QT_SLIDER_RESOLUTION   QT_SLIDER_RESOLUTION_8_BIT
#define QT_SLIDER_MAX         ((0x01 << (8 - QT_SLIDER_RESOLUTION)) - 1)

static void qtouch_task(void *pvParameters);

/** Slider position. */
uint8_t slider_pos = 0;

const portTickType qtouch_delay = 200UL / portTICK_RATE_MS;

/** Storage for QT status */
struct qt_status qtstatus;

/** Storage for QT setup block */
struct qt_setup_block setup_block;

extern uint32_t g_ip_mode;

/**
 * \brief Initialize TWI communication interface.
 */
static void init_interface(void)
{
	/** TWI master initialization options. */
	twi_master_options_t twi_opt;

	memset((void *)&twi_opt, 0, sizeof(twi_master_options_t));

	/** 100KHz for I2C speed */
	twi_opt.speed = 100000;

	/** Initialize the TWI master driver. */
	twi_master_setup(BOARD_QT_TWI_INSTANCE, &twi_opt);
}

/**
 * \brief Set QTouch parameter for the demo.
 *
 * \param setup_block Pointer to setup block buffer.
 */
static void demo_set_qt_param(struct qt_setup_block *psetup_block)
{
	/*
	 * Set parameter for QT slider.
	 */
	psetup_block->slider_num_keys = 7;
	psetup_block->slider_resolution = QT_SLIDER_RESOLUTION;

	/*
	 * Set all GPIO as output with low level.
	 */
	psetup_block->gpio_direction = 0x1C;
	psetup_block->gpio_gpo_drive2 = 0x0;
}

/**
 * \brief Process Qtouch status
 *
 * \param qt_status Pointer to Qtouch status struct variable.
 */
static void demo_process_qt_status(struct qt_status *qt_status)
{
	static uint8_t slide_position = 0;

	if (qt_status->general_status & QT_GENERAL_STATUS_RESET) {
		/** Read setup block */
		qt_read_setup_block(&setup_block);
		/** Modify setup block parameters for specific example */
		demo_set_qt_param(&setup_block);
		/** Write setup block */
		qt_write_setup_block(&setup_block);
	}

	if (qt_status->general_status & QT_GENERAL_STATUS_SDET) {
		/** Save a slider position to be used in the DSP task. */
		slide_position = qt_status->slider_position / SLIDER_SELECTOR_RANGE;
		/** Necessary to avoid sin_buffer overflow, when position equals 0. */
		if (slide_position == 0) {
			slide_position += 1;
		}
		slider_pos = SLIDER_SELECTOR_NB - slide_position;
	}
}

/**
 * \brief Create the QTouch task.
 *
 * \param stack_depth_words Task stack size in 32 bits word.
 * \param task_priority Task priority.
 */
void create_qtouch_task(uint16_t stack_depth_words,
		unsigned portBASE_TYPE task_priority)
{
	enum status_code ret;
	/** Initialize communication interface */
	init_interface();

	/** Reset QT device */
	qt_hardware_reset();

	/** Check communication is ready and able to read Chip ID */
	ret = qt_get_comm_ready();
	if (ret != STATUS_OK) {
		while (1) {
			/** Infinite loop here */
		}
	}

	/** Read setup block */
	qt_read_setup_block(&setup_block);
	/** Modify setup block parameters for specific example */
	demo_set_qt_param(&setup_block);
	/** Write setup block */
	qt_write_setup_block(&setup_block);

	/** Create the task as described above. */
	xTaskCreate(qtouch_task, (const signed char *const) "QTouch",
			stack_depth_words, NULL, task_priority,
			NULL);
}

/**
 * \brief QTouch task core function.
 *
 * \param pvParameters Junk parameter.
 */
static void qtouch_task(void *pvParameters)
{
	/* Just to avoid compiler warnings. */
	UNUSED(pvParameters);

	/** Wait for user to read instructions. */
	WAIT_FOR_TOUCH_EVENT;
	while (g_ip_mode != 3) {
		vTaskDelay(qtouch_delay);
	}

	/** QTouch task Loop. */
	while (1) {
		if (qt_is_change_line_low()) {
			/** Read all status bytes */
			qt_get_status(&qtstatus);

			/** Process the received data */
			demo_process_qt_status(&qtstatus);
		}
		vTaskDelay(qtouch_delay);
	}
}

