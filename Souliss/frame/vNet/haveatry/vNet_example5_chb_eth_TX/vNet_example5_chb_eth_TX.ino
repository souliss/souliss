/* vNet Example 5, Data Transmission

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
byte msg1[] = "I'm the Chibi node";
unsigned long cnt=0;

void setup()
{    
  Serial.begin(9600);					
  vNet_Init();							
  vNet_SetAddress(0x6612, 2);			
  vNet_SetSubnetMask(0xFF00, 2);		
  vNet_SetMySuperNode(0x6601, 2);		
}

void loop()
{ 
  
 // if any data is received, then print it to the terminal
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
  
  if((millis()-cnt) > 10000)
  {     
    cnt = millis();
    if(!vNet_Send(0x0012, msg1, 19, 0x20))
      Serial.println("Sent failed");
  }
}