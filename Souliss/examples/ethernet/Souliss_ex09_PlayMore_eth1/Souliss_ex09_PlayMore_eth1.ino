/**************************************************************************
	Souliss - Play More
	
	This is a simulation of an interaction between a node and an user interface
	able to communicate directly via MaCaco/vNet protocols. One board is used
	as driver of and ON/OFF coil, and another board simulate the user interface
	commands and interactions.
	
	The JSON Server is active, so the board is able to parse and provide the
	status also as JSON and not only using the MaCaco/vNet protocol.
	
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
	simply achieved via MaCaco/vNet or JSON commands.
	
	Once the setup of the node is completed,
	
	JSON
		Get the state of all nodes, 
			From a standard internet browser issue an HTTP request on
			address .17 (e.g.: 192.168.0.17) http://192.168.0.17/status?all

		Force lights on both nodes 2 and 3 at state ON,
			From a standard internet browser issue an HTTP request on
			address .17 (e.g.: 192.168.0.17) http://192.168.0.17/force?typ=11&val=1
			
	MaCaco/vNet
		
		Force lights on both nodes 2 and 3 at state ON,
			UDP/IP - Address .17 (e.g.: 192.168.0.17)
			Port 230
			Frame (hex) : 0D 0C 17 11 00 64 01 34 00 00 0B 01 01
				
		Force lights on both nodes 2 and 3 at state OFF,
			UDP/IP - Address .17 (e.g.: 192.168.0.17)
			Port 230
			Frame (hex) : 0D 0C 17 11 00 64 01 34 00 00 0B 01 02
		
		Force lights on node 2 at state ON,
			UDP/IP - Address .17 (e.g.: 192.168.0.17)
			Port 230
			Frame (hex) : 0D 0C 17 11 00 64 01 33 00 00 01 01 01

		Force lights on node 2 at state OFF,
			UDP/IP - Address .17 (e.g.: 192.168.0.17)
			Port 230
			Frame (hex) : 0D 0C 17 11 00 64 01 33 00 00 01 01 02

		Force lights on node 3 at state ON,
			UDP/IP - Address .17 (e.g.: 192.168.0.17)
			Port 230
			Frame (hex) : 0D 0C 17 11 00 64 01 33 00 00 02 01 01

		Force lights on node 3 at state OFF,
			UDP/IP - Address .17 (e.g.: 192.168.0.17)
			Port 230
			Frame (hex) : 0D 0C 17 11 00 64 01 33 00 00 02 01 02

		Get the state of both nodes 2 and 3,
			UDP/IP - Address .17 (e.g.: 192.168.0.17)
			Port 230
			Frame (hex) : 0C 0B 17 11 00 64 01 21 00 00 01 02
			
			
***************************************************************************/
#include "Souliss.h"
#include "Typicals.h"
#include <SPI.h>

// Network parameters
#define network_address_1	0x0011
#define network_address_2	0x0012
#define network_address_3	0x0013
#define network_address_4	0x0014
#define network_my_subnet	0xFF00
#define network_my_supern	0x0000

// Devices connected to the board
#define 	LIGHT1_N1			0					
#define 	LIGHT1_N2			0	

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
	
	// Set the addresses of the remote nodes
	Souliss_SetRemoteAddress(memory_map, network_address_2, 1);
	Souliss_SetRemoteAddress(memory_map, network_address_3, 2);	
	
}

void loop()
{ 
	if(abs(millis()-tmr_fast) > time_base_fast)
	{	
		tmr_fast = millis();
		phase_fast = (phase_fast + 1) % num_phases;
		
		// Execute the code every 5 time_base_fast		  
		if (!(phase_fast % 5))
		{   
			// Retreive data from the communication channel
			Souliss_CommunicationData(memory_map, &data_changed);		
		}

		// Execute the code every 7 time_base_fast		  
		if (!(phase_fast % 7))
		{   
			// Parse JSON requests
			JSONServer(memory_map);		
		}

		// Execute the code every 31 time_base_fast		  
		if (!(phase_fast % 31))
		{   
			// Get logic typicals once and at every refresh
			Souliss_GetTypicals(memory_map);
		}
		
		// Execute the code every 51 time_base_fast		  
		if (!(phase_fast % 51))
		{   
			// Open a communication channel with remote nodes
			Souliss_CommunicationChannels(memory_map);
		}
		
	
	}	
} 