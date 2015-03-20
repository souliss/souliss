/* vNet Example 2, Data Transmission

	Two nodes sends data via ethernet connection, they can be connected
	to a switch or cross-wired togheter.

	*************************Definitions*************************
	Configuration file		Parameter
	vNet_Config.h			#define VNET_SUPERNODE  	0
	vNet_Config.h			#define VNET_MEDIA1_ENABLE  1
	vNet_Config.h			#define VNET_MEDIA2_ENABLE  0
	vNet_Config.h			#define VNET_MEDIA3_ENABLE  0
	vNet_Config.h			#define VNET_MEDIA4_ENABLE  0
	vNet_Config.h			#define VNET_MEDIA5_ENABLE  0
	
	Define the ethernet controller based on the used hardware,
	ethUsrCfg.h				#define ETH_W5100/ENC28J60	1	
	
	ethUsrCfg.h				const uint8_t DEFAULT_BASEIPADDRESS[] = {...}
	ethUsrCfg.h				const uint8_t DEFAULT_SUBMASK[]       = {...}
	ethUsrCfg.h				const uint8_t DEFAULT_GATEWAY[]       = {...}
	ethUsrCfg.h				const uint8_t MAC_ADDRESS[]           = {...}	
	
	It's important define the proper MAC address for each board, two nodes
	that share the same MAC address will not communicate. You can found a
	proper MAC address on the sticker behind the board.

	As alternative to address the MAC for each board, the AUTO_MAC option can
	be used along with a Local MAC address. More details are available in
	ethUsrCfg.h configuration file.
	
	Only if debug on serial port is required,
	vNet_Config.h			#define VNET_DEBUG  		1
	
*/
#include "Souliss.h"
#include "Typicals.h"
#include <SPI.h>

// Message to sent over network
byte msg1[] = "Stevm scarz";
byte msg2[] = "a ftient!";

unsigned long time = 0;
uint8_t msg=0;

void setup()
{    
	// Open a serial connection	
	Serial.begin(9600);
     
  	// Set the network parameters
	eth_SetBaseIP((uint8_t *)DEFAULT_BASEIPADDRESS);
	eth_SetSubnetMask((uint8_t *)DEFAULT_SUBMASK);
	eth_SetGateway((uint8_t *)DEFAULT_GATEWAY);	
	
	vNet_Init();

	// The IP address is given by the byte-wise sum of "Base IP Address"
	// and the vNet address
	vNet_SetAddress(0x0012, 1);			// Set address on media 2
	vNet_SetSubnetMask(0xFF00, 1);		// Set subnet mask on media 2
}

void loop()
{ 
	// Run the stack
	vNet_DataAvailable();

	if(millis()-time>10000)
	{
		if(!(msg%2))
		{		
			// Send the two message on two different ports
			if(!vNet_SendData(0x0011, msg1, 12, 0x10))
				Serial.println("Message 1 not sent");
		}
		else
		{	
			if(!vNet_SendData(0x0011, msg2, 10, 0x20))
				Serial.println("Message 2 not sent");
		}
		
		msg++;
		time = millis();	
	}
		
    delay(50);
    
}