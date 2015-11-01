/**************************************************************************
    Souliss - Erase Zero Configuration
    
    The addresses from Zero Configuration examples are stored in the EEPROM
    if you want to erase the stored configuration, run this example on the
    node.
    
***************************************************************************/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include <WiFiUdp.h>

#include "bconf/MCU_ESP8266.h"
#include "conf/DynamicAddressing.h"                 // Use dynamic address
#include "conf/Gateway.h"
#define VNET_MEDIA_INSKETCH

// Include framework code and libraries
#include <EEPROM.h>

/*** All configuration includes should be above this line ***/ 
#include "Souliss.h"

// Setup the libraries for Over The Air Update
OTA_Setup();

void setup()
{   
    // Erase network configuration parameters from previous use of ZeroConf
    Store_Init();
    Store_Clear();
    Store_Commit();

    // Init Serial
    Serial.begin(9600);

    // Init the OTA
    OTA_Init();
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

    // Look for a new sketch to update over the air
    OTA_Process();
} 