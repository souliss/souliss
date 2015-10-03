/*
  Add disclaimer

	This isn't a replacement for the original controllino.h
	
Version History
1.0 - 03.10.2015 - First version based on controllino.h ver. 0.4
*/

#ifndef souliss_controllinomega_h
#define souliss_controllinomega_h

#include "SPI.h"

// For backwards compatibility
#if ARDUINO > 22
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#ifdef CONTROLLINO_MEGA

//Controllino MEGA visual aliases

#define CONTROLLINO_D0 2
#define CONTROLLINO_D1 3
#define CONTROLLINO_D2 4
#define CONTROLLINO_D3 5
#define CONTROLLINO_D4 6
#define CONTROLLINO_D5 7
#define CONTROLLINO_D6 8
#define CONTROLLINO_D7 9
#define CONTROLLINO_D8 10
#define CONTROLLINO_D9 11
#define CONTROLLINO_D10 12
#define CONTROLLINO_D11 13
#define CONTROLLINO_D12 42
#define CONTROLLINO_D13 43
#define CONTROLLINO_D14 44
#define CONTROLLINO_D15 45
#define CONTROLLINO_D16 46
#define CONTROLLINO_D17 47
#define CONTROLLINO_D18 48
#define CONTROLLINO_D19 49

#define CONTROLLINO_A0 54
#define CONTROLLINO_A1 55
#define CONTROLLINO_A2 56
#define CONTROLLINO_A3 57
#define CONTROLLINO_A4 58
#define CONTROLLINO_A5 59
#define CONTROLLINO_A6 60
#define CONTROLLINO_A7 61
#define CONTROLLINO_A8 62
#define CONTROLLINO_A9 63
#define CONTROLLINO_A10 64
#define CONTROLLINO_A11 65
#define CONTROLLINO_A12 66
#define CONTROLLINO_A13 67
#define CONTROLLINO_A14 68
#define CONTROLLINO_A15 69
#define CONTROLLINO_I16 38
#define CONTROLLINO_I17 39
#define CONTROLLINO_I18 40
#define CONTROLLINO_IN0 18
#define CONTROLLINO_IN1 19

#define CONTROLLINO_MINUS 14
#define CONTROLLINO_PLUS 15

#define CONTROLLINO_R0 22
#define CONTROLLINO_R1 23
#define CONTROLLINO_R2 24
#define CONTROLLINO_R3 25
#define CONTROLLINO_R4 26
#define CONTROLLINO_R5 27
#define CONTROLLINO_R6 28
#define CONTROLLINO_R7 29
#define CONTROLLINO_R8 30
#define CONTROLLINO_R9 31
#define CONTROLLINO_R10 32
#define CONTROLLINO_R11 33
#define CONTROLLINO_R12 34
#define CONTROLLINO_R13 35
#define CONTROLLINO_R14 36
#define CONTROLLINO_R15 37

#endif
