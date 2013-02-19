/**
 * @file main.c
 *
 * @brief MAC Example Beacon Application - Coordinator
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
 */

/**
* \mainpage
* \section preface Preface
* This is the reference manual for IEEE 802.15.4 MAC Beacon Application - Coordinator
* \section toc Table of Contents
*  - \subpage overview
*  -  \b Application \b Interface(API)
*    - \ref group_mac
*    - \ref group_pal
*    - \ref group_tal
*    - \ref group_resources
*  - \subpage main_files
*  - \subpage devsup
*  - \subpage compinfo
*  - \subpage references
*  - \subpage contactinfo
*/

/**
 * \page overview Overview
 * \section intro Introduction
 * The basic MAC Example Beacon Application deploys a beacon-enabled network with star topology.
 * The coordinator starts a PAN at channel DEFAULT_CHANNEL with the PAN ID DEFAULT_PAN_ID.
 *
 * The coordinator starts a beaconing network and transmits user data within beacon payload of transmitted beacon frames.
 * The device scans for this network,sync up with the coordinator and associates to the coordinator.The devices receive
 * these beacon frames, extract the receveived user data from the coordinator ,print the received data on the hyper
 * terminal and also sends the extracted beacon payload back to the coordinator.
 *
 * The coordinator also transmits broadcast data frames periodically.The devices receive these broadcast data frames
 * and increase a counter.
 *
 * The coordinator also puts the data in the indirect queue periodically and transmits data frames based on the
 * periodic poll request from the device.
 *
 * The results of the proper data transmission/reception are printed to a terminal program via Serial I/O (UART or USB).
 *
 * For demonstration purposes the coordinator's configuration is limited to associate maximum of 100 devices.
 */


/** \page main_files Application Files
 * - main.c\n                      Application main file.

 * \page devsup Device Support
 * - \b ATXMEGA256A3BU
 *                     - <A href="http://www.atmel.com/tools/xmega-a3buxplained.aspx"> \b   XMEGA-A3BU Xplained </A>  <A href="http://store.atmel.com/PartDetail.aspx?q=p:10500293">\a Buy </A>\n
 * - \b ATXMEGA256A3U-Zigbit (USB )
 * - \b Atmega256rfr2 Xplained Pro
 * - \b UC3A3256S
*                      - <A href="http://www.atmel.com/tools/rz600.aspx"> \b RZ600</A> <A href="http://store.atmel.com/PartDetail.aspx?q=p:10500245;c:100118">\a Buy </A>\n
 * \page compinfo Compilation Info
 * This software was written for the GNU GCC and IAR for AVR.
 * Other compilers may or may not work.
 *
 * \page references References
 * 1)  <A href="http://www.atmel.com/images/doc8412.pdf">AVR2025 IEEE 802.15.4 MAC Reference Manual\n </A>.
 * 2)  IEEE Std 802.15.4-2006 Part 15.4: Wireless Medium Access Control (MAC)
 *     and Physical Layer (PHY) Specifications for Low-Rate Wireless Personal Area
 *     Networks (WPANs).\n\n
 * 3)  AVR Wireless Support <A href="http://avr@atmel.com">avr@atmel.com</A>.\n
 * \page contactinfo Contact Information
 * For further information,visit
 * <A href="http://www.atmel.com/avr">www.atmel.com</A>.\n
 */

 /* === INCLUDES ============================================================ */
#include <string.h>
#include <inttypes.h>
#include <stdio.h>
#include "conf_board.h"
#include "avr2025_mac.h"
#include "led.h"
#include "delay.h"
#include "common_sw_timer.h"
#include "sio2host.h"
#include "ioport.h"	
#include <asf.h>
/* === TYPES =============================================================== */

/** This type definition of a structure can store the short address and the
 *  extended address of a device.
 */
typedef struct associated_device_tag
{
    uint16_t short_addr;
    uint64_t ieee_addr;
} associated_device_t;

/**
 * This enum store the current state of the coordinator.
 */
typedef enum coord_state_tag
{
    COORD_STARTING = 0,
    COORD_RUNNING
} coord_state_t;

/* === MACROS ============================================================== */

#define DEFAULT_PAN_ID                  CCPU_ENDIAN_TO_LE16(0xBABE)

/** Defines the short address of the coordinator. */
#define COORD_SHORT_ADDR                (0x0000)
/** Defines the maximum number of devices this coordinator will handle. */
#define MAX_NUMBER_OF_DEVICES           (100)

#define CHANNEL_OFFSET                  (0)

#define SCAN_CHANNEL                    (1ul << current_channel)

/** Defines the scan duration time. */
#define SCAN_DURATION_COORDINATOR       (5)

/** Defines the default Beacon Order. */
#define DEFAULT_BO                      (5)
/** Defines the default Superframe Order. */
#define DEFAULT_SO                      (5)

/**
 * Defines the length of the beacon payload delivered to the devices.
 * This is the text "Atmel beacon demo" + one space + one uin8t_t variable.
 */
#define BEACON_PAYLOAD_LEN              (17 + 1 + 1)


/**
 * Defines the time in ms to iniate an update of the beacon payload.
 */
#define APP_BCN_PAYLOAD_DURATION_MS     (3000)


/**
 * Defines the time in ms to iniate a broadcast data transmission
 * to all devices.
 */
#define APP_BC_DATA_DURATION_MS         (9000)


/** Defines the time to iniate a indirect data transmission to the device. */
#define APP_INDIRECT_DATA_DURATION_MS   (6000)


#define DEBOUNCE_DELAY_MS               (200)

#if (LED_COUNT >= 3)
#define LED_START                       (LED0)
#define LED_NWK_SETUP                   (LED1)
#define LED_DATA                        (LED2)
#elif (LED_COUNT == 2)
#define LED_START                       (LED0)
#define LED_NWK_SETUP                   (LED0)
#define LED_DATA                        (LED1)
#else
#define LED_START                       (LED0)
#define LED_NWK_SETUP                   (LED0)
#define LED_DATA                        (LED0)
#endif

/* === GLOBALS ============================================================= */

/** This array stores all device related information. */
static associated_device_t device_list[MAX_NUMBER_OF_DEVICES];

/** Stores the number of associated devices. */
static uint8_t no_of_assoc_devices;

/** This array stores the current beacon payload. */
static uint8_t beacon_payload[BEACON_PAYLOAD_LEN] = {"Atmel beacon demo 0"};

/** This variable stores the current state of the node. */
static coord_state_t coord_state = COORD_STARTING;

/** This variable counts the number of transmitted data frames. */
static uint32_t tx_cnt;

/** Store the current MSDU handle to be used for a data frame. */
static uint8_t curr_msdu_handle;

static uint8_t current_channel;
static uint8_t current_channel_page;
static uint32_t channels_supported;

static uint8_t APP_TIMER_INDIRECT_DATA;
static uint8_t APP_TIMER_BCN_PAYLOAD_UPDATE;
static uint8_t APP_TIMER_BC_DATA;
#ifdef GPIO_PUSH_BUTTON_0
static wpan_addr_spec_t dst_addr;
#endif //GPIO_PUSH_BUTTON_0


/* === PROTOTYPES ========================================================== */
/**
 * @brief Application specific function to assign a short address
 */
static bool assign_new_short_addr(uint64_t addr64, uint16_t *addr16);

/**
 * @brief Callback function for initiation of broadcast data transmission
 *
 * @param parameter Pointer to callback parameter
 *                  (not used in this application, but could be used
 *                  to indicated LED to be switched off)
 */
static void bc_data_cb(void *parameter);
/**
 * @brief Callback function for initiation of indirect data transmission
 *
 * @param parameter Pointer to callback parameter
 *                  (not used in this application, but could be used
 *                  to indicated LED to be switched off)
 */
static void indirect_data_cb(void *parameter);

/**
 * @brief Callback function for updating the beacon payload
 *
 * @param parameter Pointer to callback parameter
 *                  (not used in this application, but could be used
 *                  to indicated LED to be switched off)
 */
static void bcn_payload_update_cb(void *parameter);


/** Alert to indicate something has gone wrong in the application */
static void app_alert(void);

/* === IMPLEMENTATION ====================================================== */

/**
 * @brief Main function of the coordinator application
 *
 * This function initializes the MAC, initiates a MLME reset request
 * (@ref wpan_mlme_reset_req()), and implements a the main loop.
 */
int main(void)
{
	/* Initialize the board.
	 * The board-specific conf_board.h file contains the configuration of
	 * the board initialization.
	 */
	irq_initialize_vectors();
	board_init();
	sysclk_init();

	sw_timer_init();

	if(MAC_SUCCESS != wpan_init())
	{
		app_alert();
	}

    /* Initialize LEDs. */
    LED_On(LED_START);         // indicating application is started
    LED_Off(LED_NWK_SETUP);    // indicating network is started
    LED_Off(LED_DATA);         // indicating data transmission

	cpu_irq_enable();

#ifdef SIO_HUB
    /* Initialize the serial interface used for communication with terminal program. */
	sio2host_init();

    /* To make sure the Hyper Terminal Connected to the system*/
    sio2host_getchar();

    printf("\nBeacon_Application\r\n\n");
    printf("\nCoordinator\r\n\n");
#endif /* SIO_HUB */

	sw_timer_get_id(&APP_TIMER_INDIRECT_DATA);
	sw_timer_get_id(&APP_TIMER_BCN_PAYLOAD_UPDATE);
	sw_timer_get_id(&APP_TIMER_BC_DATA);


    /*
     * Reset the MAC layer to the default values.
     * This request will cause a mlme reset confirm message ->
     * usr_mlme_reset_conf
     */
    wpan_mlme_reset_req(true);

#ifdef GPIO_PUSH_BUTTON_0
	dst_addr.AddrMode = 2;
	dst_addr.PANId = DEFAULT_PAN_ID;
#endif //GPIO_PUSH_BUTTON_0

	while (true)
	{
		wpan_task();
#ifdef GPIO_PUSH_BUTTON_0
		if (!ioport_get_pin_level(GPIO_PUSH_BUTTON_0))
		{
			delay_ms(DEBOUNCE_DELAY_MS);
			if (!ioport_get_pin_level(GPIO_PUSH_BUTTON_0))
			{
				dst_addr.Addr.short_address = BROADCAST;
				wpan_mcps_data_req(FCF_SHORT_ADDR, &dst_addr, 14, 
								   (uint8_t *)"Broadcast Data", 1, WPAN_TXOPT_OFF);
			}
		}
#endif //GPIO_PUSH_BUTTON_0
	}
}


#if defined(ENABLE_TSTAMP)
/*
 * Callback function usr_mcps_data_conf
 *
 * @param msduHandle  Handle of MSDU handed over to MAC earlier
 * @param status      Result for requested data transmission request
 * @param Timestamp   The time, in symbols, at which the data were transmitted
 *                    (only if timestamping is enabled).
 *
 */
void usr_mcps_data_conf(uint8_t msduHandle,
                        uint8_t status,
                        uint32_t Timestamp)
#else
void usr_mcps_data_conf(uint8_t msduHandle,
                        uint8_t status)
#endif  /* ENABLE_TSTAMP */
{
#ifdef SIO_HUB
    char sio_array[255];

    sprintf(sio_array, "Result frame with handle %" PRIu8 ": ", msduHandle);
    printf(sio_array);
#endif

    /*Led_data is turned off after the confirmation*/
    LED_Off(LED_DATA);
    if (status == MAC_SUCCESS)
    {
#ifdef SIO_HUB
        printf("Success\r\n");
#endif
    }
    else if (status == MAC_TRANSACTION_OVERFLOW)
    {
#ifdef SIO_HUB
        /* Frame could not be placed into the indirect queue. */
        printf("Transaction overflow\r\n");
#endif
    }
    else if (status == MAC_TRANSACTION_EXPIRED)
    {
#ifdef SIO_HUB
        /*
         * Frame could not be delivered to the target node within
         * the proper time.
         */
        printf("Transaction expired\r\n");
#endif
    }

    /* Keep compiler happy. */
    msduHandle = msduHandle;
#ifdef ENABLE_TSTAMP
    Timestamp = Timestamp;
#endif  /* ENABLE_TSTAMP */
}

/*
 * @brief Callback function usr_mcps_data_ind
 *
 * @param SrcAddrSpec      Pointer to source address specification
 * @param DstAddrSpec      Pointer to destination address specification
 * @param msduLength       Number of octets contained in MSDU
 * @param msdu             Pointer to MSDU
 * @param mpduLinkQuality  LQI measured during reception of the MPDU
 * @param DSN              DSN of the received data frame.
 * @param Timestamp        The time, in symbols, at which the data were received
 *                         (only if timestamping is enabled).
 */
void usr_mcps_data_ind(wpan_addr_spec_t *SrcAddrSpec,
                       wpan_addr_spec_t *DstAddrSpec,
                       uint8_t msduLength,
                       uint8_t *msdu,
                       uint8_t mpduLinkQuality,
#ifdef ENABLE_TSTAMP
                       uint8_t DSN,
                       uint32_t Timestamp)
#else
                       uint8_t DSN)
#endif  /* ENABLE_TSTAMP */
{
#ifdef SIO_HUB
    char sio_array[255];

    sprintf(sio_array, "Rx frame from Device Addr%" PRIx16 ": ", LE16_TO_CPU_ENDIAN(SrcAddrSpec->Addr.short_address));
    printf(sio_array);


    for (uint8_t i = 0; i < msduLength; i++)
    {
        sio_array[i] = msdu[i];
    }
    sio_array[msduLength] = '\0';
    printf(sio_array);
    printf("\r\n");
#endif

    /* Keep compiler happy. */
    SrcAddrSpec = SrcAddrSpec;
    DstAddrSpec = DstAddrSpec;
    msduLength = msduLength;
    msdu = msdu;
    mpduLinkQuality = mpduLinkQuality;
    DSN = DSN;
#ifdef ENABLE_TSTAMP
    Timestamp = Timestamp;
#endif  /* ENABLE_TSTAMP */
}


#if ((MAC_PURGE_REQUEST_CONFIRM == 1) && (MAC_INDIRECT_DATA_BASIC == 1))

/*
 * Callback function usr_mcps_purge_conf
 *
 * @param msduHandle           Handle (id) of MSDU to be purged.
 * @param status               Result of requested purge operation.
 *
 * @return void

 */
void usr_mcps_purge_conf(uint8_t msduHandle,
                         uint8_t status)
{
	
}
#endif  /* ((MAC_PURGE_REQUEST_CONFIRM == 1) && (MAC_INDIRECT_DATA_BASIC == 1)) */


#if (MAC_ASSOCIATION_REQUEST_CONFIRM == 1)

/*
 * Callback function usr_mlme_associate_conf.
 *
 * @param AssocShortAddress    Short address allocated by the coordinator.
 * @param status               Result of requested association operation.
 *
 * @return void
 *
 */
void usr_mlme_associate_conf(uint16_t AssocShortAddress,
                             uint8_t status)
{
    /* Keep compiler happy. */
    AssocShortAddress = AssocShortAddress;
}
#endif  /* (MAC_ASSOCIATION_REQUEST_CONFIRM == 1) */


#if (MAC_ASSOCIATION_INDICATION_RESPONSE == 1)
/*
 * @brief Callback function usr_mlme_associate_ind
 *
 * @param DeviceAddress         Extended address of device requesting association
 * @param CapabilityInformation Capabilities of device requesting association
 */
void usr_mlme_associate_ind(uint64_t DeviceAddress,
                            uint8_t CapabilityInformation)
{
    /*
     * Any device is allowed to join the network.
     * Use: bool wpan_mlme_associate_resp(uint64_t DeviceAddress,
     *                                    uint16_t AssocShortAddress,
     *                                    uint8_t status);
     *
     * This response leads to comm status indication -> usr_mlme_comm_status_ind
     * Get the next available short address for this device.
     */
    uint16_t associate_short_addr = macShortAddress_def;

    if (assign_new_short_addr(DeviceAddress, &associate_short_addr) == true)
    {
        wpan_mlme_associate_resp(DeviceAddress,
                                 associate_short_addr,
                                 ASSOCIATION_SUCCESSFUL);
    }
    else
    {
        wpan_mlme_associate_resp(DeviceAddress, associate_short_addr,
                                 PAN_AT_CAPACITY);
    }

    /* Keep compiler happy. */
    CapabilityInformation = CapabilityInformation;
}
#endif  /* (MAC_ASSOCIATION_INDICATION_RESPONSE == 1) */


#if (MAC_BEACON_NOTIFY_INDICATION == 1)

/*
 * Callback function usr_mlme_beacon_notify_ind
 *
 * @param BSN            Beacon sequence number.
 * @param PANDescriptor  Pointer to PAN descriptor for received beacon.
 * @param PendAddrSpec   Pending address specification in received beacon.
 * @param AddrList       List of addresses of devices the coordinator has pending data.
 * @param sduLength      Length of beacon payload.
 * @param sdu            Pointer to beacon payload.
 *
 * @return void
 *
 */
void usr_mlme_beacon_notify_ind(uint8_t BSN,
                                wpan_pandescriptor_t *PANDescriptor,
                                uint8_t PendAddrSpec,
                                uint8_t *AddrList,
                                uint8_t sduLength,
                                uint8_t *sdu)
{
	
}
#endif  /* (MAC_BEACON_NOTIFY_INDICATION == 1) */


#if ((MAC_ORPHAN_INDICATION_RESPONSE == 1) || (MAC_ASSOCIATION_INDICATION_RESPONSE == 1))

/*
 * @brief Callback function usr_mlme_comm_status_ind
 *
 * @param SrcAddrSpec      Pointer to source address specification
 * @param DstAddrSpec      Pointer to destination address specification
 * @param status           Result for related response operation
 */
void usr_mlme_comm_status_ind(wpan_addr_spec_t *SrcAddrSpec,
                              wpan_addr_spec_t *DstAddrSpec,
                              uint8_t status)
{
    if (status == MAC_SUCCESS)
    {
        /*
         * Now the association of the device has been successful and its
         * information, like address, could  be stored.
         * But for the sake of simple handling it has been done
         * during assignment of the short address within the function
         * assign_new_short_addr()
         */

        /* Start timer to initiate indirect data transmission. */
        sw_timer_start(APP_TIMER_INDIRECT_DATA,
                        ((uint32_t)APP_INDIRECT_DATA_DURATION_MS * 1000),
                        SW_TIMEOUT_RELATIVE,
                        (FUNC_PTR)indirect_data_cb,
                        NULL);
    }
    else
    {
    }

    /* Keep compiler happy. */
    SrcAddrSpec = SrcAddrSpec;
    DstAddrSpec = DstAddrSpec;
}
#endif  /* ((MAC_ORPHAN_INDICATION_RESPONSE == 1) || (MAC_ASSOCIATION_INDICATION_RESPONSE == 1)) */


#if (MAC_DISASSOCIATION_BASIC_SUPPORT == 1)

/*
 * Callback function usr_mlme_disassociate_conf
 *
 * @param status             Result of requested disassociate operation.
 * @param DeviceAddrSpec     Pointer to wpan_addr_spec_t structure for device
 *                           that has either requested disassociation or been
 *                           instructed to disassociate by its coordinator.
 *
 * @return void
 */
void usr_mlme_disassociate_conf(uint8_t status,
                                wpan_addr_spec_t *DeviceAddrSpec)
{
	
}
#endif /* (MAC_DISASSOCIATION_BASIC_SUPPORT == 1)*/


#if (MAC_DISASSOCIATION_BASIC_SUPPORT == 1)

/*
 * Callback function usr_mlme_disassociate_ind
 *
 * @param DeviceAddress        Extended address of device which initiated the
 *                             disassociation request.
 * @param DisassociateReason   Reason for the disassociation. Valid values:
 *                           - @ref WPAN_DISASSOC_BYPARENT,
 *                           - @ref WPAN_DISASSOC_BYCHILD.
 *
 * @return void
 */
void usr_mlme_disassociate_ind(uint64_t DeviceAddress,
                               uint8_t DisassociateReason)
{
	
}
#endif  /* (MAC_DISASSOCIATION_BASIC_SUPPORT == 1) */


#if (MAC_GET_SUPPORT == 1)

/*
 * Callback function usr_mlme_get_conf
 *
 * @param status            Result of requested PIB attribute get operation.
 * @param PIBAttribute      Retrieved PIB attribute.
 * @param PIBAttributeIndex Index of the PIB attribute to be read.
 * @param PIBAttributeValue Pointer to data containing retrieved PIB attribute,
 *
 * @return void
 */
void usr_mlme_get_conf(uint8_t status,
                       uint8_t PIBAttribute,
                       void *PIBAttributeValue)
{
	
	if((status == MAC_SUCCESS) && (PIBAttribute == phyCurrentPage))
	{
		current_channel_page = *(uint8_t *)PIBAttributeValue;
		wpan_mlme_get_req(phyChannelsSupported);
	}
	else if((status == MAC_SUCCESS) && (PIBAttribute == phyChannelsSupported))
	{
		uint8_t index;
		
		channels_supported = *(uint32_t *)PIBAttributeValue;
		
		for(index = 0; index < 32; index++)
		{
			if(channels_supported & (1 << index))
			{
				current_channel = index + CHANNEL_OFFSET;
				break;
			}
		}
        /*
         * Set the short address of this node.
         * Use: bool wpan_mlme_set_req(uint8_t PIBAttribute,
         *                             void *PIBAttributeValue);
         *
         * This request leads to a set confirm message -> usr_mlme_set_conf
         */
        uint8_t short_addr[2];

        short_addr[0] = (uint8_t)COORD_SHORT_ADDR;          // low byte
        short_addr[1] = (uint8_t)(COORD_SHORT_ADDR >> 8);   // high byte
        wpan_mlme_set_req(macShortAddress, short_addr);
	}
}
#endif  /* (MAC_GET_SUPPORT == 1) */


#if (MAC_ORPHAN_INDICATION_RESPONSE == 1)

/*
 * Callback function usr_mlme_orphan_ind
 *
 * @param OrphanAddress     Address of orphaned device.
 *
 * @return void
 *
 */
void usr_mlme_orphan_ind(uint64_t OrphanAddress)
{
	
}
#endif  /* (MAC_ORPHAN_INDICATION_RESPONSE == 1) */


#if (MAC_INDIRECT_DATA_BASIC == 1)

/*
 * Callback function that must be implemented by application (NHLE) for MAC service
 * MLME-POLL.confirm.
 *
 * @param status           Result of requested poll operation.
 *
 * @return void
 *
 */
void usr_mlme_poll_conf(uint8_t status)
{
	
}
#endif  /* (MAC_INDIRECT_DATA_BASIC == 1) */

/*
 * @brief Callback function usr_mlme_reset_conf
 *
 * @param status Result of the reset procedure
 */
void usr_mlme_reset_conf(uint8_t status)
{
    if (status == MAC_SUCCESS)
    {
		wpan_mlme_get_req(phyCurrentPage);
    }
    else
    {
        /* Something went wrong; restart. */
        wpan_mlme_reset_req(true);
    }
}


#if (MAC_RX_ENABLE_SUPPORT == 1)

/*
 * Callback function usr_mlme_rx_enable_conf
 *
 * @param status           Result of requested receiver enable operation.
 *
 * @return void
 */
void usr_mlme_rx_enable_conf(uint8_t status)
{
	
}
#endif  /* (MAC_RX_ENABLE_SUPPORT == 1) */


#if ((MAC_SCAN_ED_REQUEST_CONFIRM == 1)      || \
     (MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1)  || \
     (MAC_SCAN_PASSIVE_REQUEST_CONFIRM == 1) || \
     (MAC_SCAN_ORPHAN_REQUEST_CONFIRM == 1))
	 
/*
 * @brief Callback function usr_mlme_scan_conf
 *
 * @param status            Result of requested scan operation
 * @param ScanType          Type of scan performed
 * @param ChannelPage       Channel page on which the scan was performed
 * @param UnscannedChannels Bitmap of unscanned channels
 * @param ResultListSize    Number of elements in ResultList
 * @param ResultList        Pointer to array of scan results
 */
void usr_mlme_scan_conf(uint8_t status,
                        uint8_t ScanType,
                        uint8_t ChannelPage,
                        uint32_t UnscannedChannels,
                        uint8_t ResultListSize,
                        void *ResultList)
{
    /*
     * We are not interested in the actual scan result,
     * because we start our network on the pre-defined channel anyway.
     * Start a beacon-enabled network
     * Use: bool wpan_mlme_start_req(uint16_t PANId,
     *                               uint8_t LogicalChannel,
     *                               uint8_t ChannelPage,
     *                               uint8_t BeaconOrder,
     *                               uint8_t SuperframeOrder,
     *                               bool PANCoordinator,
     *                               bool BatteryLifeExtension,
     *                               bool CoordRealignment)
     *
     * This request leads to a start confirm message -> usr_mlme_start_conf
     */
    wpan_mlme_start_req(DEFAULT_PAN_ID,
                        current_channel,
                        current_channel_page,
                        DEFAULT_BO,
                        DEFAULT_SO,
                        true, false, false);

    /* Keep compiler happy. */
    status = status;
    ScanType = ScanType;
    ChannelPage = ChannelPage;
    UnscannedChannels = UnscannedChannels;
    ResultListSize = ResultListSize;
    ResultList = ResultList;
}
#endif

/*
 * @brief Callback function usr_mlme_set_conf
 *
 * @param status        Result of requested PIB attribute set operation
 * @param PIBAttribute  Updated PIB attribute
 */
void usr_mlme_set_conf(uint8_t status,
                       uint8_t PIBAttribute)
{
    if ((status == MAC_SUCCESS) && (PIBAttribute == macShortAddress))
    {
        /*
         * Allow other devices to associate to this coordinator.
         * Use: bool wpan_mlme_set_req(uint8_t PIBAttribute,
         *                             void *PIBAttributeValue);
         *
         * This request leads to a set confirm message -> usr_mlme_set_conf
         */
        uint8_t association_permit = true;

        wpan_mlme_set_req(macAssociationPermit, &association_permit);
    }
    else if ((status == MAC_SUCCESS) && (PIBAttribute == macAssociationPermit))
    {
        /*
         * Set RX on when idle to enable the receiver as default.
         * Use: bool wpan_mlme_set_req(uint8_t PIBAttribute,
         *                             void *PIBAttributeValue);
         *
         * This request leads to a set confirm message -> usr_mlme_set_conf
         */
        bool rx_on_when_idle = true;

        wpan_mlme_set_req(macRxOnWhenIdle, &rx_on_when_idle);
    }
    else if ((status == MAC_SUCCESS) && (PIBAttribute == macRxOnWhenIdle))
    {
        /* Set the beacon payload length. */
        uint8_t beacon_payload_len = BEACON_PAYLOAD_LEN;
        wpan_mlme_set_req(macBeaconPayloadLength, &beacon_payload_len);

    }
    else if ((status == MAC_SUCCESS) && (PIBAttribute == macBeaconPayloadLength))
    {
        /*
         * Once the length of the beacon payload has been defined,
         * set the actual beacon payload.
         */
        wpan_mlme_set_req(macBeaconPayload, &beacon_payload);
    }
    else if ((status == MAC_SUCCESS) && (PIBAttribute == macBeaconPayload))
    {
        if (COORD_STARTING == coord_state)
        {
            /*
             * Initiate an active scan over all channels to determine
             * which channel to use.
             * Use: bool wpan_mlme_scan_req(uint8_t ScanType,
             *                              uint32_t ScanChannels,
             *                              uint8_t ScanDuration,
             *                              uint8_t ChannelPage);
             *
             * This request leads to a scan confirm message -> usr_mlme_scan_conf
             * Scan for about 50 ms on each channel -> ScanDuration = 1
             * Scan for about 1/2 second on each channel -> ScanDuration = 5
             * Scan for about 1 second on each channel -> ScanDuration = 6
             */
            wpan_mlme_scan_req(MLME_SCAN_TYPE_ACTIVE,
                               SCAN_CHANNEL,
                               SCAN_DURATION_COORDINATOR,
                               current_channel_page);
        }
        else
        {
            /* Do nothing once the node is properly running. */
        }
    }
    else
    {
        /* Something went wrong; restart. */
        wpan_mlme_reset_req(true);
    }
}

/*
 * @brief Callback function usr_mlme_start_conf
 *
 * @param status        Result of requested start operation
 */
#if (MAC_START_REQUEST_CONFIRM == 1)
void usr_mlme_start_conf(uint8_t status)
{
    if (status == MAC_SUCCESS)
    {
        coord_state = COORD_RUNNING;
#ifdef SIO_HUB
        printf("Started beacon-enabled network in Channel - %d\r\n",
			   current_channel);
#endif
        /*
         * Network is established.
         * Waiting for association indication from a device.
         * -> usr_mlme_associate_ind
         */
        LED_On(LED_NWK_SETUP);

        /*
         * Now that the network has been started successfully,
         * the timer for broadcast data transmission is started.
         * This is independent from the actual number of associated nodes.
         */

        /* Start timer to initiate broadcast data transmission. */
        sw_timer_start(APP_TIMER_BC_DATA,
                        ((uint32_t)APP_BC_DATA_DURATION_MS * 1000),
                        SW_TIMEOUT_RELATIVE,
                        (FUNC_PTR)bc_data_cb,
                        NULL);

        /*
         * Now that the network has been started successfully,
         * the timer for updating the beacon payload is started.
         */
        sw_timer_start(APP_TIMER_BCN_PAYLOAD_UPDATE,
                        ((uint32_t)APP_BCN_PAYLOAD_DURATION_MS * 1000),
                        SW_TIMEOUT_RELATIVE,
                        (FUNC_PTR)bcn_payload_update_cb,
                        NULL);
    }
    else
    {
        /* Something went wrong; restart. */
        wpan_mlme_reset_req(true);
    }
}
#endif  /* (MAC_START_REQUEST_CONFIRM == 1) */


/*
 * Callback function usr_mlme_sync_loss_ind
 *
 * @param LossReason     Reason for synchronization loss.
 * @param PANId          The PAN identifier with which the device lost
 *                       synchronization or to which it was realigned.
 * @param LogicalChannel The logical channel on which the device lost
 *                       synchronization or to which it was realigned.
 * @param ChannelPage    The channel page on which the device lost
 *                       synchronization or to which it was realigned.
 *
 * @return void
 *
 */
 
void usr_mlme_sync_loss_ind(uint8_t LossReason,
                            uint16_t PANId,
                            uint8_t LogicalChannel,
                            uint8_t ChannelPage)
{
	
}


/*
 * @brief Application specific function to assign a short address
 *
 */
static bool assign_new_short_addr(uint64_t addr64, uint16_t *addr16)
{
    uint8_t i;
#ifdef SIO_HUB
    char sio_array[255];
#endif

    // Check if device has been associated before
    for (i = 0; i < MAX_NUMBER_OF_DEVICES; i++)
    {
        if (device_list[i].short_addr == 0x0000)
        {
            // If the short address is 0x0000, it has not been used before
            continue;
        }
        if (device_list[i].ieee_addr == addr64)
        {
            // Assign the previously assigned short address again
            *addr16 = device_list[i].short_addr;
            return true;
        }
    }

    for (i = 0; i < MAX_NUMBER_OF_DEVICES; i++)
    {
        if (device_list[i].short_addr == 0x0000)
        {
            *addr16 = CPU_ENDIAN_TO_LE16(i + 0x0001);
            device_list[i].short_addr = CPU_ENDIAN_TO_LE16(i + 0x0001); // get next short address
            device_list[i].ieee_addr = addr64;      // store extended address
            no_of_assoc_devices++;
#ifdef SIO_HUB
            sprintf(sio_array, "Device %" PRIu8 " associated\r\n", i + 1);
            printf(sio_array);
#endif
            return true;
        }
    }

    // If we are here, no short address could be assigned.
    return false;
}
/* Alert to indicate something has gone wrong in the application */
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

/*
 * @brief Callback function for initiation of broadcast data transmission
 *
 * @param parameter Pointer to callback parameter
 *                  (not used in this application, but could be used
 *                  to indicated LED to be switched off)
 */
static void bc_data_cb(void *parameter)
{
    /* Store the current MSDU handle to be used for a broadcast data frame. */
    static uint8_t curr_msdu_handle;
    uint8_t src_addr_mode;
    wpan_addr_spec_t dst_addr;
    uint8_t payload;
#ifdef SIO_HUB
    char sio_array[255];
#endif

    /*
     * Request transmission of broadcast data to all devices.
     *
     * Since this is a beacon-enabled network,
     * this request will just queue this frame into the broadcast data queue.
     *
     * Once this the next beacon frame is about to be transmitted,
     * the broadcast data frame will be announced by setting
     * the frame pending bit of the frame control field of this particular
     * beacon frame.
     *
     * Immediately after the successful transmission of the beacon frame,
     * the pending broadcast frame will be transmitted.
     */
    src_addr_mode = WPAN_ADDRMODE_SHORT;
    dst_addr.AddrMode = WPAN_ADDRMODE_SHORT;
    dst_addr.PANId = DEFAULT_PAN_ID;
    /* Broadcast destination address is used. */
    dst_addr.Addr.short_address = BROADCAST;

    payload = (uint8_t)rand();  // Any dummy data
    curr_msdu_handle++;         // Increment handle
    tx_cnt++;

#ifdef SIO_HUB
    sprintf(sio_array, "Broadcast frame Tx count:  %" PRIu32 "\r\n", tx_cnt);
    printf(sio_array);
#endif

    /* The transmission is direct, but without acknowledgement. */
    if (wpan_mcps_data_req(src_addr_mode,
                           &dst_addr,
                           1,  // One octet
                           &payload,
                           curr_msdu_handle,
                           WPAN_TXOPT_OFF)
       )
    {
        LED_On(LED_DATA);
    }
    else
    {
        /*
         * Data could not be queued into the broadcast queue.
         * Add error handling if required.
         */
    }

    /* Start timer to initiate next broadcast data transmission. */
    sw_timer_start(APP_TIMER_BC_DATA,
                    ((uint32_t)APP_BC_DATA_DURATION_MS * 1000),
                    SW_TIMEOUT_RELATIVE,
                    (FUNC_PTR)bc_data_cb,
                    NULL);

    parameter = parameter;  /* Keep compiler happy. */
}
/*
 * @brief Callback function for initiation of indirect data transmission
 *
 * @param parameter Pointer to callback parameter
 *                  (not used in this application, but could be used
 *                  to indicated LED to be switched off)
 */
static void indirect_data_cb(void *parameter)
{
    uint8_t cur_device;
    uint8_t src_addr_mode;
    wpan_addr_spec_t dst_addr;
    uint8_t payload;
#ifdef SIO_HUB
    char sio_array[255];
#endif

    /* Loop over all associated devices. */
    for (cur_device = 0; cur_device < no_of_assoc_devices; cur_device++)
    {
#ifdef SIO_HUB
        sprintf(sio_array, "Queue data for device %" PRIu8 " ", cur_device + 1);
        printf(sio_array);
#endif

        /*
         * Request transmission of indirect data to device.
         * This will just queue this frame into the indirect data queue.
         * Once this particular device polls for pending data,
         * the frame will be delivered to the device.
         */
        src_addr_mode = WPAN_ADDRMODE_SHORT;
        dst_addr.AddrMode = WPAN_ADDRMODE_SHORT;
        dst_addr.PANId = DEFAULT_PAN_ID;
        dst_addr.Addr.short_address = device_list[cur_device].short_addr;

        payload = (uint8_t)rand();  // Any dummy data
        curr_msdu_handle++;         // Increment handle

#ifdef SIO_HUB
        sprintf(sio_array, "(MSDU handle:  %" PRIu8 ")\r\n", curr_msdu_handle);
        printf(sio_array);
#endif

        if (!wpan_mcps_data_req(src_addr_mode,
                                &dst_addr,
                                1,  // One octet
                                &payload,
                                curr_msdu_handle,
                                WPAN_TXOPT_INDIRECT_ACK)
           )
        {
            /*
             * Data could not be queued into the indirect queue.
             * Add error handling if required.
             */
        }
    }

    /* Start timer to initiate indirect data transmission. */
    sw_timer_start(APP_TIMER_INDIRECT_DATA,
                    ((uint32_t)APP_INDIRECT_DATA_DURATION_MS * 1000),
                    SW_TIMEOUT_RELATIVE,
                    (FUNC_PTR)indirect_data_cb,
                    NULL);

    parameter = parameter;  /* Keep compiler happy. */
}

/*
 * @brief Callback function for updating the beacon payload
 *
 * @param parameter Pointer to callback parameter
 *                  (not used in this application, but could be used
 *                  to indicated LED to be switched off)
 */
static void bcn_payload_update_cb(void *parameter)
{
    /*
     * Counter holding the variable portion of the beacon payload.
     *
     * Note: If this is changed, also the define BEACON_PAYLOAD_LEN needs
     * to be updated accordingly.
     * If this happens, the PIB attribute macBeaconPayloadLength needs to be
     * adjusted again as well. Since in this application the length of the
     * beacon payload never changes, this can be skipped.
     */
    static uint8_t bcn_payload_cnt;

    /* The counter transmitted in the beacon payload is updated and
     * the new beacon payload is set.
     */
    bcn_payload_cnt++;
    bcn_payload_cnt %= 10;
    /* Create printable character. */
    beacon_payload[BEACON_PAYLOAD_LEN - 1] = bcn_payload_cnt + 0x30;
    wpan_mlme_set_req(macBeaconPayload, &beacon_payload);


    /* Restart timer for updating beacon payload. */
    sw_timer_start(APP_TIMER_BCN_PAYLOAD_UPDATE,
                    ((uint32_t)APP_BCN_PAYLOAD_DURATION_MS * 1000),
                    SW_TIMEOUT_RELATIVE,
                    (FUNC_PTR)bcn_payload_update_cb,
                    NULL);

    parameter = parameter;  /* Keep compiler happy. */
}
