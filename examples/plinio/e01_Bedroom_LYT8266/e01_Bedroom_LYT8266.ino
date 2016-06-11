/**************************************************************************
   Souliss - LYT8266 WiFi RGBW LED Bulb
 
***************************************************************************/
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include <WiFiUdp.h>

// Configure the framework
#include "bconf/LYT8266_LEDBulb.h"              // Load the code directly on the ESP8266
#include "conf/IPBroadcast.h"

// **** Define the WiFi name and password ****
#define WIFICONF_INSKETCH
#define WiFi_SSID               "ssid"
#define WiFi_Password           "password"    

// Include framework code and libraries
#include "Souliss.h"
   
// Define logic slots, multicolor lights use four slots
#define LIGHTMODE           0
#define LYTLIGHT1           1    
#define LIGHTINTESITY       22

#define RED_LOWMOOD     0x05
#define GREEN_LOWMOOD   0x01
#define BLUE_LOWMOOD    0x00

#define RED_DEFAULT     0x25
#define GREEN_DEFAULT   0x10
#define BLUE_DEFAULT    0x00

// Setup the libraries for Over The Air Update
OTA_Setup();

void setup()
{   
    // Init the network stack and the bulb, turn on with a warm amber
    Initialize();
    InitLYT();

	/****
		Generally set a PWM output before the connection will lead the 
		ESP8266 to reboot for a conflict on the FLASH write access.

		Here we do the configuration during the WebConfig and so we don't
		need to write anything in the FLASH, and the module can connect
		to the last used network.
	****/
    SetWhite(LYTLIGHT1, 0xF1);

    // If there is no network to connect, just loop at low mood
    if(!Souliss_GetIPAddress())
	{
		SetColor(LYTLIGHT1, RED_LOWMOOD, GREEN_LOWMOOD, BLUE_LOWMOOD);
		
		while(1)
			yield();
	}

	// Set vNet address	
    SetAddress(0xAB02, 0xFF00, 0xAB01);

    // Define a logic to handle the bulb
    Set_T11(LIGHTMODE);
    SetLYTLamps(LYTLIGHT1);

    // This T11 is used as AUTO/MAN selector
    mInput(LIGHTMODE)=Souliss_T1n_OffCmd;
    Logic_T11(LIGHTMODE);

    // Init the OTA
    OTA_Init();   
}

void loop()
{  
    EXECUTEFAST() {                     
        UPDATEFAST();   
        
        // Is an unusual approach, but to get fast response to color change we run the LYT logic and
        // basic communication processing at maximum speed.
        LogicLYTLamps(LYTLIGHT1);       
        ProcessCommunication();
      
        FAST_PeerComms(); 

        FAST_510ms() {

            // Use an AUTO/MAN selector for the synch with the luminosity sensor
            Logic_T11(LIGHTMODE);
        }

        FAST_2110ms() {

            // If the AUTO mode is set
            if(mOutput(LIGHTMODE))
            {
                // Pull data from an external node
                PullData(0xCE02, LIGHTINTESITY, 7, 2);
                Logic_Light(LIGHTINTESITY);
               
                // Use the light to match the brightness
				if(mOutputAsFloat(LIGHTINTESITY) > 20)
					SetWhite(LYTLIGHT1, BRIGHT_DEFAULT);
                else
                    SetColor(LYTLIGHT1, RED_DEFAULT+mOutputAsFloat(LIGHTINTESITY)*3, 
                             GREEN_DEFAULT+mOutputAsFloat(LIGHTINTESITY),
                             0);
            }
        }

        // Join the Gateway
        START_PeerJoin();
    }

    EXECUTESLOW() {
        UPDATESLOW();
    
        // Slowly shut down the lamp
        SLOW_10s() {
            LYTSleepTimer(LYTLIGHT1);    
        }   

        // Here we periodically check for a gateway to join
        SLOW_PeerJoin(); 
    }

    // Look for a new sketch to update over the air
    OTA_Process();	 
}    
