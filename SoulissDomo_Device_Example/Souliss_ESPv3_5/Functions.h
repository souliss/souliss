
//************************* PINS CONFIGURATION FUNCTION ************************
void PINS_CONFIG(){
  //if(DHT_SENSOR)
      //DHTPIN = 16;     // what pin we're connected to
    
  if(PWM_MODE){
      LEDPWMP0 = 12;//5;      //LED STRIP ON PIN 12
      LEDPWMP1 = 13;//16;      //LED STRIP ON PIN 13
      LEDPWMP2 = 15;      //LED STRIP ON PIN 15
  }
  
  if(PIR_MODE){
      LEDPWMP0 = 12;//5;      //LED STRIP ON PIN 12
      LEDPWMP1 = 13;//16;      //LED STRIP ON PIN 13
      LEDP = 15;      //LED STRIP ON PIN 15
      PIRP = 2;       //LED STRIP ON PIN 2
  }
  
  if(RGB_MODE){
      LEDRP = 12;//5;      //LED STRIP ON PIN 12
      LEDGP = 13;//16;      //LED STRIP ON PIN 13
      LEDBP = 15;      //LED STRIP ON PIN 15
  }
  
  //PIN OPTIONS FOR CAPACITIVE - RELAY OR BMP180
  if(CAPACITIVE){
      //SDA 5  SCL 4  PINS
      CAP0P = 4;//12;
      CAP1P = 5;//14;
  }
  
  if(RELAY){
      RELAY0P = 4;//12;
      RELAY1P = 5;//14;
  }

}

void SLOT_CONFIG(){
  int NEXTSLOT = 0;
  LOG.println("SLOT CONFIG");  
  
  if(DHT_SENSOR){
      TEMPERATURE = NEXTSLOT;
      HUMIDITY = NEXTSLOT + 2;
      NEXTSLOT = HUMIDITY + 2;
      LOG.print("TEMP: ");
      LOG.println(TEMPERATURE);  
      LOG.print("HUMI: ");
      LOG.println(HUMIDITY);      
  }
  
  if(PWM_MODE || PIR_MODE){
      LEDPWM0 = NEXTSLOT;
      LEDPWM1 = NEXTSLOT + 2;
      NEXTSLOT = LEDPWM1 + 2;
      LOG.print("LEDPWM0: ");
      LOG.println(LEDPWM0);  
      LOG.print("LEDPWM1: ");
      LOG.println(LEDPWM1);      
  }
  if(PWM_MODE){
      LEDPWM2 = NEXTSLOT;
      NEXTSLOT = LEDPWM2 + 2;
      LOG.print("LEDPWM2: ");
      LOG.println(LEDPWM2);     
  }
  
  if(PIR_MODE){
      LED = NEXTSLOT;
      NEXTSLOT = LED + 1;
      LOG.print("LED: ");
      LOG.println(LED);       
  }
  
  if(RGB_MODE){
      LEDRGB = NEXTSLOT;
      NEXTSLOT = LEDRGB + 4;
      LOG.print("LEDRGB: ");
      LOG.println(LEDRGB);        
  }
  
  if(LDR_SENSOR){
      LDR = NEXTSLOT;
      NEXTSLOT = LDR + 2;
      LOG.print("LDR: ");
      LOG.println(LDR);        
  }
  
  if(DALLAS_SENSOR){
      DALLAS = NEXTSLOT;
      NEXTSLOT = DALLAS + 2;
      LOG.print("DALLAS: ");
      LOG.println(DALLAS);        
  }
  
  //GPIO 4-5 SLOT DEFINITIONS
  
  if(CAPACITIVE){
      CAP0 = NEXTSLOT; 
      CAP1 = NEXTSLOT + 2;
      THRE = NEXTSLOT + 4;
      NEXTSLOT = THRE + 2;
      LOG.print("CAP0: ");
      LOG.println(CAP0);   
      LOG.print("CAP1: ");
      LOG.println(CAP1);             
      LOG.print("THRE: ");
      LOG.println(THRE);                   
  }
  
  if(RELAY){
      RELAY0 = NEXTSLOT;
      RELAY1 = NEXTSLOT + 1;
      NEXTSLOT = RELAY1 + 1;
      LOG.print("RELAY0: ");
      LOG.println(RELAY0);   
      LOG.print("RELAY1: ");
      LOG.println(RELAY1);         
  }
  
  if(BMP180){
      PRESSURE0 = NEXTSLOT;  
      BMP180TEMP = NEXTSLOT + 2;
      NEXTSLOT = BMP180TEMP + 2;
      LOG.print("PRESSURE0: ");
      LOG.println(PRESSURE0);   
      LOG.print("BMP180TEMP: ");
      LOG.println(BMP180TEMP);       
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
          if(millis()%300==0) LOG.println("Pin\tSlotIn\tOut\tPWM\tcycles\tReturn");
          LOG.print(pin);
          LOG.print("\t");        
          LOG.print(InPin[pin]);
          LOG.print("\t");        
          LOG.print(mInput(slot));
          LOG.print("\t");        
          LOG.print(mOutput(slot));
          LOG.print("\t");
          LOG.print(mOutput(slot+1));
          LOG.print("\t");
          LOG.print(cycles); 
          LOG.print("\t");
          LOG.print(thresold_value); 
          LOG.print("\t");          
          LOG.print(abs(millis()-time)); 
          LOG.print("\t");          

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
                config.cap_thresold = cycles + 4;
                EEPROM.write(4, cycles + 4);
                EEPROM.commit();
            if(DEBUG_CAPSENSE) { 
                LOG.print("Autocalibrate: ");
                LOG.println(config.cap_thresold);
            }
            return config.cap_thresold;
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


