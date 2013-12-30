/**
 * \file
 *
 * \brief SAM D2x Sercom SPI driver with DMA quick start
 *
 * Copyright (C) 2013 Atmel Corporation. All rights reserved.
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
#include <asf.h>
#include "conf_quick_start.h"

void configure_spi_master(void);
void configure_spi_slave(void);
void transfer_tx_done( const struct dma_resource* const resource );
void transfer_rx_done( const struct dma_resource* const resource );
void configure_dma_resource_tx(struct dma_resource *resource);
void configure_dma_resource_rx(struct dma_resource *resource);
void setup_transfer_descriptor_tx(DmacDescriptor *descriptor);
void setup_transfer_descriptor_rx(DmacDescriptor *descriptor);

//! [setup]
//! [buf_length]
#define BUF_LENGTH 20
//! [buf_length]

/* Test Baud rate */
#define TEST_SPI_BAUDRATE             1000000UL

//! [slave_select_pin]
#define SLAVE_SELECT_PIN EXT2_PIN_SPI_SS_0
//! [slave_select_pin]
//! [buffer]
static const uint8_t buffer_tx[BUF_LENGTH] = {
		0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A,
		0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14,
};
//! [buffer]

//! [dev_inst]
struct spi_module spi_master_instance;
//! [dev_inst]
//! [slave_dev_inst]
struct spi_slave_inst slave;
//! [slave_dev_inst]
//! [buffer]
static uint8_t buffer_rx[20];
//! [buffer]

//! [dev_inst]
struct spi_module spi_slave_instance;
//! [setup]

//! [dma_resource]
struct dma_resource example_resource_tx;
struct dma_resource example_resource_rx;
//! [dma_resource]

// [transfer_done_flag]
static volatile bool transfer_tx_is_done = false;
static volatile bool transfer_rx_is_done = false;
// [transfer_done_flag]

// [transfer_descriptor]
COMPILER_ALIGNED(16)
DmacDescriptor example_descriptor_tx;
DmacDescriptor example_descriptor_rx;
// [transfer_descriptor]

//! [setup]
// [_transfer_tx_done]
void transfer_tx_done( const struct dma_resource* const resource )
{
	transfer_tx_is_done = true;
}
// [_transfer_tx_done]

// [_transfer_rx_done]
void transfer_rx_done( const struct dma_resource* const resource )
{
	transfer_rx_is_done = true;
}
// [_transfer_rx_done]

// [config_dma_resource_tx]
void configure_dma_resource_tx(struct dma_resource *resource)
{
//! [setup_1]
	struct dma_resource_config config;
//! [setup_1]

//! [setup_2]
	dma_get_config_defaults(&config);
//! [setup_2]

//! [setup_3]
	config.transfer_trigger = DMA_TRIGGER_PERIPHERAL;
	config.peripheral_trigger = PERIPHERAL_TRIGGER_SERCOM_TX;
	config.trigger_action = DMA_TRIGGER_ACTON_BEAT;
//! [setup_3]

//! [setup_4]
	dma_allocate(resource, &config);
//! [setup_4]
}
// [config_dma_resource_tx]

// [config_dma_resource_rx]
void configure_dma_resource_rx(struct dma_resource *resource)
{
//! [setup_1]
	struct dma_resource_config config;
//! [setup_1]

//! [setup_2]
	dma_get_config_defaults(&config);
//! [setup_2]

//! [setup_3]
	config.transfer_trigger = DMA_TRIGGER_PERIPHERAL;
	config.peripheral_trigger = PERIPHERAL_TRIGGER_SERCOM_RX;
	config.trigger_action = DMA_TRIGGER_ACTON_BEAT;
//! [setup_3]

//! [setup_4]
	dma_allocate(resource, &config);
//! [setup_4]
}
// [config_dma_resource_rx]

// [setup_dma_transfer_descriptor]
void setup_transfer_descriptor_tx(DmacDescriptor *descriptor)
{
//! [setup_5]
	struct dma_descriptor_config descriptor_config;
//! [setup_5]

//! [setup_6]
	dma_descriptor_get_config_defaults(&descriptor_config);
//! [setup_6]

//! [setup_7]
	descriptor_config.beat_size = DMA_BEAT_SIZE_BYTE;
	descriptor_config.dst_increment_enable = false;
	descriptor_config.block_transfer_count = sizeof(buffer_tx)/sizeof(uint8_t);
	descriptor_config.source_address = (uint32_t)buffer_tx + sizeof(buffer_tx);
	descriptor_config.destination_address = (uint32_t)(&spi_master_instance.hw->SPI.DATA.reg);
//! [setup_7]

//! [setup_8]
	dma_descriptor_create(descriptor, &descriptor_config);
//! [setup_8]
}
// [setup_dma_transfer_descriptor]

// [setup_dma_transfer_descriptor_rx]
void setup_transfer_descriptor_rx(DmacDescriptor *descriptor)
{
//! [setup_5]
	struct dma_descriptor_config descriptor_config;
//! [setup_5]

//! [setup_6]
	dma_descriptor_get_config_defaults(&descriptor_config);
//! [setup_6]

//! [setup_7]
	descriptor_config.beat_size = DMA_BEAT_SIZE_BYTE;
	descriptor_config.src_increment_enable = false;
	descriptor_config.block_transfer_count = sizeof(buffer_rx)/sizeof(uint8_t);
	descriptor_config.source_address = (uint32_t)(&spi_slave_instance.hw->SPI.DATA.reg);
	descriptor_config.destination_address = (uint32_t)buffer_rx + sizeof(buffer_rx);
//! [setup_7]

//! [setup_8]
	dma_descriptor_create(descriptor, &descriptor_config);
//! [setup_8]
}
// [setup_dma_transfer_descriptor_rx]

//! [configure_spi]
void configure_spi_master(void)
{
//! [config]
	struct spi_config config_spi_master;
//! [config]
//! [slave_config]
	struct spi_slave_inst_config slave_dev_config;
//! [slave_config]
	/* Configure and initialize software device instance of peripheral slave */
//! [slave_conf_defaults]
	spi_slave_inst_get_config_defaults(&slave_dev_config);
//! [slave_conf_defaults]
//! [ss_pin]
	slave_dev_config.ss_pin = SLAVE_SELECT_PIN;
//! [ss_pin]
//! [slave_init]
	spi_attach_slave(&slave, &slave_dev_config);
//! [slave_init]
	/* Configure, initialize and enable SERCOM SPI module */
//! [conf_defaults]
	spi_get_config_defaults(&config_spi_master);
//! [conf_defaults]
	config_spi_master.mode_specific.master.baudrate = TEST_SPI_BAUDRATE;
//! [mux_setting]
	config_spi_master.mux_setting = EXT2_SPI_SERCOM_MUX_SETTING;
//! [mux_setting]
	/* Configure pad 0 for data in */
//! [di]
	config_spi_master.pinmux_pad0 = EXT2_SPI_SERCOM_PINMUX_PAD0;
//! [di]
	/* Configure pad 1 as unused */
//! [ss]
	config_spi_master.pinmux_pad1 = PINMUX_UNUSED;
//! [ss]
	/* Configure pad 2 for data out */
//! [do]
	config_spi_master.pinmux_pad2 = EXT2_SPI_SERCOM_PINMUX_PAD2;
//! [do]
	/* Configure pad 3 for SCK */
//! [sck]
	config_spi_master.pinmux_pad3 = EXT2_SPI_SERCOM_PINMUX_PAD3;
//! [sck]
//! [init]
	spi_init(&spi_master_instance, EXT2_SPI_MODULE, &config_spi_master);
//! [init]

//! [enable]
	spi_enable(&spi_master_instance);
//! [enable]

}
//! [configure_spi]

//! [configure_spi_slave]
void configure_spi_slave(void)
{
//! [config]
	struct spi_config config_spi_slave;
//! [config]

	/* Configure, initialize and enable SERCOM SPI module */
//! [conf_defaults]
	spi_get_config_defaults(&config_spi_slave);
//! [conf_defaults]
//! [conf_spi_slave_instance]
	config_spi_slave.mode = SPI_MODE_SLAVE;
//! [conf_spi_slave_instance]
//! [conf_preload]
	config_spi_slave.mode_specific.slave.preload_enable = true;
//! [conf_preload]
//! [conf_format]
	config_spi_slave.mode_specific.slave.frame_format = SPI_FRAME_FORMAT_SPI_FRAME;
//! [conf_format]
//! [mux_setting]
	config_spi_slave.mux_setting = EXT1_SPI_SERCOM_MUX_SETTING;
//! [mux_setting]
	/* Configure pad 0 for data in */
//! [di]
	config_spi_slave.pinmux_pad0 = EXT1_SPI_SERCOM_PINMUX_PAD0;
//! [di]
	/* Configure pad 1 as unused */
//! [ss]
	config_spi_slave.pinmux_pad1 = EXT1_SPI_SERCOM_PINMUX_PAD1;
//! [ss]
	/* Configure pad 2 for data out */
//! [do]
	config_spi_slave.pinmux_pad2 = EXT1_SPI_SERCOM_PINMUX_PAD2;
//! [do]
	/* Configure pad 3 for SCK */
//! [sck]
	config_spi_slave.pinmux_pad3 = EXT1_SPI_SERCOM_PINMUX_PAD3;
//! [sck]
//! [init]
	spi_init(&spi_slave_instance, EXT1_SPI_MODULE, &config_spi_slave);
//! [init]

//! [enable]
	spi_enable(&spi_slave_instance);
//! [enable]

}
//! [configure_spi_slave]

int main(void)
{
	system_init();

//! [setup_init]
//! [setup_spi]
	configure_spi_master();
	configure_spi_slave();
//! [setup_spi]

//! [setup_dma_resource]
	configure_dma_resource_tx(&example_resource_tx);
	configure_dma_resource_rx(&example_resource_rx);
//! [setup_dma_resource]

//! [setup_transfer_descriptor]
	setup_transfer_descriptor_tx(&example_descriptor_tx);
	setup_transfer_descriptor_rx(&example_descriptor_rx);
//! [setup_transfer_descriptor]

//! [Add descriptor to DMA resource]
	dma_add_descriptor(&example_resource_tx, &example_descriptor_tx);
	dma_add_descriptor(&example_resource_rx, &example_descriptor_rx);
//! [Add descriptor to DMA resource]

//! [setup_callback_register]
	dma_register_callback(&example_resource_tx, transfer_tx_done,
			DMA_CALLBACK_TRANSFER_DONE);
	dma_register_callback(&example_resource_rx, transfer_rx_done,
			DMA_CALLBACK_TRANSFER_DONE);
//! [setup_callback_register]

//! [setup_enable_callback]
	dma_enable_callback(&example_resource_tx, DMA_CALLBACK_TRANSFER_DONE);
	dma_enable_callback(&example_resource_rx, DMA_CALLBACK_TRANSFER_DONE);
//! [setup_enable_callback]

//! [select_slave]
	spi_select_slave(&spi_master_instance, &slave, true);
//! [select_slave]

//! [main_1]
	dma_start_transfer_job(&example_resource_rx);
	dma_start_transfer_job(&example_resource_tx);
//! [main_1]

//! [main_2]
	while (!transfer_rx_is_done) {
		/* Wait for transfer done */
	}
//! [main_2]

//! [deselect_slave]
	spi_select_slave(&spi_master_instance, &slave, false);
//! [deselect_slave]

//! [endless_loop]
	while (true) {
//! [endless_loop]
	}

//! [main]
}
