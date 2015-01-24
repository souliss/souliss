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
  Serial.begin(9600);						// setup the USART
  vNet_Init();								// init the vNet layer	
  vNet_SetAddress(0x6601, 2);				// set the node address
  vNet_SetSubnetMask(0xFF00, 2);  			// set the node subnet mask
}

void loop()
{ 

  // if any data is received
  if (vNet_DataAvailable())
  {
    uint8_t buf[VNET_MAX_PAYLOAD];  			// store the received data in here
    uint8_t len;
    uint8_t used_media;
        
    len = vNet_RetrieveData(buf);				// parse incoming data and route it (because is SN)
    used_media = vNet_GetLastMedia();
  }
  
}