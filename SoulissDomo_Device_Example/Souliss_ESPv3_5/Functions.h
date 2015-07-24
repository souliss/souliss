/*void WebInterface_Start(){
    	Serial.println("Starting ES8266");
	if (!ReadConfig())
	{
		// DEFAULT CONFIG
		config.ssid = "MYSSID";
		config.password = "MYPASSWORD";
		config.dhcp = true;
		config.IP[0] = 192;config.IP[1] = 168;config.IP[2] = 1;config.IP[3] = 100;
		config.Netmask[0] = 255;config.Netmask[1] = 255;config.Netmask[2] = 255;config.Netmask[3] = 0;
		config.Gateway[0] = 192;config.Gateway[1] = 168;config.Gateway[2] = 1;config.Gateway[3] = 1;
		config.ntpServerName = "0.id.pool.ntp.org";
		config.Update_Time_Via_NTP_Every =  0;
		config.timezone = +7;
		config.daylight = true;
		config.DeviceName = "Not Named";
		config.AutoTurnOff = false;
		config.AutoTurnOn = false;
		config.TurnOffHour = 0;
		config.TurnOffMinute = 0;
		config.TurnOnHour = 0;
		config.TurnOnMinute = 0;
		WriteConfig();
		Serial.println("General config applied");
	}
	
	
	if (AdminEnabled)
	{
		Serial.println( "AP mode started" );
                WiFi.mode(WIFI_AP_STA);
		WiFi.softAP( ACCESS_POINT_NAME , ACCESS_POINT_PASSWORD);
                Serial.println(WiFi.localIP());
	}
	else
	{
		Serial.println( "STA mode started" );
                WiFi.mode(WIFI_STA);
                Serial.println(WiFi.localIP());
	}

}
void WebInteface_Responses(){
	server.on ( "/main.html", processMain);
	server.on ( "/admin/filldynamicdata", filldynamicdata );
	
	server.on ( "/favicon.ico",   []() { Serial.println("favicon.ico"); server.send ( 200, "text/html", "" );   }  );


	server.on ( "/admin.html", []() { Serial.println("admin.html"); server.send ( 200, "text/html", PAGE_AdminMainPage );   }  );
	server.on ( "/config.html", send_network_configuration_html );
	server.on ( "/info.html", []() { Serial.println("info.html"); server.send ( 200, "text/html", PAGE_Information );   }  );
	server.on ( "/ntp.html", send_NTP_configuration_html  );
	server.on ( "/general.html", send_general_html  );
//	server.on ( "/main.html", []() { server.send ( 200, "text/html", PAGE_EXAMPLE );  } );
	server.on ( "/style.css", []() { Serial.println("style.css"); server.send ( 200, "text/plain", PAGE_Style_css );  } );
	server.on ( "/microajax.js", []() { Serial.println("microajax.js"); server.send ( 200, "text/plain", PAGE_microajax_js );  } );
	server.on ( "/admin/values", send_network_configuration_values_html );
	server.on ( "/admin/connectionstate", send_connection_state_values_html );
	server.on ( "/admin/infovalues", send_information_values_html );
	server.on ( "/admin/ntpvalues", send_NTP_configuration_values_html );
	server.on ( "/admin/generalvalues", send_general_configuration_values_html);
	server.on ( "/admin/devicename",     send_devicename_value_html);
 //       server.on ( "/admin.html",     ESPreboot_html);

 

	server.onNotFound ( []() { Serial.println("Page Not Found"); server.send ( 400, "text/html", "Page not Found" );   }  );
	server.begin();
	Serial.println( "HTTP server started" );
	tkSecond.attach(1,Second_Tick);
	UDPNTPClient.begin(2390);  // Port for NTP receive

}

void WebInteface_Loop(){
      if (AdminEnabled)
	{
		if (AdminTimeOutCounter > AdminTimeOut)
		{
			 AdminEnabled = false;
			 Serial.println("Admin Mode disabled!");
			 WiFi.mode(WIFI_STA);
                         Serial.println( "STA mode started" );
                         Serial.println(WiFi.localIP());
		}
	}
	if (config.Update_Time_Via_NTP_Every  > 0 )
	{
		if (cNTP_Update > 5 && firstStart)
		{
			NTPRefresh();
			cNTP_Update =0;
			firstStart = false;
		}
		else if ( cNTP_Update > (config.Update_Time_Via_NTP_Every * 60) )
		{

			NTPRefresh();
			cNTP_Update =0;
		}
	}

	if(DateTime.minute != Minute_Old)
	{
		 Minute_Old = DateTime.minute;
		 if (config.AutoTurnOn)
		 {
			 if (DateTime.hour == config.TurnOnHour && DateTime.minute == config.TurnOnMinute)
			 {
				  Serial.println("SwitchON");
			 }
		 }


		 Minute_Old = DateTime.minute;
		 if (config.AutoTurnOff)
		 {
			 if (DateTime.hour == config.TurnOffHour && DateTime.minute == config.TurnOffMinute)
			 {
				  Serial.println("SwitchOff");
			 }
		 }
	}
	server.handleClient();
              
              

	if (Refresh)  
	{
		Refresh = false;
		 //Serial.println("Refreshing...");
		 Serial.printf("FreeMem:%d %d:%d:%d %d.%d.%d \n",ESP.getFreeHeap() , DateTime.hour,DateTime.minute, DateTime.second, DateTime.year, DateTime.month, DateTime.day);
                 Serial.print(EEPROM.read(307)); Serial.print("\t");
                 Serial.print(EEPROM.read(308)); Serial.print("\t");
                 Serial.print(EEPROM.read(309)); Serial.print("\t");
                 Serial.println(STORE__SIZE);                 
	}


}
*/
bool EEPROM_CONFIG(){
    
    //EEPROM CONFIGURATION READ.
    LOG("EEPROM DATA: ");
    byte0 = EEPROM.read(EEPROM_START+0);
    byte1 = EEPROM.read(EEPROM_START+1);
    byte2 = EEPROM.read(EEPROM_START+2);
 
    bool DATA_OK = true;
 
    //FOR TESTING PURPORSES
    if(EEPROM.read(EEPROM_START+0) == byte0){
      
    }else{ 
        EEPROM.write(EEPROM_START+0, byte0); 
        Serial.println("BYTE0_WRITED"); 
        DATA_OK = false; 
    }
    if(EEPROM.read(EEPROM_START+1) == byte1){
      
    }else{ 
        EEPROM.write(EEPROM_START+1, byte1); 
        Serial.println("BYTE1_WRITED"); 
        DATA_OK = false; 
    }
    if(EEPROM.read(EEPROM_START+2) == byte2){
      
    }else{ 
        EEPROM.write(EEPROM_START+2, byte2); 
        Serial.println("BYTE2_WRITED"); 
        DATA_OK = false; 
    }
    
    if (!DATA_OK) EEPROM.commit();      
    
    for (int i = EEPROM_START; i<=EEPROM_END; i++) {
      configuration[i] = EEPROM.read(i);
      if(configuration[i] > 7)
          return 0;
      LOG(configuration[i]);
      LOG("\t");
    }
    LOG("\r\n");
    
    // DHT LDR DALLAS OPTIONS:
    switch (configuration[EEPROM_START]) {
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
    
    LOG(DHT_SENSOR);
    LOG(LDR_SENSOR);
    LOG(DALLAS_SENSOR);
    LOG(" DLD (DHT-LDR-DALLAS)");
    LOG("\r\n");
    
    // PWM PIR RGB OPTIONS:
    switch (configuration[EEPROM_START+1]) {
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
    LOG("PPR (PWM-PIR-RGB)");
    LOG(PWM_MODE);
    LOG(PIR_MODE);
    LOG(RGB_MODE);
    LOG(" PPR (PWM-PIR-RGB)");
    LOG("\r\n");
    
    // CAPACITIVE RELAY BMP180 OPTIONS
    switch (configuration[EEPROM_START+2]) {
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
    LOG("CRB (CAP-RELAY-BMP180)");
    LOG(CAPACITIVE);
    LOG(RELAY);
    LOG(BMP180);
    LOG(" CRB (CAP-RELAY-BMP180)");
    LOG("\r\n");
    
    return 1;

}

//************************* PINS CONFIGURATION FUNCTION ************************
void PINS_CONFIG(){
  //if(DHT_SENSOR)
      //DHTPIN = 16;     // what pin we're connected to
    
  if(PWM_MODE){
      LEDPWMP0 = 5;//12;      //LED STRIP ON PIN 12
      LEDPWMP1 = 16;//13;      //LED STRIP ON PIN 13
      LEDPWMP2 = 15;      //LED STRIP ON PIN 15
  }
  
  if(PIR_MODE){
      LEDPWMP0 = 5;//12;      //LED STRIP ON PIN 12
      LEDPWMP1 = 16;//13;      //LED STRIP ON PIN 13
      LEDP = 15;      //LED STRIP ON PIN 15
      PIRP = 2;       //LED STRIP ON PIN 2
  }
  
  if(RGB_MODE){
      LEDRP = 5;//12;      //LED STRIP ON PIN 12
      LEDGP = 16;//13;      //LED STRIP ON PIN 13
      LEDBP = 15;      //LED STRIP ON PIN 15
  }
  
  //PIN OPTIONS FOR CAPACITIVE - RELAY OR BMP180
  if(CAPACITIVE){
      //SDA 5  SCL 4  PINS
      CAP0P = 12;//4;
      CAP1P = 14;//5;
  }
  
  if(RELAY){
      RELAY0P = 12;//4;
      RELAY1P = 14;//5;
  }

}

void SLOT_CONFIG(){
  int NEXTSLOT = 0;
  Serial.println("SLOT CONFIG");  
  
  if(DHT_SENSOR){
      TEMPERATURE = NEXTSLOT;
      HUMIDITY = NEXTSLOT + 2;
      NEXTSLOT = HUMIDITY + 2;
      Serial.print("TEMP: ");
      Serial.println(TEMPERATURE);  
      Serial.print("HUMI: ");
      Serial.println(HUMIDITY);      
  }
  
  if(PWM_MODE || PIR_MODE){
      LEDPWM0 = NEXTSLOT;
      LEDPWM1 = NEXTSLOT + 2;
      NEXTSLOT = LEDPWM1 + 2;
      Serial.print("LEDPWM0: ");
      Serial.println(LEDPWM0);  
      Serial.print("LEDPWM1: ");
      Serial.println(LEDPWM1);      
  }
  if(PWM_MODE){
      LEDPWM2 = NEXTSLOT;
      NEXTSLOT = LEDPWM2 + 2;
      Serial.print("LEDPWM2: ");
      Serial.println(LEDPWM2);     
  }
  
  if(PIR_MODE){
      LED = NEXTSLOT;
      NEXTSLOT = LED + 1;
      Serial.print("LED: ");
      Serial.println(LED);       
  }
  
  if(RGB_MODE){
      LEDRGB = NEXTSLOT;
      NEXTSLOT = LEDRGB + 4;
      Serial.print("LEDRGB: ");
      Serial.println(LEDRGB);        
  }
  
  if(LDR_SENSOR){
      LDR = NEXTSLOT;
      NEXTSLOT = LDR + 2;
      Serial.print("LDR: ");
      Serial.println(LDR);        
  }
  
  if(DALLAS_SENSOR){
      DALLAS = NEXTSLOT;
      NEXTSLOT = DALLAS + 2;
      Serial.print("DALLAS: ");
      Serial.println(DALLAS);        
  }
  
  //GPIO 4-5 SLOT DEFINITIONS
  
  if(CAPACITIVE){
      CAP0 = NEXTSLOT; 
      CAP1 = NEXTSLOT + 1;
      NEXTSLOT = CAP1 + 1;
      Serial.print("CAP0: ");
      Serial.println(CAP0);   
      Serial.print("CAP1: ");
      Serial.println(CAP1);             
  }
  
  if(RELAY){
      RELAY0 = NEXTSLOT;
      RELAY1 = NEXTSLOT + 1;
      NEXTSLOT = RELAY1 + 1;
      Serial.print("RELAY0: ");
      Serial.println(RELAY0);   
      Serial.print("RELAY1: ");
      Serial.println(RELAY1);         
  }
  
  if(BMP180){
      PRESSURE0 = NEXTSLOT;  
      BMP180TEMP = NEXTSLOT + 2;
      NEXTSLOT = BMP180TEMP + 2;
      Serial.print("PRESSURE0: ");
      Serial.println(PRESSURE0);   
      Serial.print("BMP180TEMP: ");
      Serial.println(BMP180TEMP);       
  }  
}


/*******************************************************NUEVA FUNCION 1 PIN CAPACITIVO ***************************************/
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
  uint8_t cycles = 35;//17;
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
  else if (digitalRead(pinToMeasure)) { cycles = 17;}
  else if (digitalRead(pinToMeasure)) { cycles = 18;}
  else if (digitalRead(pinToMeasure)) { cycles = 19;}  
  else if (digitalRead(pinToMeasure)) { cycles = 20;}
  else if (digitalRead(pinToMeasure)) { cycles = 21;}
  else if (digitalRead(pinToMeasure)) { cycles = 22;}    
  else if (digitalRead(pinToMeasure)) { cycles = 23;}
  else if (digitalRead(pinToMeasure)) { cycles = 24;}
  else if (digitalRead(pinToMeasure)) { cycles = 25;}
  else if (digitalRead(pinToMeasure)) { cycles = 26;}
  else if (digitalRead(pinToMeasure)) { cycles = 27;}
  else if (digitalRead(pinToMeasure)) { cycles = 28;}
  else if (digitalRead(pinToMeasure)) { cycles = 29;}
  else if (digitalRead(pinToMeasure)) { cycles = 30;}
  else if (digitalRead(pinToMeasure)) { cycles = 31;}
  else if (digitalRead(pinToMeasure)) { cycles = 32;}
  else if (digitalRead(pinToMeasure)) { cycles = 33;}
  else if (digitalRead(pinToMeasure)) { cycles = 34;}
  
  
  
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


uint8_t CapSense(uint8_t slot, uint8_t value, uint8_t value_hold, uint8_t pin, uint8_t thresold_value, int holdtime) {

    int cycles = readCapacitivePin(pin); 
    if(DEBUG_CAPSENSE){
          if(millis()%300==0) Serial.println("Pin\tSlotIn\tOut\tPWM\tcycles\tReturn");
          Serial.print(pin);
          Serial.print("\t");        
          Serial.print(InPin[pin]);
          Serial.print("\t");        
          Serial.print(mInput(slot));
          Serial.print("\t");        
          Serial.print(mOutput(slot));
          Serial.print("\t");
          Serial.print(mOutput(slot+1));
          Serial.print("\t");
          Serial.print(cycles); 
          Serial.print("\t");
          Serial.print(thresold_value); 
          Serial.print("\t");          
          Serial.print(abs(millis()-time)); 
          Serial.print("\t");          

    }
  
    if(cycles > thresold_value && (InPin[pin] == 10 || InPin[pin] == 11)){
               InPin[pin] = PINSET;
    }
        
    if(cycles > thresold_value && (InPin[pin]==PINRESET))
	{
		time = millis();								// Record time
		InPin[pin] = PINSET;
		
		return InPin[pin];
	}
	else if(cycles > thresold_value && (abs(millis()-time) > holdtime) && (InPin[pin]==PINSET || InPin[pin]==PINACTIVE))
	{
		if(AUTOCALIBRATE && (abs(millis()-time) > 15000)){
            thresold = cycles + 2;
            if(DEBUG_CAPSENSE) { 
                Serial.print("Autocalibrate: ");
                Serial.println(thresold);
            }
            return thresold;
        }else{
            InPin[pin] = PINACTIVE;								// Stay there till pushbutton is released
      		// Write timer value in memory map
		    if (memory_map)	memory_map[MaCaco_IN_s + slot] = value_hold;
            return value_hold;
        }
	}
    else if(cycles < thresold_value && (InPin[pin]==PINSET)) {
		InPin[pin] = 10;
        return InPin[pin];
	}
	else if(cycles < thresold_value && (InPin[pin]==PINACTIVE)) {
		InPin[pin] = 11;
        return InPin[pin];
    }
    else if(cycles < thresold_value && InPin[pin] == 10){
        if(mInput(slot) == Souliss_T1n_OffCmd || mInput(slot) == Souliss_T1n_OnCmd){
          	if(mOutput(slot+1) == 0) mOutput(slot) = Souliss_T1n_OffCoil;
            else                     mOutput(slot) = Souliss_T1n_OnCoil;
                memory_map[MaCaco_IN_s + slot] = Souliss_T1n_RstCmd;			// Reset
        }
        else memory_map[MaCaco_IN_s + slot] = value; 
        
        InPin[pin] = PINRESET;     
        return value;
    }
    else if(cycles < thresold_value && InPin[pin] == 11){
          InPin[pin] = PINRESET;
          if(mOutput(slot+1) == 0) mOutput(slot) = Souliss_T1n_OffCoil;
          else                     mOutput(slot) = Souliss_T1n_OnCoil;
          memory_map[MaCaco_IN_s + slot] = Souliss_T1n_RstCmd;			// Reset
    }          
	
    // return MaCaco_NODATACHANGED;

}


