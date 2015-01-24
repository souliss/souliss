/* vNet Example 3, Data Transmission

  A communication between and ethernet and a wireless node is estabilished,
  data are send and received trough a ethernet-wireless bridge.
  
  This sketch should run on a wireless chibiduino node.
  
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
  Serial.begin(9600);					// Open USB Serial Connection
  vNet_Init();							// Init vNet
  vNet_SetAddress(0x6512, 2);			// Set Node Address
  vNet_SetSubnetMask(0xFF00, 2);		// Set Subnet Mask
  vNet_SetMySuperNode(0x6511, 2);		// Set Associated SuperNode
}

void loop()
{ 
  // We're going to add 1 to the message length to handle the terminating null 
  // character for a string '/0'.
  
    vNet_SendData(0x0012, msg2, 12, 0x10);
    vNet_SendData(0x0012, msg1, 11, 0x20);
    delay(1000);
    
}