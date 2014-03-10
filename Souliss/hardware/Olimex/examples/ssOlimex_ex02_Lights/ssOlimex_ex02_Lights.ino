/**************************************************************************
	Souliss - Lights
	
	It handle the MOD-IO2 relays either via GPIOs inputs or using the Android
	interface. Connecting the relays to lights or similar electrial appliace, 
	you can get remote control of them. 
	
	To use MOD-IO2 and a communication board (MOD-ENC28J60 or MOD-WIFI) is
	required a ribbon cable that extend the UEXT bus to both the boards, the signal 
	dedicated for I2C shall not be extended to the communication modules.

 	Applicable for:
		- Light
		- Other ON/OFF electrical appliance
 
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
#define network_my_subnet	0xFF00
#define network_my_supern	0x0000

#define SLOT_RELAY1					0			// This is the memory slot used for the execution of the logic
#define SLOT_RELAY2					1			// This is the memory slot used for the execution of the logic

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
	
	// Set the typical logic to use, T11 is a ON/OFF Digital Output with Timer Option
	Souliss_SetT11(memory_map, SLOT_RELAY1);
	Souliss_SetT11(memory_map, SLOT_RELAY2);
	
	// Define inputs : GPIO1, GPIO0 - Outputs : GPIO6, GPIO5, the MODIO relays don't need an init
	MODIO_Init();
	mIO_PinMode(~(GPIO6 | GPIO5) & (GPIO1 | GPIO0));	

	// Set pullups for input pins
	mIO_Pullup((GPIO1 | GPIO0));		
	
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
	
		// Execute the code every 21 time_base_fast		
		if (!(phase_fast % 21))
		{
			// Get all inputs states
			Souliss_MODIO_In();
		
			// Inputs are pulled up at 3.3V, if placed at GND value the input will be trigged
			// and associated to a toggle command
			Souliss_MODIO_LowDigIn(GPIO0, Souliss_T1n_ToogleCmd, memory_map, SLOT_RELAY1);
			Souliss_MODIO_LowDigIn(GPIO1, Souliss_T1n_ToogleCmd, memory_map, SLOT_RELAY2);						
			
			// Execute the logic
			Souliss_Logic_T11(memory_map, SLOT_RELAY1, &data_changed);
			Souliss_Logic_T11(memory_map, SLOT_RELAY2, &data_changed);
									
			// Set outputs
			Souliss_MODIO_SetOut(RELAY1, Souliss_T1n_Coil, memory_map, SLOT_RELAY1);
			Souliss_MODIO_SetOut(RELAY2, Souliss_T1n_Coil, memory_map, SLOT_RELAY2);
			
			// Act on relays based on set outputs
			Souliss_MODIO_RelayOut();		
			
		} 	
		
		// Execute the code every 201 time_base_fast		  
		if (!(phase_fast % 201))
		{   
			// Period output toogle
			mIO_digitalWrite(~GPIO6 | GPIO5);
		}		
		
		// Execute the code every 251 time_base_fast		  
		if (!(phase_fast % 251))
		{   
			// Period output toogle
			mIO_digitalWrite(GPIO6 | ~GPIO5);
		}		
		
	}
} 