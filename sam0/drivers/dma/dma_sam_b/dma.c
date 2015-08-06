/*
 * \file
 *
 * \brief SAM Direct Memory Access Controller Driver
 *
 * Copyright (C) 2015 Atmel Corporation. All rights reserved.
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

#include <string.h>
#include "dma.h"
#include "conf_dma.h"

//#include "clock.h"
//#include "system_interrupt.h"

struct _dma_module {
	volatile bool _dma_init;
	volatile uint32_t allocated_channels;
	uint8_t free_channels;
};

struct _dma_module _dma_inst = {
	._dma_init = false,
	.allocated_channels = 0,
	.free_channels = CONF_MAX_USED_CHANNEL_NUM,
};

/** Internal DMA resource pool. */
static struct dma_resource* _dma_active_resource[CONF_MAX_USED_CHANNEL_NUM];

static uint32_t get_channel_reg_val(uint8_t channel, uint32_t reg){
	return *(uint32_t*)(reg + 0x100*channel);
}

static void set_channel_reg_val(uint8_t channel, uint32_t reg, uint32_t val){
	*(volatile uint32_t*)(reg + 0x100*channel) = val;
}

uint8_t dma_get_status(uint8_t channel)
{
	return (uint8_t)get_channel_reg_val(channel, (uint32_t)&PROV_DMA_CTRL0->CH0_INT_RAWSTAT_REG.reg);
}

uint8_t dma_get_interrupt_status(uint8_t channel)
{
	return get_channel_reg_val(channel, (uint32_t)&PROV_DMA_CTRL0->CH0_INT_STATUS_REG.reg);
}

void dma_clear_interrupt_status(uint8_t channel, uint8_t flag)
{
	set_channel_reg_val(channel, (uint32_t)&PROV_DMA_CTRL0->CH0_INT_CLEAR_REG.reg, 1 << flag);
}

/**
 * \brief Find a free channel for a DMA resource.
 *
 * Find a channel for the requested DMA resource.
 *
 * \return Status of channel allocation.
 * \retval DMA_INVALID_CHANNEL  No channel available
 * \retval count          Allocated channel for the DMA resource
 */
static uint8_t _dma_find_first_free_channel_and_allocate(void)
{
	uint8_t count;
	uint32_t tmp;
	bool allocated = false;

	tmp = _dma_inst.allocated_channels;

	for (count = 0; count < CONF_MAX_USED_CHANNEL_NUM; ++count) {
		if (!(tmp & 0x00000001)) {
			/* If free channel found, set as allocated and return
			 *number */

			_dma_inst.allocated_channels |= 1 << count;
			_dma_inst.free_channels--;
			allocated = true;

			break;
		}

		tmp = tmp >> 1;
	}

	if (!allocated) {
		return DMA_INVALID_CHANNEL;
	} else {
		return count;
	}
}

/**
 * \brief Release an allocated DMA channel.
 *
 * \param[in]  channel  Channel id to be released
 *
 */
static void _dma_release_channel(uint8_t channel)
{
	_dma_inst.allocated_channels &= ~(1 << channel);
	_dma_inst.free_channels++;
}

/**
 * \brief Initializes config with predefined default values.
 *
 * This function will initialize a given DMA configuration structure to
 * a set of known default values. This function should be called on
 * any new instance of the configuration structure before being
 * modified by the user application.
 *
 * The default configuration is as follows:
 *  \li Set source max burst number as 1
 *  \li Set source tokens as 1
 *  \li Set source tokens as 1
 *  \li Set source peripheral as memory
 *  \li Set source peripheral delay as 0
 *  \li Disable source top priority
 *  \li Set source top priority channel as 0
 *  \li Disable source high priority
 *  \li Set source high priority channel as 0
 *  \li Set destination max burst number as 1
 *  \li Set destination tokens as 1
 *  \li Set destination tokens as 1
 *  \li Set destination peripheral as memory
 *  \li Set destination peripheral delay as 0
 *  \li Disable destination top priority
 *  \li Set destination top priority channel as 0
 *  \li Disable destination high priority
 *  \li Set destination high priority channel as 0
 *  \li Disable the joint mode
 *  \li Disable the endian swap
 * \param[out] config Pointer to the configuration
 *
 */
void dma_get_config_defaults(struct dma_resource_config *config)
{
	/* DMA source configuration */
	config->src.max_burst = 1;
	config->src.tokens = 1;
	config->src.enable_inc_addr = true;
	config->src.periph = 0;
	config->src.periph_delay = 0;
	config->src.eanble_proi_top = false;
	config->src.proi_top_index = 0;
	config->src.eanble_proi_high = false;
	config->src.proi_high_index = 0;
	/* DMA destination configuration */
	config->des.max_burst = 1;
	config->des.tokens = 1;
	config->des.enable_inc_addr = true;
	config->des.periph = 0;
	config->des.periph_delay = 0;
	config->des.eanble_proi_top = false;
	config->des.proi_top_index = 0;
	config->des.eanble_proi_high = false;
	config->des.proi_high_index = 0;
	/* DMA channel configuration */
	config->enable_joint_mode = false;
	config->swap = DMA_ENDIAN_NO_SWAP;
	
	/* Clear global variety */
	_dma_inst._dma_init = false;
	_dma_inst.allocated_channels = 0;
	_dma_inst.free_channels = CONF_MAX_USED_CHANNEL_NUM;
	for (int i=0; i<CONF_MAX_USED_CHANNEL_NUM; i++) {
		 _dma_active_resource[i] = NULL;
	}
	
}

/**
 * \brief Configure the DMA resource.
 *
 * \param[in]  dma_resource Pointer to a DMA resource instance
 * \param[out] resource_config Configurations of the DMA resource
 *
 */
static void _dma_set_config(struct dma_resource *resource,
		struct dma_resource_config *config)
{
	uint32_t regval = 0;

	/* Static register configuration */
	regval = PROV_DMA_CTRL_CH0_STATIC_REG0_RD_BURST_MAX_SIZE(config->src.max_burst)|
			PROV_DMA_CTRL_CH0_STATIC_REG0_RD_TOKENS(config->src.tokens) |
			(config->src.enable_inc_addr << PROV_DMA_CTRL_CH0_STATIC_REG0_RD_INCR_Pos);
	set_channel_reg_val(resource->channel_id, (uint32_t)&PROV_DMA_CTRL0->CH0_STATIC_REG0.reg, regval);
	/* Static register1 configuration */
	regval = PROV_DMA_CTRL_CH0_STATIC_REG1_WR_BURST_MAX_SIZE(config->des.max_burst) |
			PROV_DMA_CTRL_CH0_STATIC_REG1_WR_TOKENS(config->des.tokens) | 
			(config->des.enable_inc_addr << PROV_DMA_CTRL_CH0_STATIC_REG0_RD_INCR_Pos);
	set_channel_reg_val(resource->channel_id, (uint32_t)&PROV_DMA_CTRL0->CH0_STATIC_REG1.reg, regval);
	/* Static register2 configuration */
	regval = (config->enable_joint_mode << PROV_DMA_CTRL_CH0_STATIC_REG2_JOINT_Pos) |
			PROV_DMA_CTRL_CH0_STATIC_REG2_END_SWAP(config->swap);
	set_channel_reg_val(resource->channel_id, (uint32_t)&PROV_DMA_CTRL0->CH0_STATIC_REG2.reg, regval);
	/* Static register4 configuration */
	regval = PROV_DMA_CTRL_CH0_STATIC_REG4_RD_PERIPH_NUM(config->src.periph) |
			PROV_DMA_CTRL_CH0_STATIC_REG4_RD_PERIPH_DELAY(config->src.periph_delay) |
			PROV_DMA_CTRL_CH0_STATIC_REG4_WR_PERIPH_NUM(config->des.periph) |
			PROV_DMA_CTRL_CH0_STATIC_REG4_WR_PERIPH_DELAY(config->des.periph_delay);
	set_channel_reg_val(resource->channel_id, (uint32_t)&PROV_DMA_CTRL0->CH0_STATIC_REG4.reg, regval);
	/* Priority channels configuration */
	regval = PROV_DMA_CTRL_CORE_PRIORITY_RD_PRIO_TOP_NUM(config->src.proi_top_index) |
			(PROV_DMA_CTRL_CORE_PRIORITY_RD_PRIO_TOP << config->src.eanble_proi_top) |
			PROV_DMA_CTRL_CORE_PRIORITY_RD_PRIO_HIGH_NUM(config->src.proi_high_index) |
			(PROV_DMA_CTRL_CORE_PRIORITY_RD_PRIO_HIGH << config->src.eanble_proi_high) |
			PROV_DMA_CTRL_CORE_PRIORITY_WR_PRIO_TOP_NUM(config->des.proi_top_index) |
			(PROV_DMA_CTRL_CORE_PRIORITY_WR_PRIO_TOP << config->des.eanble_proi_top) |
			PROV_DMA_CTRL_CORE_PRIORITY_WR_PRIO_HIGH_NUM(config->des.proi_high_index) |
			(PROV_DMA_CTRL_CORE_PRIORITY_WR_PRIO_HIGH << config->des.eanble_proi_high);
	set_channel_reg_val(resource->channel_id, (uint32_t)&PROV_DMA_CTRL0->CORE_PRIORITY.reg, regval);
}

/**
 * \brief Allocate a DMA with configurations.
 *
 * This function will allocate a proper channel for a DMA transfer request.
 *
 * \param[in,out]  dma_resource Pointer to a DMA resource instance
 * \param[in] transfer_config Configurations of the DMA transfer
 *
 * \return Status of the allocation procedure.
 *
 * \retval STATUS_OK The DMA resource was allocated successfully
 * \retval STATUS_ERR_NOT_FOUND DMA resource allocation failed
 */
enum status_code dma_allocate(struct dma_resource *resource,
		struct dma_resource_config *config)
{
	uint8_t new_channel;

	if (!_dma_inst._dma_init) {
		/* Perform a reset before enable DMA controller */
		LPMCU_MISC_REGS0->LPMCU_GLOBAL_RESET_1.reg &=
				~LPMCU_MISC_REGS_LPMCU_GLOBAL_RESET_1_DMA_CONTROLLER_RSTN;
		LPMCU_MISC_REGS0->LPMCU_GLOBAL_RESET_1.reg |=
				LPMCU_MISC_REGS_LPMCU_GLOBAL_RESET_1_DMA_CONTROLLER_RSTN;

		_dma_inst._dma_init = true;
	}
	if (resource->channel_id >= CONF_MAX_USED_CHANNEL_NUM) {
		return STATUS_ERR_NOT_FOUND;
	}

	new_channel = _dma_find_first_free_channel_and_allocate();
	/* If no channel available, return not found */
	if (new_channel == DMA_INVALID_CHANNEL) {
		return STATUS_ERR_NOT_FOUND;
	}
	
	/* Set the channel */
	resource->channel_id = new_channel;
	/* Configure the DMA control,channel registers and descriptors here */
	_dma_set_config(resource, config);
	
	resource->descriptor = NULL;

	/* Log the DMA resource into the internal DMA resource pool */
	_dma_active_resource[resource->channel_id] = resource;

	return STATUS_OK;
}

/**
 * \brief Free an allocated DMA resource.
 *
 * This function will free an allocated DMA resource.
 *
 * \param[in,out] resource Pointer to the DMA resource
 *
 * \return Status of the free procedure.
 *
 * \retval STATUS_OK The DMA resource was freed successfully
 * \retval STATUS_BUSY The DMA resource was busy and can't be freed
 * \retval STATUS_ERR_NOT_INITIALIZED DMA resource was not initialized
 */
enum status_code dma_free(struct dma_resource *resource)
{
	/* Check if channel is busy */
	if (dma_get_job_status(resource) == STATUS_BUSY) {
		return STATUS_BUSY;
	}

	/* Check if DMA resource was not allocated */
	if (!(_dma_inst.allocated_channels & (1 << resource->channel_id))) {
		return STATUS_ERR_NOT_INITIALIZED;
	}

	/* Release the DMA resource */
	_dma_release_channel(resource->channel_id);

	/* Reset the item in the DMA resource pool */
	_dma_active_resource[resource->channel_id] = NULL;

	return STATUS_OK;
}

/**
 * \brief Add a DMA transfer descriptor to a DMA resource.
 *
 * This function will add a DMA transfer descriptor to a DMA resource.
 * If there was a transfer descriptor already allocated to the DMA resource,
 * the descriptor will be linked to the next descriptor address.
 *
 * \param[in] resource Pointer to the DMA resource
 * \param[in] descriptor Pointer to the transfer descriptor
 *
 * \retval STATUS_OK The descriptor is added to the DMA resource
 * \retval STATUS_BUSY The DMA resource was busy and the descriptor is not added
 */
enum status_code dma_add_descriptor(struct dma_resource *resource,
		struct dma_descriptor *descriptor)
{
	struct dma_descriptor* desc;
	
	desc = resource->descriptor;

	/* Check if channel is busy */
	if (dma_get_job_status(resource) == STATUS_BUSY) {
		return STATUS_BUSY;
	}

	/* Look up for an empty space for the descriptor */
	if (desc == NULL) {
		resource->descriptor = descriptor;
		set_channel_reg_val(resource->channel_id, (uint32_t)&PROV_DMA_CTRL0->CH0_CMD_REG0.reg, descriptor->read_start_addr);
		set_channel_reg_val(resource->channel_id, (uint32_t)&PROV_DMA_CTRL0->CH0_CMD_REG1.reg, descriptor->write_start_addr);
		set_channel_reg_val(resource->channel_id, (uint32_t)&PROV_DMA_CTRL0->CH0_CMD_REG2.reg, descriptor->buffer_size);
		set_channel_reg_val(resource->channel_id, (uint32_t)&PROV_DMA_CTRL0->CH0_CMD_REG3.reg, 3);
	} else {
		/* Looking for end of descriptor link */
		while(((uint32_t)desc->cmd.next_addr) != 0) {
			desc = (struct dma_descriptor*)((uint32_t)desc->next);
		}
		if (resource->descriptor->cmd.next_addr == 0x0) {
			set_channel_reg_val(resource->channel_id, (uint32_t)&PROV_DMA_CTRL0->CH0_CMD_REG3.reg, ((uint32_t)descriptor & (~0x3)));
		}
		/* Set to the end of descriptor list */
		desc->next = (uint32_t)descriptor;
		/* The end of list should point to 0 */
		if (descriptor->cmd.next_addr != 0) {
			/* Enable transferred interrupt, and channel stops when buffer done */
			descriptor->next = 0x3;
		}
	}

	return STATUS_OK;
}

/**
 * \brief Start a DMA transfer.
 *
 * This function will start a DMA transfer through an allocated DMA resource.
 *
 * \param[in,out] resource Pointer to the DMA resource
 *
 * \return Status of the transfer start procedure.
 *
 * \retval STATUS_OK The transfer was started successfully
 * \retval STATUS_BUSY The DMA resource was busy and the transfer was not started
 * \retval STATUS_ERR_INVALID_ARG Transfer size is 0 and transfer was not started
 */
enum status_code dma_start_transfer_job(struct dma_resource *resource)
{
	uint32_t regval;

	/* Check if resource was busy */
	if (resource->job_status == STATUS_BUSY) {
		return STATUS_BUSY;
	}

	/* Check if transfer size is valid */
	if (resource->descriptor->buffer_size == 0) {
		return STATUS_ERR_INVALID_ARG;
	}

	/* Enable DMA interrupt */
	NVIC_EnableIRQ(PROV_DMA_CTRL0_IRQn);

	/* Set the interrupt flag */
	regval = PROV_DMA_CTRL_CH0_INT_ENABLE_REG_MASK & DMA_CALLBACK_TRANSFER_DONE;
	set_channel_reg_val(resource->channel_id, (uint32_t)&PROV_DMA_CTRL0->CH0_INT_ENABLE_REG.reg, regval);
	/* Set job status */
	resource->job_status = STATUS_BUSY;

	/* Set channel x descriptor 0 to the descriptor base address */
	//memcpy(&descriptor_section[resource->channel_id], resource->descriptor,
						//sizeof(descriptor_section[0]));

	/* Enable the transfer channel */
	set_channel_reg_val(resource->channel_id, (uint32_t)&PROV_DMA_CTRL0->CH0_CH_ENABLE_REG.reg, 1);
	/* Start the transfer channel */
	set_channel_reg_val(resource->channel_id, (uint32_t)&PROV_DMA_CTRL0->CH0_CH_START_REG.reg, 1);

	return STATUS_OK;
}