/**************************************************************************
	Souliss - Modbus Peer
	
	It handle the four relays either via IN1 to IN4 inputs or using the
	Android interface. Connecting the relays to lights or similar electrical 
	appliance, you can get remote control of them.
	
	Applicable for:
		- Light
		- Other ON/OFF electrical appliance
	
***************************************************************************/
#include "bconf/DINo_v2_Peer.h"				// Load QuickCfg.h configuration parameters automatically
#include "bconf/DINo_v2_DisableIPBROADCAST.h"
#include "bconf/SmallNetwork.h"				// Modbus as a smaller addressing area compared to MaCaco

#include "Souliss.h"
#include "SpeakEasy.h"						// Is a library to code easy Souliss examples
#include <SPI.h>

// Define the network configuration
uint8_t ip_address[4]  = {192, 168, 1, 18};
uint8_t subnet_mask[4] = {255, 255, 255, 0};
uint8_t ip_gateway[4]  = {192, 168, 1, 1};

#define RELAY1					0			// This is the memory slot used for the execution of the logic
#define RELAY2					1			
#define RELAY3					2			
#define RELAY4					3			

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
	
	// Set the status LED
	SetLED();
	
	// Define two Simple Light logics and the relays
	Set_SimpleLight(RELAY1);
	Set_SimpleLight(RELAY2);
	Set_SimpleLight(RELAY3);
	Set_SimpleLight(RELAY4);	
	
	// Set the node IP address
	Souliss_SetIPAddress(ip_address, subnet_mask, ip_gateway);
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
		FAST_PeerComms();
		
		// At first runs, we look for a gateway to join
		START_PeerJoin();
		
		// Periodically check if the peer node has joined the gateway
		FAST_1110ms() {
			if(JoinInProgress())	// If join is in progress, toogle the LED at every turn
				ToogleLED();
			else
				TurnOnLED();		// Once completed, turn ON
		}		
		
	}
	
	EXECUTESLOW() {	
		UPDATESLOW();

		SLOW_10s() {  // Process the timer every 10 seconds  
			Timer_SimpleLight(RELAY1);
			Timer_SimpleLight(RELAY2);	
			Timer_SimpleLight(RELAY3);
			Timer_SimpleLight(RELAY4);				
		} 	  
		
		// Here we periodically check for a gateway to join
		SLOW_PeerJoin();		
	}
} 