/**************************************************************************
    Souliss - Follow Me
    
    A couple of DINo boards acts as ON/OFF driver for the four available relays,
    with remote and local control. Relays can be controlled locally using the 
    IN1 to IN4 inputs or via a Android user interface. 
    
    The pushbutton toggle ON or OFF the relay, if hold turn ON for a fixed
    amount of time and later put OFF the relay. Each command is automatically 
    replicated on the twin node.
    
    Applicable for : 
            - Lights,
            - Doors and gates lock,
            - Other single relay controlled devices

***************************************************************************/

// Let the IDE point to the Souliss framework
#include "SoulissFramework.h"

#include "bconf/DINo_v2.h"                  // Define the board type
#include "conf/Gateway.h"                   // The main node is the Gateway, we have just one node

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
#define LIGHT_NO                4           
#define ONTIME                  15          // Light is on for ONTIME number of cycles

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

}

void loop()
{ 
    // Here we start to play
    EXECUTEFAST() {                     
        UPDATEFAST();   
        
        FAST_50ms() {   // We process the logic and relevant input and output every 50 milliseconds

            U8 button_pressed=1;
                    
            if(DigInHold(IN1, Souliss_T1n_ToggleCmd, ONTIME, LIGHT1));         // Read inputs from IN1
            else if(DigInHold(IN2, Souliss_T1n_ToggleCmd, ONTIME, LIGHT2));    // Read inputs from IN2
            else if(DigInHold(IN3, Souliss_T1n_ToggleCmd, ONTIME, LIGHT3));    // Read inputs from IN3
            else if(DigInHold(IN4, Souliss_T1n_ToggleCmd, ONTIME, LIGHT4));    // Read inputs from IN4
            else
               button_pressed=0;
               
            Logic_SimpleLight(LIGHT1);                          // Execute the logic for Relay 1
            Logic_SimpleLight(LIGHT2);                          // Execute the logic for Relay 2
            Logic_SimpleLight(LIGHT3);                          // Execute the logic for Relay 3
            Logic_SimpleLight(LIGHT4);                          // Execute the logic for Relay 4
            
            DigOut(RELAY1, Souliss_T1n_Coil, LIGHT1);           // Drive the Relay 1
            DigOut(RELAY2, Souliss_T1n_Coil, LIGHT2);           // Drive the Relay 2
            DigOut(RELAY3, Souliss_T1n_Coil, LIGHT3);           // Drive the Relay 3
            DigOut(RELAY4, Souliss_T1n_Coil, LIGHT4);           // Drive the Relay 4

            // If a button has been pressed
            if(button_pressed)
            {       
                U8 i, cmd[LIGHT_NO];
                
                // Define the command to align the status of the twin node
                for(i=LIGHT1;i<LIGHT_NO;i++)
                    if(mOutput(i))
                        cmd[i]=Souliss_T1n_OnCmd;
                    else
                        cmd[i]=Souliss_T1n_OffCmd;
                
                // Sync the remote node with the actual status of this node
                SendData(Peer_address, LIGHT1, cmd, LIGHT_NO);
            
                // Reset    
                button_pressed=0;                       
            }
            
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