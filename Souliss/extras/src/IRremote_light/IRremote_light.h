/*
 * IRremote, lite version
 * Version 0.2 July, 2012
 * Copyright 2009 Ken Shirriff
 * For details, see http://arcfn.com/2009/08/multi-protocol-infrared-remote-library.htm http://arcfn.com
 *
 * Interrupt code based on NECIRrcv by Joe Knapp
 * http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1210243556
 * Also influenced by http://zovirl.com/2008/11/12/building-a-universal-remote-with-an-arduino/
 
	lite version by shine@angelic.it
	send IR codes only, just a fraction of RAM used
	RC6 64-bit hack
	SendPanasonic
	SendSamsung
 */

#ifndef IRremote_light_h
#define IRremote_light_h

// Some useful constants

#define USECPERTICK 50  // microseconds per clock interrupt tick
#define VIRTUAL

class IRsend
{
public:
  IRsend() {}
  void sendNEC(unsigned long data, int nbits);
  void sendSony(unsigned long data, int nbits);
  void sendPanasonic( long unsigned data, long unsigned nbits);
  void sendSamsung(long unsigned  address, long unsigned  data);
  void sendRaw(unsigned int buf[], int len, int hz);
  void sendRC5(unsigned long data, int nbits);
  void sendRC6(unsigned long long data, int nbits);
  void enableIROut(int khz);
  VIRTUAL void mark(int usec);
  VIRTUAL void space(int usec);
};

#endif
