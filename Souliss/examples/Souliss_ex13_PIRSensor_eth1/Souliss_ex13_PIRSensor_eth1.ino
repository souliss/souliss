/**************************************************************************
	Souliss - Light with PIR Sensor
	
	This example handle two lights as previous example but add an external
	event (like a PIR sensor) to time-on the light in the active state.
	
	If the device is set in AUTO mode, the external event drive the output
	state, is always possible to control manually the light.	

	It require an Ethernet board based on Wiznet W5100 or Microchip ENC28J60.
	
	Applicable for : 
			- Lights controller via relay,
			- Plugs and other ON/OFF devices.
	
	The device has
		- A coil (relay or other) on Pin 8 and Pin 9
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
#include "Souliss.h"
#include "Typicals.h"
#include <SPI.h>

#define network_address_1	0x0011
#define network_my_subnet	0xFF00
#define network_my_supern	0x0000

#define LIGHT1_NODE1			0			 
#define LIGHT2_NODE1			1			 		 

#define LIGHT_ON_CYCLE			10			// Light ON for 10 cycles if triggered by a PIR sensor

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
	
	// Set the typical logic to handle the lights
	Souliss_SetT12(memory_map, LIGHT1_NODE1);
	Souliss_SetT12(memory_map, LIGHT2_NODE1);
	
	// Define inputs, outputs pins
	pinMode(2, INPUT);					// Hardware pulldown required
	pinMode(3, INPUT);					// Hardware pulldown required	
	pinMode(8, OUTPUT);					// Power to relay coil for light 1
	pinMode(9, OUTPUT);					// Power to relay coil for light 2
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
			// Use Pin2 as ON/OFF command
			Souliss_DigIn(2, LIGHT_ON_CYCLE, memory_map, LIGHT1_NODE1);	
			
			// Use Pin3 as ON/OFF command
			Souliss_DigIn(3, LIGHT_ON_CYCLE, memory_map, LIGHT2_NODE1);		
			
			// Execute the logic
			Souliss_Logic_T12(memory_map, LIGHT1_NODE1, &data_changed);
			Souliss_Logic_T12(memory_map, LIGHT2_NODE1, &data_changed);

			// Use Pin8 as output on the electrical load
			Souliss_DigOut(8, Souliss_T1n_Coil, memory_map, LIGHT1_NODE1);	

			// Use Pin9 as output on the electrical load
			Souliss_DigOut(9, Souliss_T1n_Coil, memory_map, LIGHT2_NODE1);						
		} 
		
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

		// Execute the code every time_base_slow
		if (!(phase_slow % 1))
		{                 
			// The timer handle timed-on states
			Souliss_T12_Timer(memory_map, LIGHT1_NODE1);
			Souliss_T12_Timer(memory_map, LIGHT2_NODE1);						
		} 	  
	}		
} 
