/**************************************************************************
    Souliss - Hello World for Expressif ESP8266
    
    This is the basic example, create a software push-button on Android
    using SoulissApp (get it from Play Store).  
    
    Load this code on ESP8266 board using the porting of the Arduino core
    for this platform.
        
***************************************************************************/

// Let the IDE point to the Souliss framework
#include "SoulissFramework.h"

// Configure the framework
#include "bconf/Olimex_ESP8266EVB.h"              // Load the code directly on the ESP8266
#include "conf/Gateway.h"                   // The main node is the Gateway, we have just one node
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

// This identify the number GPIO0 of the Button
#define IN0                 0
// This identify the number GPIO5 for control Relay 
#define RELAY_PIN           5

// This identify the number of the LED logic
#define MYLEDLOGIC          0 


void setup()
{   
    Initialize();

    // Connect to the WiFi network and get an address from DHCP
    GetIPAddress();                           
    SetAsGateway(myvNet_dhcp);       // Set this node as gateway for SoulissApp  

    // This node will serve all the others in the network providing an address
    SetAddressingServer();
    
    Set_SimpleLight(MYLEDLOGIC);        // Define a simple LED light logic
    pinMode(IN0, INPUT);                        // Use pin 0 as input
    pinMode(RELAY_PIN, OUTPUT);                 // Use pin 5 as output 
}

void loop()
{ 
    // Here we start to play
    EXECUTEFAST() {                     
        UPDATEFAST();   
        
        FAST_50ms() {   // We process the logic and relevant input and output every 50 milliseconds
            DigIn(IN0, Souliss_T1n_ToggleCmd, MYLEDLOGIC);
            Logic_SimpleLight(MYLEDLOGIC);
            DigOut(RELAY_PIN, Souliss_T1n_Coil,MYLEDLOGIC);
        } 
              
        // Here we handle here the communication with Android
        FAST_GatewayComms();                                        
    }
} 
