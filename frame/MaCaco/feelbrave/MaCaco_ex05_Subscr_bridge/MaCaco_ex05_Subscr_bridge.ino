/******************************************************************************
	MaCaco Example 5 - Subscription based data
 
	Data exchange on event based (on change of data) is achieved between two
	Chibiduino nodes (2.4GHz Wireless Point-to-point) and one Ethernet.
	 
	Use this code with a Chibiduino node with Ethernet shield.
	 
	*************************Definitions*************************
	Configuration file		Parameter
	vNet_Config.h			#define VNET_SUPERNODE  			1
	vNet_Config.h			#define VNET_MEDIA1_ENABLE  		1
	vNet_Config.h			#define VNET_MEDIA2_ENABLE  		1
	vNet_Config.h			#define VNET_MEDIA3_ENABLE  		0
	vNet_Config.h			#define VNET_MEDIA4_ENABLE  		0
	vNet_Config.h			#define VNET_MEDIA5_ENABLE  		0

	MaCaco_Config.h			#define MaCaco_EXTENDED_MAP			0
	
	ethUsrCfg.h				const uint8_t DEFAULT_BASEIPADDRESS[] = {...}
	ethUsrCfg.h				const uint8_t DEFAULT_SUBMASK[]       = {...}
	ethUsrCfg.h				const uint8_t DEFAULT_GATEWAY[]       = {...}
	ethUsrCfg.h				const uint8_t MAC_ADDRESS[]           = {...}	
	
	It's important define the proper MAC address for each board, two nodes
	that share the same MAC address will not communicate. You can found a
	proper MAC address on the sticker behind the board.
	
	The MAC address can be automatically assigned using the AUTO_MAC setting,
	and leaving the standard group address in the MAC_ADDRESS array.
	ethUsrCfg.h				#define AUTO_MAC  					1
	
	Only if debug on serial port is required,
	vNet_Config.h			#define VNET_DEBUG  				1
	MaCaco_Config.h			#define MaCaco_DEBUG  				1	
		
******************************************************************************/
#include "Souliss.h"
#include "Typicals.h"
#include <SPI.h>

// define network parameters
#define network_address1	0x6511
#define network_address2	0x6512
#define network_address3	0x0013
#define chb_address			0x6501
#define eth_address			0x0001
#define network_submask		0xFF00

U8 buf[VNET_MAX_PAYLOAD];

void setup()
{
	U8 i=0;
	
   	// Setup the serial and wireless interfaces	
	Serial.begin(9600);
	
	// Set the Ethernet network parameters
	eth_SetBaseIP((uint8_t *)DEFAULT_BASEIPADDRESS);
	eth_SetSubnetMask((uint8_t *)DEFAULT_SUBMASK);
	eth_SetGateway((uint8_t *)DEFAULT_GATEWAY);	
	
	// Set the vNet network parameters
    vNet_Init();
	
	vNet_SetAddress(chb_address, vNet_GetMedia(chb_address));			
	vNet_SetSubnetMask(network_submask, vNet_GetMedia(chb_address));
	
	vNet_SetAddress(eth_address, vNet_GetMedia(eth_address));			
	vNet_SetSubnetMask(network_submask, vNet_GetMedia(eth_address));
}

void loop()
{ 
	// run the vNet stack and bridge frames, to see the bridged data select
	// the debug mode for vNet and MaCaco.
	if(vNet_DataAvailable())
		vNet_RetrieveData(buf);
}