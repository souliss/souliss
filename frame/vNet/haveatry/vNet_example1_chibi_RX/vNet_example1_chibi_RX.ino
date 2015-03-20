/* vNet Example 1, Data Transmission

	A standard Node (TX) send data to a device out of its subnet (RX), 
	data are sent via the SuperNode (SN).
	
	The received message is printed-out by RX node, you can see the
	messagge IN/OUT activating the VNET_DEBUG functionality.

	*************************Definitions*************************
	Configuration file		Parameter
	vNet_Config.h			#define VNET_SUPERNODE  	1
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

void setup()
{
  Serial.begin(9600);  					// setup the USART
  vNet_Init();							// init the vNet layer	
  vNet_SetAddress(0x6501, 2);			// set the node address
  vNet_SetSubnetMask(0xFF00, 2);		// set the node subnet mask
  
}

void loop()
{ 

  // if any data is received, print it to the terminal
  if (vNet_DataAvailable())
  {
    uint8_t buf[VNET_MAX_PAYLOAD];  // store the received data in here
    uint8_t len;
    uint8_t used_media;
	
    len = vNet_RetrieveData(buf);
    used_media = vNet_GetLastMedia();
    
    if((len>0) && (len!=VNET_DATA_ROUTED))
    { 
    // The data consists of ASCII characters in byte (unsigned char) format. The print
    // function requires ASCII characters be in char format so we need to inform the function
    // that its okay to convert the format from unsigned char to char. 
      Serial.print("Message : ");
      for(U8 i=0;i<len;i++)
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

}