/**************************************************************************
	Souliss - Sleeping
	
	A battery operated node (Peer) is activated on press of a pushbutton and/or on
	time base, each time the node wake-ups it sends data to the gateway (that is always
	powered on).
	
	Run this code on one of the following boards:
	  - Arduino with Nordic nRF24L01 or nRF24L01+
	  
	As option you can run the same code on the following, just changing the
	relevant configuration file at begin of the sketch
	  -	Chibiduino v1
	  - Moteino
	  
***************************************************************************/

// Configure the framework
#include "bconf/StandardArduino.h"			// Use a standard Arduino
#include "conf/ethW5100.h"					// Ethernet through Wiznet W5100
#include "conf/nRF24L01.h"
#include "conf/Gateway.h"					// The main node is the Gateway

// Include framework code and libraries
#include <SPI.h>
#include "Souliss.h"

// Define the network configuration according to your router settings
uint8_t ip_address[4]  = {192, 168, 1, 77};
uint8_t subnet_mask[4] = {255, 255, 255, 0};
uint8_t ip_gateway[4]  = {192, 168, 1, 1};
#define	Gateway_address	0x6501				// The Gateway node has two address, one on the Ethernet side
											// and the other on the wireless one
#define	Peer_address	0x6502
#define myvNet_address	ip_address[3]		// The last byte of the IP address (77) is also the vNet address
#define	myvNet_subnet	0xFF00
#define	myvNet_supern	Gateway_address

#define LIGHT1_NODE1			0			 
#define LIGHT2_NODE1			1			 
#define LIGHT1_NODE2			0			 
#define LIGHT2_NODE2			1			 

void setup()
{	
	Initialize();
	
	// Set network parameters
	Souliss_SetIPAddress(ip_address, subnet_mask, ip_gateway);							// Address on the Ethernet interface
	SetAsGateway(myvNet_address);														// Set this node as gateway for SoulissApp	
	Souliss_SetAddress(Gateway_address, myvNet_subnet, myvNet_supern);					// Address on the wireless interface	

	// This node as gateway will get data from the Peer
	SetAsBatteryNode(Peer_address, 1);
}

void loop()
{   
	// Here we start to play
	EXECUTEFAST() {						
		UPDATEFAST();	
		
		// Here we process the communication
		FAST_GatewayComms();		
	}	
} 
