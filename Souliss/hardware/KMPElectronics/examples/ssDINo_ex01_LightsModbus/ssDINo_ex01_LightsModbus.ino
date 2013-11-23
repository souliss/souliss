/**************************************************************************
	Souliss - Lights
	
	It handle the four relays either via IN1 to IN4 inputs, using the
	Android interface or via Modbus TCP. Connecting the relays to 
	lights or similar electrial appliace, you can get remote control
	of them.
	
	Modbus Map (Register, Functional Code)
		- Light 1 ON/OFF  Request at (R 0x00 - 0001, FC 0x01, 0x05) or (R 0x00 - 0001, FC 0x02)	
		- Light 2 ON/OFF  Request at (R 0x08 - 0009, FC 0x01, 0x05) or (R 0x08 - 0009, FC 0x02)	
		- Light 3 ON/OFF  Request at (R 0x11 - 0017, FC 0x01, 0x05) or (R 0x11 - 0017, FC 0x02)	
		- Light 4 ON/OFF  Request at (R 0x19 - 0025, FC 0x01, 0x05) or (R 0x19 - 0025, FC 0x02)	
		
		- Light 1 State at (R 0x2000 - 8193, FC 0x01) or (R 0x2000 - 8193, FC 0x02)
		- Light 2 State at (R 0x2008 - 8201, FC 0x01) or (R 0x2008 - 8201, FC 0x02)
		- Light 3 State at (R 0x2011 - 8209, FC 0x01) or (R 0x2011 - 8209, FC 0x02)
		- Light 4 State at (R 0x2019 - 8217, FC 0x01) or (R 0x2019 - 8217, FC 0x02)
		
 	Applicable for:
		- Light
		- Other ON/OFF electrical appliance

	CONFIGURATION IS MANDATORY BEFORE COMPILING
	Before compiling this code, is mandatory the configuration of the framework
	this ensure the use of proper drivers based functionalities and requested
	communication interface.	
	
	Configuration files are located on /conf folder, is suggested to use this 
	code on one of the boards listed below.	
	
	Run this code on one of the following boards:
	
		Board Conf Code			Board Model
		0x06					KMTronic 		DINo v1	
		0x20					KMP Electronics DINo v2
	
	******************** Configuration Parameters *********************
	
		Configuration file		Parameter
		QuickCfg.h				#define	QC_ENABLE			0x01
		QuickCfg.h				#define	QC_BOARDTYPE		0x06, 0x20

		QuickCfg.h				#define	QC_GATEWAYTYPE		0x02, 0x03
		QuickCfg.h				#define	DYNAMICADDRESSING	0x01
		QuickCfg.h				#define	ETHERNETMACRAW		0x01	
	
***************************************************************************/
#include "Souliss.h"
#include "SpeakEasy.h"						// Is a library to code easy Souliss examples
#include <SPI.h>

// By default the board will get an IP address with .77 as last byte, you can change it
// in runtime using the Android application SoulissApp

#define RELAY1					0			// This is the memory slot used for the execution of the logic
#define RELAY2					1			
#define RELAY3					2			
#define RELAY4					3			

void setup()
{	
	// We don't assign a fixed IP address to the board, the board will get the base IP
	// information from the first broadcast frame sent out with the Android application.
	// By default the board will get an IP address with .77 as last byte, you can change
	// it in runtime using the Android application SoulissApp.
	SetAddressingServer();

	// Define two Simple Light logics and the relays
	Set_SimpleLight(RELAY1);
	Set_SimpleLight(RELAY2);
	Set_SimpleLight(RELAY3);
	Set_SimpleLight(RELAY4);	
	SetRelay1();
	SetRelay2();
	SetRelay3();
	SetRelay4();	
	
	// Define a Modbus slave
	ModbusInit();
}

void loop()
{ 
	// Here we start to play
	EXECUTEFAST() {						
		UPDATEFAST();	
		
		FAST_50ms() {	// We process the logic and relevant input and output every 50 milliseconds
		
			ReadInput1(RELAY1);									// Read inputs from IN1
			ReadInput2(RELAY2);									// Read inputs from IN2
			ReadInput3(RELAY3);									// Read inputs from IN3
			ReadInput4(RELAY4);									// Read inputs from IN4
		
			Logic_SimpleLight(RELAY1);							// Execute the logic for Relay 1
			Logic_SimpleLight(RELAY2);							// Execute the logic for Relay 2
			Logic_SimpleLight(RELAY3);							// Execute the logic for Relay 3
			Logic_SimpleLight(RELAY4);							// Execute the logic for Relay 4
			
			CntRelay1(RELAY1);									// Drive the Relay 1
			CntRelay2(RELAY2);                                  // Drive the Relay 2
			CntRelay3(RELAY3);									// Drive the Relay 3
			CntRelay4(RELAY4);                                  // Drive the Relay 4			
		} 
		
		FAST_110ms() {	// We process the Modbus communication here
			Modbus(myMap);
			ModbusRemoteInput(myMap);
		}
		
		// Here we process all communication with other nodes
		FAST_GatewayComms();	
	}
	
	EXECUTESLOW() {	
		UPDATESLOW();

		SLOW_10s() {  // Process the timer every 10 seconds  
			Timer_SimpleLight(RELAY1);
			Timer_SimpleLight(RELAY2);	
			Timer_SimpleLight(RELAY3);
			Timer_SimpleLight(RELAY4);				
		} 	  
	}
} 