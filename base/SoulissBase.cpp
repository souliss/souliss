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

#include "src/types.h"
#include "GetConfig.h"			// need : ethUsrCfg.h, vNetCfg.h, SoulissCfg.h, MaCacoCfg.h

#define	PINRESET	0x0
#define	PINSET		0x1
#define	PINACTIVE	0x2
#define	PINRELEASED	0x3

U8 InPin[MAXINPIN];
bool FirstInit = {false}, addrsrv = {false};
static unsigned long time;
U8 roundrob_1=1,roundrob_2=1, timeout=TIMEOUT_SET;
U16 keyidval=0;

#if (SOULISS_DEBUG)
	#define SOULISS_LOG Serial.print
#endif

/**************************************************************************
/*!
	Init
*/	
/**************************************************************************/
void Souliss_Init()
{
	for(U8 i=0; i<MAXINPIN; i++)
		InPin[i]=PINRESET;
}

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
	#if(VNET_MEDIA5_ENABLE)	
		// These information are not available in the driver itself, so are passed directly 
		// in order to calculate a collision avoidance index
		vNet_SetCollisionAvoidanceIndex_M5(addr, subnetmask);
	#endif	
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
#if(MaCaco_SUBSCRIBERS)
/**************************************************************************
/*!
	Set the addresses of local nodes into the memory map, shall be used
	for gateway node
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
#endif
/**************************************************************************
/*!
	Set an IP address and calculate automatically the vNet address, it
	overwrite all other settings
*/	
/**************************************************************************/
void Souliss_SetIPAddress(U8* ip_address, U8* subnet_mask, U8* ip_gateway)
{
	// Starting from IP configuration define the vNet ones
	U8 i=0;
	for(i=0; i<4; i++)
	{
		if(DEFAULT_BASEIPADDRESS) 	DEFAULT_BASEIPADDRESS[i]=ip_address[i];
		if(DEFAULT_SUBMASK) 		DEFAULT_SUBMASK[i] = subnet_mask[i];
		if(DEFAULT_GATEWAY) 		DEFAULT_GATEWAY[i] = ip_gateway[i];
	}
	
	U16 vNet_address = (U16)ip_address[i-1];			// The last byte of the IP address is the vNet one
	DEFAULT_BASEIPADDRESS[i-1]=0;						// The BASEIPADDRESS has last byte always zero
	
	// Set the address
	Souliss_SetAddress(vNet_address, DYNAMICADDR_SUBNETMASK, ((vNet_address & DYNAMICADDR_SUBNETMASK) | DYNAMICADDR_GATEWAY));
}

/**************************************************************************
/*!
	Define a gateway node as server for dynamic addressing
*/	
/**************************************************************************/
void Souliss_SetAddressingServer(U8 *memory_map)
{
	// Flag the addressing server
	addrsrv = true;

	// Use a free address for each media
	U8 i=0;
	for(i=0; i<VNET_MEDIA_NUMBER; i++)
		if(vnet_media_en[i])
			Souliss_SetAddress((vnet_addr_l[i] | DYNAMICADDR_GATEWAYNODE), DYNAMICADDR_SUBNETMASK, ((vnet_addr_l[i] & DYNAMICADDR_SUBNETMASK) | DYNAMICADDR_GATEWAY));
	
	for(i=0; i<VNET_MEDIA_NUMBER; i++)
		if(vnet_media_en[i])
		{
			// Only one media, by default the first, is used as reference
			Souliss_SetLocalAddress(memory_map, (vnet_addr_l[i] | DYNAMICADDR_GATEWAYNODE));
			return;
		}
}

/**************************************************************************
/*!
	Set the node to retrieve the address dynamically
*/	
/**************************************************************************/
void Souliss_SetDynamicAddressing()
{
	U8 i=0;
	for(i=0; i<VNET_MEDIA_NUMBER; i++)
	{
		U8 media = i + 1;							// Addressing in arrays starts from zero
													// rather media definition from one
		if(vnet_media_en[i])
		{
			// Set configuration parameters dedicated to the relevant interface
			if(media == 1)			
			{
			#if(VNET_MEDIA1_ENABLE)
				// Media 1 - Ethernet
				eth_SetBaseIP((uint8_t *)DEFAULT_BASEIPADDRESS);
				eth_SetSubnetMask((uint8_t *)DEFAULT_SUBMASK);
				eth_SetGateway((uint8_t *)DEFAULT_GATEWAY);
			#endif
			}
			else if(media == 2)		
			{
				// Media 2 - Chibiduino Wireless
			}
			else if(media == 3)
			{
			}
			else if(media == 4)
			{
			}
			else if(media == 5)
			{
			}

			// Set configuration general parameters
			if(!FirstInit)
			{
				vNet_Init();											// Init vNet
				FirstInit = true;
			}
			
			vNet_SetAddress(0x00, media);								// Set vNet Address
			vNet_SetSubnetMask(DYNAMICADDR_SUBNETMASK, media);			// Set vNet Subnetmask
			vNet_SetMySuperNode(0x00, media);							// Set vNet Supernode
		}
	}
}

/**************************************************************************
/*!
	Request an addressing and parse the answer, need an unique identifier
	id that is used while the node hasn't a valid address.
	
	A compiler macro can be used as id, __TIME__ is a 9 byte that indicate
	the time that compiler started. Isn't an absolute identifier but works
	in most of the cases.
*/	
/**************************************************************************/
void Souliss_DynamicAddressing (U8 *memory_map, const char id[], U8 size)
{
	U8 i, usedmedia = vNet_MyMedia();			
			
	// If no address is set
	if(!vNet_GetAddress(usedmedia))
	{
		// Generate a a key identifier
		if(!keyidval)
			for(i=0;i<size;i++)
				keyidval+=(i*i)*id[i];	
	
		// Verify if the addressing information are available in the configuration
		// parameters of the memory map
		
		// The first parameter is the keyidval number used to indetify my previous request
		U8 *confparameters_p = (memory_map + MaCaco_QUEUE_s);
		if((*(U16 *)confparameters_p) == keyidval)
		{
			// The next parameter is the media
			confparameters_p+=sizeof(U16);
			if(*confparameters_p == usedmedia)
			{
				// Load the address
				confparameters_p++;
				Souliss_SetAddress((*(U16 *)confparameters_p), DYNAMICADDR_SUBNETMASK, DYNAMICADDR_GATEWAY);
					
				// Clear the actual configuration parameters, the addressing server will load there
				// the requested address
				for(U8 i=0; i<MaCaco_QUEUELEN; i++)
					*(memory_map + MaCaco_QUEUE_s + i) = 0;
				
				return;
			}	
		}
		
		// Clear the actual configuration parameters, the addressing server will load there
		// the requested address
		for(i=0; i<MaCaco_QUEUELEN; i++)
			*(memory_map + MaCaco_QUEUE_s + i) = 0;

		// Request a new address
		MaCaco_send(0xFFFF, MaCaco_DINADDRESSREQ, (U8 *)keyidval, usedmedia, 0, 0);
	}	
}

/**************************************************************************
/*!
	Send a request to join a network, shall be periodically processed by nodes
	that requested a dynamic address
*/	
/**************************************************************************/
void Souliss_JoinNetwork()
{
	// Request to join a network only if I've got an address
	if(vNet_GetAddress(vNet_MyMedia()))
		MaCaco_send(0xFFFF, MaCaco_JOINNETWORK, (U8 *)keyidval, 0, 0, 0);
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
#if(MaCaco_SUBSCRIBERS)
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
U8 Souliss_CommunicationChannels(U8 *memory_map)
{
	U8 ret=0;

	// If not yet, setup the communication channel
	if (*(memory_map+MaCaco_HEALTY_s+roundrob_2) == 0)
		*(memory_map+MaCaco_HEALTY_s+roundrob_2) = MaCaco_SUBINITHEALTY;
	
	// Check subscription has been reset
	if((roundrob_2 > 1) && (MaCaco_subscribe_is_init()))
		roundrob_2=1;		// Start from first remote node
		
	// Handle the subscription in round robin
	if(roundrob_2 < MaCaco_NODES)
	{
		// Open and/or check one communication channel at each round
		if (((*(U16*)(memory_map+MaCaco_ADDRESSES_s+2*roundrob_2)) != 0x0000))
			ret = MaCaco_subscribe((*(U16*)(memory_map+MaCaco_ADDRESSES_s+2*roundrob_2)), memory_map, 0, MaCaco_OUT_s, MaCaco_SUBSCRLEN, roundrob_2);		// Use putin as zero to flag a passthrough		
		else
		{
			roundrob_2=1;		// Node number 0 is the local node
			return ret;
		}
		
		roundrob_2++;
	} 
	else
		roundrob_2=1;	// Node number 0 is the local node 
		
	return ret;
}

/**************************************************************************
/*!
	Define a Communication Channel as battery operated
*/	
/**************************************************************************/	
void Souliss_BatteryChannels(U8 *memory_map, U16 addr)
{
	for(U8 i=0;i<MaCaco_NODES;i++)
		if(((*(U16*)(memory_map+MaCaco_ADDRESSES_s+2*roundrob_2)) == addr))
		{
			MaCaco_subscribe_battery(i);
			return;
		}	
}

/**************************************************************************
/*!
	Get definitions for typical from multiple remote devices
	
	Do same job of Souliss_GetTypical for more nodes at same time, using
	roundrobin criteria. The data are requested only once, a periodally
	refresh can be achieved using the Souliss_RefreshTypicals() method.
*/	
/**************************************************************************/	
U8 Souliss_GetTypicals(U8 *memory_map)
{ 
	U8 s=MaCaco_reqtyp();
	
	if(s)
	{
		// Pointer to the node address
		U16* m_addr = (U16*)(memory_map+MaCaco_ADDRESSES_s+2*roundrob_1);
		
		// Update the timeout
		if(timeout)	timeout--;
		else if(timeout==TIMEOUT_EXPIRE)
		{
			// Move to next node
			roundrob_1++;
			MaCaco_reset_lastaddr();
			MaCaco_reqtyp_decrease();
			timeout=TIMEOUT_SET;
			
			return 0;	// At next cycle a request will be send out
		}	
	
		// Start from first node if is a new request from an user interface
		if (s == MaCaco_NODES)
		{
			roundrob_1 = 1;				// Reset, node 0 is the node it-self, doesn't need to send data out
			MaCaco_reset_lastaddr();
			MaCaco_reqtyp_decrease();
			
			// Pointer to the node address
			m_addr = (U16*)(memory_map+MaCaco_ADDRESSES_s+2*roundrob_1);
		}
		
		// If the node answer has been received
		if((*m_addr != 0x0000) && (*m_addr == MaCaco_reqtyp_lastaddr()))
		{
			// At next cycle move to next node
			if(roundrob_1 < MaCaco_NODES) 
				roundrob_1++;
			else
			{
				// Reset
				while(MaCaco_reqtyp()) MaCaco_reqtyp_decrease();

				// Next cycle start again from begin
				roundrob_1 = 1;		
				MaCaco_reset_lastaddr();
			}
				
			// Next node
			MaCaco_reqtyp_decrease();
			return 1;
		}
			
		// Send a request to the node, if the address is zero there are no more node to process
		if (*m_addr != 0x0000)	MaCaco_send(*m_addr, MaCaco_TYPREQ, 0, MaCaco_TYP_s, MaCaco_TYPLENGHT, 0x00);			
		else 
		{
			// Reset
			while(MaCaco_reqtyp()) MaCaco_reqtyp_decrease();
				
			// Next cycle start again from begin
			roundrob_1 = 1;		
			MaCaco_reset_lastaddr();		
		}
			
		// At next cycle the answer will be received
		return 0;
	}
	
	return 0;	
} 
#endif

/**************************************************************************
/*!
	Record into a node a subscription at code time, it has the same effect
	of a runtime subscription but avoids long wait times.
	
	This is useful for battery operated (or generally devices not running 24h)
	that may never match the subscription request because sleeping at time.
*/	
/**************************************************************************/	
U8 Souliss_HardcodedCommunicationChannel(U16 gateway_addr)
{
	if(gateway_addr)
		MaCaco_subscribe_record(gateway_addr, MaCaco_SUBSCRREQ, 0, MaCaco_OUT_s, MaCaco_SUBSCRLEN);
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
	directly without a subscription, so no channel healthy is provided.
*/	
/**************************************************************************/	
U8 Souliss_RemoteInputs(U16 addr, U8 firstslot, U8 *commands, U8 numberof)
{
	return MaCaco_send(addr, MaCaco_FORCEREGSTR, 0x00, MaCaco_IN_s + firstslot, numberof, commands);
}

/**************************************************************************
/*!
	Apply the command on all the logic assigned to that typical in the node
*/	
/**************************************************************************/	
U8 Souliss_MassiveCommand(U16 addr, U8 typ, U8 command)
{
	U8 cmd = command;
	return MaCaco_send(addr, MaCaco_FORCEREGSTR, 0x00, typ, 1, &cmd); 
}

/**************************************************************************
/*!
	Apply the command on all the logic assigned to that typical in all the
	nodes of the network
*/	
/**************************************************************************/	
U8 Souliss_BroadcastMassiveCommand(U8 typ, U8 command)
{
	U8 cmd = command;
	return MaCaco_send(0xFFFF, MaCaco_FORCEREGSTR, 0x00, typ, 1, &cmd); 
}

/**************************************************************************/
/*!
    Broadcast an action message
*/
/**************************************************************************/
U8 Souliss_BroadcastAction(U8 *memory_map, U16 message, U8 action)
{
	return MaCaco_send(0xFFFF, MaCaco_ACTIONMSG, (U8 *)message, action, 0, 0);
}

/**************************************************************************/
/*!
    Multicast an action message
*/
/**************************************************************************/
U8 Souliss_MulticastAction(U16 multicast_addr, U8 *memory_map, U16 message, U8 action)
{
	return MaCaco_send(multicast_addr, MaCaco_ACTIONMSG, (U8 *)message, action, 0, 0);
}

/**************************************************************************/
/*!
    Broadcast an action message
*/
/**************************************************************************/
U8 Souliss_BroadcastActionMessage(U8 *memory_map, U16 message, U8 action, U8* data, U8 message_len)
{
	return MaCaco_send(0xFFFF, MaCaco_ACTIONMSG, (U8 *)message, action, message_len, data);
}

/**************************************************************************/
/*!
    Multicast an action message
*/
/**************************************************************************/
U8 Souliss_MulticastActionMessage(U16 multicast_addr, U8 *memory_map, U16 message, U8 action, U8* data, U8 message_len)
{
	return MaCaco_send(multicast_addr, MaCaco_ACTIONMSG, (U8 *)message, action, message_len, data);
}

/**************************************************************************/
/*!
    Return if there is a matching action message
*/
/**************************************************************************/
U8 Souliss_GetAction(U8 *memory_map, U16 message, U8 action)
{
	// action message are in the queue
	U8*	confparameters_p = (memory_map + MaCaco_QUEUE_s);
	
	if(((*(U16 *)confparameters_p) == message) && (*(confparameters_p+sizeof(U16)) == action))
	{
		#if (SOULISS_DEBUG)
		// Print debug messages
		SOULISS_LOG("(ss)<Action Message>");
		SOULISS_LOG("<|0x");
		SOULISS_LOG((*(U16 *)confparameters_p),HEX);
		SOULISS_LOG("|0x");
		SOULISS_LOG((*(confparameters_p+sizeof(U16))),HEX);		
		SOULISS_LOG(">\r\n");
		#endif
	
		// Reset the queue 
		for(U8 i=0; i<MaCaco_QUEUELEN; i++)
			*(memory_map + MaCaco_QUEUE_s + i) = 0;
				
		return 1;		// Trigger the action
	}	
	
	return 0;			// Nothing to do
}

/**************************************************************************/
/*!
    Return if there is a matching action message
*/
/**************************************************************************/
U8 Souliss_GetActionMessage(U8 *memory_map, U16 message, U8 action, U8* data, U8* len)
{
	// action message are in the queue
	U8*	confparameters_p = (memory_map + MaCaco_QUEUE_s);
	
	// Get the message value
	U16 _message=(*(U16 *)confparameters_p);
	confparameters_p+=sizeof(U16);
	
	// Get the action value
	U8  _action=*confparameters_p;
	confparameters_p++;
	
	if((_message == message) && (_action == action))
	{
		#if (SOULISS_DEBUG)
		// Print debug messages
		SOULISS_LOG("(ss)<Action Message>");
		SOULISS_LOG("<|0x");
		SOULISS_LOG(_message,HEX);
		SOULISS_LOG("|0x");
		SOULISS_LOG(_action,HEX);		
		SOULISS_LOG(">\r\n");
		#endif
	
		// Get the value length if a payload is available
		*len=*confparameters_p;
		confparameters_p++;
		
		// Move payload
		if(*len)
			memmove(data, confparameters_p, *len);
	
		// Reset the queue 
		for(U8 i=0; i<MaCaco_QUEUELEN; i++)
			*(memory_map + MaCaco_QUEUE_s + i) = 0;
				
		return 1;		// Trigger the action
	}	
	
	return 0;			// Nothing to do
}


/**************************************************************************
/*!
	Retrieve the data from the communication channels
*/	
/**************************************************************************/
U8 Souliss_CommunicationData(U8 *memory_map, U8 *trigger)
{
	#if(MaCaco_SUBSCRIBERS)
	// If not yet, init the communication channel
	if (*(memory_map+MaCaco_HEALTY_s+0) == 0)
		*(memory_map+MaCaco_HEALTY_s+0) = MaCaco_SUBMAXHEALTY;
	#endif
	
	MaCaco_DataIn();
	U8 ret = MaCaco_retrieve(memory_map, trigger);
	
	#if (SOULISS_DEBUG)
	// Look for input values
	U8 i=0;
	for(i=0;i<MaCaco_WRITE_f;i++)
		if(*(memory_map+i) !=0)
			break;
	
	if((*trigger) || (i<MaCaco_WRITE_f))
	{
		// Print debug messages
		SOULISS_LOG("(ss)<MAP>");
		SOULISS_LOG("<|0x");
		for(i=0;i<MaCaco_MEMMAP;i++)
		{
			SOULISS_LOG(memory_map[i],HEX);
			SOULISS_LOG("|0x");
		}
		SOULISS_LOG(">\r\n");
	}	
	#endif

	return ret;
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
U8 Souliss_DigIn(U8 pin, U8 value, U8 *memory_map, U8 slot, bool filteractive=false)
{
	// If pin is ON, set the flag. If at next cycle the pin will still
	// be ON the requested action will be performed
	if(digitalRead(pin) && (InPin[pin]==PINRESET))
	{
		InPin[pin] = PINSET;

		// Copy the value in the memory map
		if(!filteractive && memory_map)
		{
			memory_map[MaCaco_IN_s + slot] = value;	
			return value;		
		}
		
	}	
	else if(filteractive && digitalRead(pin) && InPin[pin]==PINSET)
	{
		// Flag that action is executed
		InPin[pin] = PINACTIVE;

		// Copy the value in the memory map
		if(memory_map)	
		{
			memory_map[MaCaco_IN_s + slot] = value;	
			return value;
		}
	}
	else if(filteractive && !digitalRead(pin) && InPin[pin]==PINACTIVE)
		InPin[pin] = PINRELEASED;
	else if(filteractive && !digitalRead(pin) && InPin[pin]==PINRELEASED)
		InPin[pin] = PINRESET;
	else if(!filteractive && !digitalRead(pin))
		InPin[pin] = PINRESET;
	
	return MaCaco_NODATACHANGED;
}

/**************************************************************************
/*!
	Link an hardware pin to the shared memory map, active on falling edge
	
	It write directly into the inputs map of the node, these data shall be
	used for logic applications.
*/	
/**************************************************************************/
U8 Souliss_LowDigIn(U8 pin, U8 value, U8 *memory_map, U8 slot, bool filteractive=false)
{
	// If pin is ON, set the flag. If at next cycle the pin will still
	// be ON the requested action will be performed
	if(!digitalRead(pin) && (InPin[pin]==PINRESET))
	{
		InPin[pin] = PINSET;

		// Copy the value in the memory map
		if(!filteractive && memory_map)
		{
			memory_map[MaCaco_IN_s + slot] = value;	
			return value;		
		}		
	}	
	else if(filteractive && !digitalRead(pin) && InPin[pin]==PINSET)
	{
		// Flag that action is executed
		InPin[pin] = PINACTIVE;

		// Copy the value in the memory map
		if(memory_map)	
		{
			memory_map[MaCaco_IN_s + slot] = value;	
			return value;
		}
	}
	else if(filteractive && digitalRead(pin) && InPin[pin]==PINACTIVE)
		InPin[pin] = PINRELEASED;
	else if(filteractive && digitalRead(pin) && InPin[pin]==PINRELEASED)
		InPin[pin] = PINRESET;
	else if(!filteractive && digitalRead(pin))
		InPin[pin] = PINRESET;
	
	return MaCaco_NODATACHANGED;
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
		if(memory_map)	memory_map[MaCaco_IN_s + slot] = value_state_on;
		
		InPin[pin] = PINSET;
		return value_state_on;
	}
	else if(!digitalRead(pin) && InPin[pin])
	{
		if(memory_map)	memory_map[MaCaco_IN_s + slot] = value_state_off;
		
		InPin[pin] = PINRESET;
		return value_state_off;
	}
	
	return MaCaco_NODATACHANGED;
}

/**************************************************************************
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

	iPinValue = analogRead(pin);    

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

/**************************************************************************
/*!
	Link an hardware pin to the shared memory map, use with latched two state
	pushbutton, active on falling edge
*/	
/**************************************************************************/
U8 Souliss_LowDigIn2State(U8 pin, U8 value_state_on, U8 value_state_off, U8 *memory_map, U8 slot)
{
	// If pin is off, set the "value"
	if(digitalRead(pin)==0 && !InPin[pin])
	{
		if(memory_map)	memory_map[MaCaco_IN_s + slot] = value_state_on;
	 
		InPin[pin] = PINSET;
		return value_state_on;
	}
	else if(digitalRead(pin) && InPin[pin])
	{
		if(memory_map)	memory_map[MaCaco_IN_s + slot] = value_state_off;
	 
		InPin[pin] = PINRESET;
		return value_state_off;
	}
	 
	return MaCaco_NODATACHANGED;
}

/**************************************************************************
/*!
	Link an hardware pin to the shared memory map, active on rising edge
	Identify two states, press and hold.
*/	
/**************************************************************************/
U8 Souliss_DigInHold(U8 pin, U8 value, U8 value_hold, U8 *memory_map, U8 slot, U16 holdtime=1500)
{
	// If pin is on, set the "value"
	if(digitalRead(pin) && (InPin[pin]==PINRESET))
	{
		time = millis();								// Record time
		InPin[pin] = PINSET;
		
		return MaCaco_NODATACHANGED;
	}
	else if(digitalRead(pin) && (abs(millis()-time) > holdtime) && (InPin[pin]==PINSET))
	{
		InPin[pin] = PINACTIVE;								// Stay there till pushbutton is released
		
		// Write timer value in memory map
		if(memory_map)	memory_map[MaCaco_IN_s + slot] = value_hold;

		return value_hold;
	}
	else if(!digitalRead(pin) && (InPin[pin]==PINSET))
	{
		// Write input value in memory map
		if(memory_map)	memory_map[MaCaco_IN_s + slot] = value;
	
		InPin[pin] = PINRESET;
		return value;
	}
	else if(!digitalRead(pin) && (InPin[pin]==PINACTIVE))
		InPin[pin] = PINRESET;		
	
	return MaCaco_NODATACHANGED;
}

/**************************************************************************
/*!
	Link an hardware pin to the shared memory map, active on falling edge
	Identify two states, press and hold.
*/	
/**************************************************************************/
U8 Souliss_LowDigInHold(U8 pin, U8 value, U8 value_hold, U8 *memory_map, U8 slot, U16 holdtime=1500)
{
	// If pin is on, set the "value"
	if(!digitalRead(pin) && !InPin[pin])
	{
		time = millis();								// Record time
		
		InPin[pin] = PINSET;
		return MaCaco_NODATACHANGED;
	}
	else if(!digitalRead(pin) && (abs(millis()-time) > holdtime) && (InPin[pin]==PINSET))
	{
		InPin[pin] = PINRESET;								// Stay there till pushbutton is released
		
		// Write timer value in memory map
		if(memory_map)	memory_map[MaCaco_IN_s + slot] = value_hold;
		
		return value_hold;
	}
	else if(digitalRead(pin) && (InPin[pin]==PINACTIVE))
	{
		// Write input value in memory map
		if(memory_map)	memory_map[MaCaco_IN_s + slot] = value;
	
		InPin[pin] = PINRESET;
		return value;
	}
	
	return MaCaco_NODATACHANGED;
}

/**************************************************************************
/*!
	Read a single precision floating point and store it into the memory_map 
	as half-precision floating point
*/	
/**************************************************************************/
void Souliss_ImportAnalog(U8* memory_map, U8 slot, float* analogvalue)
{
	float16((U16*)(memory_map + MaCaco_IN_s + slot), analogvalue);
}

/**************************************************************************
/*!
	Read an analog input and store it into the memory_map as half-precision
	floating point
*/	
/**************************************************************************/
void Souliss_AnalogIn(U8 pin, U8 *memory_map, U8 slot, float scaling, float bias)
{
	float inval = analogRead(pin);
	
	// Scale and add bias
	inval = bias + scaling * inval;
	
	// Convert from single-precision to half-precision
	float16((U16*)(memory_map + MaCaco_IN_s + slot), &inval);
	
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

