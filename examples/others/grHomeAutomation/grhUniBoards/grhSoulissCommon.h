/**************************************************************************
	GRHOME -  Souliss Common Definitions
	Versione 0
***************************************************************************/

// Set static addressing mode
#define DYNAMICADDRESSING_INSKETCH
#define DYNAMICADDRESSING  		0x00

// Define the souliss network dimensions
#define NODESIZE_INSKETCH
#define MaCaco_NODES			30		// MaCaco_NODES < 255
#define MaCaco_SLOT				35 		// MaCaco_SLOT < VNET_MAX_PAYLOAD - MaCaco_HEADER and MaCaco_SLOT  < 255 / 3
