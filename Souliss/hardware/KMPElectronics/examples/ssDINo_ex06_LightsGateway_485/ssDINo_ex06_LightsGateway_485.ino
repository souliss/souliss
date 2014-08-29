/**************************************************************************
	Souliss - Lights
	
	It handle the four relays either via IN1 to IN4 inputs or using the
	Android interface. Connecting the relays to lights or similar electrial 
	appliace, you can get remote control of them.
	
	The Gateway node is connected via Ethernet to the Android (or any other)
	interface and via RS485 to all the peer nodes. 
	Use a twisted pair wire to connect the RS485 port of the Gateway to the
	nearest peer and from that peer to the next up to the end of the chain.
	
	Connection scheme:
	
	HOME-ROUTER						GATEWAY		
	ETHERNET		(connect)		ETHERNET	

	GATEWAY							PEER1
	RS-485 A		(connect)		RS-485 A
	RS-485 B    	(connect)   	RS-485 B
	RS-485 GND		(optional)     	RS-485 GND
	
	PEER1							PEER2
	RS-485 A		(connect)		RS-485 A
	RS-485 B    	(connect)   	RS-485 B
	RS-485 GND		(optional)     	RS-485 GND	
	
	PEER2							PEER3
	RS-485 A		(connect)		RS-485 A
	RS-485 B    	(connect)   	RS-485 B
	RS-485 GND		(optional)     	RS-485 GND	
	
 	Applicable for:
		- Light
		- Other ON/OFF electrical appliance
	
***************************************************************************/
#include "bconf/DINo_v2_Gateway_RS485.h"	// Load QuickCfg.h configuration parameters automatically
#include "Souliss.h"
#include "SpeakEasy.h"						// Is a library to code easy Souliss examples
#include <SPI.h>

#define	gateway_rs485			0xCE01
#define	peer1_rs485				0xCE11
#define	peer2_rs485				0xCE12
#define	peer3_rs485				0xCE13
#define	subnetmask				0xFF00

#define RELAY1					0			// This is the memory slot used for the execution of the logic
#define RELAY2					1			
#define RELAY3					2			
#define RELAY4					3			

// Define the network configuration
uint8_t ip_address[4]  = {192, 168, 1, 17};
uint8_t subnet_mask[4] = {255, 255, 255, 0};
uint8_t ip_gateway[4]  = {192, 168, 1, 1};

void setup()
{	
	// Init the board
	InitDINo();
	
	// Set the inputs
	SetInput1();
    SetInput2();
    SetInput3();
    SetInput4();
	
	// Set the outputs
	SetRelay1();
	SetRelay2();
	SetRelay3();
	SetRelay4();
	
	// Set and turn ON the status LED
	SetLED();
	TurnOnLED();
	
	// Define two Simple Light logics and the relays
	Set_SimpleLight(RELAY1);
	Set_SimpleLight(RELAY2);
	Set_SimpleLight(RELAY3);
	Set_SimpleLight(RELAY4);	
	
	// Setup the network configuration
	//
	// The vNet address will be equal to the last byte of the IP address
	// with the most significant one at zero, so in this case 0x0011.
	// Board reply to pings at 192.168.1.17
	Souliss_SetIPAddress(ip_address, subnet_mask, ip_gateway);
	SetAsGateway((U16)ip_address[3]);										// Last byte of the IP address is the vNet address
	
	// Define the address for the RS485 interface
	Souliss_SetAddress(gateway_rs485, subnetmask, 0x0000);         
} 

void loop()
{ 
	// Here we start to play
	EXECUTEFAST() {						
		UPDATEFAST();	
		
		FAST_50ms() {	// We process the logic and relevant input and output every 50 milliseconds
		
			ReadInput1(RELAY1);									// Read inputs from IN1
			ReadInput2(RELAY2);									// Read inputs from IN2
			ReadInput3(RELAY3);									// Read inputs from IN3
			ReadInput4(RELAY4);									// Read inputs from IN4
		
			Logic_SimpleLight(RELAY1);							// Execute the logic for Relay 1
			Logic_SimpleLight(RELAY2);							// Execute the logic for Relay 2
			Logic_SimpleLight(RELAY3);							// Execute the logic for Relay 3
			Logic_SimpleLight(RELAY4);							// Execute the logic for Relay 4
			
			CntRelay1(RELAY1);									// Drive the Relay 1
			CntRelay2(RELAY2);                                  // Drive the Relay 2
			CntRelay3(RELAY3);									// Drive the Relay 3
			CntRelay4(RELAY4);                                  // Drive the Relay 4			
		} 
		
		// Here we process all communication with other nodes
		FAST_GatewayComms();	
	}
	
	EXECUTESLOW() {	
		UPDATESLOW();

		SLOW_10s() {  // Process the timer every 10 seconds  
			Timer_SimpleLight(RELAY1);
			Timer_SimpleLight(RELAY2);	
			Timer_SimpleLight(RELAY3);
			Timer_SimpleLight(RELAY4);				
		} 	  
	}
} 
