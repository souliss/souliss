/**************************************************************************
	Souliss - MaCaco Communication Protocol
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
/**************************************************************************/

#include <stdio.h>
#include <string.h>

#include "frame/vNet/vNet.h"
#include "frame/MaCaco/MaCaco.h"
#include "GetConfig.h"			// need : MaCaco_Config.h

U8 MaCaco_data[VNET_MAX_PAYLOAD] = {0x00}; 			// Data buffer for incoming data
MaCaco_rx_data_t MaCaco_rx;							// Data structure

U8 MaCaco_header[MaCaco_HEADER]  = {0x00};			// Header for output frame				
oFrame Macaco_oFrame;								// Data structure for output frame

// store subscription state
U16 subscr_outaddr[MaCaco_MAXSUBSCR] = {0x0000};	
U16 subscr_addr[MaCaco_MAXSUBSCR] = {0x0000};	
U8* subscr_putin[MaCaco_MAXSUBSCR] = {0x0000};
U8 subscr_startoffset[MaCaco_MAXSUBSCR] = {0x00};
U8 subscr_numberof[MaCaco_MAXSUBSCR] = {0x00};
U8 subscr_funcode[MaCaco_MAXSUBSCR] = {0x00};
bool subscr_status[MaCaco_MAXSUBSCR] = {0};
U8 subscr_count[MaCaco_MAXSUBSCR] = {0x00};

U8 cmd[4] = {0, MaCaco_NODES, MaCaco_SLOT, MaCaco_MAXSUBSCR};

#if (MaCaco_DEBUG)
	#define MaCaco_LOG Serial.print
#endif

/**************************************************************************/
/*!
    Send MaCaco frame over the vNet
*/
/**************************************************************************/
U8 MaCacoTX(U16 addr, oFrame* Macaco_oFrame, U8 len)
{
	#if (MaCaco_DEBUG)
	// Print the outgoing message header
	MaCaco_LOG("(MaCaco)<OUT>");
	MaCaco_LOG("<|0x");
	for(U8 i=0;i<MaCaco_HEADER;i++)
	{
		if((i!=1) || (i!=2) || (i!=4))
			{
				MaCaco_LOG(Macaco_oFrame->header[i],HEX);
				MaCaco_LOG("|0x");
			}	
		else if((i==1))
			{
				MaCaco_LOG((U16)Macaco_oFrame->header[i],HEX);
				MaCaco_LOG("|0x");
			}	
		else if((i==5))
			MaCaco_LOG(Macaco_oFrame->header[i],HEX);
	}		
	MaCaco_LOG(">\r\n");
	#endif	
		
	return vNet_Send(addr, Macaco_oFrame, len, MaCaco_PORT); 
}
/**************************************************************************/
/*!
    Parse the input data and check for functional code consistency
*/
/**************************************************************************/
U8 MaCaco_parse(MaCaco_rx_data_t *rx)
{
    U8 i=0, len, func, *data_ptr;

    data_ptr = rx->data;

	#if (MaCaco_DEBUG)
	// Print the outgoing message header
	MaCaco_LOG("(MaCaco)<IN><|0x");
	for(U8 i=0;i<MaCaco_HEADER;i++)
	{
		if((i!=1) || (i!=2) || (i!=4))
			{
				MaCaco_LOG(data_ptr[i],HEX);
				MaCaco_LOG("|0x");
			}	
		else if((i==1))
			{
				MaCaco_LOG((U16)data_ptr[i],HEX);
				MaCaco_LOG("|0x");
			}	
		else if((i==5))
			MaCaco_LOG(data_ptr[i],HEX);
	}		
	MaCaco_LOG(">\r\n");
	#endif	
	
   // parse the buffer and extract funcode, putin and numberof
   
	rx->funcode = *data_ptr++;				// Functional Code
	func = rx->funcode;
	
	// Read the value as 16 bit and convert it to pointer for an 8 bit variable
	rx->putin = (U8 *)(*(U16 *)data_ptr);	// Put in...
	data_ptr += sizeof(U16);
	
	rx->startoffset = *data_ptr++;			// Start offset

	rx->numberof = *data_ptr++;				// Numer of
	len = rx->numberof;	
	
	// if is an "answer" functional code
	if (((func & 0xF0) == 0x10) || ((func & 0xF0) == 0x30) || ((func & 0xF0) == 0x50) || ((func & 0xF0) == 0x70))
		memmove(rx->data, data_ptr, len); // Remove message header from data
		
	// check if functional code is supported	
	while ((MaCaco_funcode[i] != func) && (i < MaCaco_FUNCODE_NO - 1))	
		i++;
	
	if ((MaCaco_funcode[i] == func))
		return func;				// functional code supported
	else	
		return MaCaco_FUNCODE_ERR;	// functional code not supported

}

/**************************************************************************/
/*!
    Build frame and write to destination device, it use the oFrame structure
*/
/**************************************************************************/
U8 MaCaco_send(U16 addr, U8 funcode, U8 *putin, U8 startoffset, U8 numberof, U8 *data)
{
	//U8 frame[MaCaco_HEADER + numberof];
	U8 *frame_pnt, *data_pnt, status;
	
	frame_pnt = &MaCaco_header[0];		// init the pointer
	data_pnt = data;					// init the pointer
	
	// Build frame header
	*frame_pnt++ = funcode;
	
	// Read the value as 16 bit and convert the pointer for an 16 bit variable
	*(U16 *)frame_pnt = (U16) putin;
	frame_pnt += sizeof(U16);
	
	*frame_pnt++ = startoffset;	
	*frame_pnt++ = numberof;
	
	// Define the output frame structure
	oFrame_Define(&Macaco_oFrame);
	
	// if is an "answer" functional code
	if (((funcode & 0xF0) == 0x10) || ((funcode & 0xF0) == 0x30) || ((funcode & 0xF0) == 0x50) || ((funcode & 0xF0) == 0x70))
	{
		// Set the header and data payload in the frame
		oFrame_Set(MaCaco_header, data_pnt, MaCaco_HEADER, numberof, 0);

		// Write to destination device
		status = MaCacoTX(addr, &Macaco_oFrame, numberof + MaCaco_HEADER);
	}
	else
	{
		oFrame_Set(MaCaco_header, 0, MaCaco_HEADER, 0, 0);
		status = MaCacoTX(addr, &Macaco_oFrame, MaCaco_HEADER);
	}	
		
	return status;
}

/**************************************************************************/
/*!
    Analyze received data, send answer and move data to shared memory map
*/
/**************************************************************************/
U8 MaCaco_peruse(U16 addr, MaCaco_rx_data_t *rx, U8 *memory_map)
{
	U8 i=0, typ_mask=0xFF;
	
	/*********** Request ************/	
	
	// answer to a digital read request
	if ((rx->funcode == MaCaco_READREQDIG))
		return MaCaco_send(addr, MaCaco_READANSDIG, rx->putin, rx->startoffset, rx->numberof, (rx->startoffset + memory_map));

	// answer to a analog read request
	if ((rx->funcode == MaCaco_READREQANA))
		return MaCaco_send(addr, MaCaco_READANSANA, rx->putin, rx->startoffset, rx->numberof, (rx->startoffset + memory_map));

	// answer to a typical logic read request
	if ((rx->funcode == MaCaco_TYPREQ))
	{
			//U8 nodeoffest, len;
			U16 nodeoffest, len;
			
			// Offset and lenght are calculated automatically while processing the request
			#if(MaCaco_EXTENDED_MAP)
				nodeoffest = MaCaco_TYP_s + (rx->startoffset * MaCaco_TYPLENGHT);
				len = (rx->numberof * MaCaco_TYPLENGHT);
			#else
				nodeoffest = MaCaco_TYP_s;
				len = MaCaco_TYPLENGHT;
			#endif
			
		return MaCaco_send(addr, MaCaco_TYPANS, rx->putin, rx->startoffset, len, (nodeoffest + memory_map));
	}
		
	// answer to a subscription request
	if ((rx->funcode == MaCaco_SUBSCRREQ) || (rx->funcode == MaCaco_STATEREQ))
	{	
		// look for the subscription channel
		i = 0;
		while((subscr_addr[i] != addr) && (subscr_addr[i] != 0x0000) && (i < MaCaco_MAXSUBSCR))
			i++;
			
		// Store the subscription data
		if(i < MaCaco_MAXSUBSCR)
		{
			subscr_addr[i]  = addr;
			subscr_funcode[i] = rx->funcode;
			subscr_putin[i] = rx->putin;
			subscr_startoffset[i] = rx->startoffset;
			subscr_numberof[i] = rx->numberof;
		}
		else
			return MaCaco_ERR85;
				
		// answer to subscription
		if(rx->funcode == MaCaco_SUBSCRREQ)
			return MaCaco_send(addr, MaCaco_SUBSCRANS, rx->putin, rx->startoffset, rx->numberof, (rx->startoffset + memory_map));
		else if(rx->funcode == MaCaco_STATEREQ)
		{
			U8 nodeoffest, len;
			
			// Offset and lenght are calculated automatically while processing the request
			#if(MaCaco_EXTENDED_MAP)
				nodeoffest = MaCaco_OUT_s + (rx->startoffset * MaCaco_OUTLENGHT);
				len = (rx->numberof * MaCaco_OUTLENGHT);
			#else
				nodeoffest = MaCaco_OUT_s;
				len = MaCaco_OUTLENGHT;
			#endif
						
			return MaCaco_send(addr, MaCaco_STATEANS, rx->putin, rx->startoffset, len, (nodeoffest + memory_map));					
		}
	}

	// answer to a data request without subscription
	if (rx->funcode == MaCaco_DATAREQ)
	{	
		U8 nodeoffest, len;
		
		// If a manual update is requested, force an update request to the remote nodes (if any)
		// to get at next process fresh data
		for(U8 i=0;i<MaCaco_MAXSUBSCR;i++)
			subscr_count[i] = 0;
		
		// Offset and lenght are calculated automatically while processing the request
		#if(MaCaco_EXTENDED_MAP)
			nodeoffest = MaCaco_OUT_s + (rx->startoffset * MaCaco_OUTLENGHT);
			len = (rx->numberof * MaCaco_OUTLENGHT);
		#else
			nodeoffest = MaCaco_OUT_s;
			len = MaCaco_OUTLENGHT;
		#endif
						
		return MaCaco_send(addr, MaCaco_DATAANS, rx->putin, rx->startoffset, len, (nodeoffest + memory_map));					
	
	}
	
	// answer to a node healty request
	if (rx->funcode == MaCaco_HEALTYREQ)	
		return MaCaco_send(addr, MaCaco_HEALTYANS, rx->putin, rx->startoffset, rx->numberof, (memory_map + MaCaco_HEALTY_s + rx->startoffset));
	
	// answer to a ping request
	if (rx->funcode == MaCaco_PINGREQ)
	{		
		return MaCaco_send(addr, MaCaco_PINGANS, rx->putin, 0x00, 0x00, 0x00);
	}

	// answer to a database structure request
	if (rx->funcode == MaCaco_DBSTRUCTREQ)
	{		
		// Count the number of nodes
		U8 nodes = 0;	
		while(((U16)(memory_map[MaCaco_ADDRESSES_s+nodes*2]) != 0x0000) && nodes < MaCaco_NODES)
			nodes++;
		
		// Add the actual number of nodes on the database structure frame
		cmd[0] = nodes;								
			
		return MaCaco_send(addr, MaCaco_DBSTRUCTANS, rx->putin, 0x00, 0x04, cmd);
	}
	
	/*********** Answer ************/
	
	// collect data from answer
	if (((rx->funcode & 0xF0) == 0x10) || ((rx->funcode & 0xF0) == 0x30) || ((rx->funcode & 0xF0) == 0x50))
	{		
		// Return for not expected functional codes
		if((rx->funcode == MaCaco_STATEANS))	// This functional codes is used only by user interfaces
			return MaCaco_FUNCODE_ERR;
	
		// check for operation type to separate bit-wise and not bit-wise operation
		switch(rx->funcode) 
		{
		// force register operation
		case(MaCaco_FORCEREGSTR) :
		
			memory_map += rx->startoffset;
			memmove(memory_map, rx->data, rx->numberof);
			return MaCaco_FUNCODE_OK;
		
		break;
		
		// bit-wise AND operation
		case(MaCaco_FORCEREGAND) :
			// no more than one register can be addressed with this functional code
			if (rx->numberof > 1)
				return MaCaco_ERR84;
			else if (rx->startoffset >= MaCaco_WRITE_s && rx->startoffset <= MaCaco_WRITE_f)
				{
					memory_map += rx->startoffset;	
					*memory_map &= *rx->data;	//bit-wise AND
					return MaCaco_FUNCODE_OK;	
				}	
		break;
		
		// bit-wise OR operation
		case(MaCaco_FORCEREGOR) :
			// no more than one register can be addressed with this functional code
			if (rx->numberof > 1)			
				return MaCaco_ERR84;
			else if (rx->startoffset >= MaCaco_WRITE_s && rx->startoffset <= MaCaco_WRITE_f)
				{
					memory_map += rx->startoffset;	
					*memory_map |= *rx->data;	//bit-wise OR
					return MaCaco_FUNCODE_OK;
				}
		break;

		// force by id
		case(MaCaco_FORCE) :
		
			// Offset is calculated while processing the request, the startoffset is the node address
			// in the memorymap
			
			if(rx->startoffset == MaCaco_LOCNODE)
			{
				memory_map += MaCaco_IN_s;
				memmove(memory_map, rx->data, rx->numberof);
				return MaCaco_FUNCODE_OK;
			}
			else	// Data shall be sent to a remote node
				return MaCaco_send(*(U16 *)(memory_map+MaCaco_ADDRESSES_s+rx->startoffset*2), MaCaco_FORCEREGSTR, 0, MaCaco_IN_s, rx->numberof, rx->data);
				
		break;

		// force by typical logic number
		case(MaCaco_TYP) :
	
			// identify if the command is issued for a typical or a typical class
			if((rx->startoffset & 0x0F) == 0x00)
				typ_mask = 0xF0;	// we look only to the typical class value
			else
				typ_mask = 0xFF;	// we look to whole typical value

			// force typical on the local node
			for(U8 i=0; i<(MaCaco_TYPLENGHT - rx->numberof + 1); i++)		
				if((*(memory_map + MaCaco_TYP_s + i) & typ_mask) == rx->startoffset)	// Start offset used as typical logic indication
					for(U8 j=0; j < rx->numberof; j++)
						*(memory_map+MaCaco_IN_s + i + j) = *(rx->data + j);

			// force typical on the remote node
			#if(MaCaco_EXTENDED_MAP)
			for(U8 j=1; j<MaCaco_NODES; j++)
				for(U8 i=0; i<MaCaco_TYPLENGHT; i++)				
					if((*((j*MaCaco_TYPLENGHT) + memory_map + MaCaco_TYP_s + i) & typ_mask) == rx->startoffset)	// Start offset used as typical logic indication
					{
						// Send the force
						MaCaco_send(*(U16 *)(memory_map + MaCaco_ADDRESSES_s + 2*j), MaCaco_TYP, 0, rx->startoffset, rx->numberof, rx->data);				
						break;
					}
			#endif
									
			return MaCaco_FUNCODE_OK;
			
		break;
		
		// Subscription answer
		case(MaCaco_SUBSCRANS) :
		
			// Look for the subcription index
			i = 0;
			while ((subscr_outaddr[i] != addr) && i < MaCaco_MAXSUBSCR)
				i++;
			
			// If no entry was found
			if(i == MaCaco_MAXSUBSCR)
				return MaCaco_FUNCODE_ERR;
			
			// Set the subcription status flag
			if(subscr_outaddr[i] == addr)
				subscr_status[i] = 1;
			
			// Check the writing permission and store received data			
			#if(MaCaco_EXTENDED_MAP)
				if (((rx->putin >= &memory_map[MaCaco_WRITE_s] && rx->putin <= &memory_map[MaCaco_WRITE_f]) || (rx->putin >= &memory_map[MaCaco_EXT_WRITE_s] && rx->putin <= &memory_map[MaCaco_EXT_WRITE_f])) && (rx->numberof > 0))				
					memmove(rx->putin, rx->data, rx->numberof); // data collected in putin address
				else
					return MaCaco_FUNCODE_ERR;
			#else
				if ((rx->putin >= &memory_map[MaCaco_WRITE_s] && rx->putin <= &memory_map[MaCaco_WRITE_f]) && (rx->numberof > 0))				
					memmove(rx->putin, rx->data, rx->numberof); // data collected in putin address
				else
					return MaCaco_FUNCODE_ERR;
			#endif
				
			return MaCaco_FUNCODE_OK;
			
		break;
		
		// All others functional codes
		default :
			
			// Check the writing permission and store received data
			#if(MaCaco_EXTENDED_MAP)
				if (((rx->putin >= &memory_map[MaCaco_WRITE_s] && rx->putin <= &memory_map[MaCaco_WRITE_f]) || (rx->putin >= &memory_map[MaCaco_EXT_WRITE_s] && rx->putin <= &memory_map[MaCaco_EXT_WRITE_f])) && (rx->numberof > 0))				
					memmove(rx->putin, rx->data, rx->numberof); // data collected in putin address
			#else
				if ((rx->putin >= &memory_map[MaCaco_WRITE_s] && rx->putin <= &memory_map[MaCaco_WRITE_f]) && (rx->numberof > 0))				
					memmove(rx->putin, rx->data, rx->numberof); // data collected in putin address			
			#endif
			
			return MaCaco_FUNCODE_OK;
		break;
		}	
	}	
}

/**************************************************************************/
/*!
	Look for incoming data from all the available vNet media	
 */
/**************************************************************************/
void MaCaco_DataIn()
{
	// If there are no pending incoming data, look for new incoming data
	if(!MaCaco_rx.datain)
		MaCaco_rx.datain = vNet_DataAvailable();

	else
	{	
		MaCaco_rx.datain--;		// Update the counter
		if(!MaCaco_rx.datain) MaCaco_rx.datain = vNet_DataAvailable();
	}
}
/**************************************************************************/
/*!
    Read data from the network and call protocol functions, change this 
	function to use a different hardware protocol.
	
	Multiple memory maps management is not allowed.
*/
/**************************************************************************/
U8 MaCaco_retrieve(U8* memory_map, U8* data_chg)
{
	U8 used_media=0, len, status=0;
	U16 src_addr = 0x0000;

	// If data are available
	if(MaCaco_rx.datain > 0)
	{
		// get data from data buffer
		len = vNet_RetrieveData(&MaCaco_data[0]);		
		
		// parse the received message
		if ((len != VNET_DATA_FAIL) && (len != VNET_DATA_ROUTED))
		{				
			// get source address
			used_media = vNet_GetLastMedia();
			src_addr = vNet_GetoSourceAddress(used_media);

			MaCaco_rx.data = &MaCaco_data[0];
			status = MaCaco_parse(&MaCaco_rx);
			
			// analyze data and take action as per protocol
			if (status != MaCaco_FUNCODE_ERR)
			{
					MaCaco_peruse(src_addr, &MaCaco_rx, memory_map);
					
					// If the functional code is a subscription answer, some subscribed data are
					// changed, so the same data can be spread to my subscribers
					if(MaCaco_rx.funcode == MaCaco_SUBSCRANS)
						MaCaco_subscriptionanswer(memory_map, 0);
						
					return MaCaco_rx.funcode;
			}
			else
				status = MaCaco_FUNCODE_ERR;
		}
		else
			status = MaCaco_NODATARECEIVED;
	}

	// if there was a change in the memory map, send data to subscriptors
	if (*data_chg == MaCaco_DATACHANGED)
		status = MaCaco_subscriptionanswer(memory_map, data_chg);

	return status;	
}

/**************************************************************************/
/*!
    Senda data to all subscribers nodes
*/
/**************************************************************************/
U8 MaCaco_subscriptionanswer(U8* memory_map, U8* data_chg)
{
	U8 status=0, i=0, j=0;

	// Send data to all subscribers
	while (i < MaCaco_MAXSUBSCR)
	{
		if(subscr_addr[i]!=0x0000)
		{
			// answer to subscription
			if(subscr_funcode[i] == MaCaco_SUBSCRREQ)
				status = MaCaco_send(subscr_addr[i], MaCaco_SUBSCRANS, subscr_putin[i], subscr_startoffset[i], subscr_numberof[i], (subscr_startoffset[i] + memory_map));				
			else if(subscr_funcode[i] == MaCaco_STATEREQ)
			{
				U8 nodeoffest, len;
				
				// Calculate offset and lenght
				#if(MaCaco_EXTENDED_MAP)
					nodeoffest = MaCaco_OUT_s + (subscr_startoffset[i] * MaCaco_OUTLENGHT);
					len = (subscr_numberof[i] * MaCaco_OUTLENGHT);
				#else
					nodeoffest = MaCaco_OUT_s;
					len = MaCaco_OUTLENGHT;
				#endif
					
				status = MaCaco_send(subscr_addr[i], MaCaco_STATEANS, subscr_putin[i], subscr_startoffset[i], len, (nodeoffest + memory_map));					
			}		
		
			// If data are successfully sent, go to next subscription					
			if(status)
			{
				i++;
				j=0;
			}	
			else
				j++;
		}
		else
		{
			i++;
			j=0;
		}	

		// Retry no more that MAXRETRY number
		if(j>Macaco_MAXRETRY)
		{
			// If connection failed, remove the subscription record
			subscr_addr[i] = 0x0000;
			
			// Next subscription
			i++;
			j=0;
		}
	}
		
	if(data_chg) *data_chg = MaCaco_NODATACHANGED;
	status = MaCaco_NODATARECEIVED;
	
	return status;
}

/**************************************************************************/
/*!
    Activate a subscription on remote device, send periodically subscription
	request based on communication healty. At first run, healty value should
	be greater than MaCaco_SUBSCRHEALTY.
	
	For each subscripted device, an unique subscribe_addr (starting from 0x00)
	should be used, subscribed data will be stored in the putin address.
	
	Multiple subscribed memory maps management is allowed for subscribed data, 
	the subscribed memory maps are not shared with other network devices and
	can be (or not) different from the shared memory map.
*/
/**************************************************************************/
U8 MaCaco_subscribe(U16 addr, U8 *memory_map, U8 *putin, U8 startoffset, U8 numberof, U8 subscr_chnl)
{
	U8 	i, used_media, *healty, *count = 0;
	U16 src_addr = 0x0000;

	// Verify the subscription index
	if(subscr_chnl >= MaCaco_MAXSUBSCR)
		return MaCaco_NOSUBSCRANSWER;
	
	// Init the pointers
	healty = memory_map+MaCaco_HEALTY_s+subscr_chnl;
	count = subscr_count+subscr_chnl;
	
	// Record the output subcription
	subscr_outaddr[subscr_chnl] = addr;
	
	// If an answer was recorded, increase healty value
	if (subscr_status[subscr_chnl] == 1)
	{
		if (*healty < MaCaco_SUBSCRHEALTY)
			*healty = 2*MaCaco_SUBSCRHEALTY;
		else
		{
			if (*healty < (MaCaco_SUBMAXHEALTY-MaCaco_SUBSINCREASE)) 
				(*healty)+=MaCaco_SUBSINCREASE;
			else
				(*healty)=MaCaco_SUBMAXHEALTY;
		}
		
		// Update the delay counter
		*count = *healty;
		
		// Reset subscription status flag
		subscr_status[subscr_chnl] = 0;
	}	
	else
	{
		// if delay is expired subsript again and decrease healty
		if ((*count < MaCaco_SUBSCRHEALTY))
			{
				//MaCaco_send(addr, MaCaco_SUBSCRREQ, putin, startoffset, numberof, 0x00);
				if ((*healty > 0x00) && (MaCaco_send(addr, MaCaco_SUBSCRREQ, putin, startoffset, numberof, 0x00))) 
					(*healty)-=MaCaco_SUBSDECREASE;
				else
					*count = MaCaco_SUBMAXHEALTY;	// If channel is not healty, delay the send request
			}
		else
			if (*count > 0x00) (*count)--;
	}		

	// return if channel is healty or failed
	if (*healty < MaCaco_SUBSCRHEALTY) 		
		return MaCaco_NOSUBSCRANSWER;
	else
		return MaCaco_SUBSCRANSWER;
		
}

/**************************************************************************/
/*!
    Retrieve the latest functional code
*/
/**************************************************************************/
U8 MaCaco_getfuncode()
{
	return MaCaco_rx.funcode;
}

/**************************************************************************/
/*!
    Retrieve the latest putin address
*/
/**************************************************************************/
U16 MaCaco_getputin()
{
	return (U16)MaCaco_rx.putin;
}

/**************************************************************************/
/*!
    Retrieve the latest startoffset value
*/
/**************************************************************************/
U8 MaCaco_getstartoffset()
{
	return MaCaco_rx.startoffset;
}

/**************************************************************************/
/*!
    Retrieve the latest numberof value
*/
/**************************************************************************/
U8 MaCaco_getnumberof()
{
	return MaCaco_rx.numberof;
}

/**************************************************************************/
/*!
    Retrieve the latest datain value
*/
/**************************************************************************/
U8 MaCaco_getdatain()
{
	return MaCaco_rx.datain;
}


/**************************************************************************/
/*!
    Get the buffer pointer
*/
/**************************************************************************/
U16 MaCaco_getdatabuffer()
{
	return (U16)(&MaCaco_data[0]);
}