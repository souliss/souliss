/*
 * IRremote
 * Version 0.11 August, 2009
 * Copyright 2009 Ken Shirriff
 * For details, see http://arcfn.com/2009/08/multi-protocol-infrared-remote-library.html
 *
 * Interrupt code based on NECIRrcv by Joe Knapp
 * http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1210243556
 * Also influenced by http://zovirl.com/2008/11/12/building-a-universal-remote-with-an-arduino/
 
	gen 2012 version by shine@angelic.it
	lite version: send only, just a fraction of RAM used
	RC6 64-bit hack
	SendPanasonic
	SendSamsung
 
 */

#include "IRremote_light.h"
#include "IRremoteInt_light.h"

void IRsend::sendNEC(unsigned long data, int nbits)
{
  enableIROut(38);
  mark(NEC_HDR_MARK);
  space(NEC_HDR_SPACE);
  for (int i = 0; i < nbits; i++) {
    if (data & TOPBIT) {
      mark(NEC_BIT_MARK);
      space(NEC_ONE_SPACE);
    } 
    else {
      mark(NEC_BIT_MARK);
      space(NEC_ZERO_SPACE);
    }
    data <<= 1;
  }
  mark(NEC_BIT_MARK);
  space(0);
}

void IRsend::sendSony(unsigned long data, int nbits) {
  enableIROut(40);
  mark(SONY_HDR_MARK);
  space(SONY_HDR_SPACE);
  data = data << (32 - nbits);
  for (int i = 0; i < nbits; i++) {
    if (data & TOPBIT) {
      mark(SONY_ONE_MARK);
      space(SONY_HDR_SPACE);
    } 
    else {
      mark(SONY_ZERO_MARK);
      space(SONY_HDR_SPACE);
    }
    data <<= 1;
  }
}

void IRsend::sendSamsung(unsigned long address, unsigned long data){
	enableIROut(38);
	
	mark(SAMSUNG_GAP_MARK);
	space(SAMSUNG_HDR_SPACE);
	//First 4 byte
	for (int i=0; i < 32; i++) {
		mark(SAMSUNG_BIT_MARK);
		if (address & 0x80000000) {
			space(SAMSUNG_ONE_SPACE);
		} else {
			space(SAMSUNG_ZERO_SPACE);		
		}
		address <<= 1;
	}
	//LAST 3 BYTES
	for (int i=0; i < 24; i++) {
		mark(SAMSUNG_BIT_MARK);
		if (data & 0x800000) {
			space(SAMSUNG_ONE_SPACE);
		} else {
			space(SAMSUNG_ZERO_SPACE);		
		}
		data <<= 1;
	}
	// Lead OUT
	mark(SAMSUNG_BIT_MARK);
	space(SAMSUNG_ZERO_SPACE);
}

void IRsend::sendPanasonic(unsigned long address, unsigned long data) {
	enableIROut(38);
	mark(PANASONIC_HDR_MARK);
	space(PANASONIC_HDR_SPACE);
	for (int i=0; i < 32; i++) {
		mark(PANASONIC_BIT_MARK);
		if (address & 0x80000000) {
			space(PANASONIC_ONE_SPACE);
		} else {
			space(PANASONIC_ZERO_SPACE);
		}
		address <<= 1;
	}
	for (int i=0; i < 16; i++) {
		mark(PANASONIC_BIT_MARK);
		if (data & 0x8000) {
			space(PANASONIC_ONE_SPACE);
		} else {
			space(PANASONIC_ZERO_SPACE);
		}
		data <<= 1;
	}
	mark(PANASONIC_BIT_MARK);
	space(30000);
	space(30000);
	space(14000);
}

void IRsend::sendRaw(unsigned int buf[], int len, int hz)
{
  enableIROut(hz);
  for (int i = 0; i < len; i++) {
    if (i & 1) {
      space(buf[i]);
    } 
    else {
      mark(buf[i]);
    }
  }
  space(0); // Just to be sure
}

// Note: first bit must be a one (start bit)
void IRsend::sendRC5(unsigned long data, int nbits)
{
  enableIROut(36);
  data = data << (32 - nbits);
  mark(RC5_T1); // First start bit
  space(RC5_T1); // Second start bit
  mark(RC5_T1); // Second start bit
  for (int i = 0; i < nbits; i++) {
    if (data & TOPBIT) {
      space(RC5_T1); // 1 is space, then mark
      mark(RC5_T1);
    } 
    else {
      mark(RC5_T1);
      space(RC5_T1);
    }
    data <<= 1;
  }
  space(0); // Turn off at end
}

// Caller needs to take care of flipping the toggle bit
/**
from
http://www.arcfn.com/2010/12/64-bit-rc6-codes-arduino-and-xbox.html
*/
void IRsend::sendRC6(unsigned long long data, int nbits)
{
  enableIROut(36);
  data = data << (64 - nbits);
  mark(RC6_HDR_MARK);
  space(RC6_HDR_SPACE);
  mark(RC6_T1); // start bit
  space(RC6_T1);
  int t;
  for (int i = 0; i < nbits; i++) {
    if (i == 3) {
      // double-wide trailer bit
      t = 2 * RC6_T1;
    } 
    else {
      t = RC6_T1;
    }
    if (data & 0x8000000000000000LL) {
      mark(t);
      space(t);
    } 
    else {
      space(t);
      mark(t);
    }

    data <<= 1;
  }
  space(0); // Turn off at end
}


void IRsend::mark(int time) {
  // Sends an IR mark for the specified number of microseconds.
  // The mark output is modulated at the PWM frequency.
  TCCR2A |= _BV(COM2B1); // Enable pin 3 PWM output
  delayMicroseconds(time);
}

/* Leave pin off for time (given in microseconds) */
void IRsend::space(int time) {
  // Sends an IR space for the specified number of microseconds.
  // A space is no output, so the PWM output is disabled.
  TCCR2A &= ~(_BV(COM2B1)); // Disable pin 3 PWM output
  delayMicroseconds(time);
}

void IRsend::enableIROut(int khz) {
  // Enables IR output.  The khz value controls the modulation frequency in kilohertz.
  // The IR output will be on pin 3 (OC2B).
  // This routine is designed for 36-40KHz; if you use it for other values, it's up to you
  // to make sure it gives reasonable results.  (Watch out for overflow / underflow / rounding.)
  // TIMER2 is used in phase-correct PWM mode, with OCR2A controlling the frequency and OCR2B
  // controlling the duty cycle.
  // There is no prescaling, so the output frequency is 16MHz / (2 * OCR2A)
  // To turn the output on and off, we leave the PWM running, but connect and disconnect the output pin.
  // A few hours staring at the ATmega documentation and this will all make sense.
  // See my Secrets of Arduino PWM at http://arcfn.com/2009/07/secrets-of-arduino-pwm.html for details.

  
  // Disable the Timer2 Interrupt (which is used for receiving IR)
  TIMSK2 &= ~_BV(TOIE2); //Timer2 Overflow Interrupt
  
  pinMode(3, OUTPUT);
  digitalWrite(3, LOW); // When not sending PWM, we want it low
  
  // COM2A = 00: disconnect OC2A
  // COM2B = 00: disconnect OC2B; to send signal set to 10: OC2B non-inverted
  // WGM2 = 101: phase-correct PWM with OCRA as top
  // CS2 = 000: no prescaling
  TCCR2A = _BV(WGM20);
  TCCR2B = _BV(WGM22) | _BV(CS20);

  // The top value for the timer.  The modulation frequency will be SYSCLOCK / 2 / OCR2A.
  OCR2A = SYSCLOCK / 2 / khz / 1000;
  OCR2B = OCR2A / 3; // 33% duty cycle
}