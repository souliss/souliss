/**************************************************************************
	Souliss - Follow Me
	
	A couple of DINo boards acts as ON/OFF driver for the four available relays,
	with remote and local control. Relays can be controlled locally using the 
	IN1 to IN4 inputs or via a Android user interface. 
	
	The hardwired comman toogle ON or OFF the relay, if hold turn ON for a fixed
	amout of time and later put OFF the relay. Each command is automatically 
	replicated on the twin node.
	
	Applicable for : 
			- Lights,
			- Doors and gates lock,
			- Other single relay controlled devices

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

#define network_address_1	0x0011
#define network_address_2	0x0012
#define network_my_subnet	0xFF00
#define network_my_supern	0x0000

#define RELAY1					0			// This is the memory slot used for the execution of the logic
#define RELAY2					1			// This is the memory slot used for the execution of the logic
#define RELAY3					2			// This is the memory slot used for the execution of the logic
#define RELAY4					3			// This is the memory slot used for the execution of the logic
#define RELAYNO					4			
#define ONTIME					15			// Light is on for ONTIME number of cycles

// define the shared memory map
U8 memory_map[MaCaco_MEMMAP];

// flag 
U8 data_changed = 0;

#define time_base_fast			10				// Time cycle in milliseconds
#define time_base_slow			10000			// Time cycle in milliseconds
#define num_phases				255				// Number of phases

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
	
	// Set the typical to use
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
			U8 hw=0;
			
			// Define the input command
			if(Souliss_DigInHold(IN1, Souliss_T1n_ToogleCmd, ONTIME, memory_map, RELAY1))
				hw++;
			if(Souliss_DigInHold(IN2, Souliss_T1n_ToogleCmd, ONTIME, memory_map, RELAY2))
				hw++;
			if(Souliss_DigInHold(IN3, Souliss_T1n_ToogleCmd, ONTIME, memory_map, RELAY3))
				hw++;
			if(Souliss_DigInHold(IN4, Souliss_T1n_ToogleCmd, ONTIME, memory_map, RELAY4))
				hw++;
						
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
			
			// If an hardwired command was received
			if(hw)
			{		
				U8 cmd[RELAYNO];
				
				// Define the command to align the status of the twin node
				for(U8 i=RELAY1;i<RELAYNO;i++)
					if(Souliss_Output(memory_map, i))
						cmd[i]=Souliss_T1n_OnCmd;
					else
						cmd[i]=Souliss_T1n_OffCmd;
				
				// Sync the remote node with the actual status of this node
				Souliss_RemoteInputs(network_address_2, RELAY1, cmd, RELAYNO);
			
				// Reset	
				hw=0;						
			}			
		}

		// Execute the code every 31 time_base_fast		  
		if (!(phase_fast % 31))
		{   
			// Get logic typicals once and at every refresh
			Souliss_GetTypicals(memory_map);
		}
		
		// Execute the code every 51 time_base_fast		  
		if (!(phase_fast % 51))
		{   
			// Open a communication channel with remote nodes
			Souliss_CommunicationChannels(memory_map);
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
			Souliss_T11_Timer(memory_map, RELAY1);	
			Souliss_T11_Timer(memory_map, RELAY2);	
			Souliss_T11_Timer(memory_map, RELAY3);	
			Souliss_T11_Timer(memory_map, RELAY4);				
		} 			
		
	}	
} 