/* vNet Example 7, Data Transmission

	Three nodes exchange data using a Binary FSK generated with
	the internal analog compator and timer of the ATmega328.
	
	Build a bus for the devices, where the Pin3 is the TX and is
	connected to the bus via a 270 Ohm resistor (for protection).
	The Pin6 (AIN0) is the RX and is connected directly on the bus.
	
	The TX is a square wave and is compared to a constant signal
	on Pin7 (AIN1). The number of crossing of the wave through the
	Pin7 values are counted into the wave period.
	Two different carriers are used for the transmission, so the
	number of cross are based on the transmitted values. Are used
	only two simbols, so this is a binary FSK.

	*************************Definitions*************************
	Configuration file		Parameter
	vNet_Config.h			#define VNET_SUPERNODE  	0
	vNet_Config.h			#define VNET_MEDIA1_ENABLE  0
	vNet_Config.h			#define VNET_MEDIA2_ENABLE  0
	vNet_Config.h			#define VNET_MEDIA3_ENABLE  0
	vNet_Config.h			#define VNET_MEDIA4_ENABLE  1
	vNet_Config.h			#define VNET_MEDIA5_ENABLE  0
	
	ethUsrCfg.h				#define ETH_CLASS   		0	
	ethUsrCfg.h				const uint8_t DEFAULT_BASEIPADDRESS[] = {...}
	ethUsrCfg.h				const uint8_t DEFAULT_SUBMASK[]       = {...}
	ethUsrCfg.h				const uint8_t DEFAULT_GATEWAY[]       = {...}
	ethUsrCfg.h				const uint8_t MAC_ADDRESS[]           = {...}	
	
	It's important define the proper MAC address for each board, two nodes
	that share the same MAC address will not communicate. You can found a
	proper MAC address on the sticker behind the board.
	
	Only if debug on serial port is required,
	vNet_Config.h			#define VNET_DEBUG  		1
	
*/
#include "Souliss.h"
#include "Typicals.h"
#include <SPI.h>

#define network_address1  0xCC01
#define network_address2  0xCC02
#define network_address3  0xCC03

// Message to sent over network
byte msg1[] = "Sta";
byte msg2[] = "bene!";

unsigned long time = 0;
uint8_t msg=0;

void setup()
{    
	// Open a serial connection	
	Serial.begin(9600);
     
	
	vNet_Init();

	// The IP address is given by the byte-wise sum of "Base IP Address"
	// and the vNet address
	vNet_SetAddress(network_address2, 4);			// Set address on media 2
	vNet_SetSubnetMask(0xFF00, 4);		// Set subnet mask on media 2
}

void loop()
{ 
  if (vNet_DataAvailable())
  {
    uint8_t buf[VNET_MAX_PAYLOAD];  // store the received data in here
    uint8_t len;
    uint8_t used_media;
	
    len = vNet_RetrieveData(buf);
    used_media = vNet_GetLastMedia();
    
    if((len>0) && (len!=VNET_DATA_ROUTED))
    { 
      Serial.print("Message : ");
      for(U8 i=0;i<len-1;i++)
        Serial.print(*(char *)(buf+i));
      Serial.println("");
      Serial.print("Port number : ");
      Serial.println(vNet_GetPortNumber(used_media),HEX);
      Serial.print("Source Address : ");
      Serial.println(vNet_GetSourceAddress(used_media),HEX);
      Serial.print("Original Source Address : ");
      Serial.println(vNet_GetoSourceAddress(used_media),HEX);  
      Serial.print("Final Destination Address : ");
      Serial.println(vNet_GetfDestinationAddress(used_media),HEX);
      if(VNET_SUPERNODE) Serial.println("I'm a SuperNode");
    }
  }

	if(millis()-time>10000)
	{
		if(!(msg%2))
		{		
			// Send the two message on two different ports
			if(!vNet_Send(network_address1, msg1, 4, 0x10))
				Serial.println("Message 1 not sent");
		}
		else
		{	
			if(!vNet_Send(network_address3, msg2, 6, 0x20))
				Serial.println("Message 2 not sent");
		}
		
		msg++;
		time = millis();	
	}
}