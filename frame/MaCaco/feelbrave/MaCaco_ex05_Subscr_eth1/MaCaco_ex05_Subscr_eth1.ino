/******************************************************************************
	MaCaco Example 5 - Subscription based data
 
	Data exchange on event based (on change of data) is achieved between two
	Chibiduino nodes (2.4GHz Wireless Point-to-point) and one Ethernet.
 
	Use this code with an Ethernet node.
	 
	*************************Definitions*************************
	Configuration file		Parameter
	vNet_Config.h			#define VNET_SUPERNODE  	0
	vNet_Config.h			#define VNET_MEDIA1_ENABLE  1
	vNet_Config.h			#define VNET_MEDIA2_ENABLE  0
	vNet_Config.h			#define VNET_MEDIA3_ENABLE  0
	vNet_Config.h			#define VNET_MEDIA4_ENABLE  0
	vNet_Config.h			#define VNET_MEDIA5_ENABLE  0

	MaCaco_Config.h			#define MaCaco_EXTENDED_MAP			0

	Define the ethernet controller based on the used hardware,
	ethUsrCfg.h				#define ETH_W5100/ENC28J60	1	
	
	ethUsrCfg.h				#define ETH_CLASS   		0	
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

// define subscribe channels
#define	network_channel1	0
#define network_channel2	1

// define the shared memory map
U8 MemoryMap[MaCaco_MEMMAP] = {0x00};

// flag 
U8 data_changed = 0;
U8 timing;

U8 i, phase=0;
unsigned long tmr=0;   
  
U8  retrieved_getdatain;
U8  retrieved_funcode;
U16 retrieved_putin;
U8  retrieved_startoffset;
U8  retrieved_numberof;

void printout()
{
    // Print address  
    Serial.print("My Address : ");
    Serial.println(network_address3,HEX);  
    Serial.print("Source Address : ");
    Serial.println(vNet_GetSourceAddress(vNet_GetLastMedia()),HEX);
      
    // Print frame   
    Serial.println("Frame header");
    Serial.print("< ");  
    Serial.print(retrieved_funcode,HEX);
    Serial.print(",");  
    Serial.print(retrieved_putin,HEX);
    Serial.print(",");  
    Serial.print(retrieved_startoffset,HEX);
    Serial.print(",");  
    Serial.print(retrieved_numberof,HEX);
    Serial.println(" >"); 
   
	Serial.println("");
	Serial.println("");

	Serial.println("Shared memory map : ");

    // Print memory map
    for (i=0; i<MaCaco_MEMMAP; i++)
    {
        Serial.print(" (");
        Serial.print(i,DEC);
        Serial.print(") ");
        Serial.print(MemoryMap[i],HEX);
        if (!((i+1)%5))  Serial.println("");
    }
    Serial.println("");
	
	Serial.print("Channel 1 Healty Value : ");
	Serial.println(MemoryMap[MaCaco_HEALTY_s+network_channel1],HEX);
	Serial.print("Channel 2 Healty Value : ");	
    Serial.println(MemoryMap[MaCaco_HEALTY_s+network_channel2],HEX);
	
	Serial.println("");
	Serial.println("");

}


void setup()
{
	U8 i=0;
	
   	// Setup the serial and wireless interfaces	
	Serial.begin(9600);

	// Set the network parameters
	eth_SetBaseIP((uint8_t *)DEFAULT_BASEIPADDRESS);
	eth_SetSubnetMask((uint8_t *)DEFAULT_SUBMASK);
	eth_SetGateway((uint8_t *)DEFAULT_GATEWAY);	
		
	// Set the network parameters
    vNet_Init();	
	vNet_SetAddress(network_address3, vNet_GetMedia(network_address3));	
	vNet_SetSubnetMask(network_submask, vNet_GetMedia(network_address3));
	vNet_SetMySuperNode(eth_address, vNet_GetMedia(network_address3));
	
	// Set the healty values
	MemoryMap[MaCaco_HEALTY_s+network_channel1] = 0x25;
	MemoryMap[MaCaco_HEALTY_s+network_channel2] = 0x25;
	
	// set the analog outputs of the shared map 
	for(i=0;i<MaCaco_SLOT;i++)
		MemoryMap[MaCaco_OUT_s+i] = 0xCA+i;	
}

void loop()
{ 
 	if(abs(millis()-tmr) > 100)
	{	
		tmr = millis();
		phase = (phase + 1) % 100;

		if (!(phase % 2))
		{                 
			MaCaco_DataIn();
			if(MaCaco_retrieve(&MemoryMap[0], &data_changed))
			{
				// get header from the last frame
				retrieved_getdatain = MaCaco_getdatain();
				retrieved_funcode = MaCaco_getfuncode();
				retrieved_putin = MaCaco_getputin();
				retrieved_startoffset = MaCaco_getstartoffset();
				retrieved_numberof = MaCaco_getnumberof();
				  
				printout();
			}    
		} 
	  
		// subscribe data
		if (!(phase % 5))
		{              
			if(!MaCaco_subscribe(network_address2, MemoryMap, &MemoryMap[MaCaco_IN_s], MaCaco_OUT_s, 4, network_channel1))
				Serial.println("Channel 0 not healty");
			  
			if(!MaCaco_subscribe(network_address1, MemoryMap, &MemoryMap[MaCaco_IN_s+4], MaCaco_OUT_s+4, 4, network_channel2))
				Serial.println("Channel 1 not healty");
		}

	 
		if (!(phase % 10000))
		{
			// Update the analog outputs of the shared map 
			for(i=0;i<MaCaco_SUBSCRLEN;i++)
			{
				MemoryMap[MaCaco_OUT_s+i]++;
				
				data_changed = 1;
			}
		}
    }	
} 
