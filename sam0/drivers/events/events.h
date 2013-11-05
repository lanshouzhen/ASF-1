/**
 * \file
 *
 * \brief SAM D2x Event System Driver
 *
 * Copyright (C) 2012-2013 Atmel Corporation. All rights reserved.
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
#ifndef EVENTS_H_INCLUDED
#define EVENTS_H_INCLUDED

/**
* \defgroup asfdoc_sam0_events_group SAM D2x Event System Driver (EVENTS)
*
* This driver for SAM D2x devices provides an interface for the configuration
* and management of the device's peripheral event channels and users within
* the device, including the enabling and disabling of peripheral source selection
* and synchronization of clock domains between various modules.
*
* The following peripherals are used by this module:
*
* - EVSYS (Event System Management)
*
* The outline of this documentation is as follows:
* - \ref asfdoc_sam0_events_prerequisites
* - \ref asfdoc_sam0_events_module_overview
* - \ref asfdoc_sam0_events_special_considerations
* - \ref asfdoc_sam0_events_extra_info
* - \ref asfdoc_sam0_events_examples
* - \ref asfdoc_sam0_events_api_overview
*
*
* \section asfdoc_sam0_events_prerequisites Prerequisites
*
* There are no prerequisites for this module.
*
*
* \section asfdoc_sam0_events_module_overview Module Overview
*
* Peripherals within the SAM D2x devices are capable of generating two types of
* actions in response to given stimulus; they can set a register flag for later
* intervention by the CPU (using interrupt or polling methods), or they can
* generate event signals which can be internally routed directly to other
* peripherals within the device. The use of events allows for direct actions
* to be performed in one peripheral in response to a stimulus in another
* without CPU intervention. This can lower the overall power consumption of the
* system if the CPU is able to remain in sleep modes for longer periods, and
* lowers the latency of the system response.
*
* The event system is comprised of a number of freely configurable Event
* Channels, plus a number of fixed Event Users. Each Event Channel can be
* configured to select the input peripheral that will generate the events on
* the channel, as well as the synchronization path and edge detection mode.
* The fixed-function Event Users, connected to peripherals within the device,
* can then subscribe to an Event Channel in a one-to-many relationship in order
* to receive events as they are generated. An overview of the event system
* chain is shown in
* \ref asfdoc_sam0_events_module_overview_fig "the figure below".
*
* \anchor asfdoc_sam0_events_module_overview_fig
* \dot
* digraph overview {
* rankdir=LR;
* node [label="Source\nPeripheral" shape=ellipse style=filled fillcolor=lightgray] src_peripheral;
* node [label="Event\nChannel a" shape=square style=""] event_gen0;
* node [label="Event\nUser x" shape=square style=""] event_user0;
* node [label="Event\nUser y" shape=square style=""] event_user1;
* node [label="Destination\nPeripheral" shape=ellipse style=filled fillcolor=lightgray] dst_peripheral0;
* node [label="Destination\nPeripheral" shape=ellipse style=filled fillcolor=lightgray] dst_peripheral1;
*
* src_peripheral -> event_gen0;
* event_gen0 -> event_user0;
* event_gen0 -> event_user1;
* event_user0 -> dst_peripheral0;
* event_user1 -> dst_peripheral1;
* }
* \enddot
*
* There are many different events that can be routed in the device, which can
* then trigger many different actions. For example, an Analog Comparator module
* could be configured to generate an event when the input signal rises above
* the compare threshold, which then triggers a Timer Counter module to capture
* the current count value for later use.
*
* \subsection asfdoc_sam0_events_module_overview_event_channels Event Channels
* The Event module in each device consists of several channels, which can be
* freely linked to an event generator (i.e. a peripheral within the device
* that is capable of generating events). Each channel can be individually
* configured to select the generator peripheral, signal path and edge detection
* applied to the input event signal, before being passed to any event user(s).
*
* Event channels can support multiple users within the device in a standardized
* manner; when an Event User is linked to an Event Channel, the channel will
* automatically handshake with all attached users to ensure that all modules
* correctly receive and acknowledge the event.
*
* \subsection asfdoc_sam0_events_module_overview_event_users Event Users
* Event Users are able to subscribe to an Event Channel, once it has been
* configured. Each Event User consists of a fixed connection to one of the
* peripherals within the device (for example, an ADC module or Timer module)
* and is capable of being connected to a single Event Channel.
*
* \subsection asfdoc_sam0_events_module_overview_edge_detection Edge Detection
* For asynchronous events, edge detection on the event input is not possible,
* and the event signal must be passed directly between the event generator and
* event user. For synchronous and re-synchronous events, the input signal from
* the event generator must pass through an edge detection unit, so that only
* the rising, falling or both edges of the event signal triggers an action in
* the event user.
*
* \subsection asfdoc_sam0_events_module_overview_path_selection Path Selection
* The event system in the SAM0 devices supports three signal path types from
* the event generator to event users: asynchronous, synchronous and
* re-synchronous events.
*
* \subsubsection asfdoc_sam0_events_module_overview_path_selection_async Asynchronous Paths
* Asynchronous event paths allow for an asynchronous connection between the
* event generator and event user(s), when the source and destination
* peripherals share the same \ref asfdoc_sam0_system_clock_group "Generic Clock"
* channel. In this mode the event is propagated between the source and
* destination directly to reduce the event latency, thus no edge detection is
* possible. The asynchronous event chain is shown in
* \ref asfdoc_sam0_events_module_async_path_fig "the figure below".
*
* \anchor asfdoc_sam0_events_module_async_path_fig
* \dot
* digraph overview {
* rankdir=LR;
* node [label="Source\nPeripheral" shape=ellipse style=filled fillcolor=lightgray] src_peripheral;
* node [label="<f0> EVSYS | <f1> Event\nChannel/User" fillcolor=white style="dashed" shape=record] events_chan;
* node [label="Destination\nPeripheral" shape=ellipse style=filled fillcolor=lightgray] dst_peripheral;
*
* src_peripheral -> events_chan;
* events_chan -> dst_peripheral;
*
* }
* \enddot
* \note Identically shaped borders in the diagram indicate a shared generic clock channel
*
* \subsubsection asfdoc_sam0_events_module_overview_path_selection_sync Synchronous Paths
* Synchronous event paths can be used when the source and destination
* peripherals, as well as the generic clock to the event system itself, use
* different generic clock channels. This case introduces additional latency in
* the event propagation due to the addition of a synchronizer and edge detector
* on the input event signal, however this allows modules of different clocks to
* communicate events to one-another. The synchronous event chain is shown in
* \ref asfdoc_sam0_events_module_sync_path_fig "the figure below".
*
* \anchor asfdoc_sam0_events_module_sync_path_fig
* \dot
* digraph overview {
* rankdir=LR;
* node [label="Source\nPeripheral" shape=ellipse style="filled, dashed" fillcolor=lightgray] src_peripheral;
* node [label="<f0> EVSYS | <f1> Event\nChannel/User" fillcolor=white shape=record] events_chan;
* node [label="Destination\nPeripheral" shape=ellipse style="filled, dotted" fillcolor=lightgray] dst_peripheral;
*
* src_peripheral -> events_chan;
* events_chan -> dst_peripheral;
*
* }
* \enddot
* \note Identically shaped borders in the diagram indicate a shared generic clock channel
*
* \subsubsection asfdoc_sam0_events_module_overview_path_selection_resync Re-synchronous Paths
* Re-synchronous event paths are a special form of synchronous events, where
* the event users share the same generic clock channel as the event system
* module itself, but the event generator does not. This reduces latency by
* performing the synchronization across the event source and event user clock
* domains once within the event channel itself, rather than in each event user.
* The re-synchronous event chain is shown in
* \ref asfdoc_sam0_events_module_resync_path_fig "the figure below".
*
* \anchor asfdoc_sam0_events_module_resync_path_fig
* \dot
* digraph overview {
* rankdir=LR;
* node [label="Source\nPeripheral" shape=ellipse style="filled, dashed" fillcolor=lightgray] src_peripheral;
* node [label="<f0> EVSYS | <f1> Event\nChannel/User" fillcolor=white shape=record] events_chan;
* node [label="Destination\nPeripheral" shape=ellipse style=filled fillcolor=lightgray] dst_peripheral;
*
* src_peripheral -> events_chan;
* events_chan -> dst_peripheral;
*
* }
* \enddot
* \note Identically shaped borders in the diagram indicate a shared generic clock channel
*
* \subsection asfdoc_sam0_events_module_overview_physical Physical Connection
*
* \ref asfdoc_sam0_events_module_int_connections_fig "The diagram below"
* shows how this module is interconnected within the device.
*
* \anchor asfdoc_sam0_events_module_int_connections_fig
* \dot
* digraph overview {
* rankdir=LR;
* node [label="Source\nPeripherals" shape=ellipse style=filled fillcolor=lightgray] src_peripheral;
*
* subgraph driver {
* node [label="<f0> EVSYS | <f1> Event Channels" fillcolor=white shape=record] events_chan;
* node [label="<f0> EVSYS | <f1> Event Users" fillcolor=white shape=record] events_user;
* }
*
* node [label="Destination\nPeripherals" shape=ellipse style=filled fillcolor=lightgray] dst_peripheral;
*
* src_peripheral -> events_chan:f1 [label="Source\nMUXs"];
* events_chan:f1 -> events_user:f1 [label="Channel\nMUXs"];
* events_user:f1 -> dst_peripheral;
* }
* \enddot
*
*
* \section asfdoc_sam0_events_special_considerations Special Considerations
*
* There are no special considerations for this module.
*
*
* \section asfdoc_sam0_events_extra_info Extra Information
*
* For extra information see \ref asfdoc_sam0_events_extra. This includes:
* - \ref asfdoc_sam0_events_extra_acronyms
* - \ref asfdoc_sam0_events_extra_dependencies
* - \ref asfdoc_sam0_events_extra_errata
* - \ref asfdoc_sam0_events_extra_history
*
*
* \section asfdoc_sam0_events_examples Examples
*
* For a list of examples related to this driver, see
* \ref asfdoc_sam0_events_exqsg.
*
*
* \section asfdoc_sam0_events_api_overview API Overview
* @{
*/

#include <compiler.h>

/**
 * \brief Edge detect enum
 *
 * Event channel edge detect setting
 *
 */
enum events_edge_detect {
	/** No event output */
	EVENTS_EDGE_DETECT_NONE,
	/** Event on rising edge */
	EVENTS_EDGE_DETECT_RISING,
	/** Event on falling edge */
	EVENTS_EDGE_DETECT_FALLING,
	/** Event on both edges */
	EVENTS_EDGE_DETECT_BOTH,
};

/**
 * \brief Path selection enum
 *
 * Event channel path selection
 *
 */
enum events_path_selection {
	/** Select the synchronous path for this event channel */
	EVENTS_PATH_SYNCHRONOUS,
	/** Select the resynchronizer path for this event channel */
	EVENTS_PATH_RESYNCHRONIZED,
	/** Select the asynchronous path for this event channel */
	EVENTS_PATH_ASYNCHRONOUS,
};

/**
 * \brief Events configuration struct
 *
 * This events configuration struct is used to configure each of the channels
 *
 */
struct events_config {
	/** Select edge detection mode */
	enum events_edge_detect    edge_detect;
	/** Select events channel path */
	enum events_path_selection path;
	/** Set event generator for the channel */
	uint8_t                    generator;
	/** Clock source for the event channel */
	uint8_t                    clock_source;
};

#define EVSYS_ID_GEN_NONE 0


/**
 * \brief Event channel descriptor
 *
 */
struct events_descriptor {
	uint8_t channel;
};

/**
 * \brief Initializes an event configurations struct to defaults
 *
 * Initailize an event configuration struct to predefined default settings.
 *
 * \param[in] config Pointer to an instance of \c struct events_config
 *
 */
void events_get_config_defaults(struct events_config *config);

/**
 * \brief Allocate a event channel and set configuration
 *
 * Allocates a event channel from the even channel pool and sets
 * the channel configuration.
 *
 * \param[out] descriptor Pointer to a events_descriptor struct instance
 * \param[in]  config     Pointer to a events_config struct
 *
 * \return Status of the configuration procedure
 * \retval STATUS_OK            Allocation and configuration went successful
 * \retval STATUS_ERR_NOT_FOUND No free event channel found
 *
 */
enum status_code events_allocate(struct events_descriptor *descriptor, struct events_config *config);

/**
 * \brief Attach user to the event channel
 *
 * Attach a user peripheral to the event channel to receive events.
 *
 * \param[in] descriptor Pointer to an event_descriptor struct instance
 * \param[in] user_id    A number identifying the user peripheral found in the device header file.
 * \return Status of the user attach procedure
 * \retval STATUS_OK No errors detected when attaching the event user
 */
enum status_code events_attach_user(struct events_descriptor *descriptor, uint8_t user_id);

/**
 * \brief Deattach an user peripheral from the event channel
 *
 * Deattach an user peripheral from the event channels so it does not receive any more events.
 *
 * \param[in] descriptor Pointer to an event_descriptor struct instance
 * \param[in] user_id    A number identifying the user peripheral found in the device header file.
 *
 * \return Status of the user deattach procedure
 * \retval STATUS_OK No errors detected when deattaching the event user
 */
enum status_code events_deattach_user(struct events_descriptor *descriptor, uint8_t user_id);

/**
 * \brief Check if a channel is busy
 *
 * Check if a channel is busy, a channels stays busy until all users connected to the channel
 * has handled an event
 *
 * \param[in] descriptor Pointer to a events_descriptor struct instance
 *
 * \return Status of the channels busy state
 * \retval true   One or more users connected to the channel has not handled the last event
 * \retval false  All users are redy handle new events
 */
bool events_is_busy(struct events_descriptor *descriptor);

/**
 * \brief Trigger software event
 *
 * Trigger an event by software
 *
 * \param[in] descriptor Pointer to an \c events_descriptor struct
 *
 * \return Status of the event software procedure
 * \retval STATUS_OK No error was detected when software tigger signal was issued
 */
enum status_code events_trigger(struct events_descriptor *descriptor);

/**
 * \brief Check if all users connected to the channel is ready
 *
 * Check if all users connected to the channel is ready to handle incomming events
 *
 * \param[in] descriptor Ppointer to an \c event_descriptor struct
 *
 * \return The ready status of users connected to an event channel
 * \retval true  All users connect to event channel is ready handle incomming events
 * \retval false One or more users connect to event channel is not ready to handle incomming events
 */
bool events_is_users_ready(struct events_descriptor *descriptor);

/**
 * \brief Release allocated channel back the the resource pool
 *
 * Release an allocated channel back to the resource pool to make it availble for other purposes.
 *
 * \param[in] descriptor Pointer to an \c event_descriptor struct
 *
 * \return Status of channel release procedure
 * \retval STATUS_OK                  No error was detected when channel was released
 * \retval STATUS_BUSY                One or more event users have not processed the last event
 * \retval STATUS_ERR_NOT_INITIALIZED Channel not allocated, and can derfor not be released
 */
enum status_code events_release(struct events_descriptor *descriptor);

/**
 * \brief Get number of free channels
 *
 * Get number of allocatable channels in the event system
 *
 * \return The number of free channels in the event system
 *
 */
uint8_t events_get_free_channels(void);

/** @} */

/**
* \page asfdoc_sam0_events_extra Extra Information for EVENTS Driver
*
* \section asfdoc_sam0_events_extra_acronyms Acronyms
* Below is a table listing the acronyms used in this module, along with their
* intended meanings.
*
* <table>
*   <tr>
*     <th>Acronym</th>
*     <th>Description</th>
*   </tr>
*   <tr>
*     <td>CPU</td>
*     <td>Central Processing Unit</td>
*   </tr>
*   <tr>
*     <td>MUX</td>
*     <td>Multiplexer</td>
*   </tr>
* </table>
*
*
* \section asfdoc_sam0_events_extra_dependencies Dependencies
* This driver has the following dependencies:
*
* - \ref asfdoc_sam0_system_clock_group "System Clock Driver"
*
*
* \section asfdoc_sam0_events_extra_errata Errata
* There are no errata related to this driver.
*
*
* \section asfdoc_sam0_events_extra_history Module History
* An overview of the module history is presented in the table below, with
* details on the enhancements and fixes made to the module since its first
* release. The current version of this corresponds to the newest version in
* the table.
*
* <table>
*   <tr>
*     <th>Changelog</th>
*   </tr>
*   <tr>
*     <td>Initial Release</td>
*   </tr>
* </table>
*/

/**
* \page asfdoc_sam0_events_exqsg Examples for EVENTS Driver
*
* This is a list of the available Quick Start guides (QSGs) and example
* applications for \ref asfdoc_sam0_events_group. QSGs are simple examples with
* step-by-step instructions to configure and use this driver in a selection of
* use cases. Note that QSGs can be compiled as a standalone application or be
* added to the user application.
*
* - \subpage asfdoc_sam0_events_basic_use_case
*
* \page asfdoc_sam0_events_document_revision_history Document Revision History
*
* <table>
*   <tr>
*     <th>Doc. Rev.</td>
*     <th>Date</td>
*     <th>Comments</td>
*   </tr>
*   <tr>
*     <td>A</td>
*     <td>09/2013</td>
*     <td>Initial release</td>
*   </tr>
* </table>
*/

#endif /* EVENTS_H_INCLUDED */
