/**************************************************************************
	Souliss - IoTuino LYT Light Bulb
	
	This sketch control the Authometion LYT bubls through Souliss, the WiFi
	module LPT200 on board of IoTuino shall be configured to access your
	local WiFi network before run this sketch.
	
***************************************************************************/
#include "bconf/IoTuino.h"						// Define the board type
#include "bconf/Gateway.h"						// Define a gateway
#include "bconf/SmallNetwork.h"					// Use small network parameters (workaround on LPT200 max frame)

#include "Souliss.h"
#include "Typicals.h"
#include "SpeakEasy.h"
#include <SPI.h>

// Define logic slots, multicolor lights use four slots each
#define LYTLIGHT1			0					

void setup()
{	
	// Init the IoTuino board and the relevant communication interfaces. The WiFi LPT200 module
	// gets the address directly in DHCP, configure the WiFi module before run the sketch
	InitIoTuino();
	
	// Se the LYT bulbs (index, bulb type, addr_a, addr_b, logic slot)
	SetLYT(0, 2, 0, 1, LYTLIGHT1);

	// Define a logic to handle the bubls
	Souliss_SetLYTLamps(memory_map, LYTLIGHT1);
}

void loop()
{ 
	// Here we start to play
	EXECUTEFAST() {						
		UPDATEFAST();	
		
		FAST_10ms() {	// We process the logic and relevant input and output every 50 milliseconds
			
			// Process communication at fast rate, use only if you need better performance
			/*ProcessCommunication();*/
			Souliss_Logic_LYTLamps(memory_map, LYTLIGHT1, &data_changed);		
		} 
		
		// Here we process all communication with other nodes
		FAST_GatewayComms();	
	}	
} 