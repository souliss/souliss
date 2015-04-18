/**************************************************************************
	Souliss - Erase Zero Configuration
	
	The addresses from Zero Configuration examples are stored in the EEPROM
	if you want to erase the stored configuration, run this example on the
	node.
	
***************************************************************************/
#include "conf/DynamicAddressing.h"					// Use dynamic address
#define VNET_MEDIA_INSKETCH

// Include framework code and libraries
#include <EEPROM.h>
#include "Souliss.h"


void setup()
{	
	// Erase network configuration parameters from previous use of ZeroConf
	Store_Clear();
}

void loop()
{ 
} 