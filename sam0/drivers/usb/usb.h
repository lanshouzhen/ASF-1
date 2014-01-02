/**
 * \file
 *
 * \brief SAMD21 USB Driver
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
#ifndef USB_H_INCLUDED
#define USB_H_INCLUDED

#include <compiler.h>
#include <clock.h>
#include <gclk.h>
#include <pinmux.h>
#include <system_interrupt.h>

/** Enum for the speed status for the USB module */
enum usb_speed {
	USB_SPEED_LOW,
	USB_SPEED_FULL,
};

//! @name USB Host related contents
//! @{

/** Enum for the possible callback types for the USB in host module */
enum usb_host_callback {
	USB_HOST_CALLBACK_SOF,
	USB_HOST_CALLBACK_RESET,
	USB_HOST_CALLBACK_WAKEUP,
	USB_HOST_CALLBACK_DNRSM,
	USB_HOST_CALLBACK_UPRSM,
	USB_HOST_CALLBACK_RAMACER,
	USB_HOST_CALLBACK_CONNECT,
	USB_HOST_CALLBACK_DISCONNECT,
	USB_HOST_CALLBACK_N,
};

/** Enum for the possible callback types for the USB pipe in host module */
enum usb_host_pipe_callback {
	USB_HOST_PIPE_CALLBACK_TRANSFER_COMPLETE,
	USB_HOST_PIPE_CALLBACK_ERROR,
	USB_HOST_PIPE_CALLBACK_SETUP,
	USB_HOST_PIPE_CALLBACK_STALL,
	USB_HOST_PIPE_CALLBACK_N,
};

/**
 * \brief Host pipe types.
 */
enum usb_host_pipe_type {
	USB_HOST_PIPE_TYPE_DISABLE,
	USB_HOST_PIPE_TYPE_CONTROL,
	USB_HOST_PIPE_TYPE_ISO,
	USB_HOST_PIPE_TYPE_BULK,
	USB_HOST_PIPE_TYPE_INTERRUPT,
	USB_HOST_PIPE_TYPE_EXTENDED,
};

/**
 * \brief Host pipe token types.
 */
enum usb_host_pipe_token {
	USB_HOST_PIPE_TOKEN_SETUP,
	USB_HOST_PIPE_TOKEN_IN,
	USB_HOST_PIPE_TOKEN_OUT,
};

/** 
 * \brief Enumeration for the possible callback types for the USB in device module 
 */
enum usb_device_callback {
	USB_DEVICE_CALLBACK_SUSPEND = 0,
	/* Reserved */
	USB_DEVICE_CALLBACK_SOF = 2,
	USB_DEVICE_CALLBACK_RESET = 3,
	USB_DEVICE_CALLBACK_RESUME = 4,
	USB_DEVICE_CALLBACK_RAMACER = 5,
	USB_DEIVCE_CALLBACK_LPMNYET = 6,
	USB_DEIVCE_CALLBACK_LPMSUSP = 7,
	USB_DEVICE_CALLBACK_N,
};

/** 
 * \brief Enumeration for the possible callback types for the USB endpoint in device module 
 */
enum usb_device_endpoint_callback {
	USB_DEVICE_ENDPOINT_CALLBACK_TRCPT,
	USB_DEVICE_ENDPOINT_CALLBACK_TRFAIL,
	USB_DEVICE_ENDPOINT_CALLBACK_RXSTP,
	USB_DEVICE_ENDPOINT_CALLBACK_STALL,
	USB_DEVICE_EP_CALLBACK_N,
};

/**
 * \brief Device Endpoint types.
 */
enum usb_device_endpoint_type {
	USB_DEVICE_ENDPOINT_TYPE_DISABLE,
	USB_DEVICE_ENDPOINT_TYPE_CONTROL,
	USB_DEVICE_ENDPOINT_TYPE_ISOCHRONOUS,
	USB_DEVICE_ENDPOINT_TYPE_BULK,
	USB_DEVICE_ENDPOINT_TYPE_INTERRUPT,
};

/**
 * \brief Endpoint Size                                                                    
 */
enum usb_endpoint_size {
	USB_ENDPOINT_8_BYTE,
	USB_ENDPOINT_16_BYTE,
	USB_ENDPOINT_32_BYTE,
    USB_ENDPOINT_64_BYTE,
	USB_ENDPOINT_128_BYTE,
	USB_ENDPOINT_256_BYTE,
	USB_ENDPOINT_512_BYTE,
	USB_ENDPOINT_1023_BYTE,
};

/**
 * \brief Link Power Management Handshake.
 */
enum usb_lpm_mode {
	USB_LPM_NOT_SUPPORT,
	USB_LPM_ACK,
	USB_LPM_NYET,
};

/**
 * \brief module structure.
 */
struct usb_module;

/**
 * \brief Host callback functions types.
 */
typedef void (*usb_host_callback_t)(struct usb_module *module_inst);
typedef void (*usb_host_pipe_callback_t)(struct usb_module *module_inst, void *);

/** 
 * \brief Device callback functions types.
 */
typedef void (*usb_device_callback_t)(struct usb_module *module_inst);
typedef void (*usb_device_endpoint_callback_t)(struct usb_module *module_inst,void* pointer);


/** USB configurations */
struct usb_config {
	/** 1 for host, 0 for device. */
	bool select_host_mode;
	/** When \c true the module is enabled during standby. */
	bool run_in_standby;
	/** Generic Clock Generator source channel. */
	enum gclk_generator source_generator;
	/** speed mode */
	enum usb_speed speed_mode;
};

/**
 * \brief USB software device instance structure.
 *
 * USB software instance structure, used to retain software state information
 * of an associated hardware module instance.
 *
 */
struct usb_module {
	/** Hardware module pointer of the associated USB peripheral. */
	Usb *hw;

	/** Array to store host related callback functions */
	usb_host_callback_t host_callback[USB_HOST_CALLBACK_N];
	usb_host_pipe_callback_t host_pipe_callback[USB_PIPE_NUM][USB_HOST_PIPE_CALLBACK_N];
	/** Bit mask for host callbacks registered */
	uint8_t host_registered_callback_mask;
	/** Bit mask for host callbacks enabled */
	uint8_t host_enabled_callback_mask;
	/** Bit mask for host pipe callbacks registered */
	uint8_t host_pipe_registered_callback_mask[USB_PIPE_NUM];
	/** Bit mask for host pipe callbacks enabled */
	uint8_t host_pipe_enabled_callback_mask[USB_PIPE_NUM];

	/** Array to store device related callback functions */
	usb_device_callback_t device_callback[USB_DEVICE_CALLBACK_N];
	usb_device_endpoint_callback_t device_endpoint_callback[USB_EPT_NUM][USB_DEVICE_EP_CALLBACK_N];
	/** Bit mask for device callbacks registered */
	uint16_t device_registered_callback_mask;
	/** Bit mask for device callbacks enabled */
	uint16_t device_enabled_callback_mask;
	/** Bit mask for device endpoint callbacks registered */
	uint8_t deivce_endpoint_registered_callback_mask[USB_EPT_NUM];
	/** Bit mask for device endpoint callbacks enabled */
	uint8_t device_endpoint_enabled_callback_mask[USB_EPT_NUM];
};

/** USB host piple configurations */
struct usb_host_pipe_config {
	/** device address */
	uint8_t device_address;
	/** endpoint address  */
	uint8_t endpoint_address;
	/** Pipe type */
	enum usb_host_pipe_type pipe_type;
	/** interval */
	uint8_t binterval;
	/** pipe size */
	uint16_t size;
};

/** USB device endpoint configurations */
struct usb_device_endpoint_config {
	/** device address */
	uint8_t ep_address;
	/** endpoint size */
	enum usb_endpoint_size ep_size;
	/** automatic zero length packet mode, 1 for enable, 0 for disable */
	bool auto_zlp;
	/** type of endpoint with Bank */
	enum usb_device_endpoint_type ep_type;
};

/** USB host piple callback parameter structure */
struct usb_pipe_callback_parameter {
	/** current pipe number */
	uint8_t pipe_num;
	/** pipe error status */
	uint8_t pipe_error_status;
	/** actual transfered data size */
	uint16_t transfered_size;
	/** required data size */
	uint16_t required_size;
};

/** USB device endpoint callback parameter structure */
struct usb_endpoint_callback_parameter {
	uint16_t received_bytes;
	uint16_t sent_bytes;
	uint16_t out_buffer_size;
	uint8_t endpoint_address;
};

/** define macros for device that is not realized in header file */
#define USB_DEVICE_EPINTENCLR_TRCPT0		USB_DEVICE_EPINTENCLR_TRCPT(1)
#define USB_DEVICE_EPINTENCLR_TRCPT1		USB_DEVICE_EPINTENCLR_TRCPT(2)
#define USB_DEVICE_EPINTENCLR_TRFAIL0		USB_DEVICE_EPINTENCLR_TRFAIL(1)
#define USB_DEVICE_EPINTENCLR_TRFAIL1		USB_DEVICE_EPINTENCLR_TRFAIL(2)
#define USB_DEVICE_EPINTENCLR_STALL0		USB_DEVICE_EPINTENCLR_STALL(1)
#define USB_DEVICE_EPINTENCLR_STALL1		USB_DEVICE_EPINTENCLR_STALL(2)

#define USB_DEVICE_EPINTENSET_TRCPT0		USB_DEVICE_EPINTENSET_TRCPT(1)
#define USB_DEVICE_EPINTENSET_TRCPT1		USB_DEVICE_EPINTENSET_TRCPT(2)
#define USB_DEVICE_EPINTENSET_TRFAIL0		USB_DEVICE_EPINTENSET_TRFAIL(1)
#define USB_DEVICE_EPINTENSET_TRFAIL1		USB_DEVICE_EPINTENSET_TRFAIL(2)
#define USB_DEVICE_EPINTENSET_STALL0		USB_DEVICE_EPINTENSET_STALL(1)
#define USB_DEVICE_EPINTENSET_STALL1		USB_DEVICE_EPINTENSET_STALL(2)

#define USB_DEVICE_EPINTFLAG_TRCPT0			USB_DEVICE_EPINTFLAG_TRCPT(1)
#define USB_DEVICE_EPINTFLAG_TRCPT1			USB_DEVICE_EPINTFLAG_TRCPT(2)
#define USB_DEVICE_EPINTFLAG_TRFAIL0		USB_DEVICE_EPINTFLAG_TRFAIL(1)
#define USB_DEVICE_EPINTFLAG_TRFAIL1		USB_DEVICE_EPINTFLAG_TRFAIL(2)
#define USB_DEVICE_EPINTFLAG_STALL0			USB_DEVICE_EPINTFLAG_STALL(1)
#define USB_DEVICE_EPINTFLAG_STALL1			USB_DEVICE_EPINTFLAG_STALL(2)

#define USB_DEVICE_EPSTATUSSET_STALLRQ0		USB_DEVICE_EPSTATUSSET_STALLRQ(1)
#define USB_DEVICE_EPSTATUSSET_STALLRQ1		USB_DEVICE_EPSTATUSSET_STALLRQ(2)
#define USB_DEVICE_EPSTATUSCLR_STALLRQ0		USB_DEVICE_EPSTATUSCLR_STALLRQ(1)
#define USB_DEVICE_EPSTATUSCLR_STALLRQ1		USB_DEVICE_EPSTATUSCLR_STALLRQ(2)

/** USB simple operation functions */
void usb_enable(struct usb_module *module_inst);
void usb_disable(struct usb_module *module_inst);
static inline uint8_t usb_get_state_machine_status(struct usb_module *module_inst)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	return module_inst->hw->HOST.FSMSTATUS.reg;
}
/** USB init functions */
void usb_get_config_defaults(struct usb_config *module_config);
enum status_code usb_init(struct usb_module *module_inst, Usb *const hw,
		struct usb_config *module_config);

/** Host simple operation functions*/
static inline void usb_host_enable(struct usb_module *module_inst)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	module_inst->hw->HOST.CTRLB.bit.VBUSOK = 1;
}
static inline void usb_host_send_reset(struct usb_module *module_inst)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	module_inst->hw->HOST.CTRLB.bit.BUSRESET = 1;
}
static inline void usb_host_enable_sof(struct usb_module *module_inst)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	module_inst->hw->HOST.CTRLB.bit.SOFE = 1;
}
static inline void usb_host_disable_sof(struct usb_module *module_inst)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	module_inst->hw->HOST.CTRLB.bit.SOFE = 0;
}
static inline bool usb_host_is_sof_enabled(struct usb_module *module_inst)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	return module_inst->hw->HOST.CTRLB.bit.SOFE;
}
static inline void usb_host_send_resume(struct usb_module *module_inst)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	module_inst->hw->HOST.CTRLB.bit.RESUME= 1;
}
static inline void usb_host_send_l1_resume(struct usb_module *module_inst)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	module_inst->hw->HOST.CTRLB.bit.L1RESUME = 1;
}
static inline enum usb_speed usb_host_get_speed(struct usb_module *module_inst)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	if (module_inst->hw->HOST.STATUS.bit.SPEED == 0) {
		return USB_SPEED_FULL;
	} else {
		return USB_SPEED_LOW;
	}
}
static inline uint16_t usb_host_get_frame_number(struct usb_module *module_inst)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	return (uint16_t)(module_inst->hw->HOST.FNUM.bit.FNUM);
}
/**
 * \brief USB device attach to the bus
 *
 * \param module_inst pointer to USB device module instance
 */
static inline void usb_device_attach(struct usb_module *module_inst)
{
	module_inst->hw->DEVICE.CTRLB.reg &= ~USB_DEVICE_CTRLB_DETACH;
}

/**
 * \brief USB device detach from the bus
 *
 * \param module_inst pointer to USB device module instance
 */
static inline void usb_device_detach(struct usb_module *module_inst)
{
	module_inst->hw->DEVICE.CTRLB.reg |= USB_DEVICE_CTRLB_DETACH;
}

/**
 * \brief Get the speed mode of USB device
 *
 * \param module_inst pointer to USB device module instance
 * \return enum usb_speed mode
 */
static inline enum usb_speed usb_device_get_speed(struct usb_module *module_inst)
{
	if (!(module_inst->hw->DEVICE.STATUS.reg & USB_DEVICE_STATUS_SPEED_Msk)) {
		return USB_SPEED_FULL; 
	} else {
		return USB_SPEED_LOW;
	}
}

/**
 * \brief Get the address of USB device
 *
 * \param module_inst pointer to USB device module instance
 * \return usb device address value
 */
static inline uint8_t usb_device_get_address(struct usb_module *module_inst)
{
	return ((uint8_t)(module_inst->hw->DEVICE.DADD.bit.DADD));
}

/**
 * \brief Set the speed mode of USB device
 *
 * \param module_inst pointer to USB device module instance
 * \param usb device address value
 */
static inline void usb_device_set_address(struct usb_module *module_inst, uint8_t address)
{
	module_inst->hw->DEVICE.DADD.reg = USB_DEVICE_DADD_ADDEN | address;
}

/**
 * \brief Get the frame number of USB device
 *
 * \param module_inst pointer to USB device module instance
 * \return usb device frame number value 
 */
static inline uint16_t usb_device_get_frame_number(struct usb_module *module_inst)
{
	return ((uint16_t)(module_inst->hw->DEVICE.FNUM.bit.FNUM));
}
/**
 * \brief Get the micro-frame number of USB device
 *
 * \param module_inst pointer to USB device module instance
 * \return usb device micro-frame number value 
 */
static inline uint16_t usb_device_get_micro_frame_number(struct usb_module *module_inst)
{
	return ((uint16_t)(module_inst->hw->DEVICE.FNUM.bit.MFNUM));
}

static inline void usb_send_remote_wake_up(struct usb_module *module_inst)
{
	module_inst->hw->DEVICE.CTRLB.reg |= USB_DEVICE_CTRLB_UPRSM;
}

/** Host interrupt functions*/
enum status_code usb_host_register_callback(struct usb_module *module_inst,
		enum usb_host_callback callback_type,
		usb_host_callback_t callback_func);
enum status_code usb_host_unregister_callback(struct usb_module *module_inst,
		enum usb_host_callback callback_type);
enum status_code usb_host_enable_callback(struct usb_module *module_inst,
		enum usb_host_callback callback_type);
enum status_code usb_host_disable_callback(struct usb_module *module_inst,
		enum usb_host_callback callback_type);

/** Device interrupt functions*/
enum status_code usb_device_register_callback(struct usb_module *module_inst,
		enum usb_device_callback callback_type,
		usb_device_callback_t callback_func);
enum status_code usb_device_unregister_callback(struct usb_module *module_inst,
		enum usb_device_callback callback_type);
enum status_code usb_device_enable_callback(struct usb_module *module_inst,
		enum usb_device_callback callback_type);
enum status_code usb_device_disable_callback(struct usb_module *module_inst,
		enum usb_device_callback callback_type);

/** Pipe init functions */
void usb_host_pipe_get_config_defaults(struct usb_host_pipe_config *ep_config);
enum status_code usb_host_pipe_set_config(struct usb_module *module_inst, uint8_t pipe_num,
		struct usb_host_pipe_config *ep_config);
enum status_code usb_host_pipe_get_config(struct usb_module *module_inst, uint8_t pipe_num,
		struct usb_host_pipe_config *ep_config);

/** Endpoint initial functions */
void usb_device_endpoint_get_config_defaults(struct usb_device_endpoint_config *ep_config);
enum status_code usb_device_endpoint_set_config(struct usb_module *module_inst,
		struct usb_device_endpoint_config *ep_config);

/** Pipe interrupt functions */
enum status_code usb_host_pipe_register_callback(
		struct usb_module *module_inst, uint8_t pipe_num,
		enum usb_host_pipe_callback callback_type,
		usb_host_pipe_callback_t callback_func);
enum status_code usb_host_pipe_unregister_callback(
		struct usb_module *module_inst, uint8_t pipe_num,
		enum usb_host_pipe_callback callback_type);
enum status_code usb_host_pipe_enable_callback(
		struct usb_module *module_inst, uint8_t pipe_num,
		enum usb_host_pipe_callback callback_type);
enum status_code usb_host_pipe_disable_callback(
		struct usb_module *module_inst, uint8_t pipe_num,
		enum usb_host_pipe_callback callback_type);

/** Endpoint interrupt functions */
enum status_code usb_device_endpoint_register_callback(
		struct usb_module *module_inst, uint8_t ep_num,
		enum usb_device_endpoint_callback callback_type,
		usb_device_endpoint_callback_t callback_func);
enum status_code usb_device_endpoint_unregister_callback(
		struct usb_module *module_inst, uint8_t ep_num,
		enum usb_device_endpoint_callback callback_type);
enum status_code usb_device_endpoint_enable_callback(
		struct usb_module *module_inst, uint8_t ep,
		enum usb_device_endpoint_callback callback_type);
enum status_code usb_device_endpoint_disable_callback(
		struct usb_module *module_inst, uint8_t ep,
		enum usb_device_endpoint_callback callback_type);



/** Pipe high level job functions */
enum status_code usb_host_pipe_setup_job(struct usb_module *module_inst,
		uint8_t pipe_num, uint8_t *buf);
enum status_code usb_host_pipe_read_job(struct usb_module *module_inst,
		uint8_t pipe_num, uint8_t *buf, uint32_t buf_size);
enum status_code usb_host_pipe_write_job(struct usb_module *module_inst,
		uint8_t pipe_num, uint8_t *buf, uint32_t buf_size);
enum status_code usb_host_pipe_abort_job(struct usb_module *module_inst, uint8_t pipe_num);

/**
 * \brief endpoint number n write job
 *
 * \param module_inst pointer to USB device module instance
 * \param ep_num number of endpoint
 * \param pbuf pointer to buffer
 * \param buf_size size of buffer
 * \return status code
 */
enum status_code usb_device_endpoint_write_buffer_job(struct usb_module *module_inst,uint8_t ep_num,
		uint8_t* pbuf, uint32_t buf_size);
/**
 * \brief endpoint number n read job
 *
 * \param module_inst pointer to USB device module instance
 * \param ep_num number of endpoint
 * \param pbuf pointer to buffer
 * \param buf_size size of buffer
 * \return status code
 */
enum status_code usb_device_endpoint_read_buffer_job(struct usb_module *module_inst,uint8_t ep_num,
		uint8_t* pbuf, uint32_t buf_size);

/**
 * \brief endpoint number n setup job
 *
 * \param module_inst pointer to USB device module instance
 * \param pbuf pointer to buffer
 * \return status code
 */
enum status_code usb_device_endpoint_setup_buffer_job(struct usb_module *module_inst,
		uint8_t* pbuf);

/** Pipe simple operation functions*/
static inline void usb_host_pipe_freeze(struct usb_module *module_inst, uint8_t pipe_num)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	module_inst->hw->HOST.HostPipe[pipe_num].PSTATUSSET.reg = USB_HOST_PSTATUSSET_PFREEZE;
}
static inline void usb_host_pipe_unfreeze(struct usb_module *module_inst, uint8_t pipe_num)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	module_inst->hw->HOST.HostPipe[pipe_num].PSTATUSCLR.reg = USB_HOST_PSTATUSCLR_PFREEZE;
}
static inline bool usb_host_pipe_is_frozen(struct usb_module *module_inst, uint8_t pipe_num)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	return (module_inst->hw->HOST.HostPipe[pipe_num].PSTATUS.bit.PFREEZE == 1);
}
static inline void usb_host_pipe_set_toggle(struct usb_module *module_inst, uint8_t pipe_num)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	module_inst->hw->HOST.HostPipe[pipe_num].PSTATUSSET.reg = USB_HOST_PSTATUSSET_DTGL;
}
static inline void usb_host_pipe_clear_toggle(struct usb_module *module_inst, uint8_t pipe_num)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	module_inst->hw->HOST.HostPipe[pipe_num].PSTATUSCLR.reg = USB_HOST_PSTATUSCLR_DTGL;
}
void usb_host_pipe_set_auto_zlp(struct usb_module *module_inst, uint8_t pipe_num, bool value);

/**
 * \brief current endpoint abort
 *
 * \param endpoint address (direction & number)
 * \return null
 */
void usb_device_endpoint_abort_job(uint8_t ep);

/**
 * \brief Is endpoint halted
 *
 * \param endpoint address (direction & number)
 * \param endpoint enable flag
 * \return true or false
 */
bool usb_device_endpoint_is_halted(uint8_t ep, bool* ep_enable);

/**
 * \brief set endpoint halt
 *
 * \param endpoint address (direction & number)
 * \return null
 */
void usb_device_endpoint_set_halt(uint8_t ep);

/**
 * \brief clear endpoint from halted
 *
 * \param endpoint address (direction & number)
 * \return null
 */
void usb_device_endpoint_clear_halt(uint8_t ep);

#endif /* USB_H_INCLUDED */
