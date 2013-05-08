/******************************************************************************
	MaCaco Example 6 - User Interface Simulation
 
	A set of request start from node number 2 to get the information used by the
	user interface. Is an useful test bench for user interfaces that are planned
	to get data directly from the nodes using the MaCaco protocol.
 
	Node number 1 is the gateway, rather node number 2 simulate the requests from
	the user interface.
	
	This device should be an Ethernet node.
 
	*************************Definitions*************************
	Configuration file		Parameter
	vNet_Config.h			#define VNET_SUPERNODE  			0
	vNet_Config.h			#define VNET_MEDIA1_ENABLE  		1
	vNet_Config.h			#define VNET_MEDIA2_ENABLE  		0
	vNet_Config.h			#define VNET_MEDIA3_ENABLE  		0
	vNet_Config.h			#define VNET_MEDIA4_ENABLE  		0
	vNet_Config.h			#define VNET_MEDIA5_ENABLE  		0

	MaCaco_Config.h			#define MaCaco_EXTENDED_MAP			1

	Define the ethernet controller based on the used hardware,
	ethUsrCfg.h				#define ETH_W5100/ENC28J60			1	
		
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
#define network_address1	0x0011
#define network_address2	0x0012
#define network_submask		0xFF00

// define the shared memory map
U8 MemoryMap[MaCaco_MEMMAP] = {0x00};

U8  retrieved_getdatain;
U8  retrieved_funcode;
U16 retrieved_putin;
U8  retrieved_startoffset;
U8  retrieved_numberof;

U16 i=0, phase_fast=0, phase_slow=0;
unsigned long tmr1=0, tmr2=0;

void printout()
{
    // Print address  
    Serial.print("My Address : ");
    Serial.println(network_address2,HEX);  
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
    Serial.println("");

}	
	

void setup()
{	
   	// setup the serial and wireless interfaces	
	Serial.begin(9600);
    
  	// Set the network parameters
	eth_SetBaseIP((uint8_t *)DEFAULT_BASEIPADDRESS);
	eth_SetSubnetMask((uint8_t *)DEFAULT_SUBMASK);
	eth_SetGateway((uint8_t *)DEFAULT_GATEWAY);	
		
	// Set the network parameters
    vNet_Init();	
	vNet_SetAddress(network_address2, vNet_GetMedia(network_address2));	
	vNet_SetSubnetMask(network_submask, vNet_GetMedia(network_address2));

}

void loop()
{ 
	if(abs(millis()-tmr1) > 100)
	{	
		tmr1 = millis();
		phase_fast = (phase_fast + 1) % 250;
	
		if (!(phase_fast % 2))
		{                 
			MaCaco_DataIn();
			if(MaCaco_retrieve(&MemoryMap[0], MaCaco_NODATACHANGED))
			{
				// get header from the last frame
				retrieved_getdatain = MaCaco_getdatain();
				retrieved_funcode = MaCaco_getfuncode();
				retrieved_putin = MaCaco_getputin();
				retrieved_startoffset = MaCaco_getstartoffset();
				retrieved_numberof = MaCaco_getnumberof();
				  
			}    
		} 
	}
	
	if(abs(millis()-tmr2) > 2000)
	{	
		tmr2 = millis();
		phase_slow = (phase_slow + 1) % 250;
	
		
		// send an typical logic request starting from node 0, for 10 nodes
		if(!(phase_slow % 21))
		{		
			Serial.println("<Send Typical Logic Request>");	
		
			#define FIRST_NODE_ID			0
			#define NUMBER_OF_NODES			10
			MaCaco_send(network_address1, MaCaco_TYPREQ, 0, FIRST_NODE_ID, NUMBER_OF_NODES, 0);
		}	
		
		// send an healty request starting from node 0, for 10 nodes
		if(!(phase_slow % 31))
		{
			Serial.println("<Send Healty Request>");	
		
			#define FIRST_NODE_ID			0
			#define NUMBER_OF_NODES			10
			MaCaco_send(network_address1, MaCaco_HEALTYREQ, 0, FIRST_NODE_ID, NUMBER_OF_NODES, 0);
		}	
		
		// send a ping request
		if(!(phase_slow % 41))
		{
			Serial.println("<Send Ping Request>");
			MaCaco_send(network_address1, MaCaco_PINGREQ, 0, 0, 0, 0);
		}
		
		// send an force request for node 0
		if(!(phase_slow % 101))
		{
			Serial.println("<Send Force Request for Node 0>");
			
			#define NODE_ID					0
			#define NUMBER_OF_NODES			1
			MaCaco_send(network_address1, MaCaco_FORCE, 0, FIRST_NODE_ID, NUMBER_OF_NODES, 0);
		}		

		// send an force request for node 2
		if(!(phase_slow % 131))
		{
			Serial.println("<Send Force Request for Node 2>");
			
			#define NODE_ID					2
			#define NUMBER_OF_NODES			1
			MaCaco_send(network_address1, MaCaco_FORCE, 0, FIRST_NODE_ID, NUMBER_OF_NODES, 0);
		}	
		
		// send a state request, note that answer is rejected from standard nodes
		if(!(phase_slow % 251))
		{
			Serial.println("<Send State Request>");
			
			#define FIRST_NODE_ID			0
			#define NUMBER_OF_NODES			10
			MaCaco_send(network_address1, MaCaco_STATEREQ, 0, FIRST_NODE_ID, NUMBER_OF_NODES, 0);
		}		
		
    }
		
}