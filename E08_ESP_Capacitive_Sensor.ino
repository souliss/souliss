/**************************************************************************
    Souliss - Hello World for Expressif ESP8266 with New WIFI MANAGER 
    and two Capacitive Buttons on pins 12 and 14 with no more wires or resistors.
    
    This is the basic example, create a software push-button on Android
    using SoulissApp (get it from Play Store).  
    
    Load this code on ESP8266 board using the porting of the Arduino core
    for this platform.
*************************************************************************/

***************************************************************************/
/************************     NEW WIFI MANAGER    *************************/
/***************************************************************************/

include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

enum {
  APPLICATION_WEBSERVER = 0,
  ACCESS_POINT_WEBSERVER
};

MDNSResponder mdns;
ESP8266WebServer server(80);
const char* ssid = "esp8266e";	// Use this as the ssid as well
                                // as the mDNS name
const char* passphrase = "esp8266e";
String st;
String content;

boolean start;

/***************************************************************************/
/************************    CAPACITIVE BUTTONS   **************************/
/***************************************************************************/
/*800k - 1MOhm resistor between pins are required*/
#define cap_pin1 14
#define cap_pin2 12

#define DEBUG_CAPSENSE 1

/***************************************************************************/
/****************************   SOULISS    *********************************/
/***************************************************************************/


#define MaCaco_DEBUG_INSKETCH
#define MaCaco_DEBUG  		1

#define VNET_DEBUG_INSKETCH
#define VNET_DEBUG  		1

// Configure the framework
#include "bconf/MCU_ESP8266.h"              // Load the code directly on the ESP8266
#include "conf/Gateway.h"                   // The main node is the Gateway, we have just one node
#include "conf/IPBroadcast.h"

// Include framework code and libraries
//#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include "Souliss.h"

// This identify the number of the LED logic
#define MYLEDLOGIC          0               

// **** Define here the right pin for your ESP module **** 
#define	OUTPUTPIN			15


void setup()
{   
  Serial.begin(115200);
  start = setup_esp();
  if(start) Serial.print("TRUE"); else Serial.print("FALSE");
  
  if(start){
    Initialize();

    // Connect to the WiFi network and get an address from DHCP
    GetIPAddress();                           
    SetAsGateway(myvNet_dhcp);       // Set this node as gateway for SoulissApp  

    // This is the vNet address for this node, used to communicate with other
    // nodes in your Souliss network
    SetAddress(0xAB01, 0xFF00, 0x0000);
    SetAsPeerNode(0xAB02, 1);
	
    Set_DimmableLight(MYLEDLOGIC);        // Define a simple LED light logic
	
    pinMode(OUTPUTPIN, OUTPUT);         // Use pin as output 
  }
}
//Souliss_T1n_BrightSwitch
void loop()
{ 
 if (!start) {
   server.handleClient();	// In this example we're not doing too much
 }  
 else 
 { 
    // Here we start to play
    EXECUTEFAST() {                     
        UPDATEFAST();   
        
        FAST_50ms() {   // We process the logic and relevant input and output every 50 milliseconds

//          Serial.print(CapSense(MYLEDLOGIC,Souliss_T1n_ToggleCmd,Souliss_T1n_BrightSwitch,cap_pin1, 2, 1500, 1));
            Serial.print(CapSense(MYLEDLOGIC,Souliss_T1n_ToggleCmd,Souliss_T1n_BrightUp,cap_pin1, 2, 1500, 1));
            Serial.print("\t");
            Serial.println(CapSense(MYLEDLOGIC,Souliss_T1n_ToggleCmd,Souliss_T1n_BrightDown,cap_pin2, 2, 1500, 1));
        

            Logic_DimmableLight(MYLEDLOGIC);
            //DigOut(OUTPUTPIN, Souliss_T1n_Coil,MYLEDLOGIC);
            analogWrite(OUTPUTPIN, mOutput(MYLEDLOGIC+1)*4);
        } 
              
        // Here we handle here the communication with Android
        FAST_GatewayComms();                                        
    }

 }
} 
/*****************************************************************************************************/
/**********************************  SOULISS CAPACITIVE PIN FUNCTION  ********************************/
/*****************************************************************************************************/

int CapSense(int slot, int value, int value_hold, int pin, int thresold, int holdtime, boolean holdrepeat) {
    
  int cycles = readCapacitivePin(pin); 
  if(DEBUG_CAPSENSE){// && stat != NONE){
        if(millis()%300==0) Serial.println("Pin\tSlotIn\tOut\tPWM\tcycles\tReturn");
        Serial.print(pin);
        Serial.print("\t");        
        Serial.print(mInput(slot));
        Serial.print("\t");        
        Serial.print(mOutput(slot));
        Serial.print("\t");
        Serial.print(mOutput(slot+1));
        Serial.print("\t");
        Serial.print(cycles); 
        Serial.print("\t");
  }
  // If pin is on, set the "value"
  if(cycles > thresold && (InPin[pin]==PINRESET))
  {
	time = millis();								// Record time
	InPin[pin] = PINSET;
	
	return MaCaco_NODATACHANGED;
  }
  else if(cycles > thresold && (abs(millis()-time) > holdtime) && (InPin[pin]==PINSET))
  {
	InPin[pin] = PINACTIVE;								// Stay there till pushbutton is released
	
	// Write timer value in memory map
	if(memory_map)	memory_map[MaCaco_IN_s + slot] = value_hold;

	return value_hold;
  }
  else if(cycles < thresold){ 
        if(InPin[pin] == PINSET){
	    // Write input value in memory map
	    if(memory_map && memory_map[MaCaco_IN_s + slot] == 0) memory_map[MaCaco_IN_s + slot] = value;
            else{
                   if(mOutput(slot+1) == 0) mOutput(slot) = Souliss_T1n_OffCoil;
                   else                     mOutput(slot) = Souliss_T1n_OnCoil;
                   memory_map[MaCaco_IN_s + slot] = Souliss_T1n_RstCmd;			// Reset
            }
            InPin[pin] = PINRESET;
            return value;
        }
	if(InPin[pin]==PINACTIVE){
            if(mOutput(slot+1) == 0) mOutput(slot) = Souliss_T1n_OffCoil;
            else                     mOutput(slot) = Souliss_T1n_OnCoil;
            memory_map[MaCaco_IN_s + slot] = Souliss_T1n_RstCmd;			// Reset
   	    InPin[pin] = PINRESET;		
	}
  }
  if(InPin[pin] == PINACTIVE && holdrepeat){  
        // Write timer value in memory map
        if(memory_map)	memory_map[MaCaco_IN_s + slot] = value_hold;
  }

return MaCaco_NODATACHANGED;

}

/*****************************************************************************************************/
/***********************************NEW FUNCTION 1 CAPACITIVE PIN ************************************/
/*****************************************************************************************************/

uint8_t readCapacitivePin(int pinToMeasure) {
  pinMode(pinToMeasure, OUTPUT);
  digitalWrite(pinToMeasure, LOW);
  delay(1);
  // Prevent the timer IRQ from disturbing our measurement
  noInterrupts();
  // Make the pin an input with the internal pull-up on
  pinMode(pinToMeasure, INPUT_PULLUP);

  // Now see how long the pin to get pulled up. This manual unrolling of the loop
  // decreases the number of hardware cycles between each read of the pin,
  // thus increasing sensitivity.
  uint8_t cycles = 17;
       if (digitalRead(pinToMeasure)) { cycles =  0;}
  else if (digitalRead(pinToMeasure)) { cycles =  1;}
  else if (digitalRead(pinToMeasure)) { cycles =  2;}
  else if (digitalRead(pinToMeasure)) { cycles =  3;}
  else if (digitalRead(pinToMeasure)) { cycles =  4;}
  else if (digitalRead(pinToMeasure)) { cycles =  5;}
  else if (digitalRead(pinToMeasure)) { cycles =  6;}
  else if (digitalRead(pinToMeasure)) { cycles =  7;}
  else if (digitalRead(pinToMeasure)) { cycles =  8;}
  else if (digitalRead(pinToMeasure)) { cycles =  9;}
  else if (digitalRead(pinToMeasure)) { cycles = 10;}
  else if (digitalRead(pinToMeasure)) { cycles = 11;}
  else if (digitalRead(pinToMeasure)) { cycles = 12;}
  else if (digitalRead(pinToMeasure)) { cycles = 13;}
  else if (digitalRead(pinToMeasure)) { cycles = 14;}
  else if (digitalRead(pinToMeasure)) { cycles = 15;}
  else if (digitalRead(pinToMeasure)) { cycles = 16;}

  // End of timing-critical section
  interrupts();

  // Discharge the pin again by setting it low and output
  //  It's important to leave the pins low if you want to 
  //  be able to touch more than 1 sensor at a time - if
  //  the sensor is left pulled high, when you touch
  //  two sensors, your body will transfer the charge between
  //  sensors.
  digitalWrite(pinToMeasure, LOW);
  pinMode(pinToMeasure, OUTPUT);

  return cycles;
}

/************************************************************************************************************/
/***************************************** NEW WIFI MANAGER FUNCTIONS   *************************************/
/************************************************************************************************************/

bool setup_esp() {
  
  WiFi.mode(WIFI_STA);  // Assume we've already been configured
  //  Serial.setDebugOutput(true);
  //  WiFi.printDiag(Serial);
  if (testWifi()) {
    setupApplication();	// WiFi established, setup application
    return 1;
  } else {
    setupAccessPoint();	// No WiFi yet, enter configuration mode
    return 0;
  }

}

bool testWifi(void) {
  int c = 0;
  Serial.println("\nWaiting for Wifi to connect...");
  while ( c < 20 ) {
    if (WiFi.status() == WL_CONNECTED) {
      return true;
    }
    delay(500);
    Serial.print(WiFi.status());
    c++;
  }
  Serial.println("\nConnect timed out, opening AP");
  return false;
}

void setupApplication() {
  if (mdns.begin(ssid, WiFi.localIP())) {
    Serial.println("\nMDNS responder started");
  }
  launchWeb(APPLICATION_WEBSERVER);	// In this example just launch a
  // web server
}

void setupAccessPoint(void) {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0)
    Serial.println("no networks found");
  else
  {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
      delay(10);
    }
  }
  Serial.println("");
  st = "<ol>";
  for (int i = 0; i < n; ++i)
  {
    // Print SSID and RSSI for each network found
    st += "<li>";
    st += WiFi.SSID(i);
    st += " (";
    st += WiFi.RSSI(i);
    st += ")";
    st += (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*";
    st += "</li>";
  }
  st += "</ol>";
  delay(100);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, passphrase, 6);
  launchWeb(ACCESS_POINT_WEBSERVER);
}

void launchWeb(int webservertype) {
  Serial.println("\nWiFi connected");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("SoftAP IP: ");
  Serial.println(WiFi.softAPIP());
  setupWebServerHandlers(webservertype);
  // Start the server
  server.begin();
  Serial.print("Server type ");
  Serial.print(webservertype);
  Serial.println(" started");
  //  WiFi.printDiag(Serial);
}

void setupWebServerHandlers(int webservertype)
{
  if ( webservertype == ACCESS_POINT_WEBSERVER ) {
    server.on("/", handleDisplayAccessPoints);
    server.on("/setap", handleSetAccessPoint);
    server.onNotFound(handleNotFound);
  } else if (webservertype == APPLICATION_WEBSERVER) {
    server.on("/", handleRoot);
    server.on("/setap", handleAccessPointAlreadySet);
    server.onNotFound(handleNotFound);
  }
}

void handleDisplayAccessPoints() {
  IPAddress ip = WiFi.softAPIP();
  String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
  uint8_t mac[6];
  WiFi.macAddress(mac);
  String macStr = macToStr(mac);
  content = "<!DOCTYPE HTML>\n<html>Hello from ";
  content += ssid;
  content += " at ";
  content += ipStr;
  content += " (";
  content += macStr;
  content += ")";
  content += "<p>";
  content += st;
  content += "<p><form method='get' action='setap'><label>SSID: </label>";
  content += "<input name='ssid' length=32><input name='pass' length=64><input type='submit'></form>";
  content += "<p>We will attempt to connect to the selected AP and reset if successful.";
  content += "<p>Wait a bit and try connecting to http://";
  content += ssid;
  content += ".local";
  content += "</html>";
  server.send(200, "text/html", content);
}

void handleSetAccessPoint() {
  int httpstatus = 200;
  String qsid = server.arg("ssid");
  String qpass = server.arg("pass");
  if (qsid.length() > 0 && qpass.length() > 0) {
    for (int i = 0; i < qsid.length(); i++)
    {
      // Deal with (potentially) plus-encoded ssid
      qsid[i] = (qsid[i] == '+' ? ' ' : qsid[i]);
    }
    for (int i = 0; i < qpass.length(); i++)
    {
      // Deal with (potentially) plus-encoded password
      qpass[i] = (qpass[i] == '+' ? ' ' : qpass[i]);
    }
    WiFi.mode(WIFI_AP_STA);
    WiFi.begin(qsid.c_str(), qpass.c_str());
    if (testWifi()) {
      Serial.println("\nGreat Success!");
      delay(3000);
      abort();
    }
    content = "<!DOCTYPE HTML>\n<html>";
    content += "Failed to connect to AP ";
    content += qsid;
    content += ", try again.</html>";
  } else {
    content = "<!DOCTYPE HTML><html>";
    content += "Error, no ssid or password set?</html>";
    Serial.println("Sending 404");
    httpstatus = 404;
  }
  server.send(httpstatus, "text/html", content);
}

void handleRoot() {
  IPAddress ip = WiFi.localIP();
  String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
  uint8_t mac[6];
  WiFi.macAddress(mac);
  String macStr = macToStr(mac);
  content = "<!DOCTYPE HTML>\n<html>Hello from ";
  content += ssid;
  content += " at ";
  content += ipStr;
  content += " (";
  content += macStr;
  content += ")";
  content += "</html>";
  server.send(200, "text/html", content);
}

void handleAccessPointAlreadySet() {
  content = "<!DOCTYPE HTML>\n<html>";
  content += "You already set up the access point and it is working if you got this far.";
  content += "</html>";
  server.send(200, "text/html", content);
}

void handleNotFound() {
  content = "File Not Found\n\n";
  content += "URI: ";
  content += server.uri();
  content += "\nMethod: ";
  content += (server.method() == HTTP_GET) ? "GET" : "POST";
  content += "\nArguments: ";
  content += server.args();
  content += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    content += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", content);
}

String macToStr(const uint8_t* mac)
{
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
    if (i < 5)
      result += ':';
  }
  return result;
}
