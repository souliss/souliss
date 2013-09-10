/**************************************************************************
	Souliss
    Copyright (C) 2013  Veseo

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
#include "Souliss.h"
#include "Typicals.h"
#include "frame/MaCaco/MaCaco.h"
#include "frame/vNet/vNet.h"
#include "GetConfig.h"			// need : SoulissCfg.h and hwBoards.h
#include <Arduino.h>

#include "Souliss_MODIO.h"
#if(IOBOARD_MODEL == 0x01)
	#include "MODIO.h"
#elif(IOBOARD_MODEL == 0x02)
	#include "MODIO2.h"
#endif

uint8_t mIO_DigIn=0, mIO_In=0, mIO_Out=0;
static unsigned long mIO_time;

/**************************************************************************
/*!
	Read state for all digital inputs
*/	
/**************************************************************************/
U8 Souliss_MODIO_In()
{
	// Read state for all pins using a full mask
	mIO_DigIn = mIO_digitalRead(0xFF);
}

/**************************************************************************
/*!
	Link an hardware pin to the shared memory map, active on rising edge
	
	It write directly into the inputs map of the node, these data shall be
	used for logic applications.
*/	
/**************************************************************************/
U8 Souliss_MODIO_DigIn(U8 pin, U8 value, U8 *memory_map, U8 slot)
{
	// If pin is ON and isn't yet processed
	if((mIO_DigIn & pin) && !(mIO_In & pin))
	{	
		memory_map[MaCaco_IN_s + slot] = value;
		
		mIO_In |= pin;
		return MaCaco_DATACHANGED;
	}
	else if(!(mIO_DigIn & pin))
	{
		mIO_In &= ~pin;
		return MaCaco_NODATACHANGED;
	}
}

/**************************************************************************
/*!
	Link an hardware pin to the shared memory map, active on falling edge
	
	It write directly into the inputs map of the node, these data shall be
	used for logic applications.
*/	
/**************************************************************************/
U8 Souliss_MODIO_LowDigIn(U8 pin, U8 value, U8 *memory_map, U8 slot)
{
	// If pin is OFF and isn't yet processed
	if(!(mIO_DigIn & pin) && !(mIO_In & pin))
	{	
		memory_map[MaCaco_IN_s + slot] = value;
		
		mIO_In |= pin;
		return MaCaco_DATACHANGED;
	}
	else if((mIO_DigIn & pin))
	{
		mIO_In &= ~pin;
		return MaCaco_NODATACHANGED;
	}
}

/**************************************************************************
/*!
	Link an hardware pin to the shared memory map, use with latched two state
	pushbutton.
*/	
/**************************************************************************/
U8 Souliss_MODIO_DigIn2State(U8 pin, U8 value_state_on, U8 value_state_off, U8 *memory_map, U8 slot)
{
	// If pin is ON and isn't yet processed
	if((mIO_DigIn & pin) && !(mIO_In & pin))
	{	
		memory_map[MaCaco_IN_s + slot] = value_state_on;
		
		mIO_In |= pin;
		return MaCaco_DATACHANGED;
	}
	else if(!(mIO_DigIn & pin) && (mIO_In & pin))
	{
		memory_map[MaCaco_IN_s + slot] = value_state_off;
		
		mIO_In &= ~pin;
		return MaCaco_DATACHANGED;
	}
	
	return MaCaco_NODATACHANGED;
}

/**************************************************************************
/*!
	Link an hardware pin to the shared memory map, active on rising edge
	Identify two states, press and hold.
*/	
/**************************************************************************/
U8 Souliss_MODIO_DigInHold(U8 pin, U8 value_state1, U8 value_state2, U8 *memory_map, U8 slot)
{
	// If pin is ON and isn't yet processed
	if((mIO_DigIn & pin) && !(mIO_In & pin))
	{
		// Write input value in memory map
		memory_map[MaCaco_IN_s + slot] = value_state1;
		mIO_time = millis();								// Record time
		
		mIO_In |= pin;
		return MaCaco_DATACHANGED;
	}
	else if((mIO_DigIn & pin) && (abs(millis()-mIO_time) > 1500))
	{
		mIO_time = millis();								// Record time
		
		// Write timer value in memory map
		memory_map[MaCaco_IN_s + slot] = value_state2;	
	}
	else if(!(mIO_DigIn & pin))
	{
		mIO_In &= ~pin;
		return MaCaco_NODATACHANGED;
	}
}


/**************************************************************************
/*!
	Link an hardware pin to the shared memory map, active on falling edge
	Identify two states, press and hold.
*/	
/**************************************************************************/
U8 Souliss_MODIO_LowDigInHold(U8 pin, U8 value_state1, U8 value_state2, U8 *memory_map, U8 slot)
{
	// If pin is OFF and isn't yet processed
	if(!(mIO_DigIn & pin) && !(mIO_In & pin))
	{
		// Write input value in memory map
		memory_map[MaCaco_IN_s + slot] = value_state1;
		mIO_time = millis();								// Record time
		
		mIO_In |= pin;
		return MaCaco_DATACHANGED;
	}
	else if(!(mIO_DigIn & pin) && abs(millis()-mIO_time) > 1500)
	{
		mIO_time = millis();								// Record time
		
		// Write timer value in memory map
		memory_map[MaCaco_IN_s + slot] = value_state2;
	}
	else if((mIO_DigIn & pin))
	{
		mIO_In &= ~pin;
		return MaCaco_NODATACHANGED;
	}
}

/**************************************************************************
/*!
	Set an output on a MODIO device based on the value into the relevant
	memory_map and slot, it need a link to an hardware output or relay.
*/	
/**************************************************************************/
void Souliss_MODIO_SetOut(U8 pin, U8 value, U8 *memory_map, U8 slot)
{
	// If output is active switch on the pin, else off
	if(memory_map[MaCaco_OUT_s + slot] == value)
		mIO_Out |=  pin;
	else
		mIO_Out &= ~pin;
}

/**************************************************************************
/*!
	Set an output on a MODIO device based on the value into the relevant
	memory_map and slot, it need a link to an hardware output or relay.

	Match criteria is based on bit-wise AND operation.
*/	
/**************************************************************************/
void Souliss_MODIO_nSetOut(U8 pin, U8 value, U8 *memory_map, U8 slot)
{
	// If output is active switch on the pin, else off
	if(memory_map[MaCaco_OUT_s + slot] & value)
		mIO_Out |=  pin;
	else
		mIO_Out &= ~pin;
}

/**************************************************************************
/*!
	Toogle an output on a MODIO device based on the value into the relevant
	memory_map and slot, it need a link to an hardware output or relay.	
*/	
/**************************************************************************/
void Souliss_MODIO_SetOutToggle(U8 pin, U8 value, U8 *memory_map, U8 slot)
{
	if(memory_map[MaCaco_OUT_s + slot] == value)
	{
		// If output is active toggle the pin, else off
		if(mIO_Out &  pin)
			mIO_Out &= ~pin;
		else
			mIO_Out |=  pin;
	}
	else
		mIO_Out &= ~pin;
}

/**************************************************************************
/*!
	Set an output on a MODIO device based on the value into the relevant
	memory_map and slot, it need a link to an hardware output or relay.

	Match criteria is a less than.	
*/	
/**************************************************************************/
void Souliss_MODIO_SetOutLessThan(U8 pin, U8 value, U8 deadband, U8 *memory_map, U8 slot)
{
	// If output is active switch on the pin, else off
	if(memory_map[MaCaco_OUT_s + slot] < value - deadband)
		mIO_Out |=  pin;
	else if(memory_map[MaCaco_OUT_s + slot] > value + deadband)
		mIO_Out &= ~pin;
}

/**************************************************************************
/*!
	Set an output on a MODIO device based on the value into the relevant
	memory_map and slot, it need a link to an hardware output or relay.

	Match criteria is a greater than.		
*/	
/**************************************************************************/
void Souliss_MODIO_SetOutGreaterThan(U8 pin, U8 value, U8 deadband, U8 *memory_map, U8 slot)
{
	// If output is active switch on the pin, else off
	if(memory_map[MaCaco_OUT_s + slot] > value + deadband)
		mIO_Out |=  pin;
	else if(memory_map[MaCaco_OUT_s + slot] < value - deadband)
		mIO_Out &= ~pin;
}

/**************************************************************************
/*!
	Force digital outputs based on values from previous "Set" methods,
	valid only MOD-IO2
*/	
/**************************************************************************/
#if(IOBOARD_MODEL == 0x02)			// MOD-IO2
void Souliss_MODIO_DigOut()
{
	mIO_digitalWrite(mIO_Out);
}
#endif
/**************************************************************************
/*!
	Force relays based on values from previous "Set" methods	
*/	
/**************************************************************************/
void Souliss_MODIO_RelayOut()
{
	mIO_Relay(mIO_Out);
}
