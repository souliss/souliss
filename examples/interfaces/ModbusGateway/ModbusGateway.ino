/**************************************************************************
	Souliss - Modbus Gateway
	
	...
	
	This node provides also an interface via Modbus, data are available at
	following registers:

						Input		Typicals		Outputs
	Node	Slot
	 0		 0			 0x000		 0x100			 0x200
	 0		 1			 0x001		 0x101			 0x201	
	 0		 2			 0x002		 0x102			 0x202
	 0		 3			 0x003		 0x103			 0x203
	 1		 0			 0x010		 0x110			 0x210
	 1		 1			 0x011		 0x111			 0x211
	 1		 2           0x012		 0x112			 0x212
	 1		 3           0x013		 0x113			 0x213
		...              ...		 ...			 ...
		...              ...		 ...			 ...
	 n		 0			 0x0n0		 0x1n0			 0x2n0
	 n		 1           0x0n1		 0x1n1			 0x2n1
	 n		 2           0x0n2		 0x1n2			 0x2n2
	 n		 3	         0x0n3		 0x1n3			 0x2n3	
	
 	Applicable for:
		- Light
		- Other ON/OFF electrical appliance
	
***************************************************************************/
#define	BOARDTYPE_INSKETCH
#define	DYNAMICADDRESSING_INSKETCH

#define	QC_BOARDTYPE			0x03
#define DYNAMICADDRESSING  		0x01

#include "bconf/ModbusTCP.h"				// Actually set 8 nodes with 15 slots
#include "bconf/SmallNetwork.h"				// Modbus as a smaller addressing area compared to MaCaco

#include "Souliss.h"
#include "SpeakEasy.h"						// Is a library to code easy Souliss examples
#include <SPI.h>

// Define the network configuration
uint8_t ip_address[4]  = {192, 168, 1, 17};
uint8_t subnet_mask[4] = {255, 255, 255, 0};
uint8_t ip_gateway[4]  = {192, 168, 1, 1};

#define RELAY1					0			// This is the memory slot used for the execution of the logic
#define RELAY2					1			
#define RELAY3					2			
#define RELAY4					3			

void setup()
{	
	// Define two Simple Light logics and the relays
	Set_SimpleLight(RELAY1);
	Set_SimpleLight(RELAY2);
	Set_SimpleLight(RELAY3);
	Set_SimpleLight(RELAY4);	
	
	// Setup the network configuration
	Souliss_SetIPAddress(ip_address, subnet_mask, ip_gateway);
	SetAsGateway((U16)ip_address[3]);										// Last byte of the IP address is the vNet address

	// Init the Modbus interface
	ModbusInit(myMap);
}

void loop()
{ 
	// Here we start to play
	EXECUTEFAST() {						
		UPDATEFAST();	
		
		FAST_50ms() {	// We process the logic and relevant input and output every 50 milliseconds
				
			Logic_SimpleLight(RELAY1);							// Execute the logic for Relay 1
			Logic_SimpleLight(RELAY2);							// Execute the logic for Relay 2
			Logic_SimpleLight(RELAY3);							// Execute the logic for Relay 3
			Logic_SimpleLight(RELAY4);							// Execute the logic for Relay 4
			
		} 
		
		// Here we process all communication with other nodes
		FAST_GatewayComms();	
		
		// Process the Modbus communication periodically
		FAST_110ms()	Modbus(myMap);
	}
	
	EXECUTESLOW() {	
		UPDATESLOW();

		SLOW_10s() {  // Process the timer every 10 seconds  
			Timer_SimpleLight(RELAY1);
			Timer_SimpleLight(RELAY2);	
			Timer_SimpleLight(RELAY3);
			Timer_SimpleLight(RELAY4);				
		} 	  
	}
} 