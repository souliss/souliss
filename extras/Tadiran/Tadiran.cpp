/*
 * Tadiran(gree) AC IR library - for remotes YAA1FB, FAA1FB1, YB1F2, YB1FA, YAG1FB
 * Version 1.00 July, 2016
 * Copyright 2016 dpressle
 *
 * Library to generate IR codes for a Tadiran air conditionner.
 *
 * Based on http://arduino.fisch.lu library
 */

#include "Tadiran.h"

/**
 * set the given temperature and updates the codes
 * @param	_value	the new value [16..30]
 */
void Tadiran::setTemeprature(uint8_t _value)
{
	temperature=_value;

	if (temperature & 8) codes[25]=CODE_high;
	else codes[25]=CODE_low;
	if (temperature & 4) codes[23]=CODE_high;
	else codes[23]=CODE_low;
	if (temperature & 2) codes[21]=CODE_high;
	else codes[21]=CODE_low;
	if (temperature & 1) codes[19]=CODE_high;
	else codes[19]=CODE_low;

	setChecksum();
}

/**
 * set the given mode and updates the codes
 * @param	_value	the new mode {MODE_auto,MODE_cold,MODE_dry,MODE_fan,MODE_heat}
 */
void Tadiran::setMode(uint8_t _value)
{
	mode=_value;

	if (mode & 4) codes[7]=CODE_high;
	else codes[7]=CODE_low;
	if (mode & 2) codes[5]=CODE_high;
	else codes[5]=CODE_low;
	if (mode & 1) codes[3]=CODE_high;
	else codes[3]=CODE_low;

	setChecksum();
}

/**
 * set the given fan and updates the codes
 * @param	_value	the new fan speed {FAN_auto, FAN_1, FAN_2, FAN_3}
 */
void Tadiran::setFan(uint8_t _value)
{
	fan=_value;
	
	if (fan & 2) codes[13]=CODE_high;
	else codes[13]=CODE_low;
	if (fan & 1) codes[11]=CODE_high;
	else codes[11]=CODE_low;
	
	setChecksum();
}

/**
 * set the given state and updates the codes
 * @param	_value	the new state {STATE_on,STATE_off}
 */
void Tadiran::setState(uint8_t _value)
{
	state=_value;
	
	if (state & 1) codes[9] = CODE_high;
	else codes[9] = CODE_low;
	
	setChecksum();
}

uint8_t Tadiran::getTemeprature()
{
	return temperature;
}

uint8_t Tadiran::getMode()
{
	return mode;
}

uint8_t Tadiran::getState()
{
	return state;
}

uint8_t Tadiran::getFan()
{
	return fan;
}

/**
 * calculates the checksum and updates the codes
 */
void Tadiran::setChecksum()
{
	// generate the checksum
	uint8_t crc = 0;
	
	//for(uint8_t b=0;b<1;b++)
	//{
		uint8_t block = 0;
		for(uint8_t i=3;i<=17;i=i+2)
		{
			if(codes[i] > CODE_threshold) block++;
			if (i <= 15) block <<= 1;
		}

		uint8_t b1 = block;
		//Serial.print(b1, DEC);
		b1 = bitReverse(b1);
		//Serial.println(b1, DEC);
		b1 = b1 & 0x0F;
		//Serial.println(b1, DEC);
		//Serial.println("");
		block = 0;
		for(uint8_t i=19;i<=33;i=i+2)
		{
			if(codes[i] > CODE_threshold) block++;
			if (i <= 31) block <<= 1;
		}

		uint8_t b2 = block;
		//Serial.println(b2, BIN);
		b2 = bitReverse(b2);
		//Serial.println(b2, BIN);
		b2 = b2 & 0x0F;
		//Serial.print(b2, DEC);
		//Serial.println("");
		/*
		checksum function:
		((byte(0) & 0x0F) + (byte(1) & 0x0F) + 
		(byte(2) & 0x0F) + (byte(3) & 0x0F) + 
		((byte(5) & 0xF0) >> 4 ) + ((byte(6) & 0xF0) >> 4 ) + 
		((byte(7) & 0xF0) >> 4 ) + 10) & 0xF0
		2 is for byte 2 with light on
		*/
		uint8_t bitSum = b1 + b2 + 2 + 10;
		//Serial.print(bitSum, DEC);
		//Serial.println("");
		crc = bitSum & 0xF;
		//Serial.print(crc, DEC);
		//Serial.println("");
		//crc = bitReverse(crc);
		//Serial.print(crc);
		//Serial.println("");
	//}

	// if (crc & 128) codes[123]=CODE_high;
	// else codes[123]=CODE_low;
	// if (crc & 64) codes[125]=CODE_high;
	// else codes[125]=CODE_low;
	// if (crc & 32) codes[127]=CODE_high;
	// else codes[127]=CODE_low;
	// if (crc & 16) codes[129]=CODE_high;
	// else codes[129]=CODE_low;
	if (crc & 1) codes[131]=CODE_high;
	else codes[131]=CODE_low;
	if (crc & 2) codes[133]=CODE_high;
	else codes[133]=CODE_low;
	if (crc & 4) codes[135]=CODE_high;
	else codes[135]=CODE_low;
	if (crc & 8) codes[137]=CODE_high;
	else codes[137]=CODE_low;
}

/**
**/
uint8_t Tadiran::bitReverse(uint8_t _value) 
{ 
   _value = ((_value >> 1) & 0x55) | ((_value << 1) & 0xaa); 
   _value = ((_value >> 2) & 0x33) | ((_value << 2) & 0xcc); 
   _value = ((_value >> 4) & 0x0f) | ((_value << 4) & 0xf0); 
   return _value;    
}

/**
 * initialiases a new code object
 */
Tadiran::Tadiran(uint8_t _mode, 
				 uint8_t _fan, 
				 uint8_t _temperature, 
				 uint8_t _state) {
	// init the codes with the fillers and default values
	for(uint8_t i=0;i<TADIRAN_BUFFER_SIZE;i++)
	{
		if (i % 2 == 0)
			codes[i]=CODE_filler;
		else 
			codes[i]=CODE_low;
	}
	// set the two first ones
	codes[0] = CODE_first;
	codes[1] = CODE_second;
	// set the "always high" fields
	codes[45] = CODE_high;//lights is allways on
	codes[59] = CODE_high;//allways 0x5 on bits 4..7 in BYTE 3
	codes[63] = CODE_high;//allways 0x5 on bits 4..7 in BYTE 3
	codes[69] = CODE_high;//allways 0x2 on bits 0..2 in BYTE 4
	//for YAG1FB dont set to high
	codes[101] = CODE_high;//allways 0x2 on bits 4..7 in BYTE 6
	
	// set the pause 
	codes[73] = CODE_pause;
	//allways end with code filler
	codes[139] = CODE_filler;

	// set fields
	setMode(_mode);
	setFan(_fan);
	setTemeprature(_temperature);
	setState(_state);
}

/**
 * gives me whatever debug output I needed dureing development
 */
void Tadiran::debug()
{
	setChecksum();
	
	for(uint8_t i=3;i<TADIRAN_BUFFER_SIZE;i++)
	{
		if (i % 2 == 1)
		{
			/**/
			// Serial.print(i,DEC);
			// Serial.print(" = ");
			// Serial.print(codes[i],DEC);
			// Serial.println("");
			
			if(codes[i]>CODE_threshold)
				Serial.print(1);
			else
				Serial.print(0);
			
			// Serial.print(",");
		}
	}
	Serial.println("");
	for(uint8_t i=0;i<TADIRAN_BUFFER_SIZE;i++)
	{
		//if (i % 2 == 1)
		//{
			/**/
			// Serial.print(i,DEC);
			// Serial.print(" = ");
			Serial.print(codes[i],DEC);

			
			
			Serial.print(",");
		//}
	}
}	

/**
 * prints the actul state of the object on the serial line
 */
void Tadiran::print()
{
	Serial.print("State = ");
	if (state==STATE_on) Serial.println("on");
	else Serial.println("off");
	
	Serial.print("Mode = ");
	if (mode==MODE_auto) Serial.println("auto");
	else if (mode==MODE_dry) Serial.println("dry");
	else if (mode==MODE_cold) Serial.println("cold");
	else if (mode==MODE_fan) Serial.println("fan");
	else Serial.println("heat");

	Serial.print("Temperature = ");
	Serial.println(temperature,DEC);
	
	Serial.print("Fan = ");
	Serial.println(fan,DEC);
	if (mode==FAN_auto) Serial.println("auto");
	else if (mode==FAN_1) Serial.println("1");
	else if (mode==FAN_2) Serial.println("2");
	else if (mode==FAN_3) Serial.println("3");
	else Serial.println("N/A");
	
}