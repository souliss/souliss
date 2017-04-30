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

#if(SLEEP_ENABLE)

#include <avr/sleep.h>
#include <avr/power.h>

// Define the input pin and the relevant interrupt vector


/**********************************************************************

Match between interrupts and associated pins

	Board			int.0	int.1	int.2	int.3	int.4	int.5
 
	ATmega328P		2		3	 	 	 	 
	ATmega2560		2		3		21		20		19		18
	ATmega32U4		3		2		0		1		7	

***********************************************************************/

#ifndef	SLEEPING_INSKETCH
#	define 	wakePin 		2
#	define	wakePinINT		0
#	define	wakeupTime		0x00FF			// The total sleep time is wakeupTime*8 seconds
#	define	wakeupCycles	5				// Number of cycle asleep
#endif

#define	SLEEPMODE_INPUT		1
#define	SLEEPMODE_TIMER		2
#define	SLEEPMODE_COMBO		3

#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__)

// Clear the reset flag, set the prescaler at 8 seconds
#define	set_sleep_timer()				(MCUSR &= ~(1<<WDRF));					\
										(WDTCSR |= (1<<WDCE) | (1<<WDE));		\
										(WDTCSR = (1<<WDP0 | 1<<WDP3))
  
// Enable and disable the WD interrupt (note no reset)
#define	sleep_timer_enable()			(WDTCSR |= _BV(WDIE))
#define	sleep_timer_disable()			(WDTCSR &= ~(_BV(WDIE)))

#else
	#error "The selected microcontrollers isn't actually supported for sleep"
#endif

bool backfromSleep = false;
uint8_t sleepmode = 0, wakeupscycles = 0;
uint16_t sleepcounter = 0;

void wakeUpNow()        // here the interrupt is handled after wakeup
{
	backfromSleep = true;
	wakeupscycles = wakeupCycles;
}

/**************************************************************************/
/*!
	This put the microcontroller and the vNet radio in sleep mode, use this
	code as follow:

	setup()
	{
		// Set the wakeup action, could be
		//
		//	SLEEPMODE_INPUT	- This wakeup with a change on the input state
		//	SLEEPMODE_TIMER	- This wakeup at period time interval
		//	SLEEPMODE_COMBO	- This combine both the previous modes
		
		sleepInit(SLEEPMODE_INPUT);
	}
	
	loop()
	{
		
		if(wasSleeping())
		{
			// Just wakeup, do something before go back in sleep
			...
		}
		
		// Sleep manually other devices (sensors, radio or whatever)
		...
		
		// Sleep the microcontroller
		sleepNow();
	}

*/	
/**************************************************************************/	
void sleepNow()         // here we put the arduino to sleep
{
	backfromSleep = false;

	// powerdown
	vNet_RadioSleep();
	
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
 	
	// set the interrupt to wake the device
	if(sleepmode & SLEEPMODE_INPUT)	attachInterrupt(wakePinINT, wakeUpNow, CHANGE); 
	if(sleepmode & SLEEPMODE_TIMER)	sleep_timer_enable();

	cli();					// we wont yet sleep
    sleep_enable();         // enables the sleep bit in the mcu register
                            // so sleep is possible. just a safety pin
	
#	if(SLEEP_BODDISALBE)	
    sleep_bod_disable();	// disabling BOD will reduce power consumption
#	endif
    
	sei();
    sleep_cpu();			// here the cpu is in sleep
	
	// the device will start back from this point once the interrupt on pin 2
	// has been fired, so disable sleep and go in normal mode
	sleep_disable();

	if(sleepmode & SLEEPMODE_INPUT)	detachInterrupt(wakePinINT);    	
	if(sleepmode & SLEEPMODE_TIMER)	sleep_timer_disable();
	sei();
	
#	if(SLEEP_WAKEUPDELAY)
	delay(10);
#	endif	

	// powerup the radio
	vNet_RadioWakeUp();

#	if(SLEEP_WAKEUPDELAY)	
	delay(10);
#	endif
}

ISR(WDT_vect)
{
	// if the timer is expired wakeup
	if(!sleepcounter)	
	{
		backfromSleep = true;
		sleepcounter  = wakeupTime;
		wakeupscycles = wakeupCycles;
	}
	else 
	{
		// is not yet time to wakeup
		sleepcounter--;
		sleepNow();
	}
}


bool wasSleeping()
{
	return backfromSleep;
} 

bool isTimeToSleep()
{
	if(wakeupscycles)
	{
		wakeupscycles--;
		return 0;
	}	
	else
		return 1;
}

void sleepInit(uint8_t mode=SLEEPMODE_INPUT)
{
	if(mode & SLEEPMODE_INPUT)				// Wakeup on input change
	{
		// Record the sleep mode that has been selected
		sleepmode = SLEEPMODE_INPUT;
		
		pinMode(wakePin, INPUT);
	}
	
	if(mode & SLEEPMODE_TIMER)		// Wakeup every 8 seconds
	{
		// Record the sleep mode that has been selected
		sleepmode = SLEEPMODE_TIMER;
		
		// Setup the watchdog timer at 8 seconds
		set_sleep_timer();
	}
	
	// This is the first run, trigger this flag to execute a first run
	// at first boot
	backfromSleep = true;
}



#endif

#endif