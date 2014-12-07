/**************************************************************************
	Souliss - Curtains (Bridge Node)
	
	It control a curtain with two devices: one device act on the relays
	that drive the motor and get the limit switches, the other has one 
	hardwired pushbutton for opening and closing the door.
	
	This logics is fine if act on device with over current protection and 
	obstacle detection, if not limit switches are mandatory.
	
	The door can be controller via Android (or any equivalent direct user 
	interface) connecting to bridge node.
	
	Applicable for:
		- Curtains,
		- Motorized Windows.
	
	Only one command input is provided, it toogle the output within three
	states. The available output states are:
			- Close,
			- Open,
			- Stop.		
	Every change from Close to Open or viceversa goes through Stop state.
	
	After a command is issued, the device wait for a limit switch to declare
	the final state, if limit switch doesn't came within a configurable time
	the final state is declared based on the issued command.
	
	The first device has
		- Limit Switch Open on Pin 2
		- Limit Switch Close on Pin 3
		- Open Coil on Pin 8
		- Close Coil on Pin 9
		- Stop with both Open and Close coil closed
		
	The second device has
		- An input on Pin 2
 
	CONFIGURATION IS MANDATORY BEFORE COMPILING
	Before compiling this code, is mandatory the configuration of the framework
	this ensure the use of proper drivers based functionalities and requested
	communication interface.	
	
	Configuration files are located on /conf folder, is suggested to use this 
	code on one of the boards listed below.	
	
	Run this code on one of the following boards:
	
		Board Conf Code			Board Model
		0x02					Freaklabs Chibiduino with Ethernet Shield		

	******************** Configuration Parameters *********************
	
		Configuration file		Parameter
		QuickCfg.h				#define	QC_ENABLE			0x01
		QuickCfg.h				#define	QC_BOARDTYPE		0x02

		QuickCfg.h				#define	QC_GATEWAYTYPE		0x01
		
	Is required an additional IP configuration using the following parameters
		QuickCfg.h				const uint8_t DEFAULT_BASEIPADDRESS[] = {...}
		QuickCfg.h				const uint8_t DEFAULT_SUBMASK[]       = {...}
		QuickCfg.h				const uint8_t DEFAULT_GATEWAY[]       = {...}
			
***************************************************************************/
#include "Souliss.h"
#include "Typicals.h"
#include <SPI.h>

#define network_address_1		0x6512
#define chibi_network_address	0x6510
#define eth_network_address		0x0010
#define network_my_subnet		0xFF00
#define network_my_supern		0x0000

#define CURTAIN_NODE1			0			
#define CURTAIN_NODE2			0			
	
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
	Souliss_SetAddress(chibi_network_address, network_my_subnet, network_my_supern);		

	// Setup the network configuration	
	Souliss_SetAddress(eth_network_address, network_my_subnet, network_my_supern);	

	// Load the address also in the memory_map
	Souliss_SetLocalAddress(memory_map, eth_network_address);
	
	// Set the addresses of the remote nodes
	Souliss_SetRemoteAddress(memory_map, network_address_1, 1);	
	
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