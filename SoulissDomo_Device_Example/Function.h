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


