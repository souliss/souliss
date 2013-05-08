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
#include "frame/MaCaco/MaCaco.cpp"
#include "frame/vNet/vNet.cpp"

#include <Arduino.h>

#include "src/types.h"
#include "GetConfig.h"			// need : ethUsrCfg.h, vNetCfg.h, SoulissCfg.h

#if(JSONSERVER && VNET_MEDIA1_ENABLE && ETH_W5100)
	#include "gateway/JSON.cpp"
#elif(HTTPSERVER && VNET_MEDIA1_ENABLE && ETH_W5100)
	#include "gateway/HTTP.cpp"
#elif(HTTPSERVER && VNET_MEDIA1_ENABLE && ETH_ENC28J60)
	#include "gateway/HTTP_uIP.cpp"
#endif
	
#if(MODBUS)
	#include "gateway/Modbus.cpp"	
#endif

// Include IO definitions and drivers for supported hardware
#include "hardware/IOdef.cpp"

bool InPin[MAXINPIN] = {false};
bool FirstInit = {false};
static unsigned long time;
U8 roundrob_1=1,roundrob_2=1;

/**************************************************************************
/*!
	Set and init network configuration
*/	
/**************************************************************************/
void Souliss_SetAddress(U16 addr, U16 subnetmask, U16 mysupernode)
{
	// Set configuration parameters dedicated to the relevant interface
	if(vNet_GetMedia(addr) == 1)			
	{
	#if(VNET_MEDIA1_ENABLE)
		// Media 1 - Ethernet
		eth_SetBaseIP((uint8_t *)DEFAULT_BASEIPADDRESS);
		eth_SetSubnetMask((uint8_t *)DEFAULT_SUBMASK);
		eth_SetGateway((uint8_t *)DEFAULT_GATEWAY);
	#endif
	}
	else if(vNet_GetMedia(addr) == 2)		
	{
		// Media 2 - Chibiduino Wireless
	}
	else if(vNet_GetMedia(addr) == 3)
	{
	}
	else if(vNet_GetMedia(addr) == 4)
	{
	}
	else if(vNet_GetMedia(addr) == 5)
	{
	}
	
	// Set configuration general parameters
    if(!FirstInit)
	{
		vNet_Init();											// Init vNet
		FirstInit = true;
	}
	
	vNet_SetAddress(addr, vNet_GetMedia(addr));					// Set vNet Address
	vNet_SetSubnetMask(subnetmask, vNet_GetMedia(addr));		// Set vNet Subnetmask
	vNet_SetMySuperNode(mysupernode, vNet_GetMedia(addr));		// Set vNet Supernode
	
}

/**************************************************************************
/*!
	Set the addresses of local nodes into the memory map
*/	
/**************************************************************************/
void Souliss_SetLocalAddress(U8 *memory_map, U16 addr)
{
	// Set the remote address of a node into the vNet
	*(U16*)(memory_map+MaCaco_ADDRESSES_s) = addr;
}

/**************************************************************************
/*!
	Set the addresses of other nodes into the network
*/	
/**************************************************************************/
void Souliss_SetRemoteAddress(U8 *memory_map, U16 addr, U8 node)
{
	// Set the remote address of a node into the vNet
	*(U16*)(memory_map+MaCaco_ADDRESSES_s+node*2) = addr;
}

/**************************************************************************
/*!
	Reset output in memory map, this shall be used in case of LinkIO method
*/	
/**************************************************************************/
void Souliss_ResetOutput(U8 *memory_map, U8 slot)
{
	memory_map[MaCaco_OUT_s + slot] = 0x00;
}

/**************************************************************************
/*!
	Reset input in memory map, this shall be used in case of RemoteInput 
	method
*/	
/**************************************************************************/
void Souliss_ResetInput(U8 *memory_map, U8 slot)
{
	memory_map[MaCaco_IN_s + slot] = 0x00;
}

/**************************************************************************
/*!
	Create a link between input and output on the same memory map
	
	This method is useful if data shall be transferred from a device to
	another, in that case is not allowed for a receiver node to subscribe
	the inputs of another node.
	A receiver node can sunscribe instead the outputs of another node, so
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

/**************************************************************************
/*!
	Create a link between output and input on the same memory map
*/	
/**************************************************************************/
void Souliss_LinkOI(U8 *memory_map, U8 input_slot, U8 output_slot)
{
	if(memory_map[MaCaco_OUT_s + output_slot] != 0x00)
		memory_map[MaCaco_IN_s + input_slot] = memory_map[MaCaco_OUT_s + output_slot];	// Link IO
}

/**************************************************************************
/*!
	Define a communication subscription channel between two nodes
	
	It use MaCaco and vNet to retreive data from another node in the network
	incoming data are placed in input_slot, the requested data are numof_slot
	bytes starting from output_slot.
	
	Each subscription require an index, called subscr_chnl, and healty status
	provide feedback on communication status.
*/	
/**************************************************************************/	
U8 Souliss_CommunicationChannel(U16 addr, U8 *memory_map, U8 input_slot, U8 output_slot, U8 numof_slot, U8 subscr_chnl)
{
	// Verify the number of data to collect
	if (!(output_slot < MaCaco_SUBSCRLEN) || !(output_slot + numof_slot <= MaCaco_SUBSCRLEN))
		return 0;
		
	// If not yet, setup the communication channel
	if (*(memory_map+MaCaco_HEALTY_s+subscr_chnl) == 0)
		*(memory_map+MaCaco_HEALTY_s+subscr_chnl) = MaCaco_SUBINITHEALTY;
	
	// Subscribe data
	return MaCaco_subscribe(addr, memory_map, memory_map + MaCaco_IN_s + input_slot, MaCaco_OUT_s + output_slot, numof_slot, subscr_chnl);
}

/**************************************************************************
/*!
	Define multiple communication channels managed in round robin, data are
	located in the output area of the memory_map, and are ready to be read
	from the user interface.
*/	
/**************************************************************************/	
U8 Souliss_CommunicationChannels(U8 *memory_map, U8 numberofnodes)
{
	U8 ret=0;

	// If not yet, setup the communication channel
	if (*(memory_map+MaCaco_HEALTY_s+roundrob_2) == 0)
		*(memory_map+MaCaco_HEALTY_s+roundrob_2) = MaCaco_SUBINITHEALTY;
	
	// Handle the subscription in round robin
	if(roundrob_2 <= numberofnodes)
	{
		// Open and/or check one communication channel at each round
		if (*(U16*)(memory_map+MaCaco_ADDRESSES_s+2*roundrob_2) != 0x0000)
			ret = MaCaco_subscribe(*(U16*)(memory_map+MaCaco_ADDRESSES_s+2*roundrob_2), memory_map, memory_map + MaCaco_OUT_s + (roundrob_2*MaCaco_SUBSCRLEN), MaCaco_OUT_s, MaCaco_SUBSCRLEN, roundrob_2);

		roundrob_2++;
	} 
	else
		roundrob_2=1;	// Node number 0 is the local node 
		
	return ret;
}

/**************************************************************************
/*!
	Get definitions for typical from a remote device
	
	It use MaCaco and vNet to retreive typical definition data, use a polling
	request.
*/	
/**************************************************************************/	
U8 Souliss_GetTypical(U16 addr, U8 *memory_map)
{
	if(addr != 0x0000)
		return MaCaco_send(addr, MaCaco_READREQDIG, memory_map + MaCaco_TYP_s, MaCaco_TYP_s, MaCaco_TYPLENGHT, 0x00);
}

/**************************************************************************
/*!
	Get definitions for typical from multiple remote devices
	
	Do same job of Souliss_GetTypical for more nodes at same time, using
	roundrobin criteria. The data are requested only once, a periodally
	refresh can be achieved using the Souliss_RefreshTypicals() method.
*/	
/**************************************************************************/	
U8 Souliss_GetTypicals(U8 *memory_map, U8 numberofnodes)
{
	U8 ret=0;

	if(roundrob_1 <= numberofnodes)
	{
		// Retreive once the typical definitions data
		if (*(U16*)(memory_map+MaCaco_ADDRESSES_s+2*roundrob_1) != 0x0000)
			ret = MaCaco_send(*(U16*)(memory_map+MaCaco_ADDRESSES_s+2*roundrob_1), MaCaco_READREQDIG, memory_map + MaCaco_TYP_s + (roundrob_1*MaCaco_TYPLENGHT), MaCaco_TYP_s, MaCaco_TYPLENGHT, 0x00);

		roundrob_1++;
	}
		
	return ret;	
} 

/**************************************************************************
/*!
	Restart the Souliss_GetTypicals function to refresh the data, this
	function shall be used periodally with a low rate frequency.
*/	
/**************************************************************************/	
void Souliss_RefreshTypicals()
{
	roundrob_1 = 1;			// Node number 0 is the local node  
} 
	
/**************************************************************************
/*!
	Write on network node, act as a remote input
	
	It write directly into the inputs map of another node, data are moved
	directly without a subscription, so no channel healty is provided.
*/	
/**************************************************************************/	
U8 Souliss_RemoteInput(U16 addr, U8 slot, U8 command)
{
	U8 cmd = command;
	return MaCaco_send(addr, MaCaco_FORCEREGSTR, 0x00, MaCaco_IN_s + slot, 1, &cmd);
}

/**************************************************************************
/*!
	Write on network node, act as a remote input
	
	It write directly into the inputs map of another node, data are moved
	directly without a subscription, so no channel healty is provided.
*/	
/**************************************************************************/	
U8 Souliss_RemoteInputs(U16 addr, U8 firstslot, U8 *commands, U8 numberof)
{
	return MaCaco_send(addr, MaCaco_FORCEREGSTR, 0x00, MaCaco_IN_s + firstslot, numberof, commands);
}

/**************************************************************************
/*!
	Retrieve the data from the communication channels
*/	
/**************************************************************************/
U8 Souliss_CommunicationData(U8 *memory_map, U8 *trigger)
{
	// If not yet, init the communication channel
	if (*(memory_map+MaCaco_HEALTY_s+0) == 0)
		*(memory_map+MaCaco_HEALTY_s+0) = MaCaco_SUBMAXHEALTY;

	MaCaco_DataIn();
	return MaCaco_retrieve(memory_map, trigger);
}

/**************************************************************************
/*!
	Build a distributed watchdog using a chain of devices
	
	Each device check the healty status of the next device, and issue an
	alarm in the output slot if the watchdog timer expire.
	
	The same slot shall be used for all the nodes of the chain.
	
	This method shall run at a slow rate, it use one memory slot.
*/	
/**************************************************************************/
U8 Souliss_Watchdog(U8 *memory_map, U16 chain_address, U8 chain_slot, U8 alarm_command)
{	
	// Watchdog, check the healty of the previous node of the chain
	if(memory_map[MaCaco_IN_s + chain_slot] == WTD_EXPIRE)			
	{
		// Watchdog timer is expired, raise the alarm
		memory_map[MaCaco_OUT_s + chain_slot] = alarm_command;		// Set the alarm
	}
	else								
	{	
		memory_map[MaCaco_IN_s + chain_slot]--;
		memory_map[MaCaco_OUT_s + chain_slot] = Souliss_RstCmd;		// Reset the alarm
	}
	
	// Write in the next node of the chain to avoid that watchdog timer expire
	U8 cmd = WTD_SET;
	return MaCaco_send(chain_address, MaCaco_FORCEREGSTR, 0x00, MaCaco_IN_s + chain_slot, 1, &cmd);
}


/**************************************************************************
/*!
	Link an hardware pin to the shared memory map, active on rising edge
	
	It write directly into the inputs map of the node, these data shall be
	used for logic applications.
*/	
/**************************************************************************/
U8 Souliss_DigIn(U8 pin, U8 value, U8 *memory_map, U8 slot)
{
	// If pin is on, set the "value"
	if(digitalRead(pin) && !InPin[pin])
	{	
		memory_map[MaCaco_IN_s + slot] = value;
		
		InPin[pin] = true;
		return MaCaco_DATACHANGED;
	}
	else if(!digitalRead(pin))
	{
		InPin[pin] = false;
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
U8 Souliss_LowDigIn(U8 pin, U8 value, U8 *memory_map, U8 slot)
{
	// If pin is on, set the "value"
	if(digitalRead(pin)==0 && !InPin[pin])
	{	
		memory_map[MaCaco_IN_s + slot] = value;
		
		InPin[pin] = true;
		return MaCaco_DATACHANGED;
	}
	else if(digitalRead(pin))
	{
		InPin[pin] = false;
		return MaCaco_NODATACHANGED;
	}
}

/**************************************************************************
/*!
	Link an hardware pin to the shared memory map, use with latched two state
	pushbutton.
*/	
/**************************************************************************/
U8 Souliss_DigIn2State(U8 pin, U8 value_state_on, U8 value_state_off, U8 *memory_map, U8 slot)
{
	// If pin is on, set the "value"
	if(digitalRead(pin) && !InPin[pin])
	{	
		memory_map[MaCaco_IN_s + slot] = value_state_on;
		
		InPin[pin] = true;
		return MaCaco_DATACHANGED;
	}
	else if(!digitalRead(pin) && InPin[pin])
	{
		memory_map[MaCaco_IN_s + slot] = value_state_off;
		
		InPin[pin] = false;
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
U8 Souliss_DigInHold(U8 pin, U8 value_state1, U8 value_state2, U8 *memory_map, U8 slot)
{
	// If pin is on, set the "value"
	if(digitalRead(pin) && !InPin[pin])
	{
		// Write input value in memory map
		memory_map[MaCaco_IN_s + slot] = value_state1;
		time = millis();								// Record time
		
		InPin[pin] = true;
		return MaCaco_DATACHANGED;
	}
	else if(digitalRead(pin) && (abs(millis()-time) > 1500))
	{
		time = millis();								// Record time
		
		// Write timer value in memory map
		memory_map[MaCaco_IN_s + slot] = value_state2;	
	}
	else if(!digitalRead(pin))
	{
		InPin[pin] = false;
		return MaCaco_NODATACHANGED;
	}
}


/**************************************************************************
/*!
	Link an hardware pin to the shared memory map, active on falling edge
	Identify two states, press and hold.
*/	
/**************************************************************************/
U8 Souliss_LowDigInHold(U8 pin, U8 value_state1, U8 value_state2, U8 *memory_map, U8 slot)
{
	// If pin is on, set the "value"
	if(!digitalRead(pin) && !InPin[pin])
	{
		// Write input value in memory map
		memory_map[MaCaco_IN_s + slot] = value_state1;
		time = millis();								// Record time
		
		InPin[pin] = true;
		return MaCaco_DATACHANGED;
	}
	else if(!digitalRead(pin) && abs(millis()-time) > 1500)
	{
		time = millis();								// Record time
		
		// Write timer value in memory map
		memory_map[MaCaco_IN_s + slot] = value_state2;
	}
	else if(digitalRead(pin))
	{
		InPin[pin] = false;
		return MaCaco_NODATACHANGED;
	}
}

/**************************************************************************
/*!
	Link the shared memory map to an hardware pin
	
	It write a digital output pin based on the value of the output into
	memory_map, let a logic act on external devices.
*/	
/**************************************************************************/
void Souliss_DigOut(U8 pin, U8 value, U8 *memory_map, U8 slot)
{
	// If output is active switch on the pin, else off
	if(memory_map[MaCaco_OUT_s + slot] == value)
		digitalWrite(pin, HIGH);
	else
		digitalWrite(pin, LOW);
}

/**************************************************************************
/*!
	Link the shared memory map to an hardware pin
	
	It write a digital output pin based on the value of the output into
	memory_map, let a logic act on external devices.
*/	
/**************************************************************************/
void Souliss_LowDigOut(U8 pin, U8 value, U8 *memory_map, U8 slot)
{
	// If output is active switch on the pin, else off
	if(memory_map[MaCaco_OUT_s + slot] == value)
		digitalWrite(pin, LOW);
	else
		digitalWrite(pin, HIGH);
}

/**************************************************************************
/*!
	Link the shared memory map to an hardware pin
	
	It write a digital output pin based on the value of the output into
	memory_map, let a logic act on external devices. Match criteria is based 
	on bit-wise AND operation.
*/	
/**************************************************************************/
void Souliss_nDigOut(U8 pin, U8 value, U8 *memory_map, U8 slot)
{
	// If output is active switch on the pin, else off
	if(memory_map[MaCaco_OUT_s + slot] & value)
		digitalWrite(pin, HIGH);
	else
		digitalWrite(pin, LOW);
}

/**************************************************************************
/*!
	Link the shared memory map to an hardware pin
	
	It write a digital output pin based on the value of the output into
	memory_map, let a logic act on external devices. Match criteria is based 
	on bit-wise AND operation.
*/	
/**************************************************************************/
void Souliss_nLowDigOut(U8 pin, U8 value, U8 *memory_map, U8 slot)
{
	// If output is active switch on the pin, else off
	if(memory_map[MaCaco_OUT_s + slot] & value)
		digitalWrite(pin, LOW);
	else
		digitalWrite(pin, HIGH);
}

/**************************************************************************
/*!
	Link the shared memory map to an hardware pin, toggle the output value
*/	
/**************************************************************************/
void Souliss_DigOutToggle(U8 pin, U8 value, U8 *memory_map, U8 slot)
{
	if(memory_map[MaCaco_OUT_s + slot] == value)
	{
		// If output is active toggle the pin, else off
		if(digitalRead(pin))
			digitalWrite(pin, LOW);
		else
			digitalWrite(pin, HIGH);
	}
	else
		digitalWrite(pin, LOW);
}

/**************************************************************************
/*!
	Link the shared memory map to an hardware pin
*/	
/**************************************************************************/
void Souliss_DigOutLessThan(U8 pin, U8 value, U8 deadband, U8 *memory_map, U8 slot)
{
	// If output is active switch on the pin, else off
	if(memory_map[MaCaco_OUT_s + slot] < value - deadband)
		digitalWrite(pin, HIGH);
	else if(memory_map[MaCaco_OUT_s + slot] > value + deadband)
		digitalWrite(pin, LOW);
}

/**************************************************************************
/*!
	Link the shared memory map to an hardware pin
*/	
/**************************************************************************/
void Souliss_DigOutGreaterThan(U8 pin, U8 value, U8 deadband, U8 *memory_map, U8 slot)
{
	// If output is active switch on the pin, else off
	if(memory_map[MaCaco_OUT_s + slot] > value + deadband)
			digitalWrite(pin, HIGH);
	else if(memory_map[MaCaco_OUT_s + slot] < value - deadband)
		digitalWrite(pin, LOW);
}

/**************************************************************************
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
}