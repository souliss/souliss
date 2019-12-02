/**************************************************************************
    Souliss - Hello World for Expressif ESP8266
    
    This is the basic example, create a software push-button on Android
    using SoulissApp (get it from Play Store).  
    
    Load this code on ESP8266 board using the porting of the Arduino core
    for this platform.

    This is an alpha webserver and notify version
    To compile it please download 
    https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot

    This sketch has a Wbserver always active
    This sketch include also Arduino OTA, WiFi RSSI Signal and Telegram Notify
    Please fill fields in webserver for VNET address, Hostname, 
    Telegram credentials, Notify Message    
        
***************************************************************************/

// Let the IDE point to the Souliss framework
#include "SoulissFramework.h"

// Include framework code and libraries
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

// Configure the framework
#include "bconf/MCU_ESP8266.h"              // Load the code directly on the ESP8266
#include "conf/IPBroadcast.h"
#include "conf/RuntimeGateway.h"            // This node is a Peer and can became a Gateway at runtime
#include "conf/DynamicAddressing.h"         // Use dynamically assigned addresses
#include "conf/WEBCONFinterface.h"          // Enable the WebConfig interface

/*** All configuration includes should be above this line ***/ 
#include "Souliss.h"

// This identify the number of the LED logic
#define MYLEDLOGIC          0      

#define T_WIFI_STRDB  2 //It takes 2 slots
#define T_WIFI_STR    4 //It takes 2 slots

//Deadband for analog values
#define NODEADBAND    0.5       //Se la variazione è superio del 0,1% aggiorno
         
// **** Define here the right pin for your ESP module **** 
#define	OUTPUTPIN			5

//Telegram notify functions - Please fill fields in Notify Section of WebServer
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
String BOTtoken;       
String chat_group;
String mychat_id;

void sendToTelegram(String choose, String text ) {
       WiFiClientSecure botclient;
       UniversalTelegramBot bot(BOTtoken, botclient);
       bot.sendSimpleMessage(choose, text, "");
       botclient.stop();
}      

//Variable to Handle WiFi Signal
long rssi = 0;
int bars = 0;
byte joined = false;
float getWiFi_rssi(){
  //float f_rssi = (float)rssi;
  return (float)rssi;
  
}
float getWiFi_bars() {
  //float f_bars = (float)bars;
  return (float)bars;
}

void setup()
{   
    Initialize();

	//Read Hostname from WebConfig
    char HostName[31];   
    String HostNameTemp = Read_NodeName();
    HostNameTemp.toCharArray(HostName,sizeof(HostName));
	
    // Connect to the WiFi network with parameters set in webconfig
    startWebServer();
	// Read the IP configuration from the EEPROM, if not available start
    // the node as access point
    if(!ReadIPConfiguration()) 
    {
	 SetAccessPoint();
    }
	 if (IsRuntimeGateway())
    {
        // Connect to the WiFi network and get an address from DHCP                      
        SetAsGateway(myvNet_dhcp);       // Set this node as gateway for SoulissApp  
        SetAddressingServer();
        
    }
	else 
    {
    // This is the vNet address for this node, used to communicate with other
	// nodes in your Souliss network - addresses from Webconfig

		U16 SoulissVNETAddress = Read_SoulissVNETAddress(); 
        U16 SoulissVNETGateway = Read_SoulissVNETGateway();     
        SetAddress(SoulissVNETAddress, 0xFF00, SoulissVNETGateway);
    }
    
    Set_SimpleLight(MYLEDLOGIC);        // Define a simple LED light logic
    pinMode(OUTPUTPIN, OUTPUT);         // Use pin as output 
	
	Souliss_SetT51(memory_map, T_WIFI_STRDB); // Set typical for wifi signal in decibel
    Souliss_SetT51(memory_map, T_WIFI_STR); // Set typical for wifi signal in bar from 1 to 5

	
	// Init the OTA
    ArduinoOTA.setHostname(HostName); // Please fill node name in webconfig  
    if (HostNameTemp == "") ArduinoOTA.setHostname("Souliss");  
    ArduinoOTA.begin();

	//Send your notify to Telegram - Please fill Notify Message and credential in webconfig
  String NotifyMessage = Read_NotifyMessage();
  
	if (Return_TelegramEnabled()) {	
		BOTtoken = Read_Telegram_BOTtoken();
    chat_group=Read_Telegram_ChatGroup();
    mychat_id=Read_Telegram_ChatID();
	
		sendToTelegram(chat_group, NotifyMessage); // if you want to send a message to a chat group 
		sendToTelegram(mychat_id, NotifyMessage); // if you want to send a message to a chat group 
	}				  
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
            
		FAST_210ms() {
		//Process logics for WiFi Signal
		Souliss_Logic_T51(memory_map, T_WIFI_STRDB, NODEADBAND, &data_changed);
		Souliss_Logic_T51(memory_map, T_WIFI_STR, NODEADBAND, &data_changed);
		}
			
        // Here we handle here the communication with Android
        FAST_PeerComms();                                        
    }
	
	EXECUTESLOW() {
        UPDATESLOW();

        SLOW_110s() {
		      checkWiFi_signal();
		      float f_rssi = getWiFi_rssi();
      		float f_bars = getWiFi_bars();
      		Souliss_ImportAnalog(memory_map, T_WIFI_STRDB, &f_rssi);
      		Souliss_ImportAnalog(memory_map, T_WIFI_STR, &f_bars);
    		}
	}
	 // Look for a new sketch to update over the air
    ArduinoOTA.handle();  
    runWebServer(); 
} 

void checkWiFi_signal() {
  rssi = WiFi.RSSI();

  if (rssi > -55) {
    bars = 5;
  }
  else if (rssi < -55 & rssi >= -65) {
    bars = 4;
  }
  else if (rssi < -65 & rssi >= -70) {
    bars = 3;
  }
  else if (rssi < -70 & rssi >= -78) {
    bars = 2;
  }
  else if (rssi < -78 & rssi > -82) {
    bars = 1;
  }
  else {
    bars = 0;
  }
}  
