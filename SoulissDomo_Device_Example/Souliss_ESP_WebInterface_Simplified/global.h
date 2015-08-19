#ifndef GLOBAL_H
#define GLOBAL_H

ESP8266WebServer server(80);									// The Webserver
boolean firstStart = true;										// On firststart = true, NTP will try to get a valid time
int AdminTimeOutCounter = 0;									// Counter for Disabling the AdminMode
//strDateTime DateTime;											// Global DateTime structure, will be refreshed every Second
//WiFiUDP UDPNTPClient;											// NTP Client
//unsigned long UnixTimestamp = 0;								// GLOBALTIME  ( Will be set by NTP)
boolean Refresh = false; // For Main Loop, to refresh things like GPIO / WS2812
//int cNTP_Update = 0;											// Counter for Updating the time via NTP
Ticker tkSecond;												// Second - Timer for Updating Datetime Structure
boolean AdminEnabled = true;		// Enable Admin Mode for a given Time
//byte Minute_Old = 100;				// Helpvariable for checking, when a new Minute comes up (for Auto Turn On / Off)
boolean nowifi = false;



struct strConfig {
	String ssid;
	String password;
	byte  IP[4];
	byte  Netmask[4];
	byte  Gateway[4];
	boolean dhcp;
	boolean NodeMode; //test node mode as gateway or peer
	boolean rst;
        
}   config;


/*
**
** CONFIGURATION HANDLING
**
*/
void Souliss_Node_Start()
{

	// Read Node Mode..
        if (config.NodeMode){
          if(nowifi != 1) {
            LOG.println("Gateway Mode");
            // Connect to the WiFi network and get an address from DHCP                      
            SetAsGateway(myvNet_dhcp);       // Set this node as gateway for SoulissApp  
            SetAddressingServer();
          }
          else {
        	LOG.println("No Gateway Mode coz No Wifi");		
          }
	
        }
        else {
            LOG.println(nowifi);
          if(nowifi != 1) {
            LOG.println("Peer Mode");
            // This board request an address to the gateway at runtime, no need
            // to configure any parameter here.
            SetDynamicAddressing();  
            GetAddress();
          }
          else {
            LOG.println("No Peer Mode coz No Wifi");	
          }
        }

}

void check_ESPMode()
{
	if (WiFi.status() != WL_CONNECTED);{
	  Serial.print("Connecting to AP");
	  for (int i=0; i < 10; i++){
		(WiFi.status() != WL_CONNECTED);
		delay(1000);
		LOG.println(".");
	  }
	}
	if (WiFi.status() == WL_CONNECTED) {
		LOG.print("Already connect to : ");
		LOG.println(WiFi.SSID());
		LOG.println("Admin Time Out");
		AdminTimeOutCounter=0;
		WiFi.mode(WIFI_STA);
		LOG.println( "STA mode started" );
		LOG.println(WiFi.localIP());
		LOG.println("Souliss update IP Address");
		GetIPAddress(); 
		delay(500);
		nowifi = false;

	}
	else
	{
		LOG.println( "AP mode started coz' No WiFi or No Config" );
		WiFi.mode(WIFI_AP);
		WiFi.softAP("Souliss");
                LOG.println(WiFi.softAPIP());
            	nowifi = true;	
	}

}

void ConfigureWifi()
{
	LOG.println("Configuring Wifi");
	WiFi.begin (config.ssid.c_str(), config.password.c_str());
	check_ESPMode();

	if (!config.dhcp)
	{
		WiFi.config(IPAddress(config.IP[0],config.IP[1],config.IP[2],config.IP[3] ),  IPAddress(config.Gateway[0],config.Gateway[1],config.Gateway[2],config.Gateway[3] ) , IPAddress(config.Netmask[0],config.Netmask[1],config.Netmask[2],config.Netmask[3] ));
		LOG.println("Souliss update IP Address for STATIC");
		GetIPAddress();
		LOG.println(WiFi.localIP());
	}
       	check_ESPMode();
}

void WriteConfig()
{

	LOG.println("Writing Config");
	EEPROM.write(0,'C');
	EEPROM.write(1,'F');
	EEPROM.write(2,'G');
	EEPROM.write(3,config.rst);
        
	EEPROM.write(16,config.dhcp);
	
	EEPROM.write(32,config.IP[0]);
	EEPROM.write(33,config.IP[1]);
	EEPROM.write(34,config.IP[2]);
	EEPROM.write(35,config.IP[3]);

	EEPROM.write(36,config.Netmask[0]);
	EEPROM.write(37,config.Netmask[1]);
	EEPROM.write(38,config.Netmask[2]);
	EEPROM.write(39,config.Netmask[3]);

	EEPROM.write(40,config.Gateway[0]);
	EEPROM.write(41,config.Gateway[1]);
	EEPROM.write(42,config.Gateway[2]);
	EEPROM.write(43,config.Gateway[3]);
	
	EEPROM.write(50,config.NodeMode);

	WriteStringToEEPROM(100,config.ssid);
	WriteStringToEEPROM(132,config.password);

	EEPROM.commit();
	
}
boolean ReadConfig()
{

	LOG.println("Reading Configuration");
	if (EEPROM.read(0) == 'C' && EEPROM.read(1) == 'F'  && EEPROM.read(2) == 'G' )
	{
  		LOG.println("Configuration Found!");
		config.rst = EEPROM.read(3);
                
		config.dhcp = 	EEPROM.read(16);

		config.IP[0] = EEPROM.read(32);
		config.IP[1] = EEPROM.read(33);
		config.IP[2] = EEPROM.read(34);
		config.IP[3] = EEPROM.read(35);
		config.Netmask[0] = EEPROM.read(36);
		config.Netmask[1] = EEPROM.read(37);
		config.Netmask[2] = EEPROM.read(38);
		config.Netmask[3] = EEPROM.read(39);
		config.Gateway[0] = EEPROM.read(40);
		config.Gateway[1] = EEPROM.read(41);
		config.Gateway[2] = EEPROM.read(42);
		config.Gateway[3] = EEPROM.read(43);
		
		config.NodeMode = EEPROM.read(50);
		
		config.ssid = ReadStringFromEEPROM(100);
		config.password = ReadStringFromEEPROM(132);
		
		return true;
	
	}
	else
	{
		LOG.println("Configuration NOT FOUND!!!!");
		return false;
	}
}

#endif


