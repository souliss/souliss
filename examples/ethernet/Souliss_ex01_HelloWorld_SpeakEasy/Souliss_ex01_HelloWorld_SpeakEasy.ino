/**************************************************************************
	Souliss - Hello World (Speak Easy Example)
	
	It simply handle one LED remotely via an Android and via an hardwired 
	pushbutton, use Souliss Speak Easy macro to make writing a sketch even
	simpler.
 
	CONFIGURATION IS MANDATORY BEFORE COMPILING

	Run this code on one of the following boards:
	
		Board Conf Code			Board Model
        0x03        			Arduino Ethernet (W5100) 
		0x04					Arduino with Ethernet Shield (W5100)
		0x05					Arduino with ENC28J60 Ethernet Shield	
	
	******************** Configuration Parameters *********************
	
		Configuration file		Parameter
		QuickCfg.h				#define	QC_ENABLE			0x01
		QuickCfg.h				#define	QC_BOARDTYPE		0x03, 0x04, 0x05

		QuickCfg.h				#define	QC_GATEWAYTYPE		0x01		
		
	Is required an additional IP configuration using the following parameters
		QuickCfg.h				const uint8_t DEFAULT_BASEIPADDRESS[] = {...}
		QuickCfg.h				const uint8_t DEFAULT_SUBMASK[]       = {...}
		QuickCfg.h				const uint8_t DEFAULT_GATEWAY[]       = {...}
	
***************************************************************************/
#include "Souliss.h"
#include "SpeakEasy.h"						// Is a library to code easy Souliss examples
#include <SPI.h>

// Define the network configuration
//
// In the QuickCfg.h file set DEFAULT_BASEIPADDRESS[] = {192,168,1,0}
#define myvNet_address		0x0011 			// 0x0011 is equal to 17 in decimal
#define myvNet_subnet		0xFF00
#define myvNet_supern		0x0000
// defining myvNet_address as 0x0011 (is hexadecimal, in decimal is uqual to 17) gives
// as IP address 192.168.1.17, use this address in SoulissApp 

#define MYLEDLOGIC			0				// This identify the number of the LED logic

void setup()
{	
	// Setup the network configuration
	//
	// The vNet address is 11(hex) that is 17(dec), so the IP address is
	// the DEFAULT_BASEIPADDRESS[] defined in ethUsrCfg.h plus 17 on last 
	// octect. If DEFAULT_BASEIPADDRESS[] = {192, 168, 1, 0} the IP address
	// for the board will be 192.168.1.17
	Souliss_SetAddress(myvNet_address, myvNet_subnet, myvNet_supern);		
	
	SetAsGateway(myvNet_address);		// Set this node as gateway for SoulissApp	
	Set_SimpleLight(MYLEDLOGIC);		// Define a simple LED light logic
	
	// We connect a pushbutton between 5V and pin2 with a pulldown resistor 
	// between pin2 and GND, the LED is connected to pin9 with a resistor to
	// limit the current amount
	pinMode(2, INPUT);					// Hardware pulldown required
	pinMode(9, OUTPUT);					// Power the LED
}

void loop()
{ 
	// Here we start to play
	EXECUTEFAST() {						
		UPDATEFAST();	
		
		FAST_50ms() {	// We process the logic and relevant input and output every 50 milliseconds
			ssDigIn(2, Souliss_T1n_ToogleCmd, MYLEDLOGIC);			// Use the pin2 as ON/OFF toogle command
			Logic_SimpleLight(MYLEDLOGIC);							// Drive the LED as per command
			ssDigOut(9, Souliss_T1n_Coil, MYLEDLOGIC);				// Use the pin9 to give power to the LED according to the logic
		} 
			  
		FAST_70ms() {   // We check incoming communication data every 70 milliseconds
			// Here we handle here the communication with Android, commands and notification
			// are automatically assigned to MYLEDLOGIC
			ProcessCommunication();										
		}			
	}
} 