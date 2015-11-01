/**************************************************************************
    Souliss - Bridge WiFi to RS485
    
    This is the basic example, create a software push-button on Android
    using SoulissApp (get it from Play Store) and bridge to an RS485 peer. 
    
    Load this code on ESP8266 board using the porting of the Arduino core
    for this platform.
        
***************************************************************************/

// Configure the framework
#include "bconf/MCU_ESP8266.h"              // Load the code directly on the ESP8266
#include "conf/usart.h"                      // USART / RS485 transceiver

// Include framework code and libraries
#include <ESP8266WiFi.h>
#include <EEPROM.h>

/*** All configuration includes should be above this line ***/ 
#include "Souliss.h"

// This identify the number of the LED logic
#define MYLEDLOGIC          0      
         
// **** Define here the right pin for your ESP module **** 
#define OUTPUTPIN           5

void setup()
{   
    Initialize();                    

    // This is the vNet address for this node, used to communicate with other
    // nodes in your Souliss network
    SetAddress(0xCE02, 0xFF00, 0xAB01);
    
    Set_SimpleLight(MYLEDLOGIC);        // Define a simple LED light logic
    pinMode(OUTPUTPIN, OUTPUT);         // Use pin as output 
}

void loop()
{ 
    // Here we start to play
    EXECUTEFAST() {                     
        UPDATEFAST();   
        
        FAST_50ms() {   // We process the logic and relevant input and output every 50 milliseconds
            Logic_SimpleLight(MYLEDLOGIC);
            DigOut(OUTPUTPIN, Souliss_T1n_Coil,MYLEDLOGIC);
        } 
              
        // Here we handle here the communication with Android
        FAST_PeerComms();                                        
    }
} 
