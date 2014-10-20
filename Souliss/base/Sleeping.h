/**************************************************************************
	Souliss
    Copyright (C) 2014  Veseo

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
	
	Copyright (C) 2006 MacSimski 2006-12-30
	Copyright (C) 2007 D. Cuartielles 2007-07-08 - Mexico DF	
	
	Included in Souliss by Dario Di Maio
	
***************************************************************************/
/*!
    \file 
    \ingroup
*/
#ifndef SLEEPING_H
#define SLEEPING_H

#include <avr/sleep.h>
#include <avr/power.h>

// Define the input pin and the relevant interrupt vector
#define wakePin 	2
#define	wakePinINT	0

// Set the interrupt pin as input
#define sleepInit()	pinMode(wakePin, INPUT)

bool backfromSleep = false;

void wakeUpNow()        // here the interrupt is handled after wakeup
{
	backfromSleep = true;
}
 
bool wasSleeping()
{
	return backfromSleep;
} 
 
void sleepNow()         // here we put the arduino to sleep
{
	backfromSleep = false;

    /* In the avr/sleep.h file, the call names of these sleep modes are to be found:
     *
     * The 5 different modes are:
     *     SLEEP_MODE_IDLE         -the least power savings
     *     SLEEP_MODE_ADC
     *     SLEEP_MODE_PWR_SAVE
     *     SLEEP_MODE_STANDBY
     *     SLEEP_MODE_PWR_DOWN     -the most power savings
     *
     */  
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);   // sleep mode is set here
 
    sleep_enable();          // enables the sleep bit in the mcucr register
                             // so sleep is possible. just a safety pin
	
	// enable the interrupt to wake the device
	attachInterrupt(wakePinINT, wakeUpNow, HIGH); 
 
    sleep_mode();            // here the device is actually put to sleep!!

	// the device will start back from this point once the interrupt on pin 2
	// has been fired, so disable sleep and go in normal mode
    sleep_disable();         
    detachInterrupt(wakePinINT);      
}

#endif