/**************************************************************************
	Souliss - Analog Measure
	
	An analog value is acquired throught Souliss and available at the user
	interfaces as half-precision floating point. All calculation shall be 
	performed as standard floating point and at the end converted in half
	precision using :
		void Souliss_AnalogIn(U8 pin, U8 *memory_map, U8 slot, float scaling, float bias);
	
	Is suggested a scaling to the vRef of the AVR's ADC, using an external
	amplification circuit, in order to use the whole resolution.
	
	The Device has
		- An analog sensor or device on PIN A0
 
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
#include "Souliss.h"
#include "Typicals.h"
#include <SPI.h>

#define network_address_1	0x0011
#define network_my_subnet	0xFF00
#define network_my_supern	0x0000

#define ANALOGDAQ				0			// This is the memory slot used for the execution of the logic in network_address1

#define DEADBAND				0.05		// Deadband value 5%  

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
	
	// Set the typical to use in slot 0
	Souliss_SetT51(memory_map, ANALOGDAQ);
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
			// Retreive data from the MaCaco communication channel
			Souliss_CommunicationData(memory_map, &data_changed);
		}
		
		// Execute the code every 101 time_base_fast		  
		if (!(phase_fast % 101))
		{
			// Compare the acquired input with the stored one, send the new value to the
			// user interface if the difference is greater than the deadband
			Souliss_Logic_T51(memory_map, ANALOGDAQ, DEADBAND, &data_changed);
		}
		
	}
else
	{
		phase_speedy = (phase_speedy + 1) % num_phases;
	  
		// Execute the code every 3 times of free MCU		
		if (!(phase_speedy % 3))
		{
			// Acquire data from the microcontroller ADC
			Souliss_AnalogIn(A0, memory_map, ANALOGDAQ, 0.09, 0);	// The raw data is 0-1024, scaled as 0-100% without bias (100/1024 = 0.09)
		} 
		
	}		
} 