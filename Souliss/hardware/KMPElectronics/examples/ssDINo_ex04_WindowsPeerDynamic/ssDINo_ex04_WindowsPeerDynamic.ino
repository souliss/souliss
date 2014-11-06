/**************************************************************************
	Souliss - Base Sketch for DINo
	
	It handle a motorized windows or curtain and a couple of light as per
	following scheme:
		
		MOTORIZED WINDOW / CURTAIN
			
			Device			Action
			RELAY1			Activated for Open request (can be wired as NO or NC)
			RELAY2			Activated for Close request(can be wired as NO or NC)
			IN1				If activated, force an Open request
			IN2				If activated, force a Closed request
			
			RELAY3			Activated for Light 1 ON
			RELAY4			Activated for Light 2 ON
			IN3				If activated, force Light 1 ON
			IN4				If activated, force Light 2 ON
	
	Doesn't need IP configuration and support control from Android application
	
***************************************************************************/
#include "bconf/DINo_v2_Peer.h"				// Load QuickCfg.h configuration parameters automatically
#include "Souliss.h"
#include "SpeakEasy.h"						// Is a library to code easy Souliss examples
#include <SPI.h>

#define RELAY1and2				0			// This is the memory slot used for the execution of the logic
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
	
	// Define logics
	Set_Windows(RELAY1and2);
	Set_SimpleLight(RELAY3);
	Set_SimpleLight(RELAY4);		
	
	// This board (peer) request an address to the gateway one at runtine, no need
	// to configure any parameter here
	SetDynamicAddressing();
}

void loop()
{ 
	// Here we start to play
	EXECUTEFAST() {						
		UPDATEFAST();	
		
		FAST_50ms() {	// We process the logic and relevant input and output every 50 milliseconds
		
			ssDigIn(IN1, Souliss_T2n_OpenCmd_Local, RELAY1and2);		// Read inputs from IN1
			ssDigIn(IN2, Souliss_T2n_CloseCmd_Local, RELAY1and2);		// Read inputs from IN2
			ReadInput3(RELAY3);											// Read inputs from IN3
			ReadInput4(RELAY4);											// Read inputs from IN4
		
			Logic_Windows(RELAY1and2);							// Execute the logic for Relay 1 and 2
			Logic_SimpleLight(RELAY3);							// Execute the logic for Relay 3
			Logic_SimpleLight(RELAY4);							// Execute the logic for Relay 4
			
			ssDigOut(OUT1, Souliss_T2n_Coil_Open,  RELAY1and2);	// Drive the Relay 1
			ssDigOut(OUT2, Souliss_T2n_Coil_Close, RELAY1and2);	// Drive the Relay 2
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
			Timer_Windows(RELAY1and2);
			Timer_SimpleLight(RELAY3);
			Timer_SimpleLight(RELAY4);					
		} 	  
		
		// Here we periodically check for a gateway to join
		SLOW_PeerJoin();		
	}
} 