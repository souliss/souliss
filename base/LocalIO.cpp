/**************************************************************************
	Souliss
    Copyright (C) 2011  Veseo

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
#include <Arduino.h>

#include "tools/types.h"
#include "GetConfig.h"			// need : ethUsrCfg.h, vNetCfg.h, SoulissCfg.h, MaCacoCfg.h
#include "Souliss.h"

#define	PINRESET			0x0
#define	PINSET				0x1
#define	PINACTIVE			0x2
#define	PINRELEASED			0x3
#define	PIN_2STATE_RESET	0x4
#define	PIN_2STATE_SET		0x5
#define PINUSED				0x6

// Use local defines to redefine the IO methods
#ifndef	LOCAL_IO
#	define	dRead	digitalRead
#	define	dWrite	digitalWrite
#	define	aRead	analogRead
#	define	aWrite	analogWrite
#endif

U8 InPin[MAXINPIN];
U8 OutPin[MAXINPIN];
static unsigned long souliss_time;

/**************************************************************************/
/*!
	Init variables
*/
/**************************************************************************/
void Souliss_Initialize(U8 *memory_map)
{
	for(U8 i=0; i<MAXINPIN; i++)
	{
		InPin[i]  = PINRESET;
	    OutPin[i] = PINRESET;
	}

	// Init MaCaco
	MaCaco_init(memory_map);
}

/**************************************************************************/
/*!
	Reset output in memory map, this shall be used in case of LinkIO method
*/
/**************************************************************************/
void Souliss_ResetOutput(U8 *memory_map, U8 slot)
{
	memory_map[MaCaco_OUT_s + slot] = 0x00;
}

/**************************************************************************/
/*!
	Reset input in memory map, this shall be used in case of RemoteInput
	method
*/
/**************************************************************************/
void Souliss_ResetInput(U8 *memory_map, U8 slot)
{
	memory_map[MaCaco_IN_s + slot] = 0x00;
}

/**************************************************************************/
/*!
	Create a link between input and output on the same memory map

	This method is useful if data shall be transferred from a device to
	another, in that case is not allowed for a receiver node to subscribe
	the inputs of another node.
	A receiver node can subscribe instead the outputs of another node, so
	linking input and output allow a transfer of data from inputs to another
	device via subscription mode.

	This method require also Souliss_ResetOutput.
*/
/**************************************************************************/
void Souliss_LinkIO(U8 *memory_map, U8 input_slot, U8 output_slot, U8 *trigger)
{
	if(memory_map[MaCaco_IN_s + input_slot] != 0x00)
	{
		memory_map[MaCaco_OUT_s + output_slot] = memory_map[MaCaco_IN_s + input_slot];	// Link IO
		memory_map[MaCaco_IN_s + input_slot] = Souliss_RstCmd;							// Reset
		*trigger = Souliss_TRIGGED;														// Trig change
	}
}

/**************************************************************************/
/*!
	Create a link between output and input on the same memory map
*/
/**************************************************************************/
void Souliss_LinkOI(U8 *memory_map, U8 input_slot, U8 output_slot)
{
	if(memory_map[MaCaco_OUT_s + output_slot] != 0x00)
		memory_map[MaCaco_IN_s + input_slot] = memory_map[MaCaco_OUT_s + output_slot];	// Link IO
}

/**************************************************************************/
/*!
	Link an hardware pin to the shared memory map, active on rising edge

	It write directly into the inputs map of the node, these data shall be
	used for logic applications.

	The following is an helper function.
	Souliss_DigIn and Souliss_LowDigIn should be used for positive or falling edge
	inputs
*/
/**************************************************************************/
inline U8 Souliss_DigIn_Helper(U8 pin, U8 pin_value, U8 value, U8 *memory_map, U8 slot, bool filteractive=false)
{
	// If pin is ON, set the flag. If at next cycle the pin will still
	// be ON the requested action will be performed
	if(pin_value && (InPin[pin]==PINRESET))
	{
		InPin[pin] = PINSET;

		// Copy the value in the memory map
		if(!filteractive && memory_map) memory_map[MaCaco_IN_s + slot] = value;
		return value;

	}
	else if(filteractive && pin_value && InPin[pin]==PINSET)
	{
		// Flag that action is executed
		InPin[pin] = PINACTIVE;

		// Copy the value in the memory map
		if(memory_map)	memory_map[MaCaco_IN_s + slot] = value;
		return value;
	}
	else if(filteractive && !pin_value && InPin[pin]==PINACTIVE)
		InPin[pin] = PINRELEASED;
	else if(filteractive && !pin_value && InPin[pin]==PINRELEASED)
		InPin[pin] = PINRESET;
	else if(filteractive && !pin_value && InPin[pin]==PINSET)
		InPin[pin] = PINRESET;
	else if(!filteractive && !pin_value)
		InPin[pin] = PINRESET;

	return MaCaco_NODATACHANGED;
}

U8 Souliss_DigIn(U8 pin, U8 value, U8 *memory_map, U8 slot, bool filteractive=false)
{
	return Souliss_DigIn_Helper(pin, dRead(pin), value, memory_map, slot, filteractive);
}

U8 Souliss_LowDigIn(U8 pin, U8 value, U8 *memory_map, U8 slot, bool filteractive=false)
{
	return Souliss_DigIn_Helper(pin, !dRead(pin), value, memory_map, slot, filteractive);
}

/**************************************************************************/
/*!
	Link an hardware pin to the shared memory map, use with latched two state
	pushbutton.

	The following is an helper function.
	Souliss_DigIn2State and Souliss_LowDigIn2State should be used for positive or
	falling edge inputs
*/
/**************************************************************************/
inline U8 Souliss_DigIn2State_Helper(U8 pin, U8 pin_value, U8 value_state_on, U8 value_state_off, U8 *memory_map, U8 slot)
{
	// If pin is on, set the "value"
	if(pin_value && ((!InPin[pin]) || (InPin[pin] == PIN_2STATE_RESET)))
	{
		if(memory_map)	memory_map[MaCaco_IN_s + slot] = value_state_on;

		InPin[pin] = PIN_2STATE_SET;
		return value_state_on;
	}
	else if(!pin_value && ((!InPin[pin]) || (InPin[pin] == PIN_2STATE_SET)))
	{
		if(memory_map)	memory_map[MaCaco_IN_s + slot] = value_state_off;

		InPin[pin] = PIN_2STATE_RESET;
		return value_state_off;
	}

	return MaCaco_NODATACHANGED;
}

U8 Souliss_DigIn2State(U8 pin, U8 value_state_on, U8 value_state_off, U8 *memory_map, U8 slot)
{
	return Souliss_DigIn2State_Helper(pin, dRead(pin), value_state_on, value_state_off, memory_map, slot);
}

U8 Souliss_LowDigIn2State(U8 pin, U8 value_state_on, U8 value_state_off, U8 *memory_map, U8 slot)
{
	return Souliss_DigIn2State_Helper(pin, !dRead(pin), value_state_on, value_state_off, memory_map, slot);
}

/**************************************************************************/
/*!
	Use a single analog input connected to two different pushbuttons, use
	different pull-up resistors to define different voltage drops for the
	two pushbuttons.

	If the analog value goes over the top limit or below the bottom one,
	the pushbuttons are pressed, if the analog value stay in the middle
	no action is taken.
*/
/**************************************************************************/
U8 Souliss_AnalogIn2Buttons(U8 pin, U8 value_button1, U8 value_button2, U8 *memory_map, U8 slot)
{
	uint16_t iPinValue = 0;
	bool bState=false;
	bool bMiddle=false;

	iPinValue = aRead(pin);

	if (iPinValue >= AIN2S_TOP)
	{
	  bState=true;
	  bMiddle=false;
	}
	else if (iPinValue <= AIN2S_BOTTOM)
	{
	  bState=false;
	  bMiddle=false;
	}
	else
		bMiddle=true;


	// If pin is on, set the "value"
    if(bState && !InPin[pin] && !bMiddle)
    {
        if(memory_map)	memory_map[MaCaco_IN_s + slot] = value_button1;

		InPin[pin] = PINSET;
        return value_button1;
    }
    else if(!bState && !InPin[pin] && !bMiddle)
    {
        if(memory_map)	memory_map[MaCaco_IN_s + slot] = value_button2;

		InPin[pin] = PINSET;
        return value_button2;
    }
	else if(bMiddle)
		InPin[pin] = PINRESET;

	return MaCaco_NODATACHANGED;
}


/**************************************************************************/
/*!
	Link an hardware pin to the shared memory map, active on rising edge
	Identify two states, press and hold.

	The following is an helper function.
	Souliss_DigInHold and Souliss_LowDigInHold should be used for positive or
	falling edge inputs
*/
/**************************************************************************/
inline U8 Souliss_DigInHold_Helper(U8 pin, U8 pin_value, U8 value, U8 value_hold, U8 *memory_map, U8 slot, U16 holdtime=1500)
{
	// If pin is on, set the "value"
	if(pin_value && (InPin[pin]==PINRESET))
	{
		souliss_time = millis();								// Record time
		InPin[pin] = PINSET;

		return MaCaco_NODATACHANGED;
	}
	else if(pin_value && (abs(millis()-souliss_time) > holdtime) && (InPin[pin]==PINSET))
	{
		InPin[pin] = PINACTIVE;								// Stay there till pushbutton is released

		// Write timer value in memory map
		if(memory_map)	memory_map[MaCaco_IN_s + slot] = value_hold;

		return value_hold;
	}
	else if(!pin_value && (InPin[pin]==PINSET))
	{
		// Write input value in memory map
		if(memory_map)	memory_map[MaCaco_IN_s + slot] = value;

		InPin[pin] = PINRESET;
		return value;
	}
	else if(!pin_value && (InPin[pin]==PINACTIVE))
		InPin[pin] = PINRESET;

	return MaCaco_NODATACHANGED;
}


U8 Souliss_DigInHold(U8 pin, U8 value, U8 value_hold, U8 *memory_map, U8 slot, U16 holdtime=1500)
{
	return Souliss_DigInHold_Helper(pin, dRead(pin), value, value_hold, memory_map, slot, holdtime);
}

U8 Souliss_LowDigInHold(U8 pin, U8 value, U8 value_hold, U8 *memory_map, U8 slot, U16 holdtime=1500)
{
	return Souliss_DigInHold_Helper(pin, !dRead(pin), value, value_hold, memory_map, slot, holdtime);
}

inline U8 Souliss_DigInHold2_Helper(U8 pin, U8 pin_value, U8 value, U8 value_hold, U8 *memory_map, U8 slot1, U8 slot2, U16 holdtime=1500)
{
	// If pin is on, set the "value"
	if(pin_value && (InPin[pin]==PINRESET))
	{
		souliss_time = millis();								// Record time
		InPin[pin] = PINSET;

		return MaCaco_NODATACHANGED;
	}
	else if(pin_value && (abs(millis()-souliss_time) > holdtime) && (InPin[pin]==PINSET))
	{
		InPin[pin] = PINACTIVE;								// Stay there till pushbutton is released

		// Write timer value in memory map
		if(memory_map)	memory_map[MaCaco_IN_s + slot2] = value_hold;

		return value_hold;
	}
	else if(!pin_value && (InPin[pin]==PINSET))
	{
		// Write input value in memory map
		if(memory_map)	memory_map[MaCaco_IN_s + slot1] = value;

		InPin[pin] = PINRESET;
		return value;
	}
	else if(!pin_value && (InPin[pin]==PINACTIVE))
		InPin[pin] = PINRESET;

	return MaCaco_NODATACHANGED;
}

U8 Souliss_DigInHold2(U8 pin, U8 value, U8 value_hold, U8 *memory_map, U8 slot1, U8 slot2, U16 holdtime=1500)
{
	return Souliss_DigInHold2_Helper(pin, dRead(pin), value, value_hold, memory_map, slot1, slot2, holdtime);
}

U8 Souliss_DLowDigInHold2(U8 pin, U8 value, U8 value_hold, U8 *memory_map, U8 slot1, U8 slot2, U16 holdtime=1500)
{
	return Souliss_DigInHold2_Helper(pin, !dRead(pin), value, value_hold, memory_map, slot1, slot2, holdtime);
}

/**************************************************************************/
/*!
	Link an hardware pin to the shared memory map, active on rising edge
	Identify two states, press and hold.

	Repeat the command every holdtime while holding the pushbutton

	The following is an helper function.
	Souliss_DigKeepHold and Souliss_LowDigKeepHold should be used for positive or
	falling edge inputs
*/
/**************************************************************************/
inline U8 Souliss_DigKeepHold_Helper(U8 pin, U8 pin_value, U8 value, U8 value_hold, U8 *memory_map, U8 slot, U16 holdtime=1500)
{
	// If pin is on, set the "value"
	if(pin_value && (InPin[pin]==PINRESET))
	{
		souliss_time = millis();								// Record time
		InPin[pin] = PINSET;

		return MaCaco_NODATACHANGED;
	}
	else if(pin_value && (abs(millis()-souliss_time) > holdtime) && ((InPin[pin]==PINSET) || (InPin[pin]==PINACTIVE)))
	{
		souliss_time = millis();
		InPin[pin] = PINACTIVE;								// Stay there till pushbutton is released

		// Write timer value in memory map
		if(memory_map)	memory_map[MaCaco_IN_s + slot] = value_hold;

		return value_hold;
	}
	else if(!pin_value && (InPin[pin]==PINSET))
	{
		// Write input value in memory map
		if(memory_map)	memory_map[MaCaco_IN_s + slot] = value;

		InPin[pin] = PINRESET;
		return value;
	}
	else if(!pin_value && (InPin[pin]==PINACTIVE))
		InPin[pin] = PINRESET;

	return MaCaco_NODATACHANGED;
}

U8 Souliss_DigKeepHold(U8 pin, U8 value, U8 value_hold, U8 *memory_map, U8 slot, U16 holdtime=1500)
{
	return Souliss_DigKeepHold_Helper(pin, dRead(pin), value, value_hold, memory_map, slot, holdtime);
}

U8 Souliss_LowDigKeepHold(U8 pin, U8 value, U8 value_hold, U8 *memory_map, U8 slot, U16 holdtime=1500)
{
	return Souliss_DigKeepHold_Helper(pin, !dRead(pin), value, value_hold, memory_map, slot, holdtime);
}

/**************************************************************************/
/*!
	Read a single precision floating point and store it into the memory_map
	as half-precision floating point
*/
/**************************************************************************/
void Souliss_ImportAnalog(U8* memory_map, U8 slot, float* analogvalue)
{
	Souliss_HalfPrecisionFloating((memory_map + MaCaco_IN_s + slot), analogvalue);
}

/**************************************************************************/
/*!
	Read an analog input and store it into the memory_map as half-precision
	floating point
*/
/**************************************************************************/
void Souliss_AnalogIn(U8 pin, U8 *memory_map, U8 slot, float scaling, float bias)
{
	float inval = aRead(pin);

	// Scale and add bias
	inval = bias + scaling * inval;

	// Convert from single-precision to half-precision
	Souliss_HalfPrecisionFloating((memory_map + MaCaco_IN_s + slot), &inval);

}

/**************************************************************************/
/*!
	Link the shared memory map to an hardware pin

	It write a digital output pin based on the value of the output into
	memory_map, let a logic act on external devices.
*/
/**************************************************************************/
inline void Souliss_DigOut_Helper(U8 pin, U8 pin_value, U8 value, U8 *memory_map, U8 slot)
{
	// If output is active switch on the pin, else off
	if(memory_map[MaCaco_OUT_s + slot] == value)
		dWrite(pin, pin_value);
	else
		dWrite(pin, !pin_value);
}

void Souliss_DigOut(U8 pin, U8 value, U8 *memory_map, U8 slot)
{
	Souliss_DigOut_Helper(pin, HIGH, value, memory_map, slot);
}

void Souliss_LowDigOut(U8 pin, U8 value, U8 *memory_map, U8 slot)
{
	Souliss_DigOut_Helper(pin, LOW, value, memory_map, slot);
}


/**************************************************************************/
/*!
	Link the shared memory map to an hardware pin

	It write a digital output pin based on the value of the output into
	memory_map, let a logic act on external devices.
*/
/**************************************************************************/
inline void Souliss_PulseDigOut_Helper(U8 pin, U8 pin_value, U8 value, U8 *memory_map, U8 slot)
{
	// If output is active switch on the pin, else off
	if((memory_map[MaCaco_OUT_s + slot] == value) && OutPin[pin] == PINRESET)
	{
		OutPin[pin] = PINSET;
		dWrite(pin, pin_value);
	}
	else if((memory_map[MaCaco_OUT_s + slot] == value) && OutPin[pin] == PINSET)
	{
		dWrite(pin, !pin_value);
	}
	else if (memory_map[MaCaco_OUT_s + slot] != value)
	{
		OutPin[pin] = PINRESET;
		dWrite(pin, !pin_value);
	}
}

void Souliss_PulseDigOut(U8 pin, U8 value, U8 *memory_map, U8 slot)
{
	Souliss_PulseDigOut_Helper(pin, HIGH, value, memory_map, slot);
}

void Souliss_PulseLowDigOut(U8 pin, U8 value, U8 *memory_map, U8 slot)
{
	Souliss_PulseDigOut_Helper(pin, LOW, value, memory_map, slot);
}

/**************************************************************************/
/*!
	Link the shared memory map to an hardware pin

	It write a digital output pin based on the value of the output into
	memory_map, let a logic act on external devices. Match criteria is based
	on bit-wise AND operation.
*/
/**************************************************************************/
inline void Souliss_nDigOut_Helper(U8 pin, U8 pin_value, U8 value, U8 *memory_map, U8 slot)
{
	// If output is active switch on the pin, else off
	if(memory_map[MaCaco_OUT_s + slot] & value)
		dWrite(pin, pin_value);
	else
		dWrite(pin, !pin_value);
}

void Souliss_nDigOut(U8 pin, U8 value, U8 *memory_map, U8 slot)
{
	Souliss_nDigOut_Helper(pin, HIGH, value, memory_map, slot);
}

void Souliss_nLowDigOut(U8 pin, U8 value, U8 *memory_map, U8 slot)
{
	Souliss_nDigOut_Helper(pin, LOW, value, memory_map, slot);
}


/**************************************************************************/
/*!
	Link the shared memory map to an hardware pin, toggle the output value
*/
/**************************************************************************/
void Souliss_DigOutToggle(U8 pin, U8 value, U8 *memory_map, U8 slot)
{
	if(memory_map[MaCaco_OUT_s + slot] == value)
	{
		// If output is active toggle the pin, else off
		if(dRead(pin))
			dWrite(pin, LOW);
		else
			dWrite(pin, HIGH);
	}
	else
		dWrite(pin, LOW);
}

/**************************************************************************/
/*!
	Link the shared memory map to an hardware pin
*/
/**************************************************************************/
void Souliss_DigOutLessThan(U8 pin, U8 value, U8 deadband, U8 *memory_map, U8 slot)
{
	// If output is active switch on the pin, else off
	if(memory_map[MaCaco_OUT_s + slot] < value - deadband)
		dWrite(pin, HIGH);
	else if(memory_map[MaCaco_OUT_s + slot] > value + deadband)
		dWrite(pin, LOW);
}

/**************************************************************************/
/*!
	Link the shared memory map to an hardware pin
*/
/**************************************************************************/
void Souliss_DigOutGreaterThan(U8 pin, U8 value, U8 deadband, U8 *memory_map, U8 slot)
{
	// If output is active switch on the pin, else off
	if(memory_map[MaCaco_OUT_s + slot] > value + deadband)
			dWrite(pin, HIGH);
	else if(memory_map[MaCaco_OUT_s + slot] < value - deadband)
		dWrite(pin, LOW);
}


/**************************************************************************/
/*!
	Link the shared memory map to an hardware pin

	Designed to work with T11Groups
	A Group is simply defined as a set of consecutive T11 slots (from firstSlot to lastSlot)

	It is used to handle lightning in a room having multiple lights with the use of
	one single monostable pushbutton.

	Starting with all lights off, a short press turns ON the first light as a standard T11.
	By holding the pushbutton all the others lights in the group are turned on in sequence.
	step_duration defines how long the button has to be held before turning on the next slot.

	Starting with some of the lights on, a short press turns off the whole group in once
	while a long press increase number of lights ON

	The following is an helper function.
	Souliss_DigInHoldSteps and Souliss_LowDigInHoldSteps should be used for positive or falling edge inputs

*/
/**************************************************************************/
inline U8 Souliss_DigInHoldSteps_Helper(U8 pin, U8 pin_value, U8 *memory_map, U8 firstSlot, U8 lastSlot, U16 step_duration)
{
	// HANDLE BUTTON RELEASE EVENT
	if( pin_value == PINRESET ) // unpressed button
	{
		// handle button release on short press
		if( InPin[pin] == PINSET )
		{
			// button is unpressed and it was a single press.
			InPin[pin] = PINRESET;

			// verify if some of the lights in the group are ON
			U8 bLightsOn = false;
			for(U8 i=firstSlot; i<=lastSlot; i++)
			{
				if(memory_map[MaCaco_OUT_s + i] == Souliss_T1n_OnCoil)
				{
					bLightsOn = true;
				}
			}

			if( bLightsOn )
			{
				// it was a single press with some lights ON
				// set all light to OFF
				for(U8 i=firstSlot; i<=lastSlot; i++)
					memory_map[MaCaco_IN_s + i] = Souliss_T1n_OffCmd;
			}
			else
			{
				// it was a single press with all lights OFF
				// turn on the first light of the group
				memory_map[MaCaco_IN_s + firstSlot] = Souliss_T1n_OnCmd;
			}
			return MaCaco_DATACHANGED;
		}

		// if here it was either a long press or no press at all
		InPin[pin] = PINRESET;
		return MaCaco_NODATACHANGED;
	}

	// HANDLE BUTTON PRESS EVENT
	// if here the button is pressed (pin_value != PINRESET)
	if( InPin[pin] == PINRESET ) // it was unpressed before
	{
		// this is the first cycle detecting the button press: current input=1, previous input=0
		InPin[pin] = PINSET;
		souliss_time = millis();								// Record time

		// no nothing here
		// short presses are handled on button release event
		return MaCaco_NODATACHANGED;
	}
	else if( (InPin[pin]==PINSET) && (abs(millis()-souliss_time) < step_duration) )
	{
		// still short press -> do nothing
		return MaCaco_NODATACHANGED;
	}
	else if( (InPin[pin]==PINSET) && (abs(millis()-souliss_time) > step_duration) )
	{
		// long press -> do nothing now, but remember it with InPin value
		InPin[pin]=PINACTIVE;
		return MaCaco_NODATACHANGED;
	}
	else if( (InPin[pin]==PINACTIVE || InPin[pin]==PINUSED) && (abs(millis()-souliss_time) > step_duration) )
	{
		// this cycle is executed while the button is kept pressed
		// the current input is 1, the previous input was 1 and some time passed from the first press
		U8 powered_lights_count = (U8) ( abs(millis()-souliss_time) / step_duration );

		if( InPin[pin]==PINACTIVE )
		{
			// first time here
			// detect if any light is already ON
			U8 i = firstSlot;
			for(; i<=lastSlot; i++)
			{
				if(memory_map[MaCaco_OUT_s + i] == Souliss_T1n_OffCoil)
					break;
			}
			// here i contains the first OFF light (group offset)
			// store it for following cycles
			OutPin[pin] = i-firstSlot;

			InPin[pin] = PINUSED;
		}

		powered_lights_count += OutPin[pin];

		if ( powered_lights_count > lastSlot - firstSlot + 1 )
			powered_lights_count = lastSlot - firstSlot + 1;

		U8 MaCaco_STATUS = MaCaco_NODATACHANGED;

		for(U8 i=0; i<powered_lights_count; i++)
		{
			if(memory_map[MaCaco_OUT_s + firstSlot + i] == Souliss_T1n_OffCoil)
			{
				// only if the light is not powered, turn it on
				memory_map[MaCaco_IN_s + firstSlot + i] = Souliss_T1n_OnCmd;
				MaCaco_STATUS = MaCaco_DATACHANGED;
			}
		}
		return MaCaco_STATUS;
	}

	return MaCaco_NODATACHANGED;
}

U8 Souliss_DigInHoldSteps(U8 pin, U8 *memory_map, U8 firstSlot, U8 lastSlot, U16 step_duration)
{
	return Souliss_DigInHoldSteps_Helper(pin, dRead(pin), memory_map, firstSlot, lastSlot, step_duration);
}

U8 Souliss_LowDigInHoldSteps(U8 pin, U8 *memory_map, U8 firstSlot, U8 lastSlot, U16 step_duration)
{
	return Souliss_DigInHoldSteps_Helper(pin, !dRead(pin), memory_map, firstSlot, lastSlot, step_duration);
}

/**************************************************************************/
/*!
	This is a special function that can be used to ensure that the output
	command is given just once.

	As per MaCaco data structure the OUTPUT slot will remain at last output
	value, this could be a problem for devices that need one shot commands,
	like IR Led Emitter.

	In the Typical is specified if this function is required
*/
/**************************************************************************/
U8 Souliss_isTrigged(U8 *memory_map, U8 slot)
{
	if(memory_map[MaCaco_AUXIN_s + slot] == Souliss_TRIGGED)
	{
		// Reset the trigger and return that trigger was there
		memory_map[MaCaco_AUXIN_s + slot] = Souliss_NOTTRIGGED;
		return Souliss_TRIGGED;
	}

	return Souliss_NOTTRIGGED;
}

/**************************************************************************/
/*!
	Convert a half-precision floating point from the memory_map in single
	precision floating point
*/
/**************************************************************************/
float Souliss_SinglePrecisionFloating(U8 *input)
{
	uint16_t input16 = C8TO16(input);
	return returnfloat32(&input16);
}

/**************************************************************************/
/*!
	Convert a single precision floating point into an half-precision one
*/
/**************************************************************************/
void Souliss_HalfPrecisionFloating(U8 *output, float *input)
{
	uint16_t output16;

	float16(&output16, input);

	*(output)   = C16TO8L(output16);
	*(output+1) = C16TO8H(output16);
}
