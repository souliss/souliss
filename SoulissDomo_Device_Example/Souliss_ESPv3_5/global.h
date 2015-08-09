#ifndef GLOBAL_H
#define GLOBAL_H

ESP8266WebServer server(80);									// The Webserver
boolean firstStart = true;										// On firststart = true, NTP will try to get a valid time
int AdminTimeOutCounter = 0;									// Counter for Disabling the AdminMode
strDateTime DateTime;											// Global DateTime structure, will be refreshed every Second
WiFiUDP UDPNTPClient;											// NTP Client
unsigned long UnixTimestamp = 0;								// GLOBALTIME  ( Will be set by NTP)
boolean Refresh = false; // For Main Loop, to refresh things like GPIO / WS2812
int cNTP_Update = 0;											// Counter for Updating the time via NTP
Ticker tkSecond;												// Second - Timer for Updating Datetime Structure
boolean AdminEnabled = true;		// Enable Admin Mode for a given Time
byte Minute_Old = 100;				// Helpvariable for checking, when a new Minute comes up (for Auto Turn On / Off)
boolean nowifi = false;



struct strConfig {
	String ssid;
	String password;
	byte  IP[4];
	byte  Netmask[4];
	byte  Gateway[4];
	boolean dhcp;
	String ntpServerName;
	long Update_Time_Via_NTP_Every;
	long timezone;
	boolean daylight;
	String DeviceName;
	boolean AutoTurnOff;
	boolean AutoTurnOn;
	byte TurnOffHour;
	byte TurnOffMinute;
	byte TurnOnHour;
	byte TurnOnMinute;
	byte LED_R;
	byte LED_G;
	byte LED_B;
	boolean NodeMode; //test node mode as gateway or peer
	byte byte0;
	byte byte1;
	byte byte2;
	String tsAPI;
	boolean rst;
        byte cap_thresold;

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
            //SetAddress(0xAB02, 0xFF00, 0xAB01); 
            //SetAddress(0x00CA, 0xFF00, 0x00C8);
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
		Serial.println(".");
		//break;
	  }
	}
	if (WiFi.status() == WL_CONNECTED) {
		Serial.print("Already connect to : ");
		Serial.println(WiFi.SSID());
		Serial.println("Admin Time Out");
		AdminTimeOutCounter=0;
		WiFi.mode(WIFI_STA);
		Serial.println( "STA mode started" );
		Serial.println(WiFi.localIP());
		Serial.println("Souliss update IP Address");
		GetIPAddress(); 
		delay(500);
		nowifi = false;

	}
	else
	{
		Serial.println( "AP mode started coz' No WiFi or No Config" );
		WiFi.mode(WIFI_AP);
		WiFi.softAP("Souliss");
                Serial.println(WiFi.softAPIP());
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

bool EEPROM_CONFIG(){
    
    //EEPROM CONFIGURATION READ.

    // DHT LDR DALLAS OPTIONS:
    //switch (configuration[EEPROM_START]) { 
    switch (config.byte0) {  
        case 0:
            DHT_SENSOR = false;
            LDR_SENSOR = false;
            DALLAS_SENSOR = false;  
            break;
        case 1:
            DHT_SENSOR = true;
            LDR_SENSOR = false;
            DALLAS_SENSOR = false;  
            break;
        case 2:
            DHT_SENSOR = false;
            LDR_SENSOR = true;
            DALLAS_SENSOR = false;  
            break;
        case 3:
            DHT_SENSOR = false;
            LDR_SENSOR = false;
            DALLAS_SENSOR = true;  
            break;
        case 4:
            DHT_SENSOR = true;
            LDR_SENSOR = true;
            DALLAS_SENSOR = false;  
            break;
        case 5:
            DHT_SENSOR = true;
            LDR_SENSOR = false;
            DALLAS_SENSOR = true;  
            break;
        case 6:
            DHT_SENSOR = false;
            LDR_SENSOR = true;
            DALLAS_SENSOR = true;  
            break;
        case 7:
            DHT_SENSOR = true;
            LDR_SENSOR = true;
            DALLAS_SENSOR = true;  
            break;            
    }
    
    Serial1.print(DHT_SENSOR);
    LOG.print(LDR_SENSOR);
    LOG.print(DALLAS_SENSOR);
    LOG.print(" DLD (DHT-LDR-DALLAS)");
    LOG.print("\r\n");
    
    // PWM PIR RGB OPTIONS:
    //switch (configuration[EEPROM_START+1]) {  
    switch (config.byte1) {
        case 0:
            PWM_MODE = false;
            PIR_MODE = false;
            RGB_MODE = false;
            break;
        case 1:
            PWM_MODE = true;
            PIR_MODE = false;
            RGB_MODE = false;
            break;
        case 2:
            PWM_MODE = false;
            PIR_MODE = true;
            RGB_MODE = false;
            break;
        case 3:
            PWM_MODE = false;
            PIR_MODE = false;
            RGB_MODE = true;
            break;
    }
    LOG.print("PPR (PWM-PIR-RGB)");
    LOG.print(PWM_MODE);
    LOG.print(PIR_MODE);
    LOG.print(RGB_MODE);
    LOG.print(" PPR (PWM-PIR-RGB)");
    LOG.print("\r\n");
    
    // CAPACITIVE RELAY BMP180 OPTIONS
    //switch (configuration[EEPROM_START+2]) {
    switch (config.byte2) { 
        case 0:
            CAPACITIVE = false;
            RELAY = false;
            BMP180 = false;
            
            break;
        case 1:
            CAPACITIVE = true;
            RELAY = false;
            BMP180 = false;
            break;
        case 2:
            CAPACITIVE = false;
            RELAY = true;
            BMP180 = false;
            break;
        case 3:
            CAPACITIVE = false;
            RELAY = false;
            BMP180 = true;
            break;
    }
    LOG.print("CRB (CAP-RELAY-BMP180)");
    LOG.print(CAPACITIVE);
    LOG.print(RELAY);
    LOG.print(BMP180);
    LOG.print(" CRB (CAP-RELAY-BMP180)");
    LOG.print("\r\n");
    
    return 1;

}

void WriteConfig()
{

	LOG.println("Writing Config");
	EEPROM.write(0,'C');
	EEPROM.write(1,'F');
	EEPROM.write(2,'G');
	EEPROM.write(3,config.rst);
        EEPROM.write(4,config.cap_thresold);

	EEPROM.write(16,config.dhcp);
	EEPROM.write(17,config.daylight);
	
	EEPROMWritelong(18,config.Update_Time_Via_NTP_Every); // 4 Byte

	EEPROMWritelong(22,config.timezone);  // 4 Byte


	EEPROM.write(26,config.LED_R);
	EEPROM.write(27,config.LED_G);
	EEPROM.write(28,config.LED_B);

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

	EEPROM.write(44,config.AutoTurnOn);
	EEPROM.write(45,config.AutoTurnOff);
	EEPROM.write(46,config.TurnOnHour);
	EEPROM.write(47,config.TurnOnMinute);
	EEPROM.write(48,config.TurnOffHour);
	EEPROM.write(49,config.TurnOffMinute);
	EEPROM.write(50,config.NodeMode);
	EEPROM.write(51,config.byte0);
	EEPROM.write(52,config.byte1);
	EEPROM.write(53,config.byte2);
	WriteStringToEEPROM(54,config.DeviceName);   	//MAX 10
	WriteStringToEEPROM(64,config.tsAPI);		//MAX 16

	WriteStringToEEPROM(100,config.ssid);
	WriteStringToEEPROM(132,config.password);
	WriteStringToEEPROM(164,config.ntpServerName);  //MAX 20

	EEPROM.commit();
	
}
boolean ReadConfig()
{

	LOG.println("Reading Configuration");
	if (EEPROM.read(0) == 'C' && EEPROM.read(1) == 'F'  && EEPROM.read(2) == 'G' )
	{
  		LOG.println("Configuration Found!");
		config.rst = EEPROM.read(3);
                config.cap_thresold = EEPROM.read(4);
                
		config.dhcp = 	EEPROM.read(16);

		config.daylight = EEPROM.read(17);

		config.Update_Time_Via_NTP_Every = EEPROMReadlong(18); // 4 Byte

		config.timezone = EEPROMReadlong(22); // 4 Byte

		config.LED_R = EEPROM.read(26);
		config.LED_G = EEPROM.read(27);
		config.LED_B = EEPROM.read(28);

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
		
		config.AutoTurnOn = EEPROM.read(44);
		config.AutoTurnOff = EEPROM.read(45);
		config.TurnOnHour = EEPROM.read(46);
		config.TurnOnMinute = EEPROM.read(47);
		config.TurnOffHour = EEPROM.read(48);
		config.TurnOffMinute = EEPROM.read(49);
		config.NodeMode = EEPROM.read(50);
		config.byte0 = EEPROM.read(51);
		config.byte1 = EEPROM.read(52);
		config.byte2 = EEPROM.read(53);
		config.DeviceName= ReadStringFromEEPROM(54);		//MAX 10
		config.tsAPI= ReadStringFromEEPROM(64);			//MAX 16
		
		config.ssid = ReadStringFromEEPROM(100);
		config.password = ReadStringFromEEPROM(132);
		config.ntpServerName = ReadStringFromEEPROM(164);	//MAX 20

		//check_ESPMode();

		return true;
	
	}
	else
	{
		LOG.println("Configuration NOT FOUND!!!!");
		return false;
	}
}

/*
**
**  NTP 
**
*/

const int NTP_PACKET_SIZE = 48; 
byte packetBuffer[ NTP_PACKET_SIZE]; 
void NTPRefresh()
{

	


	if (WiFi.status() == WL_CONNECTED)
	{
		IPAddress timeServerIP; 
		WiFi.hostByName(config.ntpServerName.c_str(), timeServerIP); 
		//sendNTPpacket(timeServerIP); // send an NTP packet to a time server


		LOG.println("sending NTP packet...");
		memset(packetBuffer, 0, NTP_PACKET_SIZE);
		packetBuffer[0] = 0b11100011;   // LI, Version, Mode
		packetBuffer[1] = 0;     // Stratum, or type of clock
		packetBuffer[2] = 6;     // Polling Interval
		packetBuffer[3] = 0xEC;  // Peer Clock Precision
		packetBuffer[12]  = 49;
		packetBuffer[13]  = 0x4E;
		packetBuffer[14]  = 49;
		packetBuffer[15]  = 52;
		UDPNTPClient.beginPacket(timeServerIP, 123); 
		UDPNTPClient.write(packetBuffer, NTP_PACKET_SIZE);
		UDPNTPClient.endPacket();


		delay(1000);
  
		int cb = UDPNTPClient.parsePacket();
		if (!cb) {
			LOG.println("NTP no packet yet");
		}
		else 
		{
			LOG.print("NTP packet received, length=");
			LOG.println(cb);
			UDPNTPClient.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
			unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
			unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
			unsigned long secsSince1900 = highWord << 16 | lowWord;
			const unsigned long seventyYears = 2208988800UL;
			unsigned long epoch = secsSince1900 - seventyYears;
			UnixTimestamp = epoch;
		}
	}
}

void Second_Tick()
{
	strDateTime tempDateTime;
	AdminTimeOutCounter++;
	cNTP_Update++;
	UnixTimestamp++;
	ConvertUnixTimeStamp(UnixTimestamp +  (config.timezone *  360) , &tempDateTime);
	if (config.daylight) // Sommerzeit beachten
		if (summertime(tempDateTime.year,tempDateTime.month,tempDateTime.day,tempDateTime.hour,0))
		{
			ConvertUnixTimeStamp(UnixTimestamp +  (config.timezone *  360) + 3600, &DateTime);
		}
		else
		{
			DateTime = tempDateTime;
		}
	else
	{
			DateTime = tempDateTime;
	}
	Refresh = true;
}
 

#endif

