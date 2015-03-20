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
		0x13		Olimex AVR-T32U4 		with MOD-ENC28J60 and MOD-IO 2(UEXT)
		0x14		Olimex OLIMEXINO-32U4 	with MOD-ENC28J60 and MOD-IO 2(UEXT)
		0x15		Olimex OLIMEXINO-328 	with MOD-ENC28J60 and MOD-IO 2(UEXT)
		0x16		Olimex AVR-T32U4 		with MOD-WIFI	  and MOD-IO 2(UEXT)
		0x17		Olimex OLIMEXINO-32U4 	with MOD-WIFI	  and MOD-IO 2(UEXT)
		0x18		Olimex OLIMEXINO-328 	with MOD-WIFI	  and MOD-IO 2(UEXT) 	
	
	******************** Configuration Parameters *********************
	
		Configuration file		Parameter
		QuickCfg.h				#define	QC_ENABLE			0x01
		QuickCfg.h				#define	QC_BOARDTYPE		0x13, 0x14, 0x15,
															0x16, 0x17, 0x18

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
#define SLOT_RGB					2			

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
	
	// Set the typical logic to use, T11 is a ON/OFF Digital Output with Timer Option
	Souliss_SetT11(memory_map, SLOT_RELAY1);
	Souliss_SetT11(memory_map, SLOT_RELAY2);
	
	// Define inputs
	MODIO_Init();
	mIO_PinMode((GPIO2 | GPIO1 | GPIO0));	

	// Set pullups for input pins
	mIO_Pullup((GPIO2 | GPIO1 | GPIO0));		
	
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
		
		// Execute the code every 71 time_base_fast		
		if (!(phase_fast % 71))
		{
			// Get all inputs states
			Souliss_MODIO_In();		
		
			// Build the command used to set the LED color
			uint8_t ledcmd[4] = {Souliss_T1n_Set, 0, 0, 0};
			
			// Set Red color for NODE 3
			if(Souliss_MODIO_LowDigIn(GPIO0, 0, memory_map, SLOT_RGB))
			{
				// Set the color for Red
				ledcmd[1] = 0xFF;
				
				// Send the ledcmd command into the slot 0 of NODE 3
				Souliss_RemoteInputs(network_address_3, 0, ledcmd, 4);
				
				// Reset the color for Red
				ledcmd[1] = 0x00;				
			}
			
			// Set Red color for NODE 3
			if(Souliss_MODIO_LowDigIn(GPIO1, 0, memory_map, SLOT_RGB))
			{
				// Set the color for Green
				ledcmd[2] = 0xFF;
				
				// Send the ledcmd command into the slot 0 of NODE 3
				Souliss_RemoteInputs(network_address_3, 0, ledcmd, 4);
				
				// Reset the color for Green
				ledcmd[2] = 0x00;				
			}

			// Set Red color for NODE 3
			if(Souliss_MODIO_LowDigIn(GPIO2, 0, memory_map, SLOT_RGB))
			{
				// Set the color for Blue
				ledcmd[3] = 0xFF;
				
				// Send the ledcmd command into the slot 0 of NODE 3
				Souliss_RemoteInputs(network_address_3, 0, ledcmd, 4);
				
				// Reset the color for Blue
				ledcmd[3] = 0x00;				
			}
			
		}
			
		// Execute the code every 21 time_base_fast		
		if (!(phase_fast % 21))
		{				
			
			// Execute the logic
			Souliss_Logic_T11(memory_map, SLOT_RELAY1, &data_changed);
			Souliss_Logic_T11(memory_map, SLOT_RELAY2, &data_changed);
									
			// Set outputs
			Souliss_MODIO_SetOut(RELAY1, Souliss_T1n_Coil, memory_map, SLOT_RELAY1);
			Souliss_MODIO_SetOut(RELAY2, Souliss_T1n_Coil, memory_map, SLOT_RELAY2);
			
			// Act on relays based on set outputs
			Souliss_MODIO_RelayOut();		
		} 		

		// Execute the code every 31 time_base_fast		  
		if (!(phase_fast % 31))
		{   
			// Get logic typicals once and at every refresh
			Souliss_GetTypicals(memory_map, 2);
		}
		
		// Execute the code every 51 time_base_fast		  
		if (!(phase_fast % 51))
		{   
			// Open a communication channel with remote nodes
			Souliss_CommunicationChannels(memory_map, 2);
		}
		
	}	
} 