/**
 * \file
 *
 * \brief Main application of Starter Kit Bootloader Demo.
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

/**
 * \mainpage Main application
 *
 * \section Purpose
 *
 * The main application will help new users get familiar with Atmel's
 * SAM4N family of microcontrollers and the IO1 and OLED1 extension boards
 * with the SAM4N Xplained Pro board.
 *
 * \section Requirements
 *
 * This package can be used with SAM4N Xplained Pro evaluation kits.
 *
 * \section Description
 *
 * The main application is part of the Starter Kit Bootloader Demo.
 * It's loaded by the Bootloader and run after it.
 *
 * The application features the IO1 and OLED1 extension boards with
 * the SAM4N Xplained Pro.
 * The demonstration program can operate in 3 different modes: temperature
 * information, light sensor information and SD card status.
 * The user can switch between the various mode by pressing Button1.
 * When running in mode 3 (SD card content), the user can browse the SD
 * content using Button2 (previous) and Button3 (next). Filenames are directly
 * printed on the OLED screen.
 *
 * If there is other version language programs in the SD card, the user can
 * switch to it by push the SW0 button on the Xplain Pro board. The
 * application will set the trigger flag and write the switch file info to the
 * Flash page used by the Bootloader and jump back to Bootloader.
 *
 * \note
 * Fatfs must be config with _USE_LFN and _LFN_UNICODE enabled.
 * IO1 extension must be connected on EXT1.
 * OLED1 extension must be connected on EXT3.
 *
 */

#include <asf.h>
#include <string.h>
#include "conf_example.h"
#include "multi_language_display.h"

enum language_mode {
	LANGUAGE_ENGLISH = 0,
	LANGUAGE_CHINESE,
	LANGUAGE_FRENCH,
	LANGUAGE_JAPANESE,
	LANGUAGE_SPANISH,
	LANGUAGE_NUMBER
};

#ifdef CONF_ENGLISH_LANGUAGE
volatile uint32_t app_language = LANGUAGE_ENGLISH;
#endif

#ifdef CONF_CHINESE_LANGUAGE
volatile uint32_t app_language = LANGUAGE_CHINESE;
#endif

#ifdef CONF_FRENCH_LANGUAGE
volatile uint32_t app_language = LANGUAGE_FRENCH;
#endif

#ifdef CONF_JAPANESE_LANGUAGE
volatile uint32_t app_language = LANGUAGE_JAPANESE;
#endif

#ifdef CONF_SPANISH_LANGUAGE
volatile uint32_t app_language = LANGUAGE_SPANISH;
#endif

/* Temperature and light sensor data buffer size. */
#define BUFFER_SIZE 128

/* IRQ priority for PIO (The lower the value, the greater the priority) */
#define IRQ_PRIOR_PIO    0

/* Flag to indicate if the application bin exist: 0 for no, 1 for yes */
volatile uint32_t application_bin_flag = 0;

/* Flag to indicate if need reset: 0 for no, 1 for yes */
volatile uint32_t reset_flag = 0;

/* These settings will force to set and refresh the temperature mode. */
volatile uint32_t app_mode = 2;
volatile uint32_t app_mode_switch = 1;

/* SD related variants. */
volatile uint32_t sd_update = 0;
volatile uint32_t sd_fs_found = 0;
volatile uint32_t sd_listing_pos = 0;
volatile uint32_t sd_num_files = 0;

/* Fatfs global variants. */
FATFS fs;
DIR dir;
FIL file_object;
/* Include the path at begin. */
TCHAR file_name_unicode[5][14] = {
	{0x0030, 0x003A, 0x0064, 0x0065, 0x006D, 0x006F, 0x005F,
	 0x0065, 0x006E, 0x002E, 0x0062, 0x0069, 0x006E, 0x0000},
	{0x0030, 0x003A, 0x0064, 0x0065, 0x006D, 0x006F, 0x005F,
	 0x0063, 0x006E, 0x002E, 0x0062, 0x0069, 0x006E, 0x0000},
	{0x0030, 0x003A, 0x0064, 0x0065, 0x006D, 0x006F, 0x005F,
	 0x0066, 0x0072, 0x002E, 0x0062, 0x0069, 0x006E, 0x0000},
	{0x0030, 0x003A, 0x0064, 0x0065, 0x006D, 0x006F, 0x005F,
	 0x006A, 0x0070, 0x002E, 0x0062, 0x0069, 0x006E, 0x0000},
	{0x0030, 0x003A, 0x0064, 0x0065, 0x006D, 0x006F, 0x005F,
	 0x0073, 0x0070, 0x002E, 0x0062, 0x0069, 0x006E, 0x0000}
};
/* Only name. */
uint8_t file_name_ascii[5][12] = {
	{0x64, 0x65, 0x6D, 0x6F, 0x5F, 0x65, 0x6E, 0x2E, 0x62, 0x69, 0x6E, 0x00},
	{0x64, 0x65, 0x6D, 0x6F, 0x5F, 0x63, 0x6E, 0x2E, 0x62, 0x69, 0x6E, 0x00},
	{0x64, 0x65, 0x6D, 0x6F, 0x5F, 0x66, 0x72, 0x2E, 0x62, 0x69, 0x6E, 0x00},
	{0x64, 0x65, 0x6D, 0x6F, 0x5F, 0x6A, 0x70, 0x2E, 0x62, 0x69, 0x6E, 0x00},
	{0x64, 0x65, 0x6D, 0x6F, 0x5F, 0x73, 0x70, 0x2E, 0x62, 0x69, 0x6E, 0x00}
};

volatile uint32_t file_exist_flag = 0;

/** Size of the data to write/read. */
#define DATA_SIZE 512

/* Read/write buffer */
static uint8_t data_buffer[DATA_SIZE];


/**
 * \brief Check the font bin file.
 */
static void application_bin_check(void)
{
	/* Check if the application bin in SD card */
	uint32_t i;
	uint8_t card_check;
	FRESULT res;
	TCHAR path[3];

	/* Init the path to 0: */
	path[0] = 0x0030;
	path[1] = 0x003A;
	path[2] = 0x0000;

	/* Is SD card present? */
	if (gpio_pin_is_low(SD_MMC_0_CD_GPIO) == false) {
		return;
	}

	sd_mmc_init();
	card_check = sd_mmc_check(0);
	while (card_check != SD_MMC_OK) {
		card_check = sd_mmc_check(0);
		delay_ms(1);
	}

	/* Try to mount file system. */
	memset(&fs, 0, sizeof(FATFS));
	res = f_mount(LUN_ID_SD_MMC_0_MEM, &fs);
	if (FR_INVALID_DRIVE == res) {
		return;
	}

	/* Test if the disk is formatted */
	res = f_opendir(&dir, path);
	if (res != FR_OK) {
		return;
	}

	for (i = 0; i < LANGUAGE_NUMBER; i++) {
		/* Open the application bin file. */
		res = f_open(&file_object, file_name_unicode[i],
				(FA_OPEN_EXISTING | FA_READ));
		if (res != FR_OK) {
			continue;
		}

		/* Close the file*/
		res = f_close(&file_object);
		if (res != FR_OK) {
			continue;
		}

		/* Set the exist flag */
		file_exist_flag |= (0x01 << i);
	}

	if ((file_exist_flag & 0x1F) == 0) {
		return;
	}

	/* Show switch info */
	multi_language_show_switch_info();

	/* Wait 6 seconds to show above info. */
	delay_s(6);

	/* Set the flag. */
	application_bin_flag = 1;

}

/**
 * \brief Handler for reset to bootloader.
 */
static void reset_handler(void)
{
	uint32_t ul_last_page_addr = LAST_PAGE_ADDRESS;
	uint32_t *pul_last_page = (uint32_t *)ul_last_page_addr;
	uint32_t ul_rc;
	uint32_t ul_idx;
	uint32_t temp_data;
	uint32_t i;

	for (ul_idx = 0; ul_idx < (IFLASH_PAGE_SIZE / 4); ul_idx++) {
		temp_data = pul_last_page[ul_idx];
		data_buffer[ul_idx * 4] = (uint8_t)(temp_data & 0xFF);
		data_buffer[ul_idx * 4 + 1] = (uint8_t)((temp_data >> 8) & 0xFF);
		data_buffer[ul_idx * 4 + 2] = (uint8_t)((temp_data >> 16) & 0xFF);
		data_buffer[ul_idx * 4 + 3] = (uint8_t)((temp_data >> 24) & 0xFF);
	}

	/* Trigger */
	data_buffer[0x14] = 0x01;
	data_buffer[0x15] = 0x00;
	data_buffer[0x16] = 0x00;
	data_buffer[0x17] = 0x00;

	for (i = 0; i < LANGUAGE_NUMBER; i++) {
		app_language++;
		app_language = app_language % LANGUAGE_NUMBER;
		/* Check the exist flag */
		if (file_exist_flag & (0x01 << app_language)) {
			data_buffer[0x18] = file_name_ascii[app_language][0];
			data_buffer[0x19] = file_name_ascii[app_language][1];
			data_buffer[0x1A] = file_name_ascii[app_language][2];
			data_buffer[0x1B] = file_name_ascii[app_language][3];
			data_buffer[0x1C] = file_name_ascii[app_language][4];
			data_buffer[0x1D] = file_name_ascii[app_language][5];
			data_buffer[0x1E] = file_name_ascii[app_language][6];
			data_buffer[0x1F] = file_name_ascii[app_language][7];
			data_buffer[0x20] = file_name_ascii[app_language][8];
			data_buffer[0x21] = file_name_ascii[app_language][9];
			data_buffer[0x22] = file_name_ascii[app_language][10];
			data_buffer[0x23] = file_name_ascii[app_language][11];
			break;
		}
	}

	ul_rc = flash_unlock(ul_last_page_addr,
			ul_last_page_addr + IFLASH_PAGE_SIZE, NULL, NULL);

	ul_rc = flash_erase_page(ul_last_page_addr, IFLASH_ERASE_PAGES_8);
	if (ul_rc != FLASH_RC_OK) {
		return;
	}

	ul_rc = flash_write(ul_last_page_addr, data_buffer,
			IFLASH_PAGE_SIZE, 0);
	if (ul_rc != FLASH_RC_OK) {
		return;
	}

	/* Clear screen. */
	ssd1306_clear();

	/* End message on the screen. */
	multi_language_show_end_info();

	/* Perform the software reset. */
	rstc_start_software_reset(RSTC);

	/* Wait for reset. */
	delay_ms(100);
}

/**
 * \brief Handler for SW0 rising edge interrupt.
 * \param id The button ID.
 * \param mask The button mask.
 */
static void sw0_handler(uint32_t id, uint32_t mask)
{
	if ((PIN_SW0_ID == id) && (PIN_SW0_MASK == mask) &&
			(application_bin_flag == 1)) {
		reset_flag = 1;
	}
}

/**
 * \brief Process Buttons Events.
 *
 * \param uc_button The button number.
 */
static void process_button_event(uint8_t uc_button)
{
	/* Switch between temperature, light and SD mode. */
	if (uc_button == 1) {
		app_mode_switch = 1;
		pio_disable_interrupt(OLED1_PIN_PUSHBUTTON_1_PIO,
				OLED1_PIN_PUSHBUTTON_1_MASK);
	} else if ((uc_button == 2) && (app_mode == 2) &&
			(sd_fs_found == 1) && (sd_update == 0)) {
		/* Page UP button in SD mode. */
		if (sd_listing_pos > 0) {
			sd_listing_pos -= 1;
			sd_update = 1;
			pio_disable_interrupt(OLED1_PIN_PUSHBUTTON_2_PIO,
					OLED1_PIN_PUSHBUTTON_2_MASK);
		}
	} else if ((uc_button == 3) && (app_mode == 2) &&
			(sd_fs_found == 1) && (sd_update == 0)) {
		/* Page DOWN button in SD mode. */
		/* Lock DOWN button when showing the last file. */
		if (sd_listing_pos < sd_num_files) {
			sd_listing_pos += 1;
			sd_update = 1;
			pio_disable_interrupt(OLED1_PIN_PUSHBUTTON_3_PIO,
					OLED1_PIN_PUSHBUTTON_3_MASK);
		}
	}
}

/**
 * \brief Handler for Button 1 rising edge interrupt.
 * \param id The button ID.
 * \param mask The button mask.
 */
static void button1_handler(uint32_t id, uint32_t mask)
{
	if ((OLED1_PIN_PUSHBUTTON_1_ID == id) &&
			(OLED1_PIN_PUSHBUTTON_1_MASK == mask)) {
		process_button_event(1);
	}
}

/**
 * \brief Handler for Button 2 rising edge interrupt.
 * \param id The button ID.
 * \param mask The button mask.
 */
static void button2_handler(uint32_t id, uint32_t mask)
{
	if ((OLED1_PIN_PUSHBUTTON_2_ID == id) &&
			(OLED1_PIN_PUSHBUTTON_2_MASK == mask)) {
		process_button_event(2);
	}
}

/**
 * \brief Handler for Button 3 rising edge interrupt.
 * \param id The button ID.
 * \param mask The button mask.
 */
static void button3_handler(uint32_t id, uint32_t mask)
{
	if ((OLED1_PIN_PUSHBUTTON_3_ID == id) &&
			(OLED1_PIN_PUSHBUTTON_3_MASK == mask)) {
		process_button_event(3);
	}
}

/**
 * \brief Handler for SD card detect rising edge interrupt.
 * \param id The button ID.
 * \param mask The button mask.
 */
static void sd_detect_handler(uint32_t id, uint32_t mask)
{
	if ((SD_MMC_0_CD_ID == id) && (SD_MMC_0_CD_MASK == mask)) {
		sd_listing_pos = 0;
		sd_num_files = 0;
		sd_fs_found = 0;
		sd_update = 1;
	}
}

/**
 * \brief Configure the Pushbuttons.
 *
 * Configure the PIO as inputs and generate corresponding interrupt when
 * pressed or released.
 */
static void configure_buttons(void)
{
	/* Configure SW0. */
	pmc_enable_periph_clk(PIN_SW0_ID);
	pio_set_debounce_filter(PIN_SW0_PIO, PIN_SW0_MASK, 10);
	pio_handler_set(PIN_SW0_PIO, PIN_SW0_ID,
			PIN_SW0_MASK, PIN_SW0_ATTR, sw0_handler);
	NVIC_EnableIRQ((IRQn_Type)PIN_SW0_ID);
	pio_handler_set_priority(PIN_SW0_PIO, (IRQn_Type)PIN_SW0_ID,
			IRQ_PRIOR_PIO);
	pio_enable_interrupt(PIN_SW0_PIO, PIN_SW0_MASK);

	/* Configure Pushbutton 1. */
	pmc_enable_periph_clk(OLED1_PIN_PUSHBUTTON_1_ID);
	pio_set_debounce_filter(OLED1_PIN_PUSHBUTTON_1_PIO,
			OLED1_PIN_PUSHBUTTON_1_MASK, 10);
	pio_handler_set(OLED1_PIN_PUSHBUTTON_1_PIO, OLED1_PIN_PUSHBUTTON_1_ID,
			OLED1_PIN_PUSHBUTTON_1_MASK,
			OLED1_PIN_PUSHBUTTON_1_ATTR,
			button1_handler);
	NVIC_EnableIRQ((IRQn_Type)OLED1_PIN_PUSHBUTTON_1_ID);
	pio_handler_set_priority(OLED1_PIN_PUSHBUTTON_1_PIO,
			(IRQn_Type)OLED1_PIN_PUSHBUTTON_1_ID, IRQ_PRIOR_PIO);
	pio_enable_interrupt(OLED1_PIN_PUSHBUTTON_1_PIO,
			OLED1_PIN_PUSHBUTTON_1_MASK);

	/* Configure Pushbutton 2. */
	pmc_enable_periph_clk(OLED1_PIN_PUSHBUTTON_2_ID);
	pio_set_debounce_filter(OLED1_PIN_PUSHBUTTON_2_PIO,
			OLED1_PIN_PUSHBUTTON_2_MASK, 10);
	pio_handler_set(OLED1_PIN_PUSHBUTTON_2_PIO, OLED1_PIN_PUSHBUTTON_2_ID,
			OLED1_PIN_PUSHBUTTON_2_MASK,
			OLED1_PIN_PUSHBUTTON_2_ATTR,
			button2_handler);
	NVIC_EnableIRQ((IRQn_Type)OLED1_PIN_PUSHBUTTON_2_ID);
	pio_handler_set_priority(OLED1_PIN_PUSHBUTTON_2_PIO,
			(IRQn_Type)OLED1_PIN_PUSHBUTTON_2_ID, IRQ_PRIOR_PIO);
	pio_enable_interrupt(OLED1_PIN_PUSHBUTTON_2_PIO,
			OLED1_PIN_PUSHBUTTON_2_MASK);

	/* Configure Pushbutton 3. */
	pmc_enable_periph_clk(OLED1_PIN_PUSHBUTTON_3_ID);
	pio_set_debounce_filter(OLED1_PIN_PUSHBUTTON_3_PIO,
			OLED1_PIN_PUSHBUTTON_3_MASK, 10);
	pio_handler_set(OLED1_PIN_PUSHBUTTON_3_PIO, OLED1_PIN_PUSHBUTTON_3_ID,
			OLED1_PIN_PUSHBUTTON_3_MASK,
			OLED1_PIN_PUSHBUTTON_3_ATTR,
			button3_handler);
	NVIC_EnableIRQ((IRQn_Type)OLED1_PIN_PUSHBUTTON_3_ID);
	pio_handler_set_priority(OLED1_PIN_PUSHBUTTON_3_PIO,
			(IRQn_Type)OLED1_PIN_PUSHBUTTON_3_ID, IRQ_PRIOR_PIO);
	pio_enable_interrupt(OLED1_PIN_PUSHBUTTON_3_PIO,
			OLED1_PIN_PUSHBUTTON_3_MASK);

	/* Configure SD card detection. */
	pmc_enable_periph_clk(SD_MMC_0_CD_ID);
	pio_set_debounce_filter(SD_MMC_0_CD_PIO, SD_MMC_0_CD_MASK, 10);
	pio_handler_set(SD_MMC_0_CD_PIO, SD_MMC_0_CD_ID, SD_MMC_0_CD_MASK,
			SD_MMC_0_CD_ATTR, sd_detect_handler);
	NVIC_EnableIRQ((IRQn_Type)SD_MMC_0_CD_ID);
	pio_handler_set_priority(SD_MMC_0_CD_PIO, (IRQn_Type)SD_MMC_0_CD_ID,
			IRQ_PRIOR_PIO);
	pio_enable_interrupt(SD_MMC_0_CD_PIO, SD_MMC_0_CD_MASK);
}

/**
 * \brief Configure the ADC for the light sensor.
 */
static void configure_adc(void)
{
	struct adc_config adc_cfg;

	/* Configure ADC pin for light sensor. */
	gpio_configure_pin(LIGHT_SENSOR_GPIO, LIGHT_SENSOR_FLAGS);

	/* Enable ADC clock. */
	pmc_enable_periph_clk(ID_ADC);

	/* Configure ADC. */
	adc_enable();
	adc_get_config_defaults(&adc_cfg);
	adc_init(ADC, &adc_cfg);
	adc_channel_enable(ADC, ADC_CHANNEL_0);
	adc_set_trigger(ADC, ADC_TRIG_SW);
}

/**
 * \brief Get the number of files at the root of the SD card.
 * Result is stored in global sd_num_files.
 */
static void get_num_files_on_sd(void)
{
	FRESULT res;
	FILINFO fno;
	char *pc_fn;
	TCHAR path[3];

	path[0] = 0x0030;
	path[1] = 0x003A;
	path[2] = 0x0000;

	TCHAR c_lfn[_MAX_LFN + 1];
	fno.lfname = c_lfn;
	fno.lfsize = sizeof(c_lfn);

	sd_num_files = 0;

	/* Open the directory */
	res = f_opendir(&dir, path);
	if (res == FR_OK) {
		for (;;) {
			res = f_readdir(&dir, &fno);
			if (res != FR_OK || fno.fname[0] == 0) {
				break;
			}

			pc_fn = (char *)(*fno.lfname ? fno.lfname : fno.fname);
			if (*pc_fn == '.') {
				continue;
			}

			sd_num_files += 1;
		}
	}
}

/**
 * \brief Show SD card status on the OLED screen.
 */
static void display_sd_info(void)
{
	FRESULT res;
	uint8_t card_check;
	uint8_t sd_card_type;
	uint32_t sd_card_size;
	char size[64];

	/* Is SD card present? */
	if (gpio_pin_is_low(SD_MMC_0_CD_GPIO) == false) {
		multi_language_show_no_sd_info();
	} else {
		multi_language_show_sd_info();

		sd_mmc_init();
		card_check = sd_mmc_check(0);
		while (card_check != SD_MMC_OK) {
			card_check = sd_mmc_check(0);
			delay_ms(1);
		}

		if (card_check == SD_MMC_OK) {
			sd_card_type = sd_mmc_get_type(0);
			sd_card_size = sd_mmc_get_capacity(0);

			/* Card type */
			switch (sd_card_type) {
			case CARD_TYPE_SD:
				multi_language_show_normal_card_info();
				break;

			case CARD_TYPE_SDIO:
				break;

			case CARD_TYPE_HC:
				multi_language_show_high_capacity_card_info();
				break;

			case CARD_TYPE_SD_COMBO:
				break;

			default:
				multi_language_show_unknow_card_info();
			}

			multi_language_show_card_size_info(size, sd_card_size);

			/* Try to mount file system. */
			memset(&fs, 0, sizeof(FATFS));
			res = f_mount(LUN_ID_SD_MMC_0_MEM, &fs);
			if (FR_INVALID_DRIVE == res) {
				multi_language_show_no_fatfs_info();
				sd_fs_found = 0;
			} else {
				get_num_files_on_sd();
				if (sd_num_files == 0) {
					multi_language_show_no_files_info();
					sd_fs_found = 1;
				} else {
					multi_language_show_browse_info();
					sd_fs_found = 1;
				}
			}
		}
	}
}

/**
 * \brief Show SD card content on the OLED screen.
 * \note Does not browse sub folders.
 */
static void display_sd_files_unicode(void)
{
	FRESULT res;
	FILINFO fno;
	uint32_t line;
	uint32_t pos;
	char *pc_fn;
	TCHAR path[3];

	path[0] = 0x0030;
	path[1] = 0x003A;
	path[2] = 0x0000;

#if _USE_LFN
	TCHAR c_lfn[_MAX_LFN + 1];
	fno.lfname = c_lfn;
	fno.lfsize = sizeof(c_lfn);
#endif

#ifndef CONF_ENGLISH_LANGUAGE
	line = 1;
#else
	line = 0;
#endif
	pos = 1;

	/* Open the directory */
	res = f_opendir(&dir, path);
	if (res == FR_OK) {
		for (;;) {
			res = f_readdir(&dir, &fno);
			if (res != FR_OK || fno.fname[0] == 0) {
				break;
			}

#if _USE_LFN
			pc_fn = (char *)(*fno.lfname ? fno.lfname : fno.fname);
#else
			pc_fn = (char *)(fno.fname);
#endif
			if (*pc_fn == '.') {
				continue;
			}

			if ((pos >= sd_listing_pos) && (line < 4)) {
				multi_language_show_file_name(line, pc_fn);
				line++;
#ifndef CONF_ENGLISH_LANGUAGE
				line++;
#endif
			}

			pos += 1;
		}
	}
}

/**
 * \brief Draw graph on the OLED screen using the provided point array.
 * \param col X coordinate.
 * \param page Y coordinate (please refer to OLED datasheet for page
 * description).
 * \param width Graph width(columns).
 * \param height Graph height(pages, 1~3).
 * \param tab Data to draw. Must contain width elements.
 */
static void ssd1306_draw_graph(uint8_t col, uint8_t page, uint8_t width,
		uint8_t height, uint8_t *tab)
{
	uint8_t i, j;
	uint8_t page_start, scale, bit_length, page_data[3];
	uint32_t bit_data;

	for (i = col; i < width; ++i) {
		scale = 8 * height;
		bit_length = tab[i] * scale / 24;
		for (bit_data = 0; bit_length > 0; --bit_length) {
			bit_data = (bit_data << 1) + 1;
		}
		page_data[0] = bit_reverse8(bit_data & 0xFF);
		page_data[1] = bit_reverse8((bit_data >> 8) & 0xFF);
		page_data[2] = bit_reverse8((bit_data >> 16) & 0xFF);
		j = height - 1;
		for (page_start = page; page_start < (page + height); ++page_start) {
			ssd1306_write_command(SSD1306_CMD_SET_PAGE_START_ADDRESS(
					page_start));
			ssd1306_set_column_address(i);
			ssd1306_write_data(page_data[j]);
			--j;
		}
	}
}

/**
 * \brief Clear one character at the cursor current position on the OLED
 * screen.
 */
static void ssd1306_clear_char(void)
{
	ssd1306_write_data(0x00);
	ssd1306_write_data(0x00);
	ssd1306_write_data(0x00);
	ssd1306_write_data(0x00);
	ssd1306_write_data(0x00);
	ssd1306_write_data(0x00);
}

/**
 * \brief The main application.
 */
int main(void)
{
	uint8_t i;
	uint8_t temperature[BUFFER_SIZE];
	uint8_t light[BUFFER_SIZE];
	char value_disp[5];
	uint32_t adc_value;
	uint32_t light_value;
	double temp;

	/* Initialize clocks. */
	sysclk_init();

	/* Initialize GPIO states. */
	board_init();

	/* Configure ADC for light sensor. */
	configure_adc();

	/* Initialize at30tse. */
	at30tse_init();

	/* Configure IO1 buttons. */
	configure_buttons();

	/* Initialize SPI and SSD1306 controller. */
	ssd1306_init();
	ssd1306_clear();

	/* Clear internal buffers. */
	for (i = 0; i < BUFFER_SIZE; ++i) {
		temperature[i] = 0;
		light[i] = 0;
	}

	/* Show the start info. */
	multi_language_show_start_info();

	/* Wait 3 seconds to show the above message. */
	delay_s(3);

	/* Check application bin in SD card. */
	application_bin_check();

	while (true) {
		/* Refresh page title only if necessary. */
		if (reset_flag) {
			reset_handler();
		}

		/* Refresh page title only if necessary. */
		if (app_mode_switch > 0) {
			app_mode = (app_mode + 1) % 3;

			/* Clear screen. */
			ssd1306_clear();
			ssd1306_set_page_address(0);
			ssd1306_set_column_address(0);

			if (app_mode == 0) {
				/* Temperature mode. */
				ioport_set_pin_level(OLED1_LED1_PIN, OLED1_LED1_ACTIVE);
				ioport_set_pin_level(OLED1_LED2_PIN, !OLED1_LED2_ACTIVE);
				ioport_set_pin_level(OLED1_LED3_PIN, !OLED1_LED3_ACTIVE);
				multi_language_show_temperature_info();
			} else if (app_mode == 1) {
				/* Light mode. */
				ioport_set_pin_level(OLED1_LED2_PIN, OLED1_LED2_ACTIVE);
				ioport_set_pin_level(OLED1_LED1_PIN, !OLED1_LED1_ACTIVE);
				ioport_set_pin_level(OLED1_LED3_PIN, !OLED1_LED3_ACTIVE);
				multi_language_show_light_info();
			} else {
				/* SD mode. */
				ioport_set_pin_level(OLED1_LED3_PIN, OLED1_LED3_ACTIVE);
				ioport_set_pin_level(OLED1_LED1_PIN, !OLED1_LED1_ACTIVE);
				ioport_set_pin_level(OLED1_LED2_PIN, !OLED1_LED2_ACTIVE);

				sd_listing_pos = 0;
				/* Show SD card info. */
				display_sd_info();
			}

			app_mode_switch = 0;
		}

		/* Shift graph buffers. */
		for (i = 0; i < (BUFFER_SIZE - 1); ++i) {
			temperature[i] = temperature[i + 1];
			light[i] = light[i + 1];
		}

		/* Get temperature. */
		if (at30tse_read_temperature(&temp) == TWI_SUCCESS) {
			/* Don't care about negative temperature. */
			if (temp < 0) {
				temp = 0;
			}

			/* Update temperature for display. */
			/* Note: rescale to 0~24 for better rendering. */
			if (temp > 40) {
				temperature[BUFFER_SIZE - 1] = 24;
			} else {
				temperature[BUFFER_SIZE - 1] = (uint8_t)temp * 24 / 40;
			}
		} else {
			/* Error print zero values. */
			temperature[BUFFER_SIZE - 1] = 0;
		}

		/* Get light sensor information. */
		/* Rescale to 0~24 for better rendering. */
		adc_start_software_conversion(ADC);
		adc_value = adc_channel_get_value(ADC, ADC_CHANNEL_0);
		light[BUFFER_SIZE - 1] = 24 - adc_value * 24 / 1024;

		if (app_mode == 0) {
			/* Print temperature in text format. */
			sprintf(value_disp, "%d", (uint8_t)temp);
			ssd1306_set_column_address(98);
			ssd1306_write_command(SSD1306_CMD_SET_PAGE_START_ADDRESS(0));
			ssd1306_write_text(" ");
			/* Avoid character overlapping. */
			if (temp < 10) {
				ssd1306_clear_char();
			}

			ssd1306_write_text(value_disp);
			/* Display degree symbol. */
			ssd1306_write_data(0x06);
			ssd1306_write_data(0x06);
			ssd1306_write_text("c");

			/* Refresh graph. */
			ssd1306_draw_graph(0, 2, BUFFER_SIZE, 2, temperature);
		} else if (app_mode == 1) {
			light_value = 100 - (adc_value * 100 / 1024);
			sprintf(value_disp, "%lu", light_value);
			ssd1306_set_column_address(98);
			ssd1306_write_command(SSD1306_CMD_SET_PAGE_START_ADDRESS(0));
			ssd1306_write_text(" ");
			/* Avoid character overlapping. */
			if (light_value < 10) {
				ssd1306_clear_char();
			}

			ssd1306_write_text(value_disp);
			ssd1306_write_text("%");
			/* Avoid character overlapping. */
			if (light_value < 100) {
				ssd1306_clear_char();
			}

			/* Refresh graph. */
			ssd1306_draw_graph(0, 2, BUFFER_SIZE, 2, light);
		} else {
			/**
			 * Refresh screen if card was inserted/removed or
			 * browsing content.
			 */
			if (sd_update == 1) {
				/* Clear screen. */
				ssd1306_clear();
				ssd1306_set_page_address(0);
				ssd1306_set_column_address(0);

				if (sd_listing_pos == 0) {
					/* Show SD card info. */
					display_sd_info();
				} else {
					/* List SD card files. */
					display_sd_files_unicode();
				}

				sd_update = 0;
			}
		}

		/* Wait and stop screen flickers. */
		delay_ms(150);

		if (app_mode_switch == 0) {
			pio_enable_interrupt(OLED1_PIN_PUSHBUTTON_1_PIO,
					OLED1_PIN_PUSHBUTTON_1_MASK);
		}
		if (sd_update == 0) {
			pio_enable_interrupt(OLED1_PIN_PUSHBUTTON_2_PIO,
					OLED1_PIN_PUSHBUTTON_2_MASK);
			pio_enable_interrupt(OLED1_PIN_PUSHBUTTON_3_PIO,
					OLED1_PIN_PUSHBUTTON_3_MASK);
		}

	}
}
