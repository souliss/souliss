/**************************************************************************
	Souliss - AirQ305 Lights
	
	It handle the AirQ305 relay board and allow the control of its four relays
	via the optoisolated inputs or using the Android interface or via any Modbus
	TCP Master application. Connecting the relays to lights or similar electrial 
	appliace, you can get remote control of them.
			
 	Applicable for:
		- Light
		- Other ON/OFF electrical appliance
 
	IMPORTANT NOTICE, the AirQ Shield use a virtual USART on pins 10 and 11 (SPI)
	those pins are also used for the serial SPI communication with the Ethernet
	module. To let this code work, you need to perform a software and an hardware
	modification to the AirQ Shield.
		
		Hardware Modification : Broke the legs relavant to pins 10 and 11 and use
								a wire to connect those to pins 5 and 6.
		
		Software Modification : In sNET library, modify the TX and RX pins from 10
								and 11 to 5 and 6.
	 
	Before compiling is MANDATORY to perform the following steps
 
	CONFIGURATION OF AIRQ SNET
	Before compiling this code, pair the AirQ Shield with the AirQ305 board that you
	would use with, insert the board address (located on the bottom of the board) in
	the AirQ_305_address define that is located below, before the setup().
 
	CONFIGURATION OF SOULISS FRAMEWORK
	Before compiling this code, is mandatory the configuration of the framework
	this ensure the use of proper drivers based functionalities and requested
	communication interface.	
	
	Configuration files are located on /conf folder, is suggested to use this 
	code on one of the boards listed below.	
	
	Run this code on one of the following boards:
	
		Board Conf Code			Board Model
		0x03        			Arduino Ethernet (W5100) 
		0x04					Arduino with Ethernet Shield (W5100) 	
	
	******************** Configuration Parameters *********************
	
		Configuration file		Parameter
		QuickCfg.h				#define	QC_ENABLE			0x01
		QuickCfg.h				#define	QC_BOARDTYPE		0x03, 0x04
		QuickCfg.h				#define	QC_GATEWAYTYPE		0x01

	Is required an additional IP configuration using the following parameters
		QuickCfg.h				const uint8_t DEFAULT_BASEIPADDRESS[] = {...}
		QuickCfg.h				const uint8_t DEFAULT_SUBMASK[]       = {...}
		QuickCfg.h				const uint8_t DEFAULT_GATEWAY[]       = {...}
	
***************************************************************************/
// Include libraries for Souliss
#include "Souliss.h"
#include "Typicals.h"
// Include libraries for AirQ sNET (needs to be downloaded separately)
#include <sNET.h>
// The sNET use a virtual USART to communicate with the 433 MHz transciver
#include <SoftwareSerial.h>
#include <SPI.h>

// Init the sNET library
sNET snet(1, 5, 6);							// Create an sNET object
AIRQ305 *board;								// Objects instance of AIRQ305 class allow to interact with an AirQ 305 board

// Souliss Network Addresses
#define network_address_1	0x0011 			// 0x0011 is equal to 17 in decimal
#define network_my_subnet	0xFF00
#define network_my_supern	0x0000

// AirQ Address
#define AirQ_305_address	5,0,1,0			// The address of you board is located on back side of the PCB

#define SLOT_RELAY1			0				// This is the memory slot used for the execution of the logic
#define SLOT_RELAY2			1				// This is the memory slot used for the execution of the logic
#define SLOT_RELAY3			2				// This is the memory slot used for the execution of the logic
#define SLOT_RELAY4			3				// This is the memory slot used for the execution of the logic

  // Set all relays with one message
#define setRELAYS(m)		setIO(0x1, &m, 1, true, -1)

// define the shared memory map
U8 memory_map[MaCaco_MEMMAP];

// flag 
U8 data_changed = 1;

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
	Souliss_SetT11(memory_map, SLOT_RELAY1);
	Souliss_SetT11(memory_map, SLOT_RELAY2);
	Souliss_SetT11(memory_map, SLOT_RELAY3);
	Souliss_SetT11(memory_map, SLOT_RELAY4);
	
	// Start sNET and wait for a control board message, then save the ID
	snet.begin();
	while((board = (AIRQ305 *)snet.getDeviceForDeviceID(AirQ_305_address)) == 0)
		snet.processMessages();
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
			// Retrieve data from the sNET radio
			snet.processMessages();
		}	
		
		// Execute the code every 31 time_base_fast		
		if (!(phase_fast % 31))
		{
			// Request input state from the AirQ board and use as Souliss inputs
			if(board->risingIN1())	Souliss_Input(memory_map, SLOT_RELAY1) = Souliss_T1n_ToogleCmd;
			if(board->risingIN2())	Souliss_Input(memory_map, SLOT_RELAY2) = Souliss_T1n_ToogleCmd;
			if(board->risingIN3())	Souliss_Input(memory_map, SLOT_RELAY3) = Souliss_T1n_ToogleCmd;
			if(board->risingIN4())	Souliss_Input(memory_map, SLOT_RELAY4) = Souliss_T1n_ToogleCmd;								
			
			// Execute the logic
			Souliss_Logic_T11(memory_map, SLOT_RELAY1, &data_changed);
			Souliss_Logic_T11(memory_map, SLOT_RELAY2, &data_changed);
			Souliss_Logic_T11(memory_map, SLOT_RELAY3, &data_changed);
			Souliss_Logic_T11(memory_map, SLOT_RELAY4, &data_changed);	
			
			// If there is a change, force data to the AirQ board
			if(data_changed) 
			{
				uint8_t relays=(AIRQ305_RELAY1_MASK*Souliss_Output(memory_map, SLOT_RELAY1) + 
								AIRQ305_RELAY2_MASK*Souliss_Output(memory_map, SLOT_RELAY2) + 
								AIRQ305_RELAY3_MASK*Souliss_Output(memory_map, SLOT_RELAY3) + 
								AIRQ305_RELAY4_MASK*Souliss_Output(memory_map, SLOT_RELAY4));
				
				// Send data
				board->setRELAYS(relays);
			}
		} 			
	}
} 
