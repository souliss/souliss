/**************************************************************************
	Souliss Support for Arduino MEGA 2560
    Copyright (C) 2015  Gabriele Ribichini
	
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
#ifndef MEGA_2560_H
#define MEGA_2560_H

#define STATLED         13


#define SetLED()            pinMode(STATLED, OUTPUT)

#define ToggleLED()         digitalWrite(STATLED, !digitalRead(STATLED))
#define TurnOnLED()         digitalWrite(STATLED, HIGH)
#define TurnOffLED()        digitalWrite(STATLED, LOW)

#define InitMEGA()          SetLED();

#endif