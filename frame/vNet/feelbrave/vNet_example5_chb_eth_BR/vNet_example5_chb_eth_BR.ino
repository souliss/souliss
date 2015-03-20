/* vNet Example 5, Data Transmission

  A communication between and ethernet and a wireless node is estabilished,
  data are send and received trough a ethernet-wireless bridge.
  
  This sketch should run on a chibiduino node with an ethernet shield.
  
	*************************Definitions*************************
	Configuration file		Parameter
	vNet_Config.h			#define VNET_SUPERNODE  	1
	vNet_Config.h			#define VNET_MEDIA1_ENABLE  1
	vNet_Config.h			#define VNET_MEDIA2_ENABLE  1
	vNet_Config.h			#define VNET_MEDIA3_ENABLE  0
	vNet_Config.h			#define VNET_MEDIA4_ENABLE  0
	vNet_Config.h			#define VNET_MEDIA5_ENABLE  0
	
	ethUsrCfg.h				#define ETH_W5100			1	

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

void setup()
{
	Serial.begin(9600);
  
  	// Set the network parameters
	eth_SetBaseIP((uint8_t *)DEFAULT_BASEIPADDRESS);
	eth_SetSubnetMask((uint8_t *)DEFAULT_SUBMASK);
	eth_SetGateway((uint8_t *)DEFAULT_GATEWAY);	
  
	vNet_Init();	
  
	// Set network parameters for Media 2 (Chibiduino wireless)
	vNet_SetAddress(0x6511, 2);
	vNet_SetSubnetMask(0xFF00, 2);  

	// The IP address is given by the byte-wise sum of "Base IP Address"
	// and the vNet address
	vNet_SetAddress(0x0011, 1);			// Set address on media 1
	vNet_SetSubnetMask(0xFF00, 1);		// Set subnet mask on media 1
  
}

void loop()
{ 
  U8 i, num_data = 0;
  num_data = vNet_DataAvailable();
  // if any data is received, then print it to the terminal
  
  for(i=0;i<num_data;i++)
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
}