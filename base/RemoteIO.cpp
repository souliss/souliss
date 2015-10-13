/**************************************************************************
	Souliss
    Copyright (C) 2015  Veseo

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

U8 s_value=0;

/**************************************************************************/
/*!
	Link an hardware pin to the shared memory map of another (remote node), 
	active on rising edge.
	
	It write directly into the inputs map of the node, these data shall be
	used for logic applications.
*/	
/**************************************************************************/
U8 Souliss_RemoteDigIn(U8 pin, U8 value, U16 addr, U8 slot, bool filteractive=false)
{
	if(Souliss_DigIn(pin, value, 0, slot, filteractive) == value)
	{
		s_value = value;
		return MaCaco_send(addr, MaCaco_FORCEREGSTR, 0x00, MaCaco_IN_s + slot, 1, &s_value);
	}	
	else
		return 0;
}

/**************************************************************************/
/*!
	Link an hardware pin to the shared memory map of another (remote node),
	active on falling edge.
	
	It write directly into the inputs map of the node, these data shall be
	used for logic applications.
*/	
/**************************************************************************/
U8 Souliss_RemoteLowDigIn(U8 pin, U8 value, U16 addr, U8 slot, bool filteractive=false)
{
	if(Souliss_LowDigIn(pin, value, 0, slot, filteractive) == value)
	{
		s_value = value;		
		return MaCaco_send(addr, MaCaco_FORCEREGSTR, 0x00, MaCaco_IN_s + slot, 1, &s_value);
	}	
	else
		return 0;
}

/**************************************************************************/
/*!
	Link an hardware pin to the shared memory map of another (remote node), 
	use with latched two state pushbutton.
*/	
/**************************************************************************/

U8 Souliss_RemoteDigIn2State(U8 pin, U8 value_state_on, U8 value_state_off, U16 addr, U8 slot)
{
	s_value = Souliss_DigIn2State(pin, value_state_on, value_state_off, 0, slot);
	
	if(s_value)
		return MaCaco_send(addr, MaCaco_FORCEREGSTR, 0x00, MaCaco_IN_s + slot, 1, &s_value);
	else
		return 0;
}	

/**************************************************************************/
/*!
	Link an hardware pin to the shared memory map of another (remote node).
	
	Use a single analog input connected to two different pushbuttons, use 
	different pull-up resistors to define different voltage drops for the
	two pushbuttons.
	
	If the analog value goes over the top limit or below the bottom one,
	the pushbuttons are pressed, if the analog value stay in the middle 
	no action is taken.
*/	
/**************************************************************************/
U8 Souliss_RemoteAnalogIn2Buttons(U8 pin, U8 value_button1, U8 value_button2, U16 addr, U8 slot)
{
	s_value = Souliss_AnalogIn2Buttons(pin, value_button1, value_button2, 0, slot);
	
	if(s_value)
		return MaCaco_send(addr, MaCaco_FORCEREGSTR, 0x00, MaCaco_IN_s + slot, 1, &s_value);
	else
		return 0;
}
/**************************************************************************/
/*!
	Link an hardware pin to the shared memory map of another (remote node), 
	use with latched two state pushbutton, active on falling edge
*/	
/**************************************************************************/
U8 Souliss_RemoteLowDigIn2State(U8 pin, U8 value_state_on, U8 value_state_off, U16 addr, U8 slot)
{
	s_value = Souliss_LowDigIn2State(pin, value_state_on, value_state_off, 0, slot);
	
	if(s_value)
		return MaCaco_send(addr, MaCaco_FORCEREGSTR, 0x00, MaCaco_IN_s + slot, 1, &s_value);
	else
		return 0;
}

/**************************************************************************/
/*!
	Link an hardware pin to the shared memory map of another (remote node), 
	active on rising edge.
	
	Identify two states, press and hold.
*/	
/**************************************************************************/
U8 Souliss_RemoteDigInHold(U8 pin, U8 value, U8 value_hold, U16 addr, U8 slot, U16 holdtime=1500)
{
	s_value = Souliss_DigInHold(pin, value, value_hold, 0, slot, holdtime);
	
	if(s_value)
		return MaCaco_send(addr, MaCaco_FORCEREGSTR, 0x00, MaCaco_IN_s + slot, 1, &s_value);
	else
		return 0;
}

/**************************************************************************/
/*!
	Link an hardware pin to the shared memory map of another (remote node), 
	active on falling edge.
	
	Identify two states, press and hold.
*/	
/**************************************************************************/
U8 Souliss_RemoteLowDigInHold(U8 pin, U8 value, U8 value_hold, U16 addr, U8 slot, U16 holdtime=1500)
{
	s_value = Souliss_LowDigInHold(pin, value, value_hold, 0, slot, holdtime);
	
	if(s_value)
		return MaCaco_send(addr, MaCaco_FORCEREGSTR, 0x00, MaCaco_IN_s + slot, 1, &s_value);
	else
		return 0;
}
