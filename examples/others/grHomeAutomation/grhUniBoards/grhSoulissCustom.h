#ifndef SOULISS_CUSTOM_H
#define SOULISS_CUSTOM_H

/**************************************************************************
/*!
	Link an hardware pin to the shared memory map, active on rising edge
	Identify multiple states, press and hold (with increasing value).
	The memory_map associated value is equal to count of STEP_DURATION elapsed during hold.

	Used to detect how long the input has been held.
*/	
/**************************************************************************/
#define STEP_DURATION 500 // duration of a step in milliseconds
/*
U8 Souliss_DigInHoldDuration(U8 pin, U8 *memory_map, U8 slot)
{
	// If pin is on, set the "value"
	if(digitalRead(pin) && !InPin[pin]) // first press: current input=1, previous input=0
	{
		if(memory_map[MaCaco_AUXIN_s + slot] != 0)
			memory_map[MaCaco_AUXIN_s + slot] = 0; // pressed to turn off
		else
			memory_map[MaCaco_AUXIN_s + slot] = 1; // first press set to 1

		time = millis();								// Record time
		
		InPin[pin] = true;
		memory_map[MaCaco_OUT_s + slot] = MaCaco_DATACHANGED; // data has changed due to pin press
		return MaCaco_DATACHANGED;
	}
	else if(digitalRead(pin) && (abs(millis()-time) > STEP_DURATION))
	{
		// Write elapsed STEP_DURATION count in memory map
		if ( memory_map[MaCaco_AUXIN_s + slot] == (U8) ( abs(millis()-time) / STEP_DURATION + 1 ) )
		{
			return MaCaco_NODATACHANGED;	
		}

		memory_map[MaCaco_AUXIN_s + slot] = (U8) ( abs(millis()-time) / STEP_DURATION + 1 );		
		memory_map[MaCaco_OUT_s + slot] = MaCaco_DATACHANGED;  // data has changed due to pin press
		return MaCaco_DATACHANGED;
	}
	else if(!digitalRead(pin))
		InPin[pin] = false;

	memory_map[MaCaco_OUT_s + slot] = MaCaco_NODATACHANGED;		
	return MaCaco_NODATACHANGED;
}
*/

U8 Souliss_DigInHoldDuration_Helper(U8 pin, U8 pin_value, U8 *memory_map, U8 firstSlot, U8 lastSlot)
{
	if(pin_value && !InPin[pin])
	{
		InPin[pin] = true;
		time = millis();								// Record time
//		Serial.println("button first press");
		// this is the first cycle detecting the button press: current input=1, previous input=0

		// verify if some of the lights in the group are ON
		for(U8 i=firstSlot; i<=lastSlot; i++)
		{
			if(memory_map[MaCaco_OUT_s + i] == Souliss_T1n_OnCoil)
			{
//				Serial.println("group ON -> turn it OFF");
				// there's at least one light ON
				// the user must have been pressing to turn everything OFF
				// then cycle on all the remaining slots to put set all of them to OFF
				for (U8 j = i; j <= lastSlot; j++)
					memory_map[MaCaco_IN_s + j] = Souliss_T1n_OffCmd;

				return MaCaco_DATACHANGED; 
			}
		}
//		Serial.println("group OFF -> turn it ON");

		// if here all lights were OFF
/*
		// the user must have been pressing to turn some lights ON
		// let's start to turn ON the first light in the group
		memory_map[MaCaco_IN_s + firstSlot] = Souliss_T1n_OnCmd;
		return MaCaco_DATACHANGED;
*/
		// do nothing to filter false actviations for spikes
		// the first slot will be set to on on the next cicle
		return MaCaco_NODATACHANGED;
	}
	else if(pin_value && InPin[pin] && (abs(millis()-time) > 0) && (abs(millis()-time) < STEP_DURATION))
	{
		if(memory_map[MaCaco_OUT_s + firstSlot] != Souliss_T1n_OnCoil)
		{
			// the user must have been pressing to turn some lights ON
			// let's start to turn ON the first light in the group
			memory_map[MaCaco_IN_s + firstSlot] = Souliss_T1n_OnCmd;
			return MaCaco_DATACHANGED;	
		}

	}	
	else if(pin_value && InPin[pin] && (abs(millis()-time) > STEP_DURATION))
	{
		// this cycle is executed while the button is kept pressed
		// the current input is 1, the previous input was 1 and some time passed from the first press

		U8 powered_lights_count = (U8) ( abs(millis()-time) / STEP_DURATION + 1 );
		if (powered_lights_count > lastSlot - firstSlot + 1)
			powered_lights_count = lastSlot - firstSlot + 1;

//		Serial.print("powered_lights_count : ");
//		Serial.println(powered_lights_count);		
		
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
	else if(!pin_value) // the button has been released
		InPin[pin] = false;
	
	return MaCaco_NODATACHANGED;
}

U8 Souliss_DigInHoldDuration(U8 pin, U8 *memory_map, U8 firstSlot, U8 lastSlot)
{
	Souliss_DigInHoldDuration_Helper(pin, digitalRead(pin), memory_map, firstSlot, lastSlot);
}

U8 Souliss_LowDigInHoldDuration(U8 pin, U8 *memory_map, U8 firstSlot, U8 lastSlot)
{
	Souliss_DigInHoldDuration_Helper(pin, !digitalRead(pin), memory_map, firstSlot, lastSlot);
}

/**************************************************************************
/*!
	Link an hardware pin to the shared memory map, active on rising edge
	It filters any pin activations shorter than duration [milliseconds]
*/	
/**************************************************************************/
U8 custom_DigInFiltered(U8 pin, U8 value, U8 *memory_map, U8 slot, U16 duration)
{
	// If pin is on, set the "value"
	if(digitalRead(pin) && !InPin[pin])
	{
		// pin just pressed
		time = millis();								// Record time
		InPin[pin] = true;
		
		return MaCaco_NODATACHANGED;
	}
	else if(!digitalRead(pin) && InPin[pin] && (abs(millis()-time) <= duration))
	{
		// pin released before duration => fitler this !
	
		InPin[pin] = false;
		return MaCaco_NODATACHANGED;
	}
	else if(!digitalRead(pin) && InPin[pin] && (abs(millis()-time) > duration))
	{
		// pin released after duration => Write input value in memory map
		if(memory_map)	memory_map[MaCaco_IN_s + slot] = value;
	
		InPin[pin] = false;
		return value;
	}
	
	return MaCaco_NODATACHANGED;
}

/**************************************************************************
/*!
	Link an hardware pin to the shared memory map, active on rising edge
	It filters any pin activations shorter than duration [milliseconds]
	Manually toggle the input since Souliss_Logic_T22 does not implement
	toggle of output
*/	
/**************************************************************************/
U8 custom_DigInFiltered_T22Toggle(U8 pin, U8 *memory_map, U8 slot, U16 duration)
{
	if (custom_DigInFiltered(pin, Souliss_T2n_ToggleCmd, memory_map, slot, duration) == MaCaco_NODATACHANGED)
		return MaCaco_NODATACHANGED;

	// if here the button press has been detected
	if((memory_map[MaCaco_OUT_s + slot] == Souliss_T2n_Coil_Close) || 
		(memory_map[MaCaco_OUT_s + slot] == Souliss_T2n_LimSwitch_Close))
	{
		memory_map[MaCaco_IN_s + slot] = Souliss_T2n_OpenCmd_SW;
	}
	else if((memory_map[MaCaco_OUT_s + slot] == Souliss_T2n_Coil_Open) || 
		(memory_map[MaCaco_OUT_s + slot] == Souliss_T2n_LimSwitch_Open))
	{
		memory_map[MaCaco_IN_s + slot] = Souliss_T2n_CloseCmd_SW;
	}
	else
		memory_map[MaCaco_IN_s + slot] = Souliss_T2n_OpenCmd_SW;


}



/*!
	A slots group is defined as a pool of consequent slots plus an additional slot 

	The group is dinamically divided into two parts (part1 and part2).
	part1 slots consists of the first powered_lights_count slots in the group,
	part2 consists of all the remaining slots

	All IN slots of part1 are set to Souliss_T1n_OnCmd
	All IN slots of part2 are set to Souliss_T1n_OffCmd

	Logic_SimpleLight is finally executed for each slot in the group.

	@group the slot defining number of lights to be powered
	@offset number of first relay handled by this board (if the group has relays on different boards)
	@firstSlot the slot in the defined group with the lower slot index
	@lastSlot the slot in the defined group with the higher slot index
*/
/**************************************************************************/
/*void Souliss_Logic_T11Group(U8 *memory_map, U8 group, U8 offset, U8 firstSlot, U8 lastSlot, U8 *trigger)
{
	if (memory_map[MaCaco_OUT_s + group] == MaCaco_DATACHANGED) // changed by pressing the physical push button
	{
		for(U8 i=firstSlot; i<=lastSlot; i++)
			memory_map[MaCaco_IN_s + i] = (i < memory_map[MaCaco_AUXIN_s + group] - offset + firstSlot) ?  Souliss_T1n_OnCmd : Souliss_T1n_OffCmd;
	
		memory_map[MaCaco_OUT_s + group] = MaCaco_NODATACHANGED; // reset
	}

	// cycle upon all OUT slots of the groups
	for(U8 i=firstSlot; i<=lastSlot; i++)
		Souliss_Logic_T11(memory_map, i, trigger);	
}
*/
void Souliss_Logic_T11Group(U8 *memory_map, U8 firstSlot, U8 lastSlot, U8 *trigger)
{
	// cycle upon all OUT slots of the groups
	for(U8 i=firstSlot; i<=lastSlot; i++)
		Souliss_Logic_T11(memory_map, i, trigger);	
}



void Souliss_T11Group_Timer(U8 *memory_map, U8 firstSlot, U8 lastSlot)
{
	for(U8 slot=firstSlot; slot<=lastSlot; slot++)
	{
		Souliss_T11_Timer(memory_map, slot);
	}
}

void Souliss_SetT11Group(U8 *memory_map, U8 firstSlot, U8 lastSlot)
{
	for(U8 slot=firstSlot; slot<=lastSlot; slot++)
	{
		memory_map[MaCaco_OUT_s + slot] = Souliss_T1n_RstCmd;
		Souliss_SetT11(memory_map, slot);
	}
}

#define Set_LightsGroup(firstSlot, lastSlot) Souliss_SetT11Group(memory_map, firstSlot, lastSlot)
#define Timer_LightsGroup(firstSlot, lastSlot) Souliss_T11Group_Timer(memory_map, firstSlot, lastSlot)
//#define Logic_LightsGroup(group, offset, firstSlot, lastSlot) Souliss_Logic_T11Group(memory_map, group, offset, firstSlot, lastSlot, &data_changed)
#define Logic_LightsGroup(firstSlot, lastSlot) Souliss_Logic_T11Group(memory_map, firstSlot, lastSlot, &data_changed)
//#define ssLightsGroupIN(pin, slot) Souliss_DigInHoldDuration(pin, memory_map, slot)
#define ssLightsGroupIN(pin, firstSlot, lastSlot) Souliss_DigInHoldDuration(pin, memory_map, firstSlot, lastSlot)
#define ssLowLightsGroupIN(pin, firstSlot, lastSlot) Souliss_LowDigInHoldDuration(pin, memory_map, firstSlot, lastSlot)

#define WINDOW_DELAY 500
#define DigInWindow(pin, value, slot) custom_DigInFiltered(pin, value, memory_map, slot, WINDOW_DELAY)
#define DigInWindowToggle(pin, slot) custom_DigInFiltered_T22Toggle(pin, memory_map, slot, WINDOW_DELAY)
//#define DigInFiltered(pin, value, slot) Souliss_DigIn(pin, value, memory_map, slot, true)

#endif