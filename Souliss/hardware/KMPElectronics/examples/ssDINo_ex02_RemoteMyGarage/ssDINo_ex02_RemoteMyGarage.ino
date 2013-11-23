/**************************************************************************
	Souliss - Remote my Garage
	
	It control a garage door acting on the relays that drive the motor and 
	get the limit switches, open and close command are only available via
	Android (or any other direct) interface.
	
	This logics is fine if act on device with over current protection and 
	obstacle detection, if not limit switches are mandatory.
	
	Applicable for:
		- Garage doors,
		- Motorized Gates.
	
	Only one command input is provided, it toogle the output within three
	states. The available output states are:
			- Close,
			- Open,
			- Stop.		
	Every change from Close to Open or viceversa goes through Stop state.

	During opening and if Open, the light is automatically turned ON.
	
	After a command is issued, the device wait for a limit switch to declare
	the final state, if limit switch doesn't came within a configurable time
	the final state is declared based on the issued command.
	
	The Local Device has
		- Limit Switch Open on input 1
		- Limit Switch Close on input 2
		- Open Coil on output 1
		- Close Coil on output 2
		- Light Coil on output 3

	CONFIGURATION IS MANDATORY BEFORE COMPILING
	Before compiling this code, is mandatory the configuration of the framework
	this ensure the use of proper drivers based functionalities and requested
	communication interface.	
	
	Configuration files are located on /conf folder, is suggested to use this 
	code on one of the boards listed below.	
	
	Run this code on one of the following boards:
	
		Board Conf Code			Board Model
		0x06					KMTronic DINo	
	
	******************** Configuration Parameters *********************
	
		Configuration file		Parameter
		QuickCfg.h				#define	QC_ENABLE			0x01
		QuickCfg.h				#define	QC_BOARDTYPE		0x06, 0x20

	Is required an additional IP configuration using the following parameters
		QuickCfg.h				const uint8_t DEFAULT_BASEIPADDRESS[] = {...}
		QuickCfg.h				const uint8_t DEFAULT_SUBMASK[]       = {...}
		QuickCfg.h				const uint8_t DEFAULT_GATEWAY[]       = {...}
	
***************************************************************************/
#include "Souliss.h"
#include "SpeakEasy.h"						// Is a library to code easy Souliss examples
#include <SPI.h>

// Define the network configuration
//
// In the QuickCfg.h file set DEFAULT_BASEIPADDRESS[] = {192,168,1,0}
#define myvNet_address		0x0011 			// 0x0011 is equal to 17 in decimal
#define myvNet_subnet		0xFF00
#define myvNet_supern		0x0000
// defining myvNet_address as 0x0011 (is hexadecimal, in decimal is uqual to 17) gives
// as IP address 192.168.1.17, use this address in SoulissApp 

#define GARAGEDOOR					0			// This identify the number of the logic
#define GARAGELIGHT					1			// This identify the number of the logic

void setup()
{	
	// Setup the network configuration
	//
	// The vNet address is 11(hex) that is 17(dec), so the IP address is
	// the DEFAULT_BASEIPADDRESS[] defined in ethUsrCfg.h plus 17 on last 
	// octect. If DEFAULT_BASEIPADDRESS[] = {192, 168, 1, 0} the IP address
	// for the board will be 192.168.1.17
	Souliss_SetAddress(myvNet_address, myvNet_subnet, myvNet_supern);		
	
	SetAsGateway(myvNet_address);		// Set this node as gateway for SoulissApp	
	
	// Define the logics for this node
	Set_GarageDoor(GARAGEDOOR);
	Set_SimpleLight(GARAGELIGHT);
	
	// Define inputs, outputs pins
	pinMode(IN1, INPUT);	// Hardware pulldown required
	pinMode(IN2, INPUT);	// Hardware pulldown required

	pinMode(OUT1, OUTPUT);	
	pinMode(OUT2, OUTPUT);
	pinMode(OUT3, OUTPUT);
}


void loop()
{ 
	// Here we start to play
	EXECUTEFAST() {						
		UPDATEFAST();	
		
		FAST_70ms() {   // We check incoming communication data every 70 milliseconds
			// Here we handle here the communication with Android, commands and notification
			// are automatically assigned to MYLEDLOGIC
			ProcessCommunication();										
		}	
		  
		FAST_110ms() {   // We check incoming communication data every 110 milliseconds
			// If the door or gate is open / opening, switch ON the light
			if((Souliss_Output(myMap, GARAGEDOOR) == Souliss_T2n_Coil_Open) || (Souliss_Output(myMap, GARAGEDOOR) == Souliss_T2n_LimSwitch_Open))
				Souliss_Input(myMap, GARAGELIGHT) = Souliss_T1n_OnCmd;
			else if((Souliss_Output(myMap, GARAGEDOOR) == Souliss_T2n_Coil_Close) || (Souliss_Output(myMap, GARAGEDOOR) == Souliss_T2n_LimSwitch_Close))
				Souliss_Input(myMap, GARAGELIGHT) = Souliss_T1n_OffCmd;
			
			// Execute the logic for light
			Logic_SimpleLight(GARAGELIGHT);
			
			// Use Pin7 as output on the electrical load (light)
			Souliss_DigOut(OUT3, Souliss_T1n_OnCoil, myMap, GARAGELIGHT);			
		}	

		FAST_510ms() {   // We check incoming communication data every 510 milliseconds
			// Use input 1 and input 2 as limit switchtes
			Souliss_DigIn(IN1, Souliss_T2n_LimSwitch_Open, myMap, GARAGEDOOR);
			Souliss_DigIn(IN2, Souliss_T2n_LimSwitch_Close, myMap, GARAGEDOOR);			
			
			// Execute the logic for door or gate
			Logic_GarageDoor(GARAGEDOOR);
			
			// Use output 1 and output 2 for the electrical load
			Souliss_DigOut(OUT1, Souliss_T2n_Coil_Open, myMap, GARAGEDOOR);	
			Souliss_DigOut(OUT2, Souliss_T2n_Coil_Close, myMap, GARAGEDOOR);				
		} 		
		
	}
	
	EXECUTESLOW()
	{	
		UPDATESLOW();

		SLOW_10s() {               
			// The timer value is set by second state of DigIn2State
			Timer_GarageDoor(GARAGEDOOR);
		} 
	}	
} 