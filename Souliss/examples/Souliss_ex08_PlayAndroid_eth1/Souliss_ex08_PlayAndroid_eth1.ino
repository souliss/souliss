/**************************************************************************
	Souliss - Play Android
	
	This is a simulation of an interaction between a node and an user interface
	able to communicate directly via MaCaco/vNet protocols. One board is used
	as driver of and ON/OFF coil, and another board simulate the user interface
	commands and interactions.
	
	Applicable for : 
			- Lights,
			- Doors and gates lock.
	
	The Local Device has
		- A coil on Pin 9
		- A couple of inputs on Pin 2 and 3, pulldown required
 
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

		QuickCfg.h				#define	QC_GATEWAYTYPE		0x01

	Is required an additional IP configuration using the following parameters
		QuickCfg.h				const uint8_t DEFAULT_BASEIPADDRESS[] = {...}
		QuickCfg.h				const uint8_t DEFAULT_SUBMASK[]       = {...}
		QuickCfg.h				const uint8_t DEFAULT_GATEWAY[]       = {...}
	
***************************************************************************/

/**************************************************************************
	Simulate an user interface
	
	An useful exercise is the simulation of an user interface, that can be
	simply achieved via MaCaco/vNet commands.
	
	Once the setup of the node is completed,
	
	MaCaco/vNet
		
		Force light at state ON,
			UDP/IP - Address .17 (e.g.: 192.168.1.17)
			Port 230
			Frame (hex) : 0D 0C 17 11 00 64 01 33 00 00 00 01 01

		Force lights at state OFF,
			UDP/IP - Address .17 (e.g.: 192.168.1.17)
			Port 230
			Frame (hex) : 0D 0C 17 11 00 64 01 33 00 00 00 01 02

		Get the state,
			UDP/IP - Address .17 (e.g.: 192.168.1.17)
			Port 230
			Frame (hex) : 0C 0B 17 11 00 64 01 21 00 00 00 01
			
			
***************************************************************************/
#include "Souliss.h"
#include "Typicals.h"
#include <SPI.h>

#define network_address_1	0x0011
#define network_address_2	0x0012
#define network_my_subnet	0xFF00
#define network_my_supern	0x0000

#define SLOT1			0			// This is the memory slot used for the execution of the logic in network_address1

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

	// Load the address also in the memory_map
	Souliss_SetLocalAddress(memory_map, network_address_1);	
	
	// Set the typical to use in slot 0
	Souliss_SetT11(memory_map, SLOT1);
	
	// Define inputs, outputs pins
	pinMode(2, INPUT);	// Hardware pulldown required
	pinMode(3, INPUT);	// Hardware pulldown required
	
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
			// Use Pin2 as ON command
			Souliss_DigIn(2, Souliss_T1n_OnCmd, memory_map, SLOT1);
			
			// Use Pin3 as OFF command
			Souliss_DigIn(3, Souliss_T1n_OffCmd, memory_map, SLOT1);			
			
			// Execute the logic
			Souliss_Logic_T11(memory_map, SLOT1, &data_changed);

			// Use Pin9 as output on the electrical load
			Souliss_DigOut(9, Souliss_T1n_Coil, memory_map, SLOT1);			
		} 
		
		// Execute the code every 5 time_base_fast		  
		if (!(phase_fast % 5))
		{   
			// Retreive data from the communication channel
			Souliss_CommunicationData(memory_map, &data_changed);		
		}
	}
} 