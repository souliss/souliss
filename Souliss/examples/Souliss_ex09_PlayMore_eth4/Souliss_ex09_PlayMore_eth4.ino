/**************************************************************************
	Souliss - Play More
	
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

		QuickCfg.h				#define	QC_GATEWAYTYPE		0x00
		
	Is required an additional IP configuration using the following parameters
		QuickCfg.h				const uint8_t DEFAULT_BASEIPADDRESS[] = {...}
		QuickCfg.h				const uint8_t DEFAULT_SUBMASK[]       = {...}
		QuickCfg.h				const uint8_t DEFAULT_GATEWAY[]       = {...}
	
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
#define time_base_slow		1000			// Time cycle in milliseconds
#define num_phases			255				// Number of phases

U8 phase_speedy=0, phase_fast=0, phase_slow=0;
unsigned long tmr_fast=0, tmr_slow=0;  

void setup()
{	
   	// setup the serial and wireless interfaces	
	Serial.begin(9600);
    
	// Setup the network configuration
	//
	//	The vNet address is 14(hex) that is 20(dec), so the IP address is
	//	the DEFAULT_BASEIPADDRESS[] defined in ethUsrCfg.h plus 18 on last 
	//  octect. If DEFAULT_BASEIPADDRESS[] = {192, 168, 1, 0} the IP address
	//  for the board will be 192.168.1.20
	Souliss_SetAddress(network_address_4, network_my_subnet, network_my_supern);		

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
	}
	else if(abs(millis()-tmr_slow) > time_base_slow)
	{	
		tmr_slow = millis();
		phase_slow = (phase_slow + 1) % num_phases;

		// Execute the code every 11 time_base_slow
		if(!(phase_slow % 11))
		{
			// Request to force the input registers for node 1
		
			Serial.println("");
			Serial.println("<Send Force Request for Node 1>");
			
			#define NODE_ID					1
			#define NUMBER_OF_SLOT			8
			U8 cmd[]={Souliss_T1n_OnCmd, 0, 0, 0, 0, 0, 0, 0};
			MaCaco_send(network_address_1, MaCaco_FORCE, 0, NODE_ID, NUMBER_OF_SLOT, cmd);
		}		

		// Execute the code every 11 time_base_slow
		if(!(phase_slow % 11))
		{
			// Request to force the input registers for node 2
		
			Serial.println("");
			Serial.println("<Send Force Request for Node 2>");
			
			#define NODE_ID					2
			#define NUMBER_OF_SLOT			8
			U8 cmd[]={Souliss_T1n_OnCmd, 0, 0, 0, 0, 0, 0, 0};
			MaCaco_send(network_address_1, MaCaco_FORCE, 0, NODE_ID, NUMBER_OF_SLOT, cmd);
		}

		
		// Execute the code every 121 time_base_slow
		if(!(phase_slow % 121))
		{	
			// Request the actual values of the typical logic used in node 0
		
			Serial.println("");
			Serial.println("<Send Typical Logic Request>");	
		
			#define FIRST_NODE_ID			1
			#define NUMBER_OF_NODES			2
			MaCaco_send(network_address_1, MaCaco_TYPREQ, 0, FIRST_NODE_ID, NUMBER_OF_NODES, 0);
		}	
	
		// Execute the code every 231 time_base_slow
		if(!(phase_slow % 231))
		{
			// Request the actual status of the devices connected to node 0
		
			Serial.println("");		
			Serial.println("<Send State Request>");
			
			#define FIRST_NODE_ID			1
			#define NUMBER_OF_NODES			2
			MaCaco_send(network_address_1, MaCaco_STATEREQ, 0, FIRST_NODE_ID, NUMBER_OF_NODES, 0);
		}		
		
    }
		
}