/**************************************************************************
	Souliss - Remote my Garage
	
	It control a garage door acting on the relays that drive the motor and 
	get the limit switches, open and close command are only available via
	Android (or any other direct) interface.
	
	This logics is fine if act on device with over current protection and 
	obstacle detection, if not limit switches are mandatory.
	
	To use MOD-IO2 and a communication board (MOD-ENC28J60 or MOD-WIFI)
	is required a ribbon cable that extend the UEXT bus to both the boards,
	the signal dedicated for I2C shall not be extended to the communication
	modules.
	
	Applicable for:
		- Garage doors,
		- Motorized Gates.
	
	Only one command input is provided, it toogle the output within three
	states. The available output states are:
			- Close,
			- Open,
			- Stop.		
	Every change from Close to Open or viceversa goes through Stop state.

	During opening and if Open, the light is automatically turned ON.
	
	After a command is issued, the device wait for a limit switch to declare
	the final state, if limit switch doesn't came within a configurable time
	the final state is declared based on the issued command.
	
	The Local Device has
		- Limit Switch Open on input 1
		- Limit Switch Close on input 2
		- Open Coil on output 1
		- Close Coil on output 2
		- Light Coil on output 3
  
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

#define network_address_1	0x0011
#define network_my_subnet	0xFF00
#define network_my_supern	0x0000

#define GARAGEDOOR_NODE		0			

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

	// Set the typical to use in slot 0 and 1
	Souliss_SetT21 (memory_map, GARAGEDOOR_NODE);
	
	// Define inputs : GPIO1 and GPIO0, the MODIO relays don't need an init
	MODIO2_Init();
	mIO_PinMode(GPIO1 | GPIO0);	

	// Set pullups for input pins
	mIO_Pullup((GPIO1 | GPIO0));	
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

		// Execute the code every 21 time_base_fast		
		if (!(phase_fast % 21))
		{
			// Get all inputs states
			Souliss_MODIO_In();
			
			// Use input 0 and input 1 as limit switchtes
			Souliss_MODIO_LowDigIn(GPIO0, Souliss_T2n_LimSwitch_Open, memory_map, GARAGEDOOR_NODE);
			Souliss_MODIO_LowDigIn(GPIO1, Souliss_T2n_LimSwitch_Close, memory_map, GARAGEDOOR_NODE);			
			
			// Execute the logic for door or gate
			Souliss_Logic_T21(memory_map, GARAGEDOOR_NODE, &data_changed);
			
			// Set outputs
			Souliss_MODIO_SetOut(RELAY1, Souliss_T2n_Coil_Open, memory_map, GARAGEDOOR_NODE);	
			Souliss_MODIO_SetOut(RELAY2, Souliss_T2n_Coil_Close, memory_map, GARAGEDOOR_NODE);

			// Act on relays based on set outputs
			Souliss_MODIO_RelayOut();				
		} 		
		
	}
	else if(abs(millis()-tmr_slow) > time_base_slow)
	{	
		tmr_slow = millis();
		phase_slow = (phase_slow + 1) % num_phases;

		// Execute the code every time_base_slow
		if (!(phase_slow % 1))
		{                 
			// The timer value is set by second state of DigIn2State
			Souliss_T21_Timer(memory_map, GARAGEDOOR_NODE);
		} 
	}	
} 