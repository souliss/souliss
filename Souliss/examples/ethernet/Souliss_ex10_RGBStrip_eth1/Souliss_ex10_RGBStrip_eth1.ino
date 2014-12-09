/**************************************************************************
	Souliss - RGB LED Strip
	
	Control and RGB LED or a Strip using the PWM, fade in/out and flash 
	effects are available as the Android interface.
 
	It require an Ethernet board based on Wiznet W5100 or Microchip ENC28J60,
	but can be used with different board types.
 
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

// network addresses
#define network_address_1	0x0011 			// 0x0011 is equal to 17 in decimal
#define network_my_subnet	0xFF00
#define network_my_supern	0x0000

#define LEDCONTROL			0				// This is the memory slot for the logic that handle the light
#define LEDRED				1				// This is the memory slot for the logic that handle the light
#define LEDGREEN			2				// This is the memory slot for the logic that handle the light
#define LEDBLUE				3				// This is the memory slot for the logic that handle the light

// define the shared memory map
U8 memory_map[MaCaco_MEMMAP];

// flag 
U8 data_changed = 0;

// Note, this example use a faster time base to allow a smooth effect
// while playing with the color picker for the RGB light. Is not raccomended
// to use a so faster time base for other cases.
#define time_base_fast		3				// Time cycle in milliseconds
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
	
	// Set the typical logic to use, T11 is a ON/OFF Digital Output with Timer Option
	Souliss_SetT16(memory_map, LEDCONTROL);
	
	// Define inputs, outputs pins
	pinMode(2, INPUT);					// Hardware pulldown required
	pinMode(3, OUTPUT);					// Power the LED
	pinMode(5, OUTPUT);					// Power the LED
	pinMode(6, OUTPUT);					// Power the LED	
}

void loop()
{ 
	// The Souliss methods are scheduled in phases, this allow load
	// balance and proper timing.

	if(abs(millis()-tmr_fast) > time_base_fast)
	{	
		tmr_fast = millis();
		phase_fast = (phase_fast + 1) % num_phases;

		// Execute the code every 1 time_base_fast		
		if (!(phase_fast % 1))
		{
			// Use Pin2 as ON command, Pin3 as OFF command
			Souliss_DigIn(2, Souliss_T1n_ToogleCmd, memory_map, LEDCONTROL);		
			
			// Execute the logic that handle the LED
			Souliss_Logic_T16(memory_map, LEDCONTROL, &data_changed);

			// Use the output values to control the PWM
			analogWrite(3, Souliss_Output(memory_map, LEDRED));
			analogWrite(5, Souliss_Output(memory_map, LEDGREEN));
			analogWrite(6, Souliss_Output(memory_map, LEDBLUE));

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
			Souliss_T16_Timer(memory_map, LEDCONTROL);						
		} 	  
	}	
	
	
} 