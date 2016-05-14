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

U8 roundrob_1=1,roundrob_2=1, timeout=TIMEOUT_SET;

/**************************************************************************/
/*!
	Define a communication subscription channel between two nodes
	
	It use MaCaco and vNet to retrieve data from another node in the network
	incoming data are placed in input_slot, the requested data are numof_slot
	bytes starting from output_slot.
	
	Each subscription require an index, called subscr_chnl, and healthy status
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
	if (*(memory_map+MaCaco_HEALTHY_s+subscr_chnl) == 0)
		*(memory_map+MaCaco_HEALTHY_s+subscr_chnl) = MaCaco_SUBINITHEALTHY;
	
	// Subscribe data
	return MaCaco_subscribe(addr, memory_map, input_slot, MaCaco_OUT_s + output_slot, numof_slot, subscr_chnl);
}

/**************************************************************************/
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
	if (*(memory_map+MaCaco_HEALTHY_s+roundrob_2) == 0)
		*(memory_map+MaCaco_HEALTHY_s+roundrob_2) = MaCaco_SUBINITHEALTHY;
	
	// Check subscription has been reset
	if((roundrob_2 > 1) && (MaCaco_subscribe_is_init()))
		roundrob_2=1;		// Start from first remote node
		
	// Handle the subscription in round robin
	if(roundrob_2 < MaCaco_NODES)
	{
		// Open and/or check one communication channel at each round
		if (((C8TO16(memory_map+MaCaco_ADDRESSES_s+2*roundrob_2)) != 0x0000))
			ret = MaCaco_subscribe((C8TO16(memory_map+MaCaco_ADDRESSES_s+2*roundrob_2)), memory_map, 0, MaCaco_OUT_s, MaCaco_SUBSCRLEN, roundrob_2);		// Use putin as zero to flag a passthrough		
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

/**************************************************************************/
/*!
	Define a Communication Channel as battery operated
*/	
/**************************************************************************/	
void Souliss_BatteryChannels(U8 *memory_map, U16 addr)
{
	for(U8 i=0;i<MaCaco_NODES;i++)
		if(((C8TO16(memory_map+MaCaco_ADDRESSES_s+2*roundrob_2)) == addr))
		{
			MaCaco_subscribe_battery(i);
			return;
		}	
}

/**************************************************************************/
/*!
	Get definitions for typical from multiple remote devices
	
	Do same job of Souliss_GetTypical for more nodes at same time, using
	round robin criteria. The data are requested only once, a periodical
	refresh can be achieved using the Souliss_RefreshTypicals() method.
*/	
/**************************************************************************/	
U8 Souliss_GetTypicals(U8 *memory_map)
{ 
	U8 s=MaCaco_reqtyp();
	
	if(s)
	{
		// Pointer to the node address
		U16 m_addr = C8TO16(memory_map+MaCaco_ADDRESSES_s+2*roundrob_1);
		
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
			m_addr = C8TO16(memory_map+MaCaco_ADDRESSES_s+2*roundrob_1);
		}
		
		// If the node answer has been received
		if((m_addr != 0x0000) && (m_addr == MaCaco_reqtyp_lastaddr()))
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
		if (m_addr != 0x0000)	MaCaco_send(m_addr, MaCaco_TYPREQ, 0, MaCaco_TYP_s, MaCaco_TYPLENGHT, 0x00);			
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

/**************************************************************************/
/*!
	Record into a node a subscription at code time, it has the same effect
	of a runtime subscription but avoids long wait times.
	
	This is useful for battery operated (or generally devices not running 24h)
	that may never match the subscription request because sleeping at time.
*/	
/**************************************************************************/	
void Souliss_HardcodedCommunicationChannel(U16 gateway_addr)
{
	if(gateway_addr)
		MaCaco_subscribe_record(gateway_addr, MaCaco_SUBSCRREQ, 0, MaCaco_OUT_s, MaCaco_SUBSCRLEN);
}
	
/**************************************************************************/
/*!
	Write on network node, act as a remote input
	
	It write directly into the inputs map of another node, data are moved
	directly without a subscription, so no channel healthy is provided.
*/	
/**************************************************************************/	
U8 Souliss_RemoteInput(U16 addr, U8 slot, U8 command)
{
	U8 cmd = command;
	return MaCaco_send(addr, MaCaco_FORCEREGSTR, 0x00, MaCaco_IN_s + slot, 1, &cmd);
}

/**************************************************************************/
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

/**************************************************************************/
/*!
	Apply the command on all the logic assigned to that typical in the node
*/	
/**************************************************************************/	
U8 Souliss_MassiveCommand(U16 addr, U8 typ, U8 command)
{
	U8 cmd = command;
	return MaCaco_send(addr, MaCaco_FORCEREGSTR, 0x00, typ, 1, &cmd); 
}

/**************************************************************************/
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
    Publish an event in broadcast
*/
/**************************************************************************/
U8 Souliss_Publish(U8 *memory_map, U16 message, U8 action)
{
	// Disable the broadcast delay
	if(VNET_BROADCAST_DEFAULT) vNet_BroadcastDelay(VNET_BROADCAST_DISABLE);
	
	U8 ret = MaCaco_send(0xFFFF, MaCaco_ACTIONMSG, message, action, 0, 0);

	// Enable the broadcast delay
	if(VNET_BROADCAST_DEFAULT) vNet_BroadcastDelay(VNET_BROADCAST_ENABLE);
			
	return ret;
}

/**************************************************************************/
/*!
    Publish an event in multicast
*/
/**************************************************************************/
U8 Souliss_MulticastPublish(U16 multicast_addr, U8 *memory_map, U16 message, U8 action)
{
	// Disable the broadcast delay
	if(VNET_BROADCAST_DEFAULT) vNet_BroadcastDelay(VNET_BROADCAST_DISABLE);
	
	U8 ret =  MaCaco_send(multicast_addr, MaCaco_ACTIONMSG, message, action, 0, 0);
	
	// Enable the broadcast delay
	if(VNET_BROADCAST_DEFAULT) vNet_BroadcastDelay(VNET_BROADCAST_ENABLE);
			
	return ret;	
}

/**************************************************************************/
/*!
    Publish data in broadcast
*/
/**************************************************************************/
U8 Souliss_PublishData(U8 *memory_map, U16 message, U8 action, U8* data, U8 message_len)
{
	// Disable the broadcast delay
	if(VNET_BROADCAST_DEFAULT) vNet_BroadcastDelay(VNET_BROADCAST_DISABLE);
	
	U8 ret =   MaCaco_send(0xFFFF, MaCaco_ACTIONMSG, message, action, message_len, data);
	
	// Enable the broadcast delay
	if(VNET_BROADCAST_DEFAULT) vNet_BroadcastDelay(VNET_BROADCAST_ENABLE);
			
	return ret;		
}

/**************************************************************************/
/*!
    Publish data in multicast
*/
/**************************************************************************/
U8 Souliss_MulticastPublishData(U16 multicast_addr, U8 *memory_map, U16 message, U8 action, U8* data, U8 message_len)
{
	// Disable the broadcast delay
	if(VNET_BROADCAST_DEFAULT) vNet_BroadcastDelay(VNET_BROADCAST_DISABLE);
	
	U8 ret =    MaCaco_send(multicast_addr, MaCaco_ACTIONMSG, message, action, message_len, data);
	
	// Enable the broadcast delay
	if(VNET_BROADCAST_DEFAULT) vNet_BroadcastDelay(VNET_BROADCAST_ENABLE);
			
	return ret;	
}

/**************************************************************************/
/*!
    Return if there is a matching subscribed event
*/
/**************************************************************************/
U8 Souliss_Subscribe(U8 *memory_map, U16 message, U8 action)
{
	// action message are in the queue
	U8*	confparameters_p = (memory_map + MaCaco_QUEUE_s);
	
	if((C8TO16(confparameters_p) == message) && (*(confparameters_p+sizeof(U16)) == action))
	{
		#if (SOULISS_DEBUG)
		// Print debug messages
		SOULISS_LOG(F("(ss)<Sub>"));
		SOULISS_LOG(F("<|0x"));
		SOULISS_LOG(C8TO16(confparameters_p),HEX);
		SOULISS_LOG(F("|0x"));
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
    Return if there is a matching subscribed event
*/
/**************************************************************************/
U8 Souliss_SubscribeData(U8 *memory_map, U16 message, U8 action, U8* data, U8* len)
{
	// action message are in the queue
	U8*	confparameters_p = (memory_map + MaCaco_QUEUE_s);
	
	// Get the message value
	U16 _message=C8TO16(confparameters_p);
	confparameters_p+=sizeof(U16);
	
	// Get the action value
	U8  _action=*confparameters_p;
	confparameters_p++;
	
	if((_message == message) && (_action == action))
	{
		#if (SOULISS_DEBUG)
		// Print debug messages
		SOULISS_LOG(F("(ss)<Sub>"));
		SOULISS_LOG(F("<|0x"));
		SOULISS_LOG(_message,HEX);
		SOULISS_LOG(F("|0x"));
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

/**************************************************************************/
/*!
    Request data from another node at every run
*/
/**************************************************************************/
U8 Souliss_PullData(U16 addr, U8 slot, U8 remote_slot, U8 remote_numbof)
{
	return MaCaco_send(addr, MaCaco_READREQDIG, (U16)slot, MaCaco_OUT_s + remote_slot, remote_numbof, 0);
}

/**************************************************************************/
/*!
	Retrieve the data from the communication channels
*/	
/**************************************************************************/
U8 Souliss_CommunicationData(U8 *memory_map, U8 *trigger)
{
	#if(MaCaco_SUBSCRIBERS)
	// If not yet, init the communication channel
	if (*(memory_map+MaCaco_HEALTHY_s+0) == 0)
		*(memory_map+MaCaco_HEALTHY_s+0) = MaCaco_SUBMAXHEALTY;
	#endif
	
	MaCaco_DataIn();
	U8 ret = MaCaco_retrieve(memory_map, trigger);
	
	#if (SOULISS_DEBUG)
	// Look for input values
	U8 i=0;
	for(i=0;i<MaCaco_WRITE_f;i++)
		if(*(memory_map+MaCaco_WRITE_s+i) !=0)
			break;
	
	if((*trigger) && (i<(MaCaco_WRITE_f-MaCaco_WRITE_s)))
	{
		// Print debug messages
		SOULISS_LOG(F("(ss)<MAP>"));
		SOULISS_LOG(F("<|0x"));
		for(i=0;i<MaCaco_MEMMAP;i++)
		{
			SOULISS_LOG(memory_map[i],HEX);
			SOULISS_LOG(F("|0x"));
		}
		SOULISS_LOG(">\r\n");
	}	
	#endif

	return ret;
}

/**************************************************************************/
/*!
	Build a distributed watchdog using a chain of devices
	
	Each device check the healthy status of the next device, and issue an
	alarm in the output slot if the watchdog timer expire.
	
	The same slot shall be used for all the nodes of the chain.
	
	This method shall run at a slow rate, it use one memory slot.
*/	
/**************************************************************************/
U8 Souliss_Watchdog(U8 *memory_map, U16 chain_address, U8 chain_slot, U8 alarm_command)
{	
	// Watchdog, check the healthy of the previous node of the chain
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
	MaCaco_send(chain_address, MaCaco_FORCEREGSTR, 0x00, MaCaco_IN_s + chain_slot, 1, &cmd);
	
	// Return the alarm state
	return memory_map[MaCaco_OUT_s + chain_slot];	
}

/**************************************************************************/
/*!
	At this level the stack doesn't know the source address, so if somenthing
	fails, just broadcast a general error
*/	
/**************************************************************************/
U8 Souliss_UnsupportedCommand()
{
	return MaCaco_send(VNET_ADDR_BRDC, MaCaco_ERR83, 0, 0, 0, 0);
}
