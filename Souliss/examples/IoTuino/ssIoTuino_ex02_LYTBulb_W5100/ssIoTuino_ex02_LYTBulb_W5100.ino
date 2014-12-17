/**************************************************************************
	Souliss - IoTuino LYT Light Bulb
	
	This sketch control the Authometion LYT bubls through Souliss, the WiFi
	module LPT200 on board of IoTuino shall be configured to access your
	local WiFi network before run this sketch.
	
***************************************************************************/
#include "bconf/IoTuino_RadioOnly_W5100.h"		// Define the board type
#include "bconf/Gateway.h"						// Define a gateway

#include "Souliss.h"
#include "Typicals.h"
#include "SpeakEasy.h"
#include <SPI.h>

// Define logic slots, multicolor lights use four slots
#define LYTLIGHT1			0					

// Define the network configuration
uint8_t ip_address[4]  = {192, 168, 1, 17};
uint8_t subnet_mask[4] = {255, 255, 255, 0};
uint8_t ip_gateway[4]  = {192, 168, 1, 1};

void setup()
{	
	// Init the IoTuino radio using pin9 as chip select for the SPI communication
	InitIoTuino_RadioOnly(9);

	// Setup the IP configuration
	//
	// The vNet address will be equal to the last byte of the IP address
	// with the most significant one at zero, so in this case 0x0011.
	// Board reply to ping at 192.168.1.17
	Souliss_SetIPAddress(ip_address, subnet_mask, ip_gateway);
	SetAsGateway((U16)ip_address[3]);
	
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
		
		FAST_10ms() {	// We process the logic and relevant input and output every 110 milliseconds
			ProcessCommunication();
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