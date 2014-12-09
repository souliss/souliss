/**************************************************************************
	Souliss - openHAB HTTP/XML Gateway Interface
	
	This node is intented to collect data from the whole Souliss network
	and provide such data to the openHAB HTTP binding as XML strings.
	It runs a bunch of logics that handle a simple ON/OFF device, like a
	standard light bulb.
	
	The sketch is intended to run on Arduino Mega with Ethernet Shield 
	(official one with Wiznet W5100) but can be modified to suit other
	type of boards.
	
	Refer to the Souliss' wiki in order to modify the sketch as per your
	needs.	

***************************************************************************/

// Define the type of board, available values and their meaning are listed in QuickCfg.h
#include "bconf/inSketch.h"
#include "bconf/SmallNetwork.h"
#define	QC_BOARDTYPE				0x04	// Arduino with Ethernet Shield (W5100)
#define	QC_GATEWAYTYPE				0x02	// Gateway and Data Persistance
#define	QC_INTERFACE				0x02	// openHAB HTTP XML Interface

#include "Souliss.h"
#include "SpeakEasy.h"						// Is a library to code easy Souliss examples
#include <SPI.h>

// Define the network configuration
uint8_t ip_address[4]  = {192, 168, 1, 17};
uint8_t subnet_mask[4] = {255, 255, 255, 0};
uint8_t ip_gateway[4]  = {192, 168, 1, 1};

#define	Gateway_vNetAddress			17		// The vNet address for an Ethernet IP node is the last 
											// number of the IP address
#define	Peer_vNetAddress			18							

// Define logics to run on the node
#define	LIGHT						0
#define	GATE						1
#define	TEMPERATURE					2
#define	HUMIDITY					4

// Setup the DHT sensor
#define	DHT_id1						1
#define DEADBAND					0.05
ssDHT22_Init(5, DHT_id1);

void setup()
{	
	// Setup the network configuration
	//
	// The vNet address will be equal to the last byte of the IP address
	// with the most significant one at zero, so in this case 0x0011.
	// Board reply to pings at 192.168.1.17
	Souliss_SetIPAddress(ip_address, subnet_mask, ip_gateway);
	SetAsGateway(Gateway_vNetAddress);										// Last byte of the IP address is the vNet address
	SetAsPeerNode(Peer_vNetAddress,1);
	
	// Define the logics for this node
	Souliss_SetT11(memory_map, LIGHT);
	Souliss_SetT14(memory_map, GATE);
	Souliss_SetT52(memory_map, TEMPERATURE);
	Souliss_SetT53(memory_map, HUMIDITY);
	
	// Define inputs, outputs pins
	pinMode(22, OUTPUT); 													// Output to control the GATE (relay)
	pinMode(23, INPUT); 													// Pushbutton to power ON and the OFF the LIGHT (pulldown required)
	pinMode(26, OUTPUT);													// Output to control the LIGHT (relay)

	// Init the openHAB HTTP/XML Interface
	XMLSERVERInit(myMap);
	
	// Init the DHT sensor
	ssDHT_Begin(DHT_id1);	
}

void loop()
{ 
	// Here we start to play
	EXECUTEFAST() {						
		UPDATEFAST();	
		
		FAST_GatewayComms();	

		FAST_110ms() {   // We check the openHAB HTTP/XML interface every 70 milliseconds
			XMLSERVERInterface(myMap);		
		}
		
		FAST_510ms() {   // We check incoming communication data every 510 milliseconds
				
			// Control the LIGHT	
			ssDigIn(23, Souliss_T1n_ToogleCmd, LIGHT); 				// Use the pin 23 as ON/OFF toggle command
			Logic_SimpleLight(LIGHT);								// Execute the logic for LIGHT
			ssDigOut(26, Souliss_T1n_Coil, LIGHT);					// Control the pin as per LIGHT logic

			// Control the GATE
			Souliss_Logic_T14(memory_map, GATE, &data_changed);		// Execute the logic for GATE
			Souliss_DigOut(22, Souliss_T1n_Coil, memory_map, GATE);	// Control the pin as per GATE logic
		}

		FAST_1110ms() {
			Souliss_Logic_T52(memory_map, TEMPERATURE, DEADBAND, &data_changed);
			Souliss_Logic_T53(memory_map, HUMIDITY, DEADBAND, &data_changed);
		}
	}
	
	EXECUTESLOW()
	{	
		UPDATESLOW();

		SLOW_50s() {               
			// Read temperature value from DHT sensor and convert from single-precision to half-precision
			float temperature = ssDHT_readTemperature(DHT_id1);
			Souliss_ImportAnalog(memory_map, TEMPERATURE, &temperature);
						
			// Read humidity value from DHT sensor and convert from single-precision to half-precision
			float humidity = ssDHT_readHumidity(DHT_id1);
			Souliss_ImportAnalog(memory_map, HUMIDITY, &humidity);						
		} 
	}	
}