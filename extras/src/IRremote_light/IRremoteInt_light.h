/*
 * IRremote
 * Version 0.1 July, 2009
 * Copyright 2009 Ken Shirriff
 * For details, see http://arcfn.com/2009/08/multi-protocol-infrared-remote-library.html
 *
 * Interrupt code based on NECIRrcv by Joe Knapp
 * http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1210243556
 * Also influenced by http://zovirl.com/2008/11/12/building-a-universal-remote-with-an-arduino/
 */

#ifndef IRremoteint_light_h
#define IRremoteint_light_h

#include <Arduino.h>
//#include "src/IRremote_light/IRremote_light.h"

#define CLKFUDGE 		5      							// fudge factor for clock interrupt overhead
#define CLK 			256      						// max value for clock (timer 2)
#define PRESCALE 		8     							// timer2 clock prescale
#define SYSCLOCK 		16000000 						// main Arduino clock
#define CLKSPERUSEC 	(SYSCLOCK/PRESCALE/1000000)  	// timer clocks per microsecond

#define ERR 			0
#define DECODED 		1

#define BLINKLED 		13

// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

// clock timer reset value
#define INIT_TIMER_COUNT2 (CLK - USECPERTICK*CLKSPERUSEC + CLKFUDGE)
#define RESET_TIMER2 TCNT2 = INIT_TIMER_COUNT2

// pulse parameters in usec
#define NEC_HDR_MARK				9000
#define NEC_HDR_SPACE				4500
#define NEC_BIT_MARK				560
#define NEC_ONE_SPACE				1600
#define NEC_ZERO_SPACE				560
#define NEC_RPT_SPACE				2250

#define SONY_HDR_MARK				2400
#define SONY_HDR_SPACE				600
#define SONY_ONE_MARK				1200
#define SONY_ZERO_MARK				600
#define SONY_RPT_LENGTH 			45000

//Samsung and panasonic
#define PANASONIC_HDR_MARK 			3502
#define PANASONIC_HDR_SPACE 		1750
#define PANASONIC_BIT_MARK 			450
#define PANASONIC_ONE_SPACE 		1244
#define PANASONIC_ZERO_SPACE 		450

#define SAMSUNG_GAP_MARK 			3000 //mark between bursts
#define SAMSUNG_HDR_SPACE 			9000
#define SAMSUNG_BIT_MARK 			550
#define SAMSUNG_ONE_SPACE 			550
#define SAMSUNG_ZERO_SPACE 			1540

#define RC5_T1		889
#define RC5_RPT_LENGTH	46000

#define RC6_HDR_MARK	2666
#define RC6_HDR_SPACE	889
#define RC6_T1		444
#define RC6_RPT_LENGTH	46000

#define TOLERANCE 30  // percent tolerance in measurements
#define LTOL (1.0 - TOLERANCE/100.) 
#define UTOL (1.0 + TOLERANCE/100.) 

#define _GAP 15000 // Minimum map between transmissions
#define GAP_TICKS (_GAP/USECPERTICK)

#define TICKS_LOW(us) (int) (((us)*LTOL/USECPERTICK))
#define TICKS_HIGH(us) (int) (((us)*UTOL/USECPERTICK + 1))


// receiver states
#define STATE_IDLE     2
#define STATE_MARK     3
#define STATE_SPACE    4
#define STATE_STOP     5


// IR detector output is active low
#define MARK  0
#define SPACE 1

#define TOPBIT 0x80000000

#define NEC_BITS 32
#define WMC_BITS 36
#define SONY_BITS 12
#define SAMSUNG_BITS 56
#define MIN_RC5_SAMPLES 11
#define MIN_RC6_SAMPLES 1

#endif

