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

#define OUT1			4
#define OUT2			12
#define OUT3			11
#define OUT4			7		
#define	STATLED			13

#define	RS485ENABLE		3

#define	SetInput1()			pinMode(IN1, INPUT)
#define	SetInput2()			pinMode(IN2, INPUT)
#define	SetInput3()			pinMode(IN3, INPUT)
#define	SetInput4()			pinMode(IN4, INPUT)

#define	SetRelay1()			pinMode(OUT1, OUTPUT)
#define	SetRelay2()			pinMode(OUT2, OUTPUT)
#define	SetRelay3()			pinMode(OUT3, OUTPUT)
#define	SetRelay4()			pinMode(OUT4, OUTPUT)
#define	SetLED()			pinMode(STATLED, OUTPUT)

#define	ReadInput1(slot)	ssDigIn(IN1, Souliss_T1n_ToogleCmd, slot)	
#define	ReadInput2(slot)	ssDigIn(IN2, Souliss_T1n_ToogleCmd, slot)	
#define	ReadInput3(slot)	ssDigIn(IN3, Souliss_T1n_ToogleCmd, slot)	
#define	ReadInput4(slot)	ssDigIn(IN4, Souliss_T1n_ToogleCmd, slot)	

#define	CntRelay1(slot)		ssDigOut(OUT1, Souliss_T1n_Coil, slot)
#define	CntRelay2(slot)		ssDigOut(OUT2, Souliss_T1n_Coil, slot)
#define	CntRelay3(slot)		ssDigOut(OUT3, Souliss_T1n_Coil, slot)
#define	CntRelay4(slot)		ssDigOut(OUT4, Souliss_T1n_Coil, slot)

#define ToogleLED()			digitalWrite(STATLED, !digitalRead(STATLED))
#define	TurnOnLED()			digitalWrite(STATLED, HIGH)
#define	TurnOffLED()		digitalWrite(STATLED, LOW)

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
