/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 */
/**********************************************************************************
 *
 * This example is based on a Moteino board with the ENC28J60 ethernet module attached
 *
 * Connections:
 *				+-----------------------+
 * 				| Moteino R4 | ENC28J60 |
 * 				|            | or W5100 |
 * 				+------------+----------+
 *				|   7        |   CS     |
 *				|   11	     |   MOSI 	|
 * 				|   12	     |   MISO   |
 * 				|   13       | 	 SCK	|
 * 				|   RST      |	 RST	|
 * 				|   3.3V     |	 VCC	|
 * 				|   GND      |	 GND	|
 * 				+------------+----------+
 *				+-----------------------+
 * 				|   Moteino  | ENC28J60 |
 * 				|   Mega     | or W5100 |
 * 				+------------+----------+
 *				|   0        |   CS	|
 *				|   5        |   SI 	|
 * 				|   6	     |   SO     |
 * 				|   7        | 	 SCK	|
 * 				|   RST      |	 RST	|
 * 				|   3.3V     |	 VCC	|
 * 				|   GND	     |	 GND	|
 * 				+------------+----------+
 *
 *
 * This gateway is capable of using dynamic joining of nodes. The nodes contain code to
 * join the network.
 *
 * Valid boardtypes:
 *		0x60		Moteino or Moteino Mega plus W5100 Ethernet transceiver
 *		0x61		Moteino or Moteino Mega plus ENC28J60 Ethernet transceiver
 *		0x62		Moteino or Moteino Mega with RFM69CW/RFM69HCW
 *		0x63		Moteino or Moteino Mega with RFM69CW/RFM69HCW as range extender
 *
 * Gateway
 * 2014 Kim Mølgaard kim(at)moelgaardssite.dk
 **********************************************************************************/
// Comment out the below lines to use the conf/QuickCfg.h method
//
#include "bconf/inSketch.h"					// Using the inSketch mode.
#define  BOARDTYPE_INSKETCH
#define  QC_BOARDTYPE  			0x61        // Moteino or Moteino Mega plus ENC28J60 Ethernet transceiver
#define  QC_GATEWAYTYPE  		0x01    	// Define board as gateway
// End of configuration block

#include "Souliss.h"
#include "Typicals.h"
#include "SpeakEasy.h"
#include <SPI.h>

#define SWITCH					0			// This is the memory slot used for a dummy switch
#define SWITCH_PIN				3

// network addresses
#define eth_bridge	        	0x0011  	// Local ethernet address base + this gives 192.168.1.17
#define rfm69_bridge            0x6501  	// RFM69 address on this node must be 0x6501 to bridge interfaces
#define	rfm69_node				0x6502		// RFM69 address on the other node of the network
#define	network_my_subnet      	0xFF00
#define network_my_supern		0x0000

//*************************************************************************
/* Setup
 //*************************************************************************/
void setup()
{	
	Serial.begin(115200);	
	Serial.println("Gateway Init");

	// Setup the network configuration
	Souliss_SetAddress(rfm69_bridge, network_my_subnet, network_my_supern);
	Souliss_SetAddress(eth_bridge, network_my_subnet, network_my_supern);

	// Set this node as bridge for to Ethernet
	SetAsGateway(eth_bridge);

	// Set the addresses of the remote nodes, if you wish to have them join dynamically, then comment them out
	// The gateway is always node 0, so remote nodes begin at node 1, nodes has to be numbered without gaps.
	SetAsPeerNode(rfm69_node, 1); // Node on same subnet this is node 1

	// Set the logic (also called, typical) to use
	Set_DigitalInput(SWITCH);
	
	// Set the selected pin as input
	pinMode(SWITCH_PIN, INPUT);
}
//*************************************************************************
/* Main loop
 //*************************************************************************/
void loop()
{ 
	// The Souliss methods are scheduled in phases, this allow load
	// balance and proper timing.

	EXECUTEFAST() { 
		UPDATEFAST();   
		
		// Execute the code every 30 milliseconds
		FAST_50ms() { 
			// dummy input
			Souliss_DigIn(SWITCH_PIN, Souliss_T1n_OnCmd, memory_map, SWITCH);
			Logic_DigitalInput(SWITCH);
		}

		// Execute the code every 50 milliseconds
		FAST_30ms() {
			// Retreive data from the MaCaco communication channel
			Souliss_CommunicationData(memory_map, &data_changed);
		}
		
		// Execute the code every 2110 milliseconds
		FAST_2110ms() {
			// Get logic typicals once and at every refresh
			Souliss_GetTypicals(memory_map);
		}
		
		// Execute the code every 1110 milliseconds
		FAST_1110ms() {
			// Open a communication channel with remote nodes
			Souliss_CommunicationChannels(memory_map);
		}
	}
	EXECUTESLOW()  {
		UPDATESLOW();
		
		// Here we can place code that should run slowly
	}
}
