/**************************************************************************
	Souliss - My Network 
	
	It build a network of three nodes using MOD-IO2, MOD-IO and MOD-RGB each
	node has an Ethernet interface using MOD-ENC28J60 or MOD-WIFI. Nodes can
	interact directly and be controller via Android.
	
	The nodes are :
		
		NODE 1 
			Use MOD-IO2 as IO board, its relays are controller from NODE 2 or
			from the Android application. Its inputs are used to control the
			light color on NODE 3.
			
		NODE 2
			Use MOD-IO as IO board, its relays are controller from its inputs.
			The inputs are also used to control the relays on NODE 1, as result
			the relays on NODE 1 and 2 are controlled from same inputs.
			
		NODE 3
			Use MOD-RGB for control of RGB LED (and strips), using Android can
			be selected the LED color or sync the LEDs with the music played
			by the Android device.
			
			Color can be forced also from the inputs on NODE 1.
			
	To communication module (MOD-ENC28J60 or MOD-WIFI) require a ribbon cable 
	that extend the UEXT without the I2C signals, these are used for the IO
	boards.
 
	CONFIGURATION IS MANDATORY BEFORE COMPILING
	Before compiling this code, is mandatory the configuration of the framework
	this ensure the use of proper drivers based functionalities and requested
	communication interface.	
	
	Configuration files are located on /conf folder, is suggested to use this 
	code on one of the boards listed below.	
	
	Run this code on one of the following boards:
	
		Board Conf Code			Board Model
		0x0D		Olimex AVR-T32U4 		with MOD-ENC28J60 and MOD-IO (UEXT)
		0x0E		Olimex OLIMEXINO-32U4 	with MOD-ENC28J60 and MOD-IO (UEXT)
		0x0F		Olimex OLIMEXINO-328 	with MOD-ENC28J60 and MOD-IO (UEXT)
		0x10		Olimex AVR-T32U4 		with MOD-WIFI	  and MOD-IO (UEXT)
		0x11		Olimex OLIMEXINO-32U4 	with MOD-WIFI	  and MOD-IO (UEXT)
		0x12		Olimex OLIMEXINO-328 	with MOD-WIFI	  and MOD-IO (UEXT)	
	
	******************** Configuration Parameters *********************
	
		Configuration file		Parameter
		QuickCfg.h				#define	QC_ENABLE			0x01
		QuickCfg.h				#define	QC_BOARDTYPE		0x0D, 0x0E, 0x0F,
															0x10, 0x11, 0x12

	Is required an additional IP configuration using the following parameters
		QuickCfg.h				const uint8_t DEFAULT_BASEIPADDRESS[] = {...}
		QuickCfg.h				const uint8_t DEFAULT_SUBMASK[]       = {...}
		QuickCfg.h				const uint8_t DEFAULT_GATEWAY[]       = {...}
	
***************************************************************************/
#include "Souliss.h"
#include "Typicals.h"

// network addresses
#define network_address_1	0x0011 			// 0x0011 is equal to 17 in decimal
#define network_address_2	0x0012 			// 0x0011 is equal to 18 in decimal
#define network_address_3	0x0013 			// 0x0011 is equal to 19 in decimal
#define network_my_subnet	0xFF00
#define network_my_supern	0x0000

#define SLOT_RELAY1					0			// This is the memory slot used for the execution of the logic
#define SLOT_RELAY2					1			// This is the memory slot used for the execution of the logic
#define SLOT_RELAY3					2			// This is the memory slot used for the execution of the logic
#define SLOT_RELAY4					3			// This is the memory slot used for the execution of the logic

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
	//	The vNet address is 12(hex) that is 18(dec), so the IP address is
	//	the DEFAULT_BASEIPADDRESS[] defined in ethUsrCfg.h plus 18 on last 
	//  octect. If DEFAULT_BASEIPADDRESS[] = {192, 168, 1, 0} the IP address
	//  for the board will be 192.168.1.18
	Souliss_SetAddress(network_address_2, network_my_subnet, network_my_supern);		
	
	// Set the typical logic to use, T11 is a ON/OFF Digital Output with Timer Option
	Souliss_SetT11(memory_map, SLOT_RELAY1);
	Souliss_SetT11(memory_map, SLOT_RELAY2);
	Souliss_SetT11(memory_map, SLOT_RELAY3);
	Souliss_SetT11(memory_map, SLOT_RELAY4);
	
	// Init the IO board
	MODIO_Init();		
	
}

void loop()
{ 
	// The Souliss methods are scheduled in phases, this allow load
	// balance and proper timing.

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
			// Get all inputs states
			Souliss_MODIO_In();		
		
			// Inputs opto-isolated, you should provide an external voltage between the pins
			// is suggested a low 3.3V - 5V value.
			Souliss_MODIO_DigIn(IN1, Souliss_T1n_ToogleCmd, memory_map, SLOT_RELAY1);
			Souliss_MODIO_DigIn(IN2, Souliss_T1n_ToogleCmd, memory_map, SLOT_RELAY2);						
			Souliss_MODIO_DigIn(IN3, Souliss_T1n_ToogleCmd, memory_map, SLOT_RELAY3);
			Souliss_MODIO_DigIn(IN4, Souliss_T1n_ToogleCmd, memory_map, SLOT_RELAY4);	
				
			// Use IN1 as input also for NODE 1
			if(Souliss_Input(memory_map, SLOT_RELAY1) != 0x00)
				Souliss_RemoteInput(network_address_1, SLOT_RELAY1, Souliss_Input(memory_map, SLOT_RELAY1));
				
			// Use IN2 as input also for NODE 1			
			if(Souliss_Input(memory_map, SLOT_RELAY1) != 0x00)
				Souliss_RemoteInput(network_address_1, SLOT_RELAY1, Souliss_Input(memory_map, SLOT_RELAY1));
		}
		
		// Execute the code every 37 time_base_fast		
		if (!(phase_fast % 37))
		{			
			// Execute the logic
			Souliss_Logic_T11(memory_map, SLOT_RELAY1, &data_changed);
			Souliss_Logic_T11(memory_map, SLOT_RELAY2, &data_changed);
			Souliss_Logic_T11(memory_map, SLOT_RELAY3, &data_changed);
			Souliss_Logic_T11(memory_map, SLOT_RELAY4, &data_changed);
			
			// Set outputs
			Souliss_MODIO_SetOut(RELAY1, Souliss_T1n_Coil, memory_map, SLOT_RELAY1);
			Souliss_MODIO_SetOut(RELAY2, Souliss_T1n_Coil, memory_map, SLOT_RELAY2);
			Souliss_MODIO_SetOut(RELAY3, Souliss_T1n_Coil, memory_map, SLOT_RELAY3);
			Souliss_MODIO_SetOut(RELAY4, Souliss_T1n_Coil, memory_map, SLOT_RELAY4);
			
			// Act on relays based on set outputs
			Souliss_MODIO_RelayOut();		
		} 			
		
	}
} 