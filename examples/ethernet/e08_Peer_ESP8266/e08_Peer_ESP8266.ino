/**************************************************************************
    Souliss - Hello World for Expressif ESP8266
    
    This is the basic example, create a software push-button on Android
    using SoulissApp (get it from Play Store).  
    
    Load this code on ESP8266 board using the porting of the Arduino core
    for this platform.
        
***************************************************************************/

// Configure the framework
#include "bconf/MCU_ESP8266.h"              // Load the code directly on the ESP8266
#include "conf/DisableEEPROM.h"

// Define the WiFi name and password
#define WIFICONF_INSKETCH
#define WiFi_SSID               "mywifi"
#define WiFi_Password           "mypassword"    

// Include framework code and libraries
#include <ESP8266WiFi.h>
#include "Souliss.h"

// This identify the number of the LED logic
#define MYLEDLOGIC          0               

void setup()
{   
    Initialize();

    // Connect to the WiFi network and get an address from DHCP
    Setup_ESP8266();                           

    // Get address dynamically
	SetDynamicAddressing();
    GetAddress();
    
    Set_SimpleLight(MYLEDLOGIC);        // Define a simple LED light logic
    pinMode(5, OUTPUT);                 // Use pin 5 as output 
}

void loop()
{ 
    // Here we start to play
    EXECUTEFAST() {                     
        UPDATEFAST();   
        
        FAST_50ms() {   // We process the logic and relevant input and output every 50 milliseconds
            Logic_SimpleLight(MYLEDLOGIC);
            DigOut(5, Souliss_T1n_Coil,MYLEDLOGIC);
        } 
              
        // Here we handle here the communication with Android
        FAST_PeerComms();                                        
    }
} 