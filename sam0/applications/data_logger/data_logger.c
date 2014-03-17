/**
 * \file
 *
 * \brief SAM D21 Data Logger Application
 *
 * Copyright (C) 2014 Atmel Corporation. All rights reserved.
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

#include "data_logger.h"

/** Structure for managing Serial Flash */
struct sf_status {
	/** Indicates whether something is already written */
	bool sf_is_clean;
	/** Address starting from which the last write is performed */
	uint32_t start_addr_last_written;
};

/** The descriptors should be quadword-aligned */
COMPILER_ALIGNED(16) DmacDescriptor uart_tx_dma_descriptor;
COMPILER_ALIGNED(16) DmacDescriptor adc_to_sram_dma_descriptor;

/** SPI module instance for the AT25DFX Serial Flash */
static at25dfx_spi_module_t at25dfx_spi;
/** Serial Flash chip driver instance */
static struct at25dfx_chip_module at25dfx_chip;
/** ADC instance */
struct adc_module adc_instance;
/** DMA resource for ADC to SRAM transfer */
struct dma_resource dma_resource_adc_to_sram;
/** RTC instance */
struct rtc_module rtc_inst;
/** USART instance */
struct usart_module usart_instance;
/** DMA resource for USART transmit */
struct dma_resource dma_resource_usart_tx;
/** Structure instance for serial flash management */
struct sf_status sf_access_status;

/**
 * Flag to check successful initialization. Used for deubg purpose
 */
volatile bool init_success;

/**
 * Indicates whether the number of bytes defined by SF_TRANSFER_SIZE is
 * transferred to SRAM
 */
volatile bool adc_sram_dma_transfer_done;
/** This flag will be made 'true' in USART receive interrupt */
volatile bool sf_read_request_received;

/**
 * Flag which indicates whether the request received is to read all logs
 * from the serial flash
 */
volatile bool read_entire_logs;
/** This flag is used to check if a DMA transfer to USART is in progress */
volatile bool usart_dma_transfer_triggered = false;

/**
 * The number of times the RTC is overflown
 * Used to keep track of the number of ADC to SRAM transfers
 */
static uint8_t rtc_ticks;
/** The DMAC will transfer ADC results into this array */
static uint16_t adc_results_array[NUM_OF_BEATS_IN_ADC_SRAM_TRANSFER];

/**
 * Holds the count of readings stored into the Serial Flash
 */
uint32_t sf_write_count;
/** Array used for transferring the logs to USART */
char serial_transfer_array[SF_TRANSFER_SIZE * 2][10];
/** Array used for receiving commands via USART */
volatile uint8_t rx_buffer[CMD_RX_BUFFER_LENGTH];
/** Storage for  message that will be send via USART */
static char transfer_message[INFO_BUFFER_SIZE] = {"Module ready \r\n"};

/**
 * \brief Callback function for RTC overflow
 *
 * This function is invoked from the RTC_Handler.
 */
void rtc_overflow_callback(void)
{
	port_pin_toggle_output_level(LED_0_PIN);

	/*
	 * If the specified number of samples are copied into the given array,
	 * modify the flag to indicate that the values can be written to the
	 * non-volatile media.
	 */
	if (rtc_ticks > SF_TRANSFER_SIZE) {
		rtc_ticks = 0;
		adc_sram_dma_transfer_done = true;
	}

	rtc_ticks++;
}

/**
 * \brief Callback function for USART receive
 *
 * This function is invoked from the USART interrupt handler.
 *
 * \param usart_module Pointer to the USART module software instance
 */
void usart_read_callback(const struct usart_module *const usart_module)
{
	sf_read_request_received = true;

	if (rx_buffer[0] == '1') {
		read_entire_logs = true;
	}
}

/**
 * \brief Configures RTC callbacks
 *
 * This function configures RTC callbacks. There can be more than one
 * callback registered for the RTC. More details are in available in the
 * driver documentation for RTC.
 */
void configure_rtc_callbacks(void)
{
	rtc_count_register_callback(
		&rtc_inst, rtc_overflow_callback, RTC_COUNT_CALLBACK_OVERFLOW);
	rtc_count_enable_callback(&rtc_inst, RTC_COUNT_CALLBACK_OVERFLOW);
}

/**
 * \brief Invoked when DMA transfer from SRAM to USART is done
 *
 * This function is invoked from the DMAC handler once the SRAM to USART
 * transfer is done.
 *
 * \param resource DMA resource which handles SRAM to USART transfer
 */
void transfer_done_usart_tx_dma( const struct dma_resource *const resource )
{
	/*
	 * Update the flag to indicate that there is no SRAM to USART
	 * transfer in progress
	 */
	usart_dma_transfer_triggered = false;
}

/**
 * \brief Configures the ADC
 *
 * This function configures the ADC to get periodic readings from
 * the thermistor connected to PA04 on the SAM D21 Xplained Pro.
 */
void configure_adc(void)
{
	struct adc_config config_adc;

	/*
	 * Get the default values and modify the ADC configuration
	 * The main configuration changes are
	 * - It uses VCC/1.48 internal reference
	 * - The ADC resolution is made 16 bits by using the macros
	 *      ADC_RESOLUTION_CUSTOM and ADC_ACCUMULATE_SAMPLES_256
	 */
	adc_get_config_defaults(&config_adc);

	config_adc.gain_factor        = ADC_INPUTCTRL_GAIN_1X;
	config_adc.clock_prescaler    = ADC_CLOCK_PRESCALER_DIV16;
	config_adc.reference          = ADC_REFERENCE_INTVCC0;
	config_adc.positive_input     = ADC_POSITIVE_INPUT_PIN4;
	config_adc.resolution         = ADC_RESOLUTION_CUSTOM;
	config_adc.freerunning        = false;
	config_adc.left_adjust        = false;
	config_adc.accumulate_samples = ADC_ACCUMULATE_SAMPLES_256;
	config_adc.divide_result      = ADC_DIVIDE_RESULT_DISABLE;
	config_adc.event_action       = ADC_EVENT_ACTION_START_CONV;

	adc_init(&adc_instance, ADC, &config_adc);
	adc_enable(&adc_instance);
}

/**
 * \brief Configures the given DMA resource for ADC to SRAM transfer
 *
 * This function configures the DMA resource for ADC to SRAM transfer.
 * A beat transfer is performed on ADC conversion complete event.
 *
 * \param resource Pointer to the DMA resource
 */
void configure_dma_resource_adc(struct dma_resource *resource)
{
	struct dma_resource_config config;

	dma_get_config_defaults(&config);

	config.transfer_trigger = DMA_TRIGGER_PERIPHERAL;
	config.peripheral_trigger = ADC_DMAC_ID_RESRDY;
	config.trigger_action = DMA_TRIGGER_ACTON_BEAT;

	dma_allocate(resource, &config);
}

/**
 * \brief Configures the transfer descriptor for ADC to SRAM transfer
 *
 * This function configures transfer descriptor for ADC to SRAM transfer.
 * A beat transfer is performed on ADC conversion complete event.
 *
 * \param descriptor Pointer to the DMA descriptor
 */
void setup_transfer_descriptor_adc(DmacDescriptor *descriptor)
{
	struct dma_descriptor_config descriptor_config;

	dma_descriptor_get_config_defaults(&descriptor_config);

	descriptor_config.beat_size = DMA_BEAT_SIZE_HWORD;
	descriptor_config.dst_increment_enable = true;
	descriptor_config.src_increment_enable = false;
	descriptor_config.block_transfer_count
		= NUM_OF_BEATS_IN_ADC_SRAM_TRANSFER;
	descriptor_config.source_address
		= (uint32_t)(&adc_instance.hw->RESULT.reg);
	descriptor_config.destination_address
		= ((uint32_t)adc_results_array +
	(NUM_OF_BEATS_IN_ADC_SRAM_TRANSFER << DMA_BEAT_SIZE_HWORD));
	/*Configure it for linked transfer */
	descriptor_config.next_descriptor_address = (uint32_t)descriptor;

	dma_descriptor_create(descriptor, &descriptor_config);
}

/**
 * \brief Configure and initialize the RTC
 *
 * The RTC will be configured to operate at 1Hz.
 */
void init_rtc(void)
{
	enum status_code status;
	struct rtc_count_config config_rtc_count;

	init_success = true;
	/* Initialize the RTC module */
	rtc_count_get_config_defaults(&config_rtc_count);
	config_rtc_count.prescaler           = TIMER_PRESCALER;
	config_rtc_count.mode                = RTC_COUNT_MODE_16BIT;
	config_rtc_count.continuously_update = true;

	status = rtc_count_init(&rtc_inst, RTC, &config_rtc_count);

	if (status != STATUS_OK) {
		init_success = false;
	}
}

/**
 * \brief Configures the event system and RTC
 *
 * Configures the event system and RTC to trigger ADC conversions
 * on RTC overflow.
 */
void configure_transfer_event(void)
{
	init_success = true;
	struct rtc_count_events rtc_event_config;
	struct events_resource event_res;
	struct events_config config;

	/* Configure and enable RTC events */
	rtc_event_config.generate_event_on_overflow = true;
	rtc_count_enable_events(&rtc_inst, &rtc_event_config);

	events_get_config_defaults(&config);

	/* RTC overflow is the event generator */
	config.generator      = LOGGER_EVENT_GENERATOR;
	config.edge_detect    = EVENTS_EDGE_DETECT_NONE;
	config.path           = EVENTS_PATH_ASYNCHRONOUS;

	events_allocate(&event_res, &config);
	/* Attach the ADC start conversion as event user */
	events_attach_user(&event_res, LOGGER_EVENT_USER);
}

/**
 * \brief Enables the timer which triggers ADC conversion
 *
 * Set the period and enable the RTC to trigger ADC conversion
 * periodically..
 */
void enable_transfer_trigger(void)
{
	rtc_count_set_period(&rtc_inst, 1);
	rtc_count_enable(&rtc_inst);
}

/**
 * \brief Configures the given DMA resource for serial transfer
 *
 * Configures the DMA resource to use transmission via SERCOM3 as the
 * trigger. A beat transfer is performed on each trigger.
 * \param resource Pointer to the DMA resource
 */

static void configure_dma_resource_usart_tx(struct dma_resource *resource)
{
	struct dma_resource_config config;

	dma_get_config_defaults(&config);
	config.transfer_trigger = DMA_TRIGGER_PERIPHERAL;
	config.peripheral_trigger = SERCOM3_DMAC_ID_TX;
	config.trigger_action = DMA_TRIGGER_ACTON_BEAT;

	dma_allocate(resource, &config);
}

/**
 * \brief Configures the given DMA resource for serial transfer
 *
 * Configures the DMA resource to use transmission via SERCOM3 as the
 * trigger. A beat transfer is performed on each trigger.
 *
 * \param descriptor Pointer to the DMA descriptor
 */
static void setup_transfer_descriptor_adc_usart_tx(DmacDescriptor *descriptor)
{
	struct dma_descriptor_config descriptor_config;

	dma_descriptor_get_config_defaults(&descriptor_config);

	descriptor_config.beat_size = DMA_BEAT_SIZE_BYTE;
	descriptor_config.dst_increment_enable = false;
	descriptor_config.block_transfer_count = sizeof(transfer_message);
	descriptor_config.source_address = (uint32_t)transfer_message +
		sizeof(transfer_message);
	descriptor_config.destination_address
		= (uint32_t)(&usart_instance.hw->USART.DATA.reg);

	dma_descriptor_create(descriptor, &descriptor_config);
}

/**
 * \brief Configures the USART
 *
 * SERCOM3 is used as USART
 */
static void configure_usart(void)
{
	struct usart_config config_usart;

	usart_get_config_defaults(&config_usart);

	config_usart.baudrate    = 115200;
	config_usart.mux_setting = EDBG_CDC_SERCOM_MUX_SETTING;
	config_usart.pinmux_pad0 = EDBG_CDC_SERCOM_PINMUX_PAD0;
	config_usart.pinmux_pad1 = EDBG_CDC_SERCOM_PINMUX_PAD1;
	config_usart.pinmux_pad2 = EDBG_CDC_SERCOM_PINMUX_PAD2;
	config_usart.pinmux_pad3 = EDBG_CDC_SERCOM_PINMUX_PAD3;

	while (usart_init(&usart_instance,
		EDBG_CDC_MODULE, &config_usart) != STATUS_OK) {
	}
	usart_enable(&usart_instance);
}

/**
 * \brief Configures the callback functions for the USART
 *
 * Registers a callback which is invoked when data is received
 * via the USART.
 */
void configure_usart_callbacks(void)
{
	usart_register_callback(&usart_instance,
		usart_read_callback, USART_CALLBACK_BUFFER_RECEIVED);
	usart_enable_callback(&usart_instance, USART_CALLBACK_BUFFER_RECEIVED);
}

/**
 * \brief Configures the descriptor for SRAM to USART transfer
 *
 * Configures the descriptor for SRAM to USART transfer.
 *
 * \param descriptor Pointer to the DMA descriptor
 * \param buf Pointer to the source array
 * \param len Size of the source array
 */
static void configure_sram_usart_dma_transfer(DmacDescriptor *descriptor,
	char *buf, uint32_t len)
{
	struct dma_descriptor_config descriptor_config;

	dma_descriptor_get_config_defaults(&descriptor_config);

	descriptor_config.beat_size = DMA_BEAT_SIZE_BYTE;
	descriptor_config.dst_increment_enable = false;
	descriptor_config.block_transfer_count = len;
	descriptor_config.source_address = (uint32_t)buf + len;
	descriptor_config.destination_address
		= (uint32_t)(&usart_instance.hw->USART.DATA.reg);

	dma_descriptor_create(descriptor, &descriptor_config);
}

/**
 * \brief Initialize the serial flash
 *
 * Initialize the AT25DFX081A serial flash.
 */
static void serial_flash_at25dfx_init(void)
{
	struct at25dfx_chip_config at25dfx_chip_config;

	#ifdef CONF_TEST_VECTORED_MASTER
	struct spi_master_vec_config at25dfx_spi_config;
	at25dfx_spi_master_vec_get_config_defaults(&at25dfx_spi_config);
	at25dfx_spi_config.baudrate = AT25DFX_CLOCK_SPEED;
	#else
	struct spi_config at25dfx_spi_config;
	at25dfx_spi_get_config_defaults(&at25dfx_spi_config);
	at25dfx_spi_config.mode_specific.master.baudrate = AT25DFX_CLOCK_SPEED;
	#endif

	at25dfx_spi_config.mux_setting = AT25DFX_SPI_PINMUX_SETTING;
	at25dfx_spi_config.pinmux_pad0 = AT25DFX_SPI_PINMUX_PAD0;
	at25dfx_spi_config.pinmux_pad1 = AT25DFX_SPI_PINMUX_PAD1;
	at25dfx_spi_config.pinmux_pad2 = AT25DFX_SPI_PINMUX_PAD2;
	at25dfx_spi_config.pinmux_pad3 = AT25DFX_SPI_PINMUX_PAD3;

	#ifdef CONF_TEST_VECTORED_MASTER
	spi_master_vec_init(&at25dfx_spi, AT25DFX_SPI, &at25dfx_spi_config);
	spi_master_vec_enable(&at25dfx_spi);
	#else
	spi_init(&at25dfx_spi, AT25DFX_SPI, &at25dfx_spi_config);
	spi_enable(&at25dfx_spi);
	#endif

	at25dfx_chip_config.type = AT25DFX_MEM_TYPE;
	at25dfx_chip_config.cs_pin = AT25DFX_CS;

	at25dfx_chip_init(&at25dfx_chip, &at25dfx_spi, &at25dfx_chip_config);
}

/**
 * \brief Stores the temperature readings into the serial flash
 *
 * Stores the temperature readings into the serial flash.
 * When the predefined number of readings are ready in the SRAM array,
 * store them into the non-volatile media.
 * Two bytes are used for storing a temperature reading.
 *
 * \param envir_temp_sf_buff The buffer which holds temperature readings
 */
void sf_store_logs(float *envir_temp_sf_buff)
{
	char envir_temp_decim[SF_TRANSFER_SIZE * BYTES_USED_FOR_A_READING];
	enum status_code status;
	char integer_part;
	char fract_part;
	uint8_t i = 0;

	while (i < SF_TRANSFER_SIZE) {
		/*
		 * Represent the float value in two bytes,
		 * one for the integer part and another for fractional part.
		 */
		integer_part = envir_temp_sf_buff[i];
		fract_part = ((int)(envir_temp_sf_buff[i] * 100)) % 100;
		sprintf(&envir_temp_decim[i * BYTES_USED_FOR_A_READING], "%c%c",
			integer_part, fract_part);
		i++;
	}

	/*
	 * Write the temperature values into the serial flash
	 */
	status = at25dfx_chip_write_buffer(&at25dfx_chip,
			SF_4K_BLOCK1_START +
			(sf_write_count * BYTES_USED_FOR_A_READING),
			&envir_temp_decim[0],
			SF_TRANSFER_SIZE * BYTES_USED_FOR_A_READING);
	if (STATUS_OK != status) {
		init_success = false;
	}

	/* Indicate that we've stored n reading into the SF */
	sf_write_count += SF_TRANSFER_SIZE;
}

/**
 * \brief Read the recent logs from the serial flash
 *
 * Read the recent logs from the serial flash.
 *
 **\param sf_readback_buff Pointer to the buffer into which the logs
 *							are loaded
 */
void sf_get_recent_logs(char *sf_readback_buff)
{
	enum status_code status;

	status = at25dfx_chip_read_buffer(&at25dfx_chip,
			SF_4K_BLOCK1_START +
			sf_access_status.start_addr_last_written *
			BYTES_USED_FOR_A_READING,
			sf_readback_buff,
			SF_TRANSFER_SIZE * BYTES_USED_FOR_A_READING);
	if (STATUS_OK != status) {
		init_success = false;
	}
}

/**
 * \brief Get the storage status of the serial flash
 *
 * Get the storage status of the serial flash. The storage status includes
 * the last address written and a flag which indicates whether the serial
 * flash is empty.
 */
void sf_access_get_status(void)
{
	/* We store data for Serial Flash management, in the first 4K block */
	at25dfx_chip_read_buffer(&at25dfx_chip,
		SF_4K_BLOCK0_START, &sf_access_status, sizeof(sf_access_status));
}

/**
 * \brief Set the storage status of the serial flash
 *
 * Set the storage status of the serial flash. The storage status includes
 * the last address written and a flag which indicates whether the serial
 * flash is empty.
 */
void sf_access_set_status()
{
	/*
	 * Erase the first 4K block where the serial flash management data
	 * resides
	 */
	at25dfx_chip_erase_block(&at25dfx_chip, SF_4K_BLOCK0_START,
		AT25DFX_BLOCK_SIZE_4KB);
	at25dfx_chip_write_buffer(&at25dfx_chip,
		SF_4K_BLOCK0_START, (void *)&sf_access_status,
	sizeof(sf_access_status));
}

/**
 * \brief Read all temperature readings from the serial flash
 *
 * Read all temperature readings from the serial flash.
 */
void sf_read_all_logs()
{
	uint8_t j = 0;
	uint8_t k = 0;
	uint32_t num_of_readings = 0;
	char sf_log_buff[SF_TRANSFER_SIZE * BYTES_USED_FOR_A_READING] = {0};

	/* Clear the flag before sending the data */
	read_entire_logs = false;

	sprintf(transfer_message, "\r\n%lu logs will be read\r\n",
		sf_access_status.start_addr_last_written + SF_TRANSFER_SIZE);
	/* Wait for the ongoing DMA transfer to complete */
	while (usart_dma_transfer_triggered) {
	}
	/* Make the flag true and initiate an SRAM to USART transfer */
	usart_dma_transfer_triggered = true;
	configure_sram_usart_dma_transfer(&uart_tx_dma_descriptor,
		&transfer_message[0], sizeof(transfer_message));
	dma_start_transfer_job(&dma_resource_usart_tx);

	/*
	 * Get a defined number readings at a time and send them to USART.
	 * If there are more readings, perform another read
	 */
	while (num_of_readings <= sf_access_status.start_addr_last_written) {
		j = 0;
		k = 0;

		at25dfx_chip_read_buffer(&at25dfx_chip,
			SF_4K_BLOCK1_START + (num_of_readings * BYTES_USED_FOR_A_READING),
			&sf_log_buff[0],
			(SF_TRANSFER_SIZE * BYTES_USED_FOR_A_READING));

		while (j < SF_TRANSFER_SIZE) {
			sprintf(&serial_transfer_array[j][0], "%2d.%d\r\n",
			sf_log_buff[k], sf_log_buff[k + 1]);
			j++;
			/* Two bytes are used for representing a temperature
			 *reading */
			k += 2;
		}
		num_of_readings += SF_TRANSFER_SIZE;
		/* Wait for the ongoing DMA transfer to complete */
		while (usart_dma_transfer_triggered) {
		};
		/* Make the flag true and initiate an SRAM to USART transfer */
		usart_dma_transfer_triggered = true;
		configure_sram_usart_dma_transfer(&uart_tx_dma_descriptor,
			&serial_transfer_array[0][0], sizeof(serial_transfer_array));
		dma_start_transfer_job(&dma_resource_usart_tx);
	}

	sf_read_request_received = false;
	/* Check for further read requests */
	usart_read_buffer_job(&usart_instance,
	(uint8_t *)rx_buffer, CMD_RX_BUFFER_LENGTH);
}

/**
 * \brief Main application routine
 */
int main(void)
{
	system_init();
	configure_adc();
	configure_usart();
	configure_usart_callbacks();
	init_rtc();
	configure_rtc_callbacks();
	configure_transfer_event();
	serial_flash_at25dfx_init();
	at25dfx_chip_set_global_sector_protect(&at25dfx_chip, false);
	sf_access_get_status();

	/*
	 * We write zero to the first location in serial flash, before storing
	 * the logs. If the first byte is not zero, we haven't written anything yet.
	 */
	if (sf_access_status.sf_is_clean) {
		sf_access_status.sf_is_clean = false;
		sf_access_status.start_addr_last_written = 0;
		at25dfx_chip_erase_block(&at25dfx_chip, SF_4K_BLOCK1_START,
			AT25DFX_BLOCK_SIZE_4KB);
		/* Write the status into the serial flash */
		sf_access_set_status();
	} else {
		sf_write_count = sf_access_status.start_addr_last_written +
			SF_TRANSFER_SIZE;
	}

	configure_dma_resource_adc(&dma_resource_adc_to_sram);
	configure_dma_resource_usart_tx(&dma_resource_usart_tx);
	setup_transfer_descriptor_adc(&adc_to_sram_dma_descriptor);
	setup_transfer_descriptor_adc_usart_tx(&uart_tx_dma_descriptor);
	dma_add_descriptor(&dma_resource_adc_to_sram,
		&adc_to_sram_dma_descriptor);
	dma_add_descriptor(&dma_resource_usart_tx, &uart_tx_dma_descriptor);
	dma_register_callback(&dma_resource_usart_tx,
		transfer_done_usart_tx_dma,
		DMA_CALLBACK_TRANSFER_DONE);
	dma_enable_callback(&dma_resource_adc_to_sram,
		DMA_CALLBACK_TRANSFER_DONE);
	dma_enable_callback(&dma_resource_usart_tx,
		DMA_CALLBACK_TRANSFER_DONE);

	/* Send the init message via USART */
	dma_start_transfer_job(&dma_resource_usart_tx);

	/*
	 * Start the DMA transfer from ADC to the given array in SRAM.
	 * A beat transfer is performed on every RTC overflow event.
	 */
	dma_start_transfer_job(&dma_resource_adc_to_sram);
	/* Enable the RTC, which generates the event to trigger ADC conversions
	 **/
	enable_transfer_trigger();

	/* Listen to USART for read command */
	usart_read_buffer_job(&usart_instance,
	(uint8_t *)rx_buffer, CMD_RX_BUFFER_LENGTH);
	/* Select the sleep mode as Idle sleep */
	system_set_sleepmode(SYSTEM_SLEEPMODE_IDLE_0);

	while (1) {
		/*
		 * Let the CPU sleep till an RTC overflow interrupt or USART
		 *receive
		 * interrupt wake the CPU.
		 */

		system_sleep();

		/*
		 * The CPU is waken up by an Interrupt
		 * Check who needs a service
		 */
		if (adc_sram_dma_transfer_done) {
			uint8_t j = 0;

			/*
			 * Initialize the Steinhart-Hart coefficients
			 * Reference points taken for this example are 0, 25 and
			 * 50 Deg C
			 */
			float shh_a = SHH_COEFF_A;
			float shh_b = SHH_COEFF_B;
			float shh_c = SHH_COEFF_C;
			float thermistor_res;
			float input_voltage;
			float envir_temp;
			float envir_temp_sf[SF_TRANSFER_SIZE] = {0};
			volatile uint16_t adc_results_conv_array[
				SF_TRANSFER_SIZE] = {0};

			/*
			 * Copy the contents from adc_results_array into a local
			 * array, right after we wakeup from sleep.
			 */
			while (j < SF_TRANSFER_SIZE) {
				adc_results_conv_array[j]
					= adc_results_array[j];
				j++;
			}
			j = 0;

			while (j < SF_TRANSFER_SIZE) {
				input_voltage = adc_results_conv_array[j] *
					THERM_ADC_VOLTS_PER_BIT;
				thermistor_res = (float)(input_voltage *
					THERM_SERIES_RESISTOR) /
					(THERM_EXCITATION_VOLT - input_voltage);
				envir_temp
					= (float)(1 /
					(shh_a + shh_b *
					log(thermistor_res) +
					shh_c * (pow(log(thermistor_res), 3)))) - 273;
				envir_temp_sf[j] = envir_temp;
				j++;
			}

			if (sf_write_count >= SF_ADDRESS_LIMIT) {
				sf_write_count = 0;
				sf_access_status.sf_is_clean = true;
				sf_access_status.start_addr_last_written = 0;
				at25dfx_chip_erase_block(&at25dfx_chip,
					SF_4K_BLOCK1_START,
					AT25DFX_BLOCK_SIZE_4KB);
			} else {
				sf_access_status.sf_is_clean = false;
				sf_access_status.start_addr_last_written
					= sf_write_count;
			}

			sf_access_set_status();
			sf_store_logs(envir_temp_sf);
			adc_sram_dma_transfer_done = false;
		}

		/*
		 * Once we've received a read request, check if the
		 * write count is a non-zero
		 */
		if (sf_read_request_received && sf_write_count) {
			if (true == read_entire_logs) {
				sf_read_all_logs();
			} else {
				uint8_t j = 0;
				uint8_t k = 0;
				char envir_temp_sf_readback[SF_TRANSFER_SIZE *2]= {0};

				sprintf(transfer_message,
					"\r\nRecent %d readings\r\n",
					SF_TRANSFER_SIZE);
				/* Wait for the ongoing DMA transfer to complete*/
				while (usart_dma_transfer_triggered) {
				};
				/* Make the flag true and initiate an SRAM to
				 * USART transfer
				 */
				usart_dma_transfer_triggered = true;
				configure_sram_usart_dma_transfer(
					&uart_tx_dma_descriptor,
					&transfer_message[0], sizeof(transfer_message));
				dma_start_transfer_job(&dma_resource_usart_tx);

				sf_get_recent_logs(&envir_temp_sf_readback[0]);

				while (j < SF_TRANSFER_SIZE) {
					sprintf(&serial_transfer_array[j][0],
					"%2d.%d\r\n",
					envir_temp_sf_readback[k],
					envir_temp_sf_readback[k + 1]);
					j++;
					k += 2;
				}

				/* Wait for the ongoing DMA transfer to complete*/
				while (usart_dma_transfer_triggered) {
				}
				/* Make the flag true and initiate an SRAM to
				 * USART transfer
				 */
				usart_dma_transfer_triggered = true;
				configure_sram_usart_dma_transfer(
					&uart_tx_dma_descriptor,
					&serial_transfer_array[0][0],
				sizeof(serial_transfer_array));
				dma_start_transfer_job(&dma_resource_usart_tx);

				sf_read_request_received = false;
				/* Check for further requests */
				usart_read_buffer_job(&usart_instance,
					(uint8_t *)rx_buffer, CMD_RX_BUFFER_LENGTH);
			}
		}
	}
}
