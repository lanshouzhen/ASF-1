/**
 * \file
 *
 * \brief USB Device Mass Storage Class (MSC) interface definitions.
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

/**
 * \defgroup asfdoc_udi_msc_group USB Device Interface (UDI) for Mass Storage Class (MSC)
 *
 * USB Device Interface (UDI) for Mass Storage Class (MSC) provides an
 * interface for the configuration and management of USB MSC storage device.
 *
 * The outline of this documentation is as follows:
 * - \ref asfdoc_udi_msc_api_overview
 * - \ref asfdoc_udi_msc_exqsg
 * - \ref asfdoc_udi_msc_config_examples
 *
 * For more details for Atmel Software Framework (ASF) USB Device Stack , please
 * refer to following Application Notes:
 * - <a href="http://www.atmel.com/dyn/resources/prod_documents/doc8360.pdf">
 *   AVR4900: ASF - USB Device Stack</a>
 * - <a href="http://www.atmel.com/dyn/resources/prod_documents/doc8410.pdf">
 *   AVR4920: ASF - USB Device Stack - Compliance and Performance Figures</a>
 * - <a href="http://www.atmel.com/dyn/resources/prod_documents/doc8411.pdf">
 *   AVR4921: ASF - USB Device Stack Differences between ASF V1 and V2</a>
 *
 * \section asfdoc_udi_msc_api_overview API Overview
 * @{
 */

/**
 * \name UDI MSC interface for UDC
 * @{
 */
/** Global structure which contains standard UDI interface for UDC */
extern UDC_DESC_STORAGE udi_api_t udi_api_msc;
//@}

/**
 * \name USB interface descriptors
 *
 * The following structures provide predefined USB interface descriptors.
 * It must be used to define the final USB descriptors.
 */
//@{

/** Interface descriptor structure for MSC */
typedef struct {
	usb_iface_desc_t iface;
	usb_ep_desc_t ep_in;
	usb_ep_desc_t ep_out;
} udi_msc_desc_t;

/** By default no string associated to this interface */
#ifndef UDI_MSC_STRING_ID
#define UDI_MSC_STRING_ID     0
#endif

/** MSC endpoints size for full speed */
#define UDI_MSC_EPS_SIZE_FS   64
/** MSC endpoints size for high speed */
#define UDI_MSC_EPS_SIZE_HS   512

/** Content of MSC interface descriptor for all speeds */
#define UDI_MSC_DESC      \
   .iface.bLength             = sizeof(usb_iface_desc_t),\
   .iface.bDescriptorType     = USB_DT_INTERFACE,\
   .iface.bInterfaceNumber    = UDI_MSC_IFACE_NUMBER,\
   .iface.bAlternateSetting   = 0,\
   .iface.bNumEndpoints       = 2,\
   .iface.bInterfaceClass     = MSC_CLASS,\
   .iface.bInterfaceSubClass  = MSC_SUBCLASS_TRANSPARENT,\
   .iface.bInterfaceProtocol  = MSC_PROTOCOL_BULK,\
   .iface.iInterface          = UDI_MSC_STRING_ID,\
   .ep_in.bLength             = sizeof(usb_ep_desc_t),\
   .ep_in.bDescriptorType     = USB_DT_ENDPOINT,\
   .ep_in.bEndpointAddress    = UDI_MSC_EP_IN,\
   .ep_in.bmAttributes        = USB_EP_TYPE_BULK,\
   .ep_in.bInterval           = 0,\
   .ep_out.bLength            = sizeof(usb_ep_desc_t),\
   .ep_out.bDescriptorType    = USB_DT_ENDPOINT,\
   .ep_out.bEndpointAddress   = UDI_MSC_EP_OUT,\
   .ep_out.bmAttributes       = USB_EP_TYPE_BULK,\
   .ep_out.bInterval          = 0,

/** Content of MSC interface descriptor for full speed only */
#define UDI_MSC_DESC_FS   {\
   UDI_MSC_DESC \
   .ep_in.wMaxPacketSize      = LE16(UDI_MSC_EPS_SIZE_FS),\
   .ep_out.wMaxPacketSize     = LE16(UDI_MSC_EPS_SIZE_FS),\
   }

/** Content of MSC interface descriptor for high speed only */
#define UDI_MSC_DESC_HS   {\
   UDI_MSC_DESC \
   .ep_in.wMaxPacketSize      = LE16(UDI_MSC_EPS_SIZE_HS),\
   .ep_out.wMaxPacketSize     = LE16(UDI_MSC_EPS_SIZE_HS),\
   }
//@}

/**
 * \brief Process the background read/write commands
 *
 * Routine called by the main loop
 */
bool udi_msc_process_trans(void);

/**
 * \brief Transfers data to/from USB MSC endpoints
 *
 *
 * \param[in]    b_read     Memory to USB, if true
 * \param[inout] block      Buffer on Internal RAM to send or fill
 * \param[in]    block_size Buffer size to send or fill
 * \param[in]    callback   Function to call at the end of transfer.
 *                          If NULL then the routine exit when transfer is finish.
 *
 * \return \c 1 if function was successfully done, otherwise \c 0.
 */
bool udi_msc_trans_block(bool b_read, uint8_t * block, iram_size_t block_size,
		void (*callback) (udd_ep_status_t status, iram_size_t n, udd_ep_id_t ep));

//@}

/**
 * \page asfdoc_udi_msc_exqsg Quick start guide for USB device Mass Storage module (UDI MSC)
 *
 * This is the quick start guide for the \ref asfdoc_udi_msc_group
 * "USB device interface MSC module (UDI MSC)" with step-by-step instructions on
 * how to configure and use the modules in a selection of use cases.
 *
 * The use cases contain several code fragments. The code fragments in the
 * steps for setup can be copied into a custom initialization function, while
 * the steps for usage can be copied into, e.g., the main application function.
 *
 * \section udi_msc_basic_use_case Basic use case
 * In this basic use case, the "USB MSC (Single Interface Device)" module is used.
 * The "USB MSC (Composite Device)" module usage is described in \ref udi_msc_use_cases
 * "Advanced use cases".
 *
 * \section udi_msc_basic_use_case_setup Setup steps
 * As a USB device, it follows common USB device setup steps. Please refer to
 * \ref asfdoc_udc_basic_use_case_setup "USB Device Basic Setup".
 *
 * Common abstraction layer is applied for memory interfaces. It provides interfaces between:
 * Memory and USB, Memory and RAM, Memory and Memory.
 *
 *
 /** \name Control Interface
 */
//@{
/** \brief Initializes the LUN access locker.
 *
 * \return \c true if the locker was successfully initialized, else \c false.
 */
extern bool ctrl_access_init(void);

/** \brief Returns the number of LUNs.
 *
 * \return Number of LUNs in the system.
 */
extern U8 get_nb_lun(void);

/** \brief Returns the current LUN.
 *
 * \return Current LUN.
 *
 * \todo Implement.
 */
extern U8 get_cur_lun(void);

/** \brief Tests the memory state and initializes the memory if required.
 *
 * The TEST UNIT READY SCSI primary command allows an application client to poll
 * a LUN until it is ready without having to allocate memory for returned data.
 *
 * This command may be used to check the media status of LUNs with removable
 * media.
 *
 * \param lun Logical Unit Number.
 *
 * \return Status.
 */
extern Ctrl_status mem_test_unit_ready(U8 lun);

/** \brief Returns the address of the last valid sector (512 bytes) in the
 *         memory.
 *
 * \param lun           Logical Unit Number.
 * \param u32_nb_sector Pointer to the address of the last valid sector.
 *
 * \return Status.
 */
extern Ctrl_status mem_read_capacity(U8 lun, U32 *u32_nb_sector);

/** \brief Returns the size of the physical sector.
 *
 * \param lun Logical Unit Number.
 *
 * \return Sector size (unit: 512 bytes).
 */
extern U8 mem_sector_size(U8 lun);

/** \brief Unload/load the medium.
 *
 * \param lun Logical Unit Number.
 * \param unload \c true to unload the medium, \c false to load the medium.
 *
 * \return \c true if unload/load success, else \c false.
 */
extern bool mem_unload(U8 lun, bool unload);

/** \brief Returns the write-protection state of the memory.
 *
 * \param lun Logical Unit Number.
 *
 * \return \c true if the memory is write-protected, else \c false.
 *
 * \note Only used by removable memories with hardware-specific write
 *       protection.
 */
extern bool mem_wr_protect(U8 lun);

/** \brief Tells whether the memory is removable.
 *
 * \param lun Logical Unit Number.
 *
 * \return \c true if the memory is removable, else \c false.
 */
extern bool mem_removal(U8 lun);

/** \brief Returns a pointer to the LUN name.
 *
 * \param lun Logical Unit Number.
 *
 * \return Pointer to the LUN name string.
 */
extern const char *mem_name(U8 lun);
//@}

 /**
 * \name MEM <-> USB Interface
 *
 */
//@{

 /**
 * \brief Transfers data from the memory to USB.
 *
 * \param[in] lun       Logical Unit Number.
 * \param[in] addr      Address of first memory sector to read.
 * \param[in] nb_sector Number of sectors to transfer.
 *
 * \return Status.
 */
extern Ctrl_status memory_2_usb(U8 lun, U32 addr, U16 nb_sector);

/** \brief Transfers data from USB to the memory.
 *
 * \param[in] lun       Logical Unit Number.
 * \param[in] addr      Address of first memory sector to write.
 * \param[in] nb_sector Number of sectors to transfer.
 *
 * \return Status.
 */
extern Ctrl_status usb_2_memory(U8 lun, U32 addr, U16 nb_sector);
//@}

/** \name MEM <-> RAM Interface
 */
//@{

/** \brief Copies 1 data sector from the memory to RAM.
 *
 * \param[in] lun   Logical Unit Number.
 * \param[in] addr  Address of first memory sector to read.
 * \param[in] ram   Pointer to RAM buffer to write.
 *
 * \return Status.
 */
extern Ctrl_status memory_2_ram(U8 lun, U32 addr, void *ram);

/** \brief Copies 1 data sector from RAM to the memory.
 *
 * \param[in] lun   Logical Unit Number.
 * \param[in] addr  Address of first memory sector to write.
 * \param[in] ram   Pointer to RAM buffer to read.
 *
 * \return Status.
 */
extern Ctrl_status ram_2_memory(U8 lun, U32 addr, const void *ram);
//@}

/** \name Streaming MEM <-> MEM Interface
 */
//@{

/** Erroneous streaming data transfer ID.*/
#define ID_STREAM_ERR         0xFF

/** \brief Copies data from one memory to another.
 *
 * \param[in] src_lun   Source Logical Unit Number.
 * \param[in] src_addr  Source address of first memory sector to read.
 * \param[in] dest_lun  Destination Logical Unit Number.
 * \param[in] dest_addr Destination address of first memory sector to write.
 * \param[in] nb_sector Number of sectors to copy.
 *
 * \return Status.
 */
extern Ctrl_status stream_mem_to_mem(U8 src_lun, U32 src_addr, U8 dest_lun, U32 dest_addr, U16 nb_sector);

/** \brief Returns the state of a streaming data transfer.
 *
 * \param[in] id  Transfer ID.
 *
 * \return Status.
 *
 * \todo Implement.
 */
extern Ctrl_status stream_state(U8 id);

/*! \brief Stops a streaming data transfer.
 *
 * \param[in] id  Transfer ID.
 *
 * \return Number of remaining sectors.
 *
 * \todo Implement.
 */
extern U16 stream_stop(U8 id);
//@}

/**
 * \section udi_msc_basic_use_case_usage Usage steps
 *
 * \subsection udi_msc_basic_use_case_usage_code Example code
 * Content of conf_usb.h:
 * \code
	#define  USB_DEVICE_SERIAL_NAME  "12...EF" // Disk SN for MSC
	#define UDI_MSC_GLOBAL_VENDOR_ID \
	   'A', 'T', 'M', 'E', 'L', ' ', ' ', ' '
	#define UDI_MSC_GLOBAL_PRODUCT_VERSION \
	   '1', '.', '0', '0'
	#define UDI_MSC_ENABLE_EXT() my_callback_msc_enable()
	extern bool my_callback_msc_enable(void);
	#define UDI_MSC_DISABLE_EXT() my_callback_msc_disable()
	extern void my_callback_msc_disable(void);
	#include "udi_msc_conf.h" // At the end of conf_usb.h file
\endcode
 *
 * Add to application C-file:
 * \code
	 static bool my_flag_autorize_msc_transfert = false;
	 bool my_callback_msc_enable(void)
	 {
	    my_flag_autorize_msc_transfert = true;
	    return true;
	 }
	 void my_callback_msc_disable(void)
	 {
	    my_flag_autorize_msc_transfert = false;
	 }

	 void task(void)
	 {
	    udi_msc_process_trans();
	 }
\endcode
 *
 * \subsection udi_msc_basic_use_case_setup_flow Workflow
 * -# Ensure that conf_usb.h is available and contains the following configuration
 * which is the USB device MSC configuration:
 *   - \code #define USB_DEVICE_SERIAL_NAME  "12...EF" // Disk SN for MSC \endcode
 *     \note The USB serial number is mandatory when a MSC interface is used.
 *   - \code //! Vendor name and Product version of MSC interface
	#define UDI_MSC_GLOBAL_VENDOR_ID \
	   'A', 'T', 'M', 'E', 'L', ' ', ' ', ' '
	#define UDI_MSC_GLOBAL_PRODUCT_VERSION \
	   '1', '.', '0', '0' \endcode
 *     \note The USB MSC interface requires a vendor ID (8 ASCII characters)
 *     and a product version (4 ASCII characters).
 *   - \code #define UDI_MSC_ENABLE_EXT() my_callback_msc_enable()
	extern bool my_callback_msc_enable(void); \endcode
 *     \note After the device enumeration (detecting and identifying USB devices),
 *     the USB host starts the device configuration. When the USB MSC interface
 *     from the device is accepted by the host, the USB host enables this interface and the
 *     UDI_MSC_ENABLE_EXT() callback function is called and return true.
 *     Thus, when this event is received, the tasks which call
 *     udi_msc_process_trans() must be enabled.
 *   - \code #define UDI_MSC_DISABLE_EXT() my_callback_msc_disable()
	extern void my_callback_msc_disable(void); \endcode
 *     \note When the USB device is unplugged or is reset by the USB host, the USB
 *     interface is disabled and the UDI_MSC_DISABLE_EXT() callback function
 *     is called. Thus, it is recommended to disable the task which is called udi_msc_process_trans().
 * -# The MSC is automatically linked with memory control access component
 * which provides the memories interfaces. However, the memory data transfers
 * must be done outside USB interrupt routine. This is done in the MSC process
 * ("udi_msc_process_trans()") called by main loop:
 *   - \code  * void task(void) {
	udi_msc_process_trans();
	} \endcode
 * -# The MSC speed depends on task periodicity. To get the best speed
 * the notification callback "UDI_MSC_NOTIFY_TRANS_EXT" can be used to wakeup
 * this task (Example, through a mutex):
 *   - \code #define  UDI_MSC_NOTIFY_TRANS_EXT()    msc_notify_trans()
	void msc_notify_trans(void) {
	wakeup_my_task();
	} \endcode
 *
 * \section udi_msc_use_cases Advanced use cases
 * \ifnot ASF_MANUAL
 * For more advanced use of the UDI MSC module, see the following use cases:
 * - \subpage udi_msc_use_case_composite
 * - \subpage udc_use_case_1
 * - \subpage udc_use_case_2
 * - \subpage udc_use_case_3
 * - \subpage udc_use_case_5
 * - \subpage udc_use_case_6
 * - \subpage asfdoc_udc_device_cfg
 * - \subpage asfdoc_udc_vbus_monitoring
 * \else
 * For multiple interface use of UDI MSC module, see the following:
 * - \subpage udi_msc_use_case_composite
 *
 * For more advanced use of the UDI CDC module, see the following:
 * - \ref asfdoc_udc_use_cases.
 * \endif
 */

/**
 * \page udi_msc_use_case_composite MSC in a composite device
 *
 * A USB Composite Device is a USB Device which uses more than one USB class.
 * In this use case, the "USB MSC (Composite Device)" module is used to
 * create a USB composite device. Thus, this USB module can be associated with
 * another "Composite Device" module, like "USB HID Mouse (Composite Device)".
 *
 * Also, you can refer to application note
 * <A href="http://www.atmel.com/dyn/resources/prod_documents/doc8445.pdf">
 * AVR4902 ASF - USB Composite Device</A>.
 *
 * \section udi_msc_use_case_composite_setup Setup steps
 * For the setup code of this use case to work, the
 * \ref udi_msc_basic_use_case "basic use case" must be followed.
 *
 * \section udi_msc_use_case_composite_usage Usage steps
 *
 * \subsection udi_msc_use_case_composite_usage_code Example code
 * Content of conf_usb.h:
 * \code
	 #define USB_DEVICE_EP_CTRL_SIZE  64
	 #define USB_DEVICE_NB_INTERFACE (X+1)
	 #define USB_DEVICE_MAX_EP (X+2)

	 #define UDI_MSC_EP_IN  (X | USB_EP_DIR_IN)
	 #define UDI_MSC_EP_OUT (Y | USB_EP_DIR_OUT)
	 #define UDI_MSC_IFACE_NUMBER  X

	 #define UDI_COMPOSITE_DESC_T \
	    udi_msc_desc_t udi_msc; \
	    ...
	 #define UDI_COMPOSITE_DESC_FS \
	    .udi_msc = UDI_MSC_DESC, \
	    ...
	 #define UDI_COMPOSITE_DESC_HS \
	    .udi_msc = UDI_MSC_DESC, \
	    ...
	 #define UDI_COMPOSITE_API \
	    &udi_api_msc, \
	    ...
\endcode
 *
 * \subsection udi_msc_use_case_composite_usage_flow Workflow
 * -# Ensure that conf_usb.h is available and contains the following parameters
 * required for a USB composite device configuration:
 *   - \code // Endpoint control size, This must be:
	// - 8, 16, 32 or 64 for full speed device (8 is recommended to save RAM)
	// - 64 for a high speed device
	#define USB_DEVICE_EP_CTRL_SIZE  64
	// Total Number of interfaces on this USB device.
	// Add 1 for MSC.
	#define USB_DEVICE_NB_INTERFACE (X+1)
	// Total number of endpoints on this USB device.
	// This must include each endpoint for each interface.
	// Add 2 for MSC.
	#define USB_DEVICE_MAX_EP (X+2) \endcode
 * -# Ensure that conf_usb.h contains the description of
 * composite device:
 *   - \code // The endpoint numbers chosen by you for the MSC.
	// The endpoint numbers starting from 1.
	#define UDI_MSC_EP_IN  (X | USB_EP_DIR_IN)
	#define UDI_MSC_EP_OUT (Y | USB_EP_DIR_OUT)
	// The interface index of an interface starting from 0
	#define UDI_MSC_IFACE_NUMBER  X \endcode
 * -# Ensure that conf_usb.h contains the following parameters
 * required for a USB composite device configuration:
 *   - \code // USB Interfaces descriptor structure
	#define UDI_COMPOSITE_DESC_T \
	   ...
	   udi_msc_desc_t udi_msc; \
	   ...
	// USB Interfaces descriptor value for Full Speed
	#define UDI_COMPOSITE_DESC_FS \
	   ...
	   .udi_msc = UDI_MSC_DESC_FS, \
	   ...
	// USB Interfaces descriptor value for High Speed
	#define UDI_COMPOSITE_DESC_HS \
	   ...
	   .udi_msc = UDI_MSC_DESC_HS, \
	   ...
	// USB Interface APIs
	#define UDI_COMPOSITE_API \
	   ...
	   &udi_api_msc, \
	   ... \endcode
 *   - \note The descriptors order given in the four lists above must be the
 *     same as the order defined by all interface indexes. The interface index
 *     orders are defined through UDI_X_IFACE_NUMBER defines.
 */

/**
 * \page asfdoc_udi_msc_config_examples Configuration File Examples
 *
 * \section asfdoc_udi_msc_config_examples_1 conf_usb.h
 * \include module_config\conf_usb.h
 *
 * \section asfdoc_udi_msc_config_examples_2 conf_clock.h
 *
 * \subsection asfdoc_udi_msc_config_examples_2_1 XMEGA (USB)
 * \include example\atxmega128b1_xmega_b1_xplained\conf_clock.h
 *
 * \subsection asfdoc_udi_msc_config_examples_2_2 AT32UC3A0, AT32UC3A1, AT32UC3B devices (USBB)
 * \include example\at32uc3a0512_evk1100\conf_clock.h
 *
 * \subsection asfdoc_udi_msc_config_examples_2_3 AT32UC3A3, AT32UC3A4 devices (USBB with high speed support)
 * \include example\at32uc3a3256_evk1104\conf_clock.h
 *
 * \subsection asfdoc_udi_msc_config_examples_2_4 AT32UC3C, ATUCXXD, ATUCXXL3U, ATUCXXL4U devices (USBC)
 * \include example\at32uc3c0512c_uc3c_ek\conf_clock.h
 *
 * \subsection asfdoc_udi_msc_config_examples_2_5 SAM3S, SAM3SD, SAM4S devices (UPD: USB Peripheral Device)
 * \include example\sam3s4c_sam3s_ek\conf_clock.h
 *
 * \subsection asfdoc_udi_msc_config_examples_2_6 SAM3U device (UPDHS: USB Peripheral Device High Speed)
 * \include example\sam3u4e_sam3u_ek\conf_clock.h
 *
 * \subsection asfdoc_udi_msc_config_examples_2_7 SAM3X, SAM3A devices (UOTGHS: USB OTG High Speed)
 * \include example\sam3x8h_sam3x_ek\conf_clock.h
 *
 * \section asfdoc_udi_msc_config_examples_3 conf_clocks.h
 *
 * \subsection asfdoc_udi_msc_config_examples_3_1 SAMD devices (USB)
 * \include example\samd21j18a_samd21_xplained_pro\conf_clocks.h
 *
 * \section asfdoc_udi_msc_config_examples_4 conf_board.h
 *
 * \subsection asfdoc_udi_msc_config_examples_4_1 AT32UC3A0, AT32UC3A1, AT32UC3B devices (USBB)
 * \include example\at32uc3a0512_evk1100\conf_board.h
 *
 * \subsection asfdoc_udi_msc_config_examples_4_2 AT32UC3A3, AT32UC3A4 devices (USBB with high speed support)
 * \include example\at32uc3a3256_evk1104\conf_board.h
 *
 * \subsection asfdoc_udi_msc_config_examples_4_3 AT32UC3C, ATUCXXD, ATUCXXL3U, ATUCXXL4U devices (USBC)
 * \include example\at32uc3c0512c_uc3c_ek\conf_board.h
 *
 * \subsection asfdoc_udi_msc_config_examples_4_4 SAM3X, SAM3A devices (UOTGHS: USB OTG High Speed)
 * \include example\sam3x8h_sam3x_ek\conf_board.h
 *
 * \subsection asfdoc_udi_msc_config_examples_4_5 SAMD devices (USB)
 * \include example\samd21j18a_samd21_xplained_pro\conf_board.h
 *
 * \section asfdoc_udi_msc_config_examples_5 conf_access.h
 *
 * \subsection asfdoc_udi_msc_config_examples_5_1 AT32UC3A0, AT32UC3A1, AT32UC3B devices (USBB)
 * \include example\at32uc3a0512_evk1100\conf_access.h
 *
 * \subsection asfdoc_udi_msc_config_examples_5_2 AT32UC3A3, AT32UC3A4 devices (USBB with high speed support)
 * \include example\at32uc3a3256_evk1104\conf_access.h
 *
 * \subsection asfdoc_udi_msc_config_examples_5_3 AT32UC3C, ATUCXXD, ATUCXXL3U, ATUCXXL4U devices (USBC)
 * \include example\at32uc3c0512c_uc3c_ek\conf_access.h
 *
 * \subsection asfdoc_udi_msc_config_examples_5_4 SAM3X, SAM3A devices (UOTGHS: USB OTG High Speed)
 * \include example\sam3x8h_sam3x_ek\conf_access.h
 *
 * \subsection asfdoc_udi_msc_config_examples_5_5 SAMD devices (USB)
 * \include example\samd21j18a_samd21_xplained_pro\conf_access.h
 */

