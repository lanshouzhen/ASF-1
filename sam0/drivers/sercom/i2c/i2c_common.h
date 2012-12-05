/**
 * \file
 *
 * \brief SAM0+ SERCOM I2C Common Driver
 *
 * Copyright (C) 2012 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
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
#ifndef I2C_COMMON_H_INCLUDED
#define I2C_COMMON_H_INCLUDED

#include <sercom.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup sam0_i2c_group SAM0+ I2C Driver (I2C)
 *
 * Driver for the SAM0+ architecture devices. This driver provides an
 * interface for configuration and management of the SERCOM I2C module in
 * master mode, as well as data transfer via I2C.
 * This driver encompasses the following module within the SAM0+ devices:
 * \li \b I2C
 *
 * \section module_introduction Introduction
 *
 * \subsection module_overview I2C Overview
 * The I2C provides a simple two-wire bidirectional bus consisting of a
 * wired-AND type serial clock line (SCA) and a wired-AND type serial data line
 * (SDA).
 * The I2C bus provides a simple, but efficient method of interconnecting
 * multiple master and slave devices. An arbitration mechanism is provided for
 * resolving bus ownership between masters, as only one master device may own
 * the bus at any given time. The arbitration mechanism relies on the wired-AND
 * connections to avoid bus drivers short-circuiting.
 *
 * A unique address is assigned to all slave devices connected to the bus. A
 * device can contain both math and slave logic, and can emulate multiple slave
 * devices by responding to more than one address.
 *
 *
 * \subsection bus_topology Bus Topology
 * The I2C bus topology is illustrated in the figure below. The pullup
 * resistors (Rs) will provide a high level on the bus lines when none of the
 * I2C devices are driving the bus. These are optional, and can be replaced
 * with a constant current source.
 * \dot
 * digraph bus_topology{
 *
 * splines = false;
 *
 * vcc [label="VCC", shape=none];
 * vcc_end [label="", style=invisible];
 * sda [label="SDA", shape=none];
 * sda_end [label="", style=invisible];
 * scl [label="SCL", shape=none];
 * scl_end [label="", style=invisible];
 *
 * res1 [label="Rp", shape=box, height=1, width=0.1];
 * res2 [label="Rp", shape=box, height=1, width=0.1];
 * res1dev1 [label="Rs", shape=box, style=dotted, height=1, width=0.1];
 * res2dev1 [label="Rs", shape=box, style=dotted, height=1, width=0.1];
 * res1dev2 [label="Rs", shape=box, style=dotted, height=1, width=0.1];
 * res2dev2 [label="Rs", shape=box, style=dotted, height=1, width=0.1];
 * res1devn [label="Rs", shape=box, style=dotted, height=1, width=0.1];
 * res2devn [label="Rs", shape=box, style=dotted, height=1, width=0.1];
 *
 * i2c1 [label="I2C DEVICE 1", shape=box, height=1.5];
 * i2c2 [label="I2C DEVICE 2", shape=box, height=1.5];
 * i2cn [label="I2C DEVICE N", shape=box, height=1.5];
 * dev_invis [label="", style=invisible];
 * dev_invis_end [label="", style="invis"];
 *
 * vcc_res1 [label="", style="invis", height=0];
 * vcc_res2 [label="", style="invis", height=0];
 * vcc_dev1 [label="", style="invis", height=0];
 * vcc_dev2 [label="", style="invis", height=0];
 * vcc_devn [label="", style="invis", height=0];
 *
 * scl_res1 [label="", style="invis", height=0];
 * sda_res2 [label="", style="invis", height=0];
 * scl_res1dev1 [label="", style="invis", height=0];
 * sda_res2dev1 [label="", style="invis", height=0];
 * scl_res1dev2 [label="", style="invis", height=0];
 * sda_res2dev2 [label="", style="invis", height=0];
 * scl_res1devn [label="", style="invis", height=0];
 * sda_res2devn [label="", style="invis", height=0];
 * vcc_devn [label="", style="invis", height=0];
 *
 * {rank=same; dev_invis res1 res2 i2c1 i2c2 i2cn dev_invis_end}
 * {rank=same; res1dev1 res2dev1 res1dev2 res2dev2 res1devn res2devn}
 * {rank=same; vcc vcc_res1 vcc_res2 vcc_dev1 vcc_dev2 vcc_devn vcc_end}
 * {rank=same; sda sda_res2 sda_res2dev1 sda_res2dev2 sda_res2devn sda_end}
 * {rank=same; scl scl_res1 scl_res1dev1 scl_res1dev2 scl_res1devn scl_end}
 *
 * edge [dir=none];
 * vcc->vcc_res1:e;
 * vcc_res1->vcc_res2:e [constraint=true];
 * vcc_res1->res1 [constraint=true];
 * vcc_res2->vcc_dev1:e [constraint=true];
 * vcc_res2->res2 [constraint=true];
 * vcc_dev1->vcc_dev2:e [constraint=true];
 * vcc_dev2->vcc_devn:e [constraint=true];
 * vcc_dev2->i2c2 [constraint=true];
 * vcc_devn->vcc_end [constraint=true];
 * vcc_devn:c->i2cn [constraint=true];
 *
 * vcc_end->sda_end [constraint=true, style="invis"];
 *
 * vcc->dev_invis [constraint=true, style="invis"];
 * dev_invis->sda [constraint=true, style="invis"];
 *
 * sda->sda_res2:e;
 * sda_res2->sda_res2dev1:e;
 * sda_res2dev1->sda_res2dev2:e;
 * sda_res2dev2->sda_res2devn:e;
 * sda_res2devn->sda_end;
 *
 * sda->scl [constraint=true, style="invis"];
 * scl->scl_res1:e;
 * scl_res1->scl_res1dev1:e;
 * scl_res1dev1->scl_res1dev2:e;
 * scl_res1dev2->scl_res1devn:e;
 * scl_res1devn->scl_end;
 *
 * res1->res2 [constraint=true, style="invis"];
 * res2->i2c1 [constraint=true, style="invis"];
 * i2c1->i2c2 [constraint=true, style="invis"];
 * i2c2->i2cn [constraint=true, style="dotted"];
 *
 * res1->scl_res1;
 * res2->sda_res2;
 *
 * i2c1->res1dev1;
 * i2c1->res2dev1;
 * res1dev1->scl_res1dev1;
 * res2dev1->sda_res2dev1;
 *
 * i2c2->res1dev2;
 * i2c2->res2dev2;
 * res1dev2->scl_res1dev2;
 * res2dev2->sda_res2dev2;
 *
 * i2cn->res1devn;
 * i2cn->res2devn;
 * res1devn->scl_res1devn;
 * res2devn->sda_res2devn;
 *
 * i2cn->dev_invis_end [constraint=true, style="invis"];
 * vcc_end->dev_invis_end [constraint=true, style="invis"];
 * dev_invis_end->sda_end [constraint=true, style="invis"];
 * sda_end->scl_end [constraint=true, style="invis"];
 * }
 * \enddot
 *
 *
 * \subsection timeout Timeout
 * Inactive bus timeout and sda hold time is configurable in the driver.
 * \subsubsection inactive_bus Inactive Bus Timeout
 * When a master is enabled or connected to the bus, the bus state will change
 * from UNKNOWN to IDLE after a timeout.
 * The bus state will also change from BUSY to IDLE after a timeout.
 * \subsubsection sda_hold SDA Hold Timeout
 * \subsubsection scl_low SCL Low Timeout
 * If
 *
 * \subsection Transactions
 * There are three fundamental transaction formats:
 * \li Master Write
 *   - The master transmits data packets to the slave after addressing it.
 * \li Master Read
 *   - The slave transmits data packets to the master after being addressed.
 * \li Combined
 *   - A combined transaction consists of several write and read transactions.
 *
 * A data transfer starts with the master issuing a \b Start condition on the
 * bus  followed by the address of the slave together with a bit to indicate
 * whether the master wants to read from or write to the slave.
 * The addressed slave must respond to this by sending an \b ACK back to the
 * master.
 *
 * After this, data packets are sent from the master or slave, according to the
 * read/write bit. Each packet must be acknowledged (ACK) or not
 * acknowledged (NACK) by the receiver.
 * If a slave responds with a NACK, the master must assume that the slave
 * cannot receive any more data and issue a \b Stop condition to end the
 * transaction.
 *
 * The master completes a transaction by issuing a \b Stop condition.
 * \n\n
 * A master can issue multiple \b Start conditions during a transaction; this
 * is then called a \b Repeated \b Start condition.
 *
 * \subsubsection address_packets Address Packets
 * The slave address consists of seven bits. The 8th bit in the transfer
 * determines the data direction (read or write). An address packet always
 * succeeds a \b Start or \b Repeated \b Start condition.
 *
 * \subsubsection data_packets Data Packets
 * 9 bits long, consists of one data byte and an acknowledgement bit.
 * Data packets succeed either an address packet or another data packet.
 *
 * \subsubsection trans_examples Transaction Examples
 * The grey bits in the following examples are sent from master to slave, and
 * the white bits are sent from slave to master.
 *
 * Example of a read transaction is shown below. Here, the master first issues
 * a \b Start condition and gets ownership of the bus. An address packet with
 * the dir flag set to read is then sent and acknowledged by the slave. Then the
 * slave sends one data packet which is acknowledged by the master. The slave
 * sends another packet, which is not acknowledged by the master, which
 * indicates that the master will terminate the transaction. In the end
 * the transaction is terminated by the master issuing a \b Stop condition.
 *
 * <table>
 *   <tr>
 *      <td>0</td>
 *      <td>1</td>
 *      <td>2</td>
 *      <td>3</td>
 *      <td>4</td>
 *      <td>5</td>
 *      <td>6</td>
 *      <td>7</td>
 *      <td>8</td>
 *      <td>9</td>
 *      <td>10</td>
 *      <td>11</td>
 *      <td>12</td>
 *      <td>13</td>
 *      <td>14</td>
 *      <td>15</td>
 *      <td>16</td>
 *      <td>17</td>
 *      <td>18</td>
 *      <td>19</td>
 *      <td>20</td>
 *      <td>21</td>
 *      <td>22</td>
 *      <td>23</td>
 *      <td>24</td>
 *      <td>25</td>
 *      <td>26</td>
 *      <td>27</td>
 *      <td>28</td>
 *   </tr>
 *   <tr>
 *      <td BGCOLOR="lightgray">START</td>
 *      <td COLSPAN="7" BGCOLOR="lightgray">ADDRESS</td>
 *      <td BGCOLOR="lightgray">READ</td>
 *      <td>ACK</td>
 *      <td COLSPAN="8" BGCOLOR="">DATA</td>
 *      <td BGCOLOR="lightgray">ACK</td>
 *      <td COLSPAN="8" BGCOLOR="">DATA</td>
 *      <td BGCOLOR="lightgray">NACK</td>
 *      <td BGCOLOR="lightgray">STOP</td>
 *   </tr>
 * </table>
 *
 * Example of a write transaction is shown below. Here, the master first issues
 * a \b Start condition and gets ownership of the bus. An address packet with
 * the dir flag set to write is then sent and acknowledged by the slave. Then
 * the master sends two data packets, each acknowledged by the slave. In the
 * end the transaction is terminated by the master issuing a \b Stop condition.
 *
 * <table>
 *   <tr>
 *      <td>0</td>
 *      <td>1</td>
 *      <td>2</td>
 *      <td>3</td>
 *      <td>4</td>
 *      <td>5</td>
 *      <td>6</td>
 *      <td>7</td>
 *      <td>8</td>
 *      <td>9</td>
 *      <td>10</td>
 *      <td>11</td>
 *      <td>12</td>
 *      <td>13</td>
 *      <td>14</td>
 *      <td>15</td>
 *      <td>16</td>
 *      <td>17</td>
 *      <td>18</td>
 *      <td>19</td>
 *      <td>20</td>
 *      <td>21</td>
 *      <td>22</td>
 *      <td>23</td>
 *      <td>24</td>
 *      <td>25</td>
 *      <td>26</td>
 *      <td>27</td>
 *      <td>28</td>
 *   </tr>
 *   <tr>
 *      <td BGCOLOR="lightgray">START</td>
 *      <td COLSPAN="7" BGCOLOR="lightgray">ADDRESS</td>
 *      <td BGCOLOR="lightgray">WRITE</td>
 *      <td>ACK</td>
 *      <td COLSPAN="8" BGCOLOR="lightgray">DATA</td>
 *      <td>ACK</td>
 *      <td COLSPAN="8" BGCOLOR="lightgray">DATA</td>
 *      <td>ACK</td>
 *      <td BGCOLOR="lightgray">STOP</td>
 *   </tr>
 * </table>
 * \subsection multi_master Multi Master
 *
 * In a multi master environment, arbitration of the bus is important, as only
 * one master can own the bus at any point.
 * \subsubsection arbitration Arbitration
 * \par Clock stretching
 * The serial clock line is always driven by a master device. However, all
 * devices connected to the bus are allowed stretch the low period of the clock
 * to slow down the overall clock frequency or to insert wait states while
 * processing data.
 * Both master and slave can randomly stretch the clock, which will force the
 * other device into a wait-state until the clock line goes high again.
 *
 * \par Arbitration on the data line
 * If two masters start transmitting at the same time, they will both transmit
 * until one master detects that the other master is pulling the data line low.
 * When this is detected, the master not pulling the line low, will stop the
 * transmission and wait until the bus is idle.
 * As it is the master trying to contact the slave with the lowest address that
 * will get the bus ownership, this will create an arbitration scheme always
 * prioritizing the slaves with the lowest address in case of a bus collision.
 *
 * \subsubsection clock_sync Clock Synchronization
 * In situations where more than one master is trying to control the bus clock
 * line at the same time, a clock synchronization algorithm based on the same
 * principles used for clock stretching is necessary.
 *
 *
 * \subsection bus_states Bus States
 * As the I2C bus is limited to one transaction at the time, a master that
 * wants to perform a bus transaction must wait until the bus is free.
 * Because of this, it is necessary for all masters in a multi-master system to
 * know the current status of the bus to be able to avoid conflicts and to
 * ensure data integrity.
 * \li \b IDLE No activity on the bus (between a \b Stop and a new \b Start
 * condition)
 * \li \b OWNER If the master initiates a transaction successfully
 * \li \b BUSY If another master is driving the bus
 * \li \b UNKNOWN If the master that has recently been enabled or connected to
 * the bus.
 *
 * The bus state diagram can be seen below.
 * \li S: Start condition
 * \li P: Stop condition
 * \li Sr: Repeated start condition
 * \dot
 * digraph bus_states{
 *   reset [label="", style=invisible];
 *   unknown [label="UNKNOWN", shape=circle, height=1.5];
 *   idle [label="IDLE", shape=circle, height=1.5];
 *   owner [label="OWNER", shape=circle, height=1.5];
 *   busy [label="BUSY", shape=circle, height=1.5];
 *
 *  {rank=same; idle busy}
 *
 *   reset->unknown [label="RESET"];
 *   unknown->idle [label="P + Timeout"];
 *   idle->owner [label="Write ADDR \n(S)"];
 *   idle->busy [label="S"];
 *   owner->owner [label="Write ADDR (Sr)"];
 *   owner->idle [label="Command P"];
 *   owner->busy [label="Arbitration \nLost"];
 *   busy->busy [label="Sr"];
 *   busy->idle [label="P + Timeout"];
 * }
 * \enddot
 *
 * \subsection sleep_modes Operation in Sleep Modes
 * The I2C module can operate in all sleep modes by setting the run_in_standby
 * option in the \ref i2c_master_conf or \ref i2c_slave_conf struct. The operation in Slave and Master Mode
 * is shown in the table below.
 *
 * <table>
 *   <tr>
 *      <th>active_in_sleep</th>
 *      <th>Slave</th>
 *      <th>Master</th>
 *   </tr>
 *   <tr>
 *      <th>false</th>
 *      <td>Disabled, all reception is dropped</td>
 *      <td>GCLK disabled when master is idle</td>
 *   </tr>
 *   <tr>
 *      <th>true</th>
 *      <td>Wake on address match when enabled</td>
 *      <td>GCLK is enabled while in sleep modes</td>
 *   </tr>
 * </table>
 *
 * \section dependencies Dependencies
 * The I2C driver has the following dependencies:
 * \li \b SERCOM
 *
 * \section special_cons Special Considerations
 *
 * \section extra_info Extra Information
 * For extra information see \ref i2c_extra_info.
 *
 * \section module_examples Examples
 * - \ref quickstart
 * \section api_overview API Overview
 * @{
 */

/**
 * \brief I2C packet for read/write
 *
 * Structure to be used when transferring I2C packets. Used both for master and
 * slave.
 */
typedef struct {
	/** Address to slave device.  */
	uint8_t address;
	/** Length of data array. */
	uint8_t data_length;
	/** Data array containing all data to be transfered.*/
	uint8_t *data;
}i2c_packet_t;

/** @} */


#ifdef __cplusplus
}
#endif

/**
 * \page i2c_extra_info Extra Information
 *
 * \section acronyms Acronyms
 * Below is a table listing the acronyms used in this module, along with their
 * intended meanings.
 *
 * <table>
 *	<tr>
 *		<th>Acronym</th>
 *		<th>Description</th>
 *	</tr>
 *	<tr>
 *		<td>SDA</td>
 *		<td>Serial Data Line</td>
 *	</tr>
 *	<tr>
 *		<td>SCL</td>
 *		<td>Serial Clock Line</td>
 *	</tr>
  * </table>
 *
 * \section workarounds Workarounds implemented by driver
 * No workarounds in driver.
 *
 * \section module_history Module History
 * Below is an overview of the module history, detailing enhancements and fixes
 * made to the module since its first release. The current version of this
 * corresponds to the newest version listed in the table below.
 *
 * <table>
 *	<tr>
 *		<th>Changelog</th>
 *	</tr>
 *	<tr>
 *		<td>Initial Release</td>
 *	</tr>
 * </table>
 */

/**
 * \page quickstart Quick Start Guides for the SERCOM I2C module
 *
 * This is the quick start guide list for the \ref sam0_i2c_group module, with
 * step-by-step instructions on how to configure and use the driver in a
 * selection of use cases.
 *
 * The use cases contain several code fragments. The code fragments in the
 * steps for setup can be copied into a custom initialization function of the
 * user application and run at system startup, while the steps for usage can be
 * copied into the normal user application program flow.
 *
 * \see General list of module \ref module_examples "examples".
 *
 * \section use_cases I2C driver use cases
 * - \subpage basic_use_case
 */

#endif /* I2C_MASTER_H_INCLUDED */