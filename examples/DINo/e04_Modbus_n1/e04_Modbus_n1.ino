/**************************************************************************
    Souliss - Modbus Gateway
    
    It handle the four relays either via IN1 to IN4 inputs or using the
    Android interface. Connecting the relays to lights or similar electrical 
    appliance, you can get remote control of them.
    
    This node provides also an interface via Modbus, data are available at
    following registers:

                        Input       Typicals        Outputs
    Node    Slot
     0       0           0x000       0x100           0x200
     0       1           0x001       0x101           0x201  
     0       2           0x002       0x102           0x202
     0       3           0x003       0x103           0x203
     1       0           0x010       0x110           0x210
     1       1           0x011       0x111           0x211
     1       2           0x012       0x112           0x212
     1       3           0x013       0x113           0x213
        ...              ...         ...             ...
        ...              ...         ...             ...
     n       0           0x0n0       0x1n0           0x2n0
     n       1           0x0n1       0x1n1           0x2n1
     n       2           0x0n2       0x1n2           0x2n2
     n       3           0x0n3       0x1n3           0x2n3  
    
    Applicable for:
        - Light
        - Other ON/OFF electrical appliance
    
***************************************************************************/

// Let the IDE point to the Souliss framework
#include "SoulissFramework.h"

#include "bconf/DINo_v2.h"                  // Define the board type
#include "conf/Gateway.h"                   // The main node is the Gateway, we have just one node
#include "conf/ModbusTCP.h"                 // Define Modbus as interface
#include "conf/SmallNetwork.h"              // Lower the number of slot and nodes to match Modbus limits


// Include framework code and libraries
#include <SPI.h>

/*** All configuration includes should be above this line ***/ 
#include "Souliss.h"

// Define the network configuration
uint8_t ip_address[4]  = {192, 168, 1, 77};
uint8_t subnet_mask[4] = {255, 255, 255, 0};
uint8_t ip_gateway[4]  = {192, 168, 1, 1};
#define Gateway_address 77
#define Peer_address    78
#define myvNet_address  ip_address[3]       // The last byte of the IP address (77) is also the vNet address
#define myvNet_subnet   0xFF00
#define myvNet_supern   Gateway_address

#define LIGHT1                  0           // This is the memory slot used for the execution of the logic
#define LIGHT2                  1           // This is the memory slot used for the execution of the logic
#define LIGHT3                  2           // This is the memory slot used for the execution of the logic
#define LIGHT4                  3           // This is the memory slot used for the execution of the logic

void setup()
{   
    InitDINo();

    // Setup the network configuration
    //
    Souliss_SetIPAddress(ip_address, subnet_mask, ip_gateway);
    SetAsGateway((U16)ip_address[3]);                                       // Last byte of the IP address is the vNet address

    // This node as gateway will get data from the Peer
    SetAsPeerNode(Peer_address, 1); 
    
    // Set the inputs
    SetInput1();
    SetInput2();
    SetInput3();
    SetInput4();
    
    // Set the outputs
    SetRelay1();
    SetRelay2();
    SetRelay3();
    SetRelay4();
    
    // Define Simple Light logics for the relays
    Set_SimpleLight(LIGHT1);
    Set_SimpleLight(LIGHT2);
    Set_SimpleLight(LIGHT3);
    Set_SimpleLight(LIGHT4);    

    // Init the Modbus interface
    ModbusInit(myMap);
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
        
        // Process the Modbus communication periodically
        FAST_110ms()    Modbus(myMap);
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