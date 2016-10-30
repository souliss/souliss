/**************************************************************************
    Souliss - Lights

    It handle the four relays either via IN1 to IN4 inputs or using the
    Android interface. Connecting the relays to lights or similar electrical 
    appliance, you can get remote control of them.

    Connect multiple ProDINo Wroom via RS485 using the e08_WiFi_Lights_n2.ino
    on the peer boards.
        
    Applicable for:
        - Light
        - Other ON/OFF electrical appliance
    
    Runs on KMP Electronics ProDINo Wroom (WiFi ESP8266)

***************************************************************************/

// Let the IDE point to the Souliss framework
#include "SoulissFramework.h"

#include "bconf/DINo_WiFi_Bridge_RS485.h"   // Define the board type
#include "conf/Gateway.h"                   // The main node is the Gateway, we have just one node
#include "conf/Webhook.h"                   // Enable DHCP and DNS
#include "conf/DynamicAddressing.h"         // Use dynamic address

// **** Define the WiFi name and password ****
#define WIFICONF_INSKETCH
#define WiFi_SSID               "mywifi"
#define WiFi_Password           "mypassword"    

// Include framework code and libraries
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <EEPROM.h>

/*** All configuration includes should be above this line ***/ 
#include "Souliss.h"

#define LIGHT1                  0           // This is the memory slot used for the execution of the logic
#define LIGHT2                  1           
#define LIGHT3                  2           
#define LIGHT4                  3           

void setup()
{   
    // Init the board
    InitDINo();

    // Connect to the WiFi network and get an address from DHCP
    GetIPAddress();                           
    SetAsGateway(myvNet_dhcp);       // Set this node as gateway for SoulissApp  
    
    // Define Simple Light logics for the relays
    Set_SimpleLight(LIGHT1);
    Set_SimpleLight(LIGHT2);
    Set_SimpleLight(LIGHT3);
    Set_SimpleLight(LIGHT4);    
    
    // This node will act as addressing server for the other peers in the network
    SetAddressingServer();
}

void loop()
{ 
    // Here we start to play
    EXECUTEFAST() {                     
        UPDATEFAST();   
        
        FAST_50ms() {   // We process the logic and relevant input and output every 50 milliseconds
        
            DigIn(IN1, Souliss_T1n_ToggleCmd, LIGHT1);          // Read inputs from IN1
            DigIn(IN2, Souliss_T1n_ToggleCmd, LIGHT2);          // Read inputs from IN2
            DigIn(IN3, Souliss_T1n_ToggleCmd, LIGHT3);          // Read inputs from IN3
            DigIn(IN4, Souliss_T1n_ToggleCmd, LIGHT4);          // Read inputs from IN4
        
            Logic_SimpleLight(LIGHT1);                          // Execute the logic for Relay 1
            Logic_SimpleLight(LIGHT2);                          // Execute the logic for Relay 2
            Logic_SimpleLight(LIGHT3);                          // Execute the logic for Relay 3
            Logic_SimpleLight(LIGHT4);                          // Execute the logic for Relay 4
            
            DigOut(RELAY1, Souliss_T1n_Coil, LIGHT1);           // Drive the Relay 1
            DigOut(RELAY2, Souliss_T1n_Coil, LIGHT2);           // Drive the Relay 2
            DigOut(RELAY3, Souliss_T1n_Coil, LIGHT3);           // Drive the Relay 3
            DigOut(RELAY4, Souliss_T1n_Coil, LIGHT4);           // Drive the Relay 4
        } 
        
        // Here we process all communication with other nodes
        FAST_GatewayComms();    
    }
    
    EXECUTESLOW() { 
        UPDATESLOW();

        SLOW_10s() {  // Process the timer every 10 seconds  
            Timer_SimpleLight(LIGHT1);
            Timer_SimpleLight(LIGHT2);  
            Timer_SimpleLight(LIGHT3);
            Timer_SimpleLight(LIGHT4);
        }     
    }
} 