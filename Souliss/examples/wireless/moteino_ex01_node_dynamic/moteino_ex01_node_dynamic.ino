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
 * Dynamic Node on the same subnet as the gateway
 *
 * 2014 Kim Mølgaard kim(at)moelgaardssite.dk
 * 
 **********************************************************************************/
// Comment out the below lines to use the conf/QuickCfg.h method
//
#include "bconf/inSketch.h"
#define  BOARDTYPE_INSKETCH
#define QC_BOARDTYPE				0x62 	// 0x62		Moteino or Moteino Mega with RFM69CW/RFM69HCW

// Either gateway and interface option aren't required
#define  GATEWAYTYPE_INSKETCH
#define QC_GATEWAYTYPE			0x00
// End of configuration block

#include "Souliss.h"
#include "Typicals.h"
#include "SpeakEasy.h"
#include <SPI.h>

#define INPUT0					0			// This is the memory slot used for a dummy switch

// network addresses
#define	rfm69_node				0x6502		// RFM69 address 
#define	network_my_subnet      	0xFF00
#define network_my_supern		0x6501		// RFM69 gateway


//*******************************************************
// setup, called once on startup
//*******************************************************
void setup() {

	Serial.begin(115200);
	Serial.println("Node Init");
  
	// Setup the network configuration
	Souliss_SetAddress(rfm69_node,network_my_subnet,network_my_supern);
  
	// Set the typical to use
	Set_AnalogIn(INPUT0);	// analog input for test purpose
  
	// Define inputs, outputs pins
	pinMode(A0, INPUT);

}
//*******************************************************
// main loop
//*******************************************************
void loop()
{
  // The Souliss methods are scheduled in phases, this allow load
  // balance and proper timing.
  
	EXECUTEFAST() { 
		UPDATEFAST();   
    
		// Execute the code every 30 milliseconds
		FAST_30ms() { 
			// Retreive data from the MaCaco communication channel
			Souliss_CommunicationData(memory_map, &data_changed);
		}
	
		FAST_1110ms() {
			float analogRaw=analogRead(A0);			// Read analog value from A0
			Serial.println(analogRaw);

			ssImportAnalog(INPUT0, &analogRaw); 	// Import value from A0
			Read_AnalogIn(INPUT0);
		}
	}
	EXECUTESLOW()  {
		UPDATESLOW();
		
		// Here we will attempt to join the network, if we already have joined, then it does nothing.
		// Executes every 50 seconds
		SLOW_50s() {
			Souliss_JoinNetwork();	// Join network
		}
	}
}
