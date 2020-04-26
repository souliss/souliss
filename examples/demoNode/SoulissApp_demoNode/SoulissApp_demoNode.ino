/**************************************************************************
	Souliss - DEMO NODE

	demo node shown in soulissApp

	Refer to Souliss Wiki to know about souliss framework APIs and functions used below

	shine@angelic.it
	11/2019

***************************************************************************/

// Configure the framework

#include <SoulissFramework.h>

#include "bconf/Olimex_ESP8266EVB.h"    // Load the code directly on the ESP8266
#include "conf/Gateway.h"               // The main node is the Gateway, we have just one node
#include "conf/DynamicAddressing.h"


// Define the WiFi name and password
#define WIFICONF_INSKETCH
#define WiFi_SSID               "mywifi"
#define WiFi_Password           "mypassword"    

// Include framework code and libraries
#include <ESP8266WiFi.h>
#include <EEPROM.h>

/*** All configuration includes should be above this line ***/
#include "Souliss.h"
 
// This identify theonly real device (relay)
#define RELAY_PIN           5

// This identify the number of the LED logic
#define MYLEDLOGIC             0
#define USED_SLOT1_N1          1
#define USED_SLOT3_N1          3
#define USED_SLOT5_N1          5
#define USED_SLOT7_N1          7
#define HUM                    9                // Leave 2 slots for T58
#define TEMP0                  11               // Leave 2 slots for T52
#define USED_SLOT13_N1         13

void setup()
{
	Serial.begin(115200);
	Initialize();
	
	// Connect to the WiFi network and get an address from DHCP
	GetIPAddress();

	Serial.println(myvNet_dhcp);
	SetAsGateway(myvNet_dhcp);  // Set this node as gateway for SoulissApp  

	// This node will serve all the others in the network providing an address
	SetAddressingServer();

	Set_T14(MYLEDLOGIC);        // Define a simple LED light logic
	Set_T12(USED_SLOT1_N1);
	Set_T32(USED_SLOT3_N1);
	Set_T16(USED_SLOT5_N1);
	Set_T15(USED_SLOT7_N1);
	Set_T19(USED_SLOT13_N1);
	Set_Humidity(HUM);
	Set_Temperature(TEMP0);
	pinMode(RELAY_PIN, OUTPUT); // Use pin 5 as output 
}

void loop()
{
	// Here we start to play
	EXECUTEFAST() {
		UPDATEFAST();

		FAST_50ms() {   // We process the logic and relevant input and output every 50 milliseconds
			//DigIn(IN0, Souliss_T1n_ToggleCmd, MYLEDLOGIC);
			Logic_T14(MYLEDLOGIC);
			DigOut(RELAY_PIN, Souliss_T1n_Coil, MYLEDLOGIC);
		}

		FAST_110ms() {//less frequent update
			Logic_T12(USED_SLOT1_N1);
			Logic_T32(USED_SLOT3_N1);
			Logic_T16(USED_SLOT5_N1);
			Logic_T15(USED_SLOT7_N1);
			Logic_T19(USED_SLOT13_N1);
		}

		// Here we handle here the communication with Android
		FAST_GatewayComms();
	}
	EXECUTESLOW()
	{
		UPDATESLOW();

		SLOW_10s() {
			Logic_Humidity(HUM);
			Logic_Temperature(TEMP0);
		}

		SLOW_50s() {
			float h = 65;
			float t = 22;
			ImportAnalog(HUM, &h);
			ImportAnalog(TEMP0, &t);
		}
	}
}
