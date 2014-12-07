/**************************************************************************
	Souliss - Twin RGB LED Strip
	
	Control a couple of RGB LED or a Strip using the RGB Shield and Ethernet 
	Shield from Open Electronics, control bright and color from an Android 
	smartphone and additionaly with an hardware switch.
 
	Brightness and color can be controller from the Android device and even
	be connected to the playing music, intesisty of Red, Green and Blue is
	forced based on the intesity of low, medium and high frequency on the
	music.
	
	If the synchronization logic is set, the first node will force its state
	on the twin one, all commands from Android or the hardware switch are then
	replicated on the twin node.
	
	CONFIGURATION IS MANDATORY BEFORE COMPILING
	Before compiling this code, is mandatory the configuration of the framework
	this ensure the use of proper drivers based functionalities and requested
	communication interface.	
	
	Configuration files are located in /conf folder, is suggested to use this 
	code on one of the boards listed below, the code can also compile on other
	boards but may require modification on I/O definitions.	
	
	Run this code on one of the following boards:
	
		Board Conf Code			Board Model
		0x05					Arduino with ENC28J60 Ethernet Shield	
								and RGB Shield
								
	******************** Configuration Parameters *********************
	
		Configuration file		Parameter
		QuickCfg.h				#define	QC_ENABLE			0x01
		QuickCfg.h				#define	QC_BOARDTYPE		0x05

		QuickCfg.h				#define	QC_GATEWAYTYPE		0x01		
		
	Is required an additional IP configuration using the following parameters
		QuickCfg.h				const uint8_t DEFAULT_BASEIPADDRESS[] = {...}
		QuickCfg.h				const uint8_t DEFAULT_SUBMASK[]       = {...}
		QuickCfg.h				const uint8_t DEFAULT_GATEWAY[]       = {...}
	
***************************************************************************/
#include "Souliss.h"
#include "Typicals.h"

// network addresses
#define network_address_1	0x0011 			// 0x0011 is equal to 17 in decimal
#define network_address_2	0x0012 			// 0x0012 is equal to 18 in decimal
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
U8 led_changed = 0;

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
	
	// Set the addresses of the remote nodes
	Souliss_SetRemoteAddress(memory_map, network_address_2, 1);	
	
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
			// Use Pin2 as ON/OFF command, use a pulldown resistor (10 k) connected between 
			// Pin2 and GND. Pressing for more than 1,5 seconds will set the GoodNight mode
			// and the light will be progressly be turned off.
			// It get the digital input state and store it in the LEDCONTROL logic
			Souliss_DigInHold(2, Souliss_T1n_ToogleCmd, 0x255, memory_map, LEDCONTROL);		
			
			// Use in inputs stored in the LEDCONTROL to control the RGB LED or Strip,
			// it provide automatically fade in and out effect and color change.
			Souliss_Logic_T16(memory_map, LEDCONTROL, &data_changed);
			
			// Use the output from the logic to control the colors via PWM in pins 3, 5 and 6
			//
			// If using a LED Strip wire as
			//	Strip Pin			RGB Shield Pin
			//	+					R+ or G+ or B+
			//	R					R-
			//	G					G-
			//	B					B-
			//
			// If using an RGB LED wire as
			//	LED Pin				RGB Shield Pin
			//	+					R+ or G+ or B+
			//	R					R- with resistor
			//	G					G- with resistor
			//	B					B- with resistor
			//			
			// If using separated LEDs for each color
			//	Strip Pin			RGB Shield Pin
			//	+ (Red)				R+ with resistor
			//	+ (Green)			G+ with resistor
			//	+ (Blue)			B+ with resistor
			//	- (Red)				R-
			//	- (Green)			G-
			//	- (Blue)			B-
			//			
			analogWrite(3, Souliss_Output(memory_map, LEDRED));
			analogWrite(5, Souliss_Output(memory_map, LEDGREEN));
			analogWrite(6, Souliss_Output(memory_map, LEDBLUE));

			// Automatically handle the communication with other nodes and
			// user interfaces, drivers for ENC28J60 are included with the framework
			Souliss_CommunicationData(memory_map, &data_changed);
		}	
		
		// Execute the code every 91 time_base_fast		  
		if (!(phase_fast % 91))
		{   
			// Get logic typicals once and at every refresh
			Souliss_GetTypicals(memory_map, 1);
		}
		
		// Execute the code every 111 time_base_fast		  
		if (!(phase_fast % 111))
		{   
			// Open a communication channel with remote nodes
			Souliss_CommunicationChannels(memory_map, 1);
		}
		
	}
	else if(abs(millis()-tmr_slow) > time_base_slow)
	{	
		tmr_slow = millis();
		phase_slow = (phase_slow + 1) % num_phases;

		// Execute the code every 2 time_base_slow
		if (!(phase_slow % 2))
		{                 
			// Timer associated to the LED logic control
			Souliss_T16_Timer(memory_map, LEDCONTROL);						
		} 	  
	}	
	
	
} 