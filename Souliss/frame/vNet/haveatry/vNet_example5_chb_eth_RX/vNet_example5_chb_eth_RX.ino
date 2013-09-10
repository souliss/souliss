/* vNet Example 5, Data Transmission

  A communication between and ethernet and a wireless node is estabilished,
  data are send and received trough a ethernet-wireless bridge.
  
  This sketch should run on a ethernet node node.
  
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
	MaCaco_Config.h			#define MaCaco_DEBUG  		1	

*/
#include "Souliss.h"
#include "Typicals.h"
#include <SPI.h>

// Message to sent over network
byte msg1[] = "I'm the Eth node";
unsigned long cnt=0;

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
	vNet_SetAddress(0x0012, 1);			// Set address on media 1
	vNet_SetSubnetMask(0xFF00, 1);		// Set subnet mask on media 1
    vNet_SetMySuperNode(0x0011, 1);		// Set Associated SuperNode on media 1
  
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
	if(!vNet_Send(0x6612, msg1, 17, 0x10))
      Serial.println("Sent failed");
  }

  delay(50);
}