/******************************************************************************
	MaCaco Example 3 - Multiple peer to peer polling
	 
	In this example three devices poll data within them self, run this code on
	Chibiduino compatible (2.4 GHz point-to-point Wireless) nodes.
 
	*************************Definitions*************************
	Configuration file		Parameter
	vNet_Config.h			#define VNET_SUPERNODE  			0
	vNet_Config.h			#define VNET_MEDIA1_ENABLE  		0
	vNet_Config.h			#define VNET_MEDIA2_ENABLE  		1
	vNet_Config.h			#define VNET_MEDIA3_ENABLE  		0
	vNet_Config.h			#define VNET_MEDIA4_ENABLE  		0
	vNet_Config.h			#define VNET_MEDIA5_ENABLE  		0

	MaCaco_Config.h			#define MaCaco_EXTENDED_MAP			0	
	
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
#define network_address3	0x6513
#define network_submask		0xFF00

// define the shared memory map
U8 MemoryMap[MaCaco_MEMMAP] = {0x00};

U8  retrieved_getdatain;
U8  retrieved_funcode;
U16 retrieved_putin;
U8  retrieved_startoffset;
U8  retrieved_numberof;

U8 i=0;
unsigned long tmr=0;

void printout()
{
    // Print address  
    Serial.print("My Address : ");
    Serial.println(network_address1,HEX);  
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
    vNet_Init();	
	vNet_SetAddress(network_address1, vNet_GetMedia(network_address1));	
	vNet_SetSubnetMask(network_submask, vNet_GetMedia(network_address1));
	
	// set the analog outputs of the shared map 
	for(i=0;i<MaCaco_SLOT;i++)
		MemoryMap[MaCaco_OUT_s+i] = 0xBA+i;	

}

void loop()
{ 	  

	if(abs(millis()-tmr) > 100)
	{	 
		tmr = millis();
		
		MaCaco_DataIn();
		if(MaCaco_retrieve(&MemoryMap[0], MaCaco_NODATACHANGED))
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
	
	if(abs(millis()-tmr) > 10000)
	{
		tmr = millis();
		
		// send data to node 2
		MaCaco_send(network_address2, MaCaco_READREQANA, (&MemoryMap[MaCaco_IN_s]), MaCaco_OUT_s, MaCaco_OUTLENGHT, 0x00);
	}   
}
