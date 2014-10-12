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

// Define logic slots, multicolor lights use four slots
#define LYTLIGHT1			0					

void setup()
{	
	/////////////////////////////////
	/*	Workaround, the GETIP from the WiFi module isn't working properly, so we set the IP
		address manually. This address should match with the once received from the DHCP server
	*/	
	uint8_t ipaddr[4]={192,168,0,222};
	setip(ipaddr);
	////////////////////////////////

	// Init the IoTuino board and the relevant communication interfaces. The WiFi LPT200 module
	// gets the address directly in DHCP, configure the WiFi module before run the sketch
	InitIoTuino();
	
	// Se the LYT bulbs (index, bulb type, addr_a, addr_b, logic slot)
	SetLYT(0, 2, 0, 1, LYTLIGHT1);

	// Define a logic to handle the bulb(s)
	SetLYTLamps(LYTLIGHT1);	
}

void loop()
{ 
	// Here we start to play
	EXECUTEFAST() {						
		UPDATEFAST();	
		
		FAST_110ms() {	// We process the logic and relevant input and output every 110 milliseconds
			LogicLYTLamps(LYTLIGHT1);		
		} 
		
		// Here we process all communication with other nodes
		FAST_GatewayComms();	
	}	
	EXECUTESLOW() {
		UPDATESLOW();
		
		// Slowly shut down the lamp
		SLOW_10s() {
			LYTSleepTimer(LYTLIGHT1);
		}

	}
} 