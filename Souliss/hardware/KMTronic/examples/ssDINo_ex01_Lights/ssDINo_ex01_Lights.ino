/**************************************************************************
	Souliss - Lights
	
	It handle the four relays either via IN1 to IN4 inputs, using the
	Android interface or via Modbus TCP. Connecting the relays to 
	lights or similar electrial appliace, you can get remote control
	of them.
	
	Modbus Map (Register, Functional Code)
		- Light 1 ON/OFF  Request at (R 0x00 - 0001, FC 0x01, 0x05) or (R 0x00 - 0001, FC 0x02)	
		- Light 2 ON/OFF  Request at (R 0x08 - 0009, FC 0x01, 0x05) or (R 0x08 - 0009, FC 0x02)	
		- Light 3 ON/OFF  Request at (R 0x11 - 0017, FC 0x01, 0x05) or (R 0x11 - 0017, FC 0x02)	
		- Light 4 ON/OFF  Request at (R 0x19 - 0025, FC 0x01, 0x05) or (R 0x19 - 0025, FC 0x02)	
		
		- Light 1 State at (R 0x2000 - 8193, FC 0x01) or (R 0x2000 - 8193, FC 0x02)
		- Light 2 State at (R 0x2008 - 8201, FC 0x01) or (R 0x2008 - 8201, FC 0x02)
		- Light 3 State at (R 0x2011 - 8209, FC 0x01) or (R 0x2011 - 8209, FC 0x02)
		- Light 4 State at (R 0x2019 - 8217, FC 0x01) or (R 0x2019 - 8217, FC 0x02)
		
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
		0x06					KMTronic DINo	
	
	******************** Configuration Parameters *********************
	
		Configuration file		Parameter
		QuickCfg.h				#define	QC_ENABLE			0x01
		QuickCfg.h				#define	QC_BOARDTYPE		0x06

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

#define RELAY1					0			// This is the memory slot used for the execution of the logic
#define RELAY2					1			// This is the memory slot used for the execution of the logic
#define RELAY3					2			// This is the memory slot used for the execution of the logic
#define RELAY4					3			// This is the memory slot used for the execution of the logic

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
	
	// Init the Modbus protocol, board act as Modbus slave
	ModbusInit();
	
	// Set the typical logic to use, T11 is a ON/OFF Digital Output with Timer Option
	Souliss_SetT11(memory_map, RELAY1);
	Souliss_SetT11(memory_map, RELAY2);
	Souliss_SetT11(memory_map, RELAY3);
	Souliss_SetT11(memory_map, RELAY4);
	
	// Define inputs, outputs pins
	pinMode(IN1, INPUT);	
	pinMode(IN2, INPUT);	
	pinMode(IN3, INPUT);	
	pinMode(IN4, INPUT);	
	pinMode(OUT1, OUTPUT);
	pinMode(OUT2, OUTPUT);
	pinMode(OUT3, OUTPUT);
	pinMode(OUT4, OUTPUT);	
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

		// Execute the code every 7 time_base_fast		  
		if (!(phase_fast % 7))
		{   
			// Retrieve data from the Modbus communication channel
			Modbus(memory_map);
		}			
		
		// Execute the code every 21 time_base_fast		
		if (!(phase_fast % 21))
		{
			// Define the input command
			Souliss_DigIn(IN1, Souliss_T1n_ToogleCmd, memory_map, RELAY1);
			Souliss_DigIn(IN2, Souliss_T1n_ToogleCmd, memory_map, RELAY2);						
			Souliss_DigIn(IN3, Souliss_T1n_ToogleCmd, memory_map, RELAY3);
			Souliss_DigIn(IN4, Souliss_T1n_ToogleCmd, memory_map, RELAY4);
			
			// Execute the logic
			Souliss_Logic_T11(memory_map, RELAY1, &data_changed);
			Souliss_Logic_T11(memory_map, RELAY2, &data_changed);
			Souliss_Logic_T11(memory_map, RELAY3, &data_changed);
			Souliss_Logic_T11(memory_map, RELAY4, &data_changed);
									
			// Define output relays
			Souliss_DigOut(OUT1, Souliss_T1n_Coil, memory_map, RELAY1);			
			Souliss_DigOut(OUT2, Souliss_T1n_Coil, memory_map, RELAY2);			
			Souliss_DigOut(OUT3, Souliss_T1n_Coil, memory_map, RELAY3);			
			Souliss_DigOut(OUT4, Souliss_T1n_Coil, memory_map, RELAY4);						
		} 	
		
	}
} 