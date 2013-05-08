/* vNet Example 1, Data Transmission

	A standard Node (TX) send data to a device out of its subnet (RX), 
	data are sent via the SuperNode (SN).

	The received message is printed-out by RX node, you can see the
	messagge IN/OUT activating the VNET_DEBUG functionality.	
	
	*************************Definitions*************************
	Configuration file		Parameter
	vNet_Config.h			#define VNET_SUPERNODE  	0
	vNet_Config.h			#define VNET_MEDIA1_ENABLE  0
	vNet_Config.h			#define VNET_MEDIA2_ENABLE  1
	vNet_Config.h			#define VNET_MEDIA3_ENABLE  0
	vNet_Config.h			#define VNET_MEDIA4_ENABLE  0
	vNet_Config.h			#define VNET_MEDIA5_ENABLE  0
	
	Only if debug on serial port is required,
	vNet_Config.h			#define VNET_DEBUG  		1

*/
#include "Souliss.h"
#include "Typicals.h"
#include <SPI.h>

// Message to sent over network
byte msg1[] = "Stevm scarz";
byte msg2[] = "a ftient!";

void setup()
{    
  Serial.begin(9600);					// setup the USART
  vNet_Init();							// init the vNet layer	
  vNet_SetAddress(0x6612, 2);			// set the node address
  vNet_SetSubnetMask(0xFF00, 2);		// set the node subnet mask
  vNet_SetMySuperNode(0x6601, 2);		// set the associated supernode
  
}

void loop()
{ 
  // We're going to add 1 to the message length to handle the terminating null 
  // character for a string '/0'.
	
    vNet_SendData(0x6501, msg1, 11, 0x10);		// send msg1
	delay(5000);
	
    vNet_SendData(0x6501, msg2, 12, 0x10);		// send msg2
	delay(5000);
    
}