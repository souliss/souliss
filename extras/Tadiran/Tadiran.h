/*
 * Tadiran(gree) AC IR library - for remotes YAA1FB, FAA1FB1, YB1F2, YB1FA, YAG1FB
 * Copyright 2016 dpressle
 *
 * https://github.com/dpressle/MySensors/tree/Development/AcControl/Library
 *
 * Library to generate IR codes for a Tadiran air conditionner.
 *
 * Based on http://arduino.fisch.lu library
 *
 * Depends on IRremote (https://github.com/shirriff/Arduino-IRremote) 
 *
 * Versions
 * Version 1.00 July, 2016
 *
 */

#ifndef Tadiran_h
#define Tadiran_h

#include <Arduino.h>

// Values for STATE
#define STATE_off 0
#define STATE_on 1

// Values for MODE
#define MODE_auto 0
#define MODE_cold 1
#define MODE_dry 2
#define MODE_fan 3
#define MODE_heat 4

// Values for FAN
#define FAN_auto 0
#define FAN_1 1
#define FAN_2 2
#define FAN_3 3

// the length of the buffer
#define TADIRAN_BUFFER_SIZE 139

// Values for the codes
#define CODE_filler 620
#define CODE_high   1600
#define CODE_low    540
#define CODE_first  9000
#define CODE_second 4000
#define CODE_threshold 700
#define CODE_pause 19000

// main class for manipulating a TADIRAN buffer
class Tadiran
{
	public:
		// fields
		unsigned int codes[TADIRAN_BUFFER_SIZE];
		// methods
		// setter
		Tadiran(uint8_t _mode, 
				uint8_t _fan, 
				uint8_t _temperature,
				uint8_t _state);
		void setTemeprature(uint8_t _value);
		void setMode(uint8_t _value);
		void setState(uint8_t _value);
		void setFan(uint8_t _value);
		// getter
		uint8_t getTemeprature();
		uint8_t getMode();
		uint8_t getState();
		uint8_t getFan();
		// debugging
		void debug();
		void print();
	private:
		// fields
		uint8_t mode;
		uint8_t fan;
		uint8_t temperature;
		uint8_t state;
		uint8_t counter;
		// methods
		void setChecksum();
		uint8_t bitReverse(uint8_t _value);
};

#endif;
