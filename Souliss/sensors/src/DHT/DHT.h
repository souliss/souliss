/*
	Souliss

    Modified by Dario Di Maio to match Souliss requirements	

*/

/* DHT library 

MIT license
written by Adafruit Industries

*/

#ifndef ssDHT_H
#define ssDHT_H
#include "Arduino.h"

// how many timing transitions we need to keep track of. 2 * number bits + extra
#define MAXTIMINGS 85

#define DHT11 11
#define DHT22 22
#define DHT21 21

class ssDHT {
 private:
  uint8_t data[6];
  uint8_t _pin, _type, _count;
  boolean read(void);
  unsigned long _lastreadtime;
  boolean firstreading;

 public:
  ssDHT(uint8_t pin, uint8_t type, uint8_t count=3);
  void begin(void);
  float readTemperature();
  float readHumidity(void);

};
#endif
