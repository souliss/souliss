/**************************************************************************
    Souliss - Erase Zero Configuration
    
    The addresses from Zero Configuration examples are stored in the EEPROM
    if you want to erase the stored configuration, run this example on the
    node.
    
***************************************************************************/

// Let the IDE point to the Souliss framework
#include "SoulissFramework.h"

/*
    Use bconf/StandardArduino.h for Arduino board based on AVR
    Use bconf/MCU_ESP8266.h for ESP8266 based board

    To compile as ESP8266, you need to add #include <WiFiUdp.h> and
    have the relevant cores installed in your IDE
*/

#include "bconf/StandardArduino.h"
#include "conf/DynamicAddressing.h"                 // Use dynamic address
#include "conf/Gateway.h"
#define VNET_MEDIA_INSKETCH

// Include framework code and libraries
#include <EEPROM.h>

/*** All configuration includes should be above this line ***/ 
#include "Souliss.h"


void setup()
{   
    // Erase network configuration parameters from previous use of ZeroConf
    Store_Init();
    Store_Clear();
    Store_Commit();

    // Init Serial
    Serial.begin(9600);
}

void loop()
{ 
    // Give a bit of time to open the Serial Monitor
    delay(10000);
    
    Serial.println("Printing EEPROM Contents:");
    
    // Print the EEPROM contents, if erase has been succesfull you should see only zeros.
    for(uint16_t i=0; i<STORE__USABLESIZE; i++)
        Serial.println(Return_8bit(i));

    // Stops here
    while(1);
} 