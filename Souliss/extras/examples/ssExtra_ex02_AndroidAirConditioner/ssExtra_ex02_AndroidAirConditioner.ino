/**************************************************************************
	Souliss - Android Control for Air Conditioner
	
	Control an air conditioner using the Android user interface, communication
	between the board and Android is via Ethernet and MaCaco/vNet protocol.
	
	The conditioner is controlled via IR led that emulate the original remote, is
	required a dedicated map for the device that shall be controlled, in this
	example there is a mapping for a Samsung MH026FB.
	
	Applicable for : 
			- Common RGB Led Lamps with IR Control
	
	The Board has
		- A PWM output on Pin 3	

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

#include "extras/SamsungMH026FB.h"			// Include the map for the IR codes
#include "extras/SamsungMH026FB.cpp"			

#define network_address_1	0x0011
#define network_my_subnet	0xFF00
#define network_my_supern	0x0000

#define AIRCON_SLOT			0				// This is the memory slot used for the execution of the logic in network_address1

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
	//	the base IP address with 17 as last octect (ex. 192.168.0.17)
	
	Souliss_SetAddress(network_address_1, network_my_subnet, network_my_supern);		

	// Load the address also in the memory_map
	Souliss_SetLocalAddress(memory_map, network_address_1);	
	
	// Set the air conditioner typical
	Souliss_SetT32(memory_map, AIRCON_SLOT);
	
	// Define outputs pins for IR led
	pinMode(3, OUTPUT);
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
			// Execute the logic for RGB control
			Souliss_Logic_T32(memory_map, AIRCON_SLOT, &data_changed);
			
			// Send the command to the lamp via IR
			if(Souliss_isTrigged(memory_map, AIRCON_SLOT))
			{
				sendMH026FB(memory_map, AIRCON_SLOT);
				Souliss_ResetTrigger(data_changed);
			}
			
		} 
		
		// Execute the code every 5 time_base_fast		  
		if (!(phase_fast % 5))
		{   
			// Retreive data from the communication channel
			Souliss_CommunicationData(memory_map, &data_changed);		
		}
	}
} 