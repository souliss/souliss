/**************************************************************************
	Souliss - XBoard Relay (Peer Node)
	
	It handle the two relays either using the Android interface. 
	Connecting the relays to lights or similar electrial appliace, you can 
	get remote control of them.

	Check the current and voltage rating against the board specification.
		
 	Applicable for:
		- Light
		- Other ON/OFF electrical appliance

	CONFIGURATION IS MANDATORY BEFORE COMPILING
	Before compiling this code, is mandatory the configuration of the framework
	this ensure the use of proper drivers based functionalities and requested
	communication interface.	
	
	Configuration files are located on /conf folder, is suggested to use this 
	code on one of the boards listed below.	
	
	Run this code on one of the following boards:
	
		Board Conf Code			Board Model
		0x30					DFRobots XBoard Relay	
	
	******************** Configuration Parameters *********************
	
		Configuration file		Parameter
		QuickCfg.h				#define	QC_ENABLE			0x01
		QuickCfg.h				#define	QC_BOARDTYPE		0x30

		QuickCfg.h				#define	QC_GATEWAYTYPE		0x00
		QuickCfg.h				#define	DYNAMICADDRESSING	0x01
		QuickCfg.h				#define	ETHERNETMACRAW		0x01
		
***************************************************************************/
#include "Souliss.h"
#include "SpeakEasy.h"						// Is a library to code easy Souliss examples
#include <SPI.h>

#define RELAY1					0			// This is the memory slot used for the execution of the logic
#define RELAY2					1			// This is the memory slot used for the execution of the logic

void setup()
{	
	// This board (peer) request an address to the gateway one at runtine, no need
	// to configure any parameter here
	SetDynamicAddressing();

	// Define two Simple Light logics and the relays
	Set_SimpleLight(RELAY1);
	Set_SimpleLight(RELAY2);
	SetRelay1();
	SetRelay2();
}

void loop()
{ 
	// Here we start to play
	EXECUTEFAST() {						
		UPDATEFAST();	
		
		FAST_50ms() {	// We process the logic and relevant input and output every 50 milliseconds
			Logic_SimpleLight(RELAY1);							// Execute the logic for Relay 1
			Logic_SimpleLight(RELAY2);							// Execute the logic for Relay 2
			
			CntRelay1(RELAY1);									// Drive the Relay 1
			CntRelay2(RELAY2);                                  // Drive the Relay 2
		} 
			  
		FAST_70ms() {   // We check incoming communication data every 70 milliseconds
			// Here we handle here the communication with Android, commands and notification
			// are automatically assigned to RELAY1 and RELAY2
			ProcessCommunication();										
		}	

		// At first runs, we look for a gateway to join
		START_PeerJoin();
		
	}
	
	EXECUTESLOW() {	
		UPDATESLOW();

		SLOW_10s() {  // Process the timer every 10 seconds  
			Timer_SimpleLight(RELAY1);
			Timer_SimpleLight(RELAY2);	
		} 

		// Here we periodically check for a gateway to join
		SLOW_PeerJoin();
	}
} 