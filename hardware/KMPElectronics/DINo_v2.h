/**************************************************************************
	Souliss Support for DINo
    Copyright (C) 2012  Veseo

    Arduino compatible boards DINo version 1 from KMTronic and DINo version 2
	from KPMElectronics.
	
***************************************************************************/

/**************************************************************************
	Souliss Home Automation
    Copyright (C) 2012  Veseo

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
	
	Originally developed by Dario Di Maio
	
***************************************************************************/
/*!
    \file 
    \ingroup
*/
#ifndef DINOv2_H
#define DINOv2_H

// KMP Electronics DINo Pinouts
#define IN1				A2
#define IN2				A3
#define IN3				A4
#define IN4				A5

#define RELAY1			4
#define RELAY2			12
#define RELAY3			11
#define RELAY4			7		
#define	STATLED			13

#define	SetInput1()			pinMode(IN1, INPUT)
#define	SetInput2()			pinMode(IN2, INPUT)
#define	SetInput3()			pinMode(IN3, INPUT)
#define	SetInput4()			pinMode(IN4, INPUT)

#define	SetRelay1()			pinMode(RELAY1, OUTPUT)
#define	SetRelay2()			pinMode(RELAY2, OUTPUT)
#define	SetRelay3()			pinMode(RELAY3, OUTPUT)
#define	SetRelay4()			pinMode(RELAY4, OUTPUT)

#define	SetLED()			pinMode(STATLED, OUTPUT)
#define ToggleLED()			digitalWrite(STATLED, !digitalRead(STATLED))
#define	TurnOnLED()			digitalWrite(STATLED, HIGH)
#define	TurnOffLED()		digitalWrite(STATLED, LOW)
#define	StartupLED()		for(uint8_t l=0;l<10;l++){							\
								digitalWrite(STATLED, !digitalRead(STATLED));	\
								delay(500);}									\
								digitalWrite(STATLED, LOW)

// Set the Reset and PWDN pins in DINo version 2
#define	InitDINo()			pinMode(10, OUTPUT);					\
							digitalWrite(10, HIGH); 				\
							pinMode(9, OUTPUT);						\
							digitalWrite(9, LOW);					\
							pinMode(1, OUTPUT);						\
							pinMode(0, INPUT);						\
							delay(1000)	

// Reduce power consumption switching off the Wiznet W5200 if not used
#define	PowerDownEthernet()	digitalWrite(9, HIGH)
							
#endif
