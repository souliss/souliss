/**************************************************************************
	Souliss - Control my Garage
	
	It control a garage door with two devices: one device act on the relays
	that drive the motor and get the limit switches, the other has one 
	hardwired pushbutton for opening and closing the door.
	
	This logics is fine if act on device with over current protection and 
	obstacle detection, if not limit switches are mandatory.
	
	The door can be controller via Android (or any equivalent direct user 
	interface), connecting to node number two.
		
	Applicable for:
		- Garage doors
	
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
	
	The first device has
		- Limit Switch Open on Pin 2
		- Limit Switch Close on Pin 3
		- Open Coil on Pin 8
		- Close Coil on Pin 9
		- Light Coil on Pin 7
		
	The second device has
		- An input on Pin 2
 
	CONFIGURATION IS MANDATORY BEFORE COMPILING
	Before compiling this code, is mandatory the configuration of the framework
	this ensure the use of proper drivers based functionalities and requested
	communication interface.	
	
	Configuration files are located on /conf folder, is suggested to use this 
	code on one of the boards listed below, the code can also compile on other
	boards but may require modification on I/O definitions.	
	
	Run this code on one of the following boards:
	
		Board Conf Code			Board Model
        0x03        			Arduino Ethernet (W5100) 
		0x04					Arduino with Ethernet Shield (W5100)
		0x05					Arduino with ENC28J60 Ethernet Shield	
	
	******************** Configuration Parameters *********************
	
		Configuration file		Parameter
		QuickCfg.h				#define	QC_ENABLE			0x01
		QuickCfg.h				#define	QC_BOARDTYPE		0x03, 0x04, 0x05

	Is required an additional IP configuration using the following parameters
		QuickCfg.h				const uint8_t DEFAULT_BASEIPADDRESS[] = {...}
		QuickCfg.h				const uint8_t DEFAULT_SUBMASK[]       = {...}
		QuickCfg.h				const uint8_t DEFAULT_GATEWAY[]       = {...}
			
***************************************************************************/
#include "Souliss.h"
#include "Typicals.h"
#include <SPI.h>

#define network_address_1	0x0011
#define network_address_2	0x0012
#define network_my_subnet	0xFF00
#define network_my_supern	0x0000

#define GARAGEDOOR_NODE1			0			
#define GARAGELIGHT_NODE1			1			
#define GARAGEDOOR_NODE2			0			

// define the shared memory map
U8 memory_map[MaCaco_MEMMAP];

// flag 
U8 data_changed = 0;

#define time_base_fast		10				// Time cycle in milliseconds
#define time_base_slow		10000			// Time cycle in milliseconds
#define num_phases			255				// Number of phases

U8 phase_speedy=0, phase_fast=0, phase_slow=0;
unsigned long tmr_fast=0, tmr_slow=0;  

void setup()
{	
	// Setup the network configuration
	//
	//	The vNet address is 11(hex) that is 17(dec), so the IP address is
	//	the DEFAULT_BASEIPADDRESS[] defined in ethUsrCfg.h plus 17 on last 
	//  octect. If DEFAULT_BASEIPADDRESS[] = {192, 168, 1, 0} the IP address
	//  for the board will be 192.168.1.17
	Souliss_SetAddress(network_address_1, network_my_subnet, network_my_supern);		

	// Set the typical logic to handle the garage door and light
	Souliss_SetT21 (memory_map, GARAGEDOOR_NODE1);
	Souliss_SetT11 (memory_map, GARAGELIGHT_NODE1);
	
	// Define inputs, outputs pins
	pinMode(2, INPUT);					// Hardware pulldown required
	pinMode(3, INPUT);					// Hardware pulldown required

	pinMode(7, OUTPUT);	
	pinMode(8, OUTPUT);
	pinMode(9, OUTPUT);
}


void loop()
{ 
	if(abs(millis()-tmr_fast) > time_base_fast)
	{	
		tmr_fast = millis();
		phase_fast = (phase_fast + 1) % num_phases;

		// Execute the code every 3 time_base_fast		
		if (!(phase_fast % 3))
		{
			// Use Pin3 and Pin 4 as limit switchtes
			Souliss_DigIn(2, Souliss_T2n_LimSwitch_Open, memory_map, GARAGEDOOR_NODE1);
			Souliss_DigIn(3, Souliss_T2n_LimSwitch_Close, memory_map, GARAGEDOOR_NODE1);			
			
			// Execute the logic for door or gate (It use slot 0 and 1)
			Souliss_Logic_T21(memory_map, GARAGEDOOR_NODE1, &data_changed);
			
			// Use Pin8 and Pin9 as output on the electrical load
			Souliss_DigOut(8, Souliss_T2n_Coil_Open, memory_map, GARAGEDOOR_NODE1);	
			Souliss_DigOut(9, Souliss_T2n_Coil_Close, memory_map, GARAGEDOOR_NODE1);				
		} 
		
		// Execute the code every 5 time_base_fast		  
		if (!(phase_fast % 5))
		{   
			// Retreive data from the communication channel
			Souliss_CommunicationData(memory_map, &data_changed);	
		}

		// Execute the code every 7 time_base_fast		  
		if (!(phase_fast % 7))
		{   
			// If the door or gate is open / opening, switch ON the light
			if((Souliss_Output(memory_map, GARAGEDOOR_NODE1) == Souliss_T2n_Coil_Open) || (Souliss_Output(memory_map, GARAGEDOOR_NODE1) == Souliss_T2n_LimSwitch_Open))
				Souliss_Input(memory_map, GARAGELIGHT_NODE1) = Souliss_T1n_OnCmd;
			else if((Souliss_Output(memory_map, GARAGEDOOR_NODE1) == Souliss_T2n_Coil_Close) || (Souliss_Output(memory_map, GARAGEDOOR_NODE1) == Souliss_T2n_LimSwitch_Close))
				Souliss_Input(memory_map, GARAGELIGHT_NODE1) = Souliss_T1n_OffCmd;
			
			// Execute the logic for light
			Souliss_Logic_T11(memory_map, GARAGELIGHT_NODE1, &data_changed);
			
			// Use Pin7 as output on the electrical load (light)
			Souliss_DigOut(7, Souliss_T1n_OnCoil, memory_map, GARAGELIGHT_NODE1);			
		}		
	}
	else if(abs(millis()-tmr_slow) > time_base_slow)
	{	
		tmr_slow = millis();
		phase_slow = (phase_slow + 1) % num_phases;

		// Execute the code every time_base_slow
		if (!(phase_slow % 1))
		{                 
			// Time out commands if no limit switches are received
			Souliss_T21_Timer(memory_map, GARAGEDOOR_NODE1);
		} 
	}	
} 