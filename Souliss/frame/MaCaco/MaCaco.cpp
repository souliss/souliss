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
#include "GetConfig.h"								// need : MaCaco_Config.h

U8 MaCaco_data[MaCaco_FRAME] = {0x00}; 				// Data buffer for incoming data
MaCaco_rx_data_t MaCaco_rx;							// Data structure

U8 MaCaco_header[MaCaco_HEADER]  = {0x00};			// Header for output frame				
oFrame Macaco_oFrame;								// Data structure for output frame (local data)
oFrame MacacoUserMode_oFrame;						// Data structure for output frame (data from other nodes)

// store incoming subscription state
U16 subscr_addr[MaCaco_INMAXSUBSCR] = {0x0000};	
U8* subscr_putin[MaCaco_INMAXSUBSCR] = {0x0000};
U8 subscr_startoffset[MaCaco_INMAXSUBSCR] = {0x00};
U8 subscr_numberof[MaCaco_INMAXSUBSCR] = {0x00};
U8 subscr_funcode[MaCaco_INMAXSUBSCR] = {0x00};
U8 subscr_count[MaCaco_INMAXSUBSCR] = {0x00};

// store outgoing subscription state
U16 subscr_outaddr[MaCaco_OUTMAXSUBSCR] = {0x0000};	
bool subscr_status[MaCaco_OUTMAXSUBSCR] = {0};

// store incoming typical logic request
U16 reqtyp_addr = 0x0000;	
U8* reqtyp_putin = 0x0000;
U8 reqtyp_startoffset = 0x00;
U8 reqtyp_numberof = 0x00;
U8 reqtyp_times = 0;

// store incoming addressing request information
U16 randomkeyid = 0;
U16 proposedaddress = 0;

// buffer for temporary use
U8 ipaddrs[4], cmd[4] = {0, MaCaco_NODES, MaCaco_SLOT, MaCaco_INMAXSUBSCR};

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
    Build frame and write to destination device in case of User Mode,
	it use the oFrame structure
*/
/**************************************************************************/
U8 MaCacoUserMode_send(U16 addr, U8 funcode, U8 *putin, U8 startoffset, U8 numberof, U8 l_numberof, U8 u_numberof, U8 *l_data, U8 *u_data)
{
	U8 *frame_pnt, *l_data_pnt, *u_data_pnt, status;
	
	frame_pnt = &MaCaco_header[0];		// Init the pointer for the frame header
	l_data_pnt = l_data;				// Init the pointer for the local data
	u_data_pnt = u_data;				// Init the pointer for the data from other nodes
	
	// Build frame header
	*frame_pnt++ = funcode;
	
	// Read the value as 16 bit and convert the pointer for an 16 bit variable
	*(U16 *)frame_pnt = (U16) putin;
	frame_pnt += sizeof(U16);
	
	*frame_pnt++ = startoffset;	
	*frame_pnt++ = numberof;
		
	// if is an "answer" functional code
	if (((funcode & 0xF0) == 0x10) || ((funcode & 0xF0) == 0x30) || ((funcode & 0xF0) == 0x50) || ((funcode & 0xF0) == 0x70))
	{
		// Define the output frame structure for data from nodes in the network
		oFrame_Define(&MacacoUserMode_oFrame);
	
		// Set the data payload in the frame, these are relevant to data from other nodes in the network
		oFrame_Set(0, u_data_pnt, 0, u_numberof, 0);

		// Define the output frame structure for local data
		oFrame_Define(&Macaco_oFrame);
		
		// Set the header and payload, these are relevant to data from local node
		oFrame_Set(MaCaco_header, l_data_pnt, MaCaco_HEADER, l_numberof, &MacacoUserMode_oFrame);

		// Write to destination device
		status = MaCacoTX(addr, &Macaco_oFrame, l_numberof + u_numberof + MaCaco_HEADER);
	}
	else
	{
		// Define the output frame structure for local data
		oFrame_Define(&Macaco_oFrame);
		
		// Send only the header
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
			U16 nodeoffest, len, u_nodeoffest, u_len;
			
			// These points the local data
			nodeoffest = MaCaco_TYP_s;
			len = MaCaco_TYPLENGHT;				
		
			// Record the request info
			reqtyp_addr = addr;	
			reqtyp_putin = rx->putin;
			reqtyp_startoffset = rx->startoffset;
			reqtyp_numberof = rx->numberof;
				
			// Flag that the request shall be processed for all nodes
			reqtyp_times = MaCaco_NODES;		
		
			// Offset and lenght are calculated automatically while processing the request
			#if(MaCaco_USERMODE && !MaCaco_PASSTHROUGH)
				// We need to identify if data from local node are requested or not, local node has alway index equal to zero
				if((rx->startoffset == 0) && (rx->numberof > 1))
				{
					// We want either local and remote data
					u_nodeoffest = MaCacoUserMode_TYP_s;	
					u_len = ((rx->numberof-1) * MaCaco_TYPLENGHT);
				
					// Send the data for etiher local and remote nodes
					return MaCacoUserMode_send(addr, MaCaco_TYPANS, rx->putin, rx->startoffset, (len + u_len), len, u_len, (nodeoffest + memory_map), (u_nodeoffest + memory_map));				
				}
				else if(rx->startoffset != 0)	// Only data relevant to other nodes (remote) are requested
				{
					// We want either local and remote data
					nodeoffest = MaCacoUserMode_TYP_s + ((rx->startoffset-1) * MaCaco_TYPLENGHT);
					len = ((rx->numberof) * MaCaco_TYPLENGHT);
					
					// Send data for remote nodes
					return MaCaco_send(addr, MaCaco_TYPANS, rx->putin, rx->startoffset, len, (nodeoffest + memory_map));				
				}
				else if((rx->startoffset == 0) && (rx->numberof == 1))	// Only local data are requested
					return MaCaco_send(addr, MaCaco_TYPANS, rx->putin, rx->startoffset, len, (nodeoffest + memory_map));
			#elif(MaCaco_PASSTHROUGH)
				// In passthrough mode data from other nodes are not stored, at this time we can send out only local
				// data, then when other nodes will send back data these will be bridged to the User Interface
				if(rx->putin == 0)
					return MaCaco_send(addr, MaCaco_TYPANS, rx->putin, rx->startoffset, len, (nodeoffest + memory_map));		
			#endif
			
	}
		
	// answer to a subscription request
	if ((rx->funcode == MaCaco_SUBSCRREQ) || (rx->funcode == MaCaco_STATEREQ))
	{	
		// look for the subscription channel
		i = 0;
		while((subscr_addr[i] != addr) && (subscr_addr[i] != 0x0000) && (i < MaCaco_INMAXSUBSCR))
			i++;
			
		// Store the subscription data
		if(i < MaCaco_INMAXSUBSCR)
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
			U16 nodeoffest, len, u_nodeoffest, u_len;
			
			// Force an update request to refresh data
			for(U8 i=0;i<MaCaco_INMAXSUBSCR;i++)
				subscr_count[i] = 0;
			
			// These points the local data
			nodeoffest = MaCaco_OUT_s;
			len = MaCaco_OUTLENGHT;			
			
			// Offset and lenght are calculated automatically while processing the request
			#if(MaCaco_USERMODE && !MaCaco_PASSTHROUGH)
				// We need to identify if data from local node are requested or not, local node has alway index equal to zero
				if((rx->startoffset == 0) && (rx->numberof > 1))
				{
					// We want either local and remote data
					u_nodeoffest = MaCacoUserMode_OUT_s;
					u_len = ((rx->numberof-1) * MaCaco_OUTLENGHT);
				
					// Send the data for etiher local and remote nodes
					return MaCacoUserMode_send(addr, MaCaco_STATEANS, rx->putin, rx->startoffset, len + u_len, len, u_len, (nodeoffest + memory_map), (u_nodeoffest + memory_map));				
				}
				else if(rx->startoffset != 0)	// Only data relevant to other nodes (remote) are requested
				{
					// We want either local and remote data
					nodeoffest = MaCacoUserMode_OUT_s + ((rx->startoffset-1) * MaCaco_OUTLENGHT);
					len = ((rx->numberof) * MaCaco_OUTLENGHT);
					
					// Send data for remote nodes
					return MaCaco_send(addr, MaCaco_STATEANS, rx->putin, rx->startoffset, len, (nodeoffest + memory_map));				
				}
				else if((rx->startoffset == 0) && (rx->numberof == 1))	// Only local data are requested
					return MaCaco_send(addr, MaCaco_STATEANS, rx->putin, rx->startoffset, len, (nodeoffest + memory_map));
				
			#elif(MaCaco_PASSTHROUGH)
				// The nodes contains only local data, send them
				return MaCaco_send(addr, MaCaco_STATEANS, rx->putin, rx->startoffset, len, (nodeoffest + memory_map));
			#endif
		}
	}

	// answer to a data request without subscription
	if (rx->funcode == MaCaco_DATAREQ)
	{	
		U16 nodeoffest, len, u_nodeoffest, u_len;
		
		// If a manual update is requested, force an update request to the remote nodes (if any)
		// to get at next process fresh data
		for(U8 i=0;i<MaCaco_INMAXSUBSCR;i++)
			subscr_count[i] = 0;
				
		// These points the local data
		nodeoffest = MaCaco_OUT_s;
		len = MaCaco_OUTLENGHT;			
			
		// Offset and lenght are calculated automatically while processing the request
		#if(MaCaco_USERMODE && !MaCaco_PASSTHROUGH)
			// We need to identify if data from local node are requested or not, local node has alway index equal to zero
			if((rx->startoffset == 0) && (rx->numberof > 1))
			{
				// We want either local and remote data
				u_nodeoffest = MaCacoUserMode_OUT_s;
				u_len = ((rx->numberof-1) * MaCaco_OUTLENGHT);
				
				// Send the data for etiher local and remote nodes
				return MaCacoUserMode_send(addr, MaCaco_DATAANS, rx->putin, rx->startoffset, len + u_len, len, u_len, (nodeoffest + memory_map), (u_nodeoffest + memory_map));				
			}
			else if(rx->startoffset != 0)	// Only data relevant to other nodes (remote) are requested
			{
				// We want either local and remote data
				nodeoffest = MaCacoUserMode_OUT_s + ((rx->startoffset-1) * MaCaco_OUTLENGHT);
				len = ((rx->numberof) * MaCaco_OUTLENGHT);
					
				// Send data for remote nodes
				return MaCaco_send(addr, MaCaco_DATAANS, rx->putin, rx->startoffset, len, (nodeoffest + memory_map));				
			}
			else if((rx->startoffset == 0) && (rx->numberof == 1))	// Only local data are requested
				return MaCaco_send(addr, MaCaco_DATAANS, rx->putin, rx->startoffset, len, (nodeoffest + memory_map));
		#elif(MaCaco_PASSTHROUGH)	
			// The nodes contains only local data, data from other nodes will came with the subscription reset
			// that we have done at begin of this case
			if(rx->startoffset == 0)
				return MaCaco_send(addr, MaCaco_DATAANS, rx->putin, rx->startoffset, len, (nodeoffest + memory_map));	
		#else
			// The nodes contains only local data, send them
			if((rx->startoffset == 0) && (rx->numberof == 1))
				return MaCaco_send(addr, MaCaco_DATAANS, rx->putin, rx->startoffset, len, (nodeoffest + memory_map));
		#endif			
	}
	
	// answer to a node healty request
	if (rx->funcode == MaCaco_HEALTYREQ)	
		return MaCaco_send(addr, MaCaco_HEALTYANS, rx->putin, rx->startoffset, rx->numberof, (memory_map + MaCaco_HEALTY_s + rx->startoffset));
	
	// answer to a ping request
	if (rx->funcode == MaCaco_PINGREQ)
		return MaCaco_send(addr, MaCaco_PINGANS, rx->putin, 0x00, 0x00, 0x00);

	#if(MaCaco_USERMODE)	
	// answer to a database structure request
	if (rx->funcode == MaCaco_DBSTRUCTREQ)
	{		
		// Count the number of nodes
		U8 nodes = 0;	
		while(((*(U16 *)(memory_map + MaCaco_ADDRESSES_s + 2*nodes)) != 0x0000) && nodes < MaCaco_NODES)
			nodes++;
		
		// Add the actual number of nodes on the database structure frame
		cmd[0] = nodes;								
			
		return MaCaco_send(addr, MaCaco_DBSTRUCTANS, rx->putin, 0x00, 0x04, cmd);
	}
	#endif
	
	#if(MaCaco_USERMODE && VNET_MEDIA1_ENABLE)	
	// answer to a discover request
	if (rx->funcode == MaCaco_DISCOVERREQ)
	{		
		// in the answer we include the IP address
		eth_GetIP(ipaddrs);
	
		// we just need to reply back, like a ping but available only on gateway nodes
		return MaCaco_send(addr, MaCaco_DISCOVERANS, rx->putin, 0x00, 0x04, ipaddrs);	
	}	
	#endif
	
	#if(DYNAMICADDRESSING && MaCaco_USERMODE)	
	// answer to a dimanyc addressing request
	if (rx->funcode == MaCaco_DINADDRESSREQ)
	{	
		// Process new and yet issued requests
		if ((randomkeyid == 0) || (randomkeyid == (U16)rx->putin))
		{
			// If there are no pending request or a yet open pending request
			// proceed and provide an address
			
			// the putin is used ad random key identifier
			randomkeyid = (U16)rx->putin;
			
			// the startoffset is used as media number
			U8 vNetMedia = rx->startoffset;
			
			// identify if there are yet devices on the same media
			U16 nodeaddress=0x0002;
			for(U8 nodes=0;nodes<MaCaco_NODES;nodes++)
				if(vNetMedia == vNet_GetMedia(*(U16 *)(memory_map + MaCaco_ADDRESSES_s + 2*nodes)))
					nodeaddress++;
			
			// define the node address
			nodeaddress = nodeaddress + (vNet_GetAddress(vNetMedia) & vNet_GetSubnetMask(vNetMedia));
			
			// store the proposed address, will be used to identify later if the request
			// is completed
			proposedaddress = nodeaddress;
			
			// send the assigned address
			U8*	nodeaddress_p = (U8 *)(&nodeaddress);
			return MaCaco_send(0xFFFF, MaCaco_DINADDRESSANS, rx->putin, vNetMedia, 0x02, nodeaddress_p);
		}
		else
			return MaCaco_FUNCODE_ERR;		
	}		
	#endif
	
	/*********** Answer ************/
	
	// collect data from answer
	if (((rx->funcode & 0xF0) == 0x10) || ((rx->funcode & 0xF0) == 0x30) || ((rx->funcode & 0xF0) == 0x50))
	{
		#if(MaCaco_USERMODE)	
		// record a join request
		if (rx->funcode == MaCaco_JOINNETWORK)
		{			
			// look for a non user address register
			U8 nodes=0;
			while((((*(U16 *)(memory_map + MaCaco_ADDRESSES_s + 2*nodes)) != addr) && ((*(U16 *)(memory_map + MaCaco_ADDRESSES_s + 2*nodes)) != 0x0000)) && (nodes < MaCaco_NODES))
				nodes++;

			// if the node wasn't recorded, assign it
			if((*(U16 *)(memory_map + MaCaco_ADDRESSES_s + 2*nodes) != addr) && nodes < MaCaco_NODES)
				(*(U16 *)(memory_map + MaCaco_ADDRESSES_s + 2*nodes)) = addr;
	
			// if the join request is from a nodes that previously got an address, flag the
			// request as completed
			for(nodes=0; nodes<MaCaco_NODES; nodes++)
				if(proposedaddress == (*(U16 *)(memory_map + MaCaco_ADDRESSES_s + 2*nodes)))
					randomkeyid = 0;
	
			return MaCaco_FUNCODE_OK;	
		}	
		#endif
	
		#if(DYNAMICADDRESSING && VNET_MEDIA1_ENABLE)	
		
		// set an IP address at runtime
		if (rx->funcode == MaCaco_SETIP)
		{	
			// the payload contains the IPv4 address in the first four bytes
			// then the subnetmask and gateway IP
			U8 i, setbaseipaddr[4];
			U16 vnetaddress = 0;
			
			for(i=0;i<4;i++)
				setbaseipaddr[i] = (*(rx->data+i) & *(rx->data+4+i));
			
			eth_SetBaseIP((uint8_t *)setbaseipaddr);									// Base IP address			
			eth_SetSubnetMask((uint8_t *)(rx->data+4));									// Subnetmask
			eth_SetGateway((uint8_t *)(rx->data+8));									// Gateway IP
			
			// use the last byte from the IP address to define the vNet one
			vnetaddress += (setbaseipaddr[4] & DYNAMICADDR_SUBNETMASK);
			
			vNet_SetAddress(vnetaddress, vNet_GetMedia(vnetaddress));					// Set vNet Address
			vNet_SetSubnetMask(DYNAMICADDR_SUBNETMASK, vNet_GetMedia(vnetaddress));		// Set vNet Subnetmask
			vNet_SetMySuperNode(DYNAMICADDR_GATEWAY, vNet_GetMedia(vnetaddress));		// Set vNet Supernode
		}
		#endif
		
		#if(DYNAMICADDRESSING)	
		// record the dynamic address provided
		if (rx->funcode == MaCaco_DINADDRESSANS)
		{			
			if(rx->numberof + 3 < MaCaco_CONFPARAM) // if the number of payload bytes, plus the size of rx->putin and rx->startoffset
			{
				// load data into the configuration parameters, an upper method will use them
				// for configuration purpose
				U8*	confparameters_p = (memory_map + MaCaco_CONFPARAM);
				
				// store the putin value
				(*(U16 *)confparameters_p) = (U16)rx->putin;
				confparameters_p += sizeof(U16);
				
				// store the startoffset value
				*confparameters_p = rx->startoffset;
				confparameters_p++;
				
				// load payload data
				if(rx->numberof)
					memmove(confparameters_p, rx->data, rx->numberof);
			}
		}
		#endif
		
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
			#if(MaCaco_USERMODE || MaCaco_PASSTHROUGH)
			for(U8 j=0; j<(MaCaco_NODES-1-1); j++) 
			{
				// Send the force
				if(*(U16 *)(memory_map + MaCaco_ADDRESSES_s + 2*(j+1)))											// The space used for data coming from other nodes has index 0 for node 1
					MaCaco_send(*(U16 *)(memory_map + MaCaco_ADDRESSES_s + 2*(j+1)), MaCaco_TYP, 0, rx->startoffset, rx->numberof, rx->data);				
			}
			#endif
									
			return MaCaco_FUNCODE_OK;
			
		break;
		
		// Typical logic answer
		#if(MaCaco_USERMODE)
		case(MaCaco_TYPANS) :
			#if(MaCaco_USERMODE && !MaCaco_PASSTHROUGH)
				// Store the value
				if (((rx->putin >= &memory_map[MaCaco_WRITE_s] && rx->putin <= &memory_map[MaCaco_WRITE_f]) || (rx->putin >= &memory_map[MaCaco_EXT_WRITE_s] && rx->putin <= &memory_map[MaCaco_EXT_WRITE_f])) && (rx->numberof > 0))				
					memmove(rx->putin, rx->data, rx->numberof); // data collected in putin address
			#elif(MaCaco_PASSTHROUGH)
				// Value cannot be stored and shall be sent to the User Interface that requested it
				
				// Identify the node index
				U8 nodeindex;
				for(nodeindex=1; nodeindex<MaCaco_NODES ; nodeindex++)
					if(addr == (*(U16*)(memory_map+MaCaco_ADDRESSES_s+2*nodeindex)))
						break;
					
				if(reqtyp_addr)		// If there is a stored address from a User Interface
					MaCaco_send(reqtyp_addr, MaCaco_TYPANS, reqtyp_putin, nodeindex, rx->numberof, rx->data);								
			#endif
		break;
		#endif
		
		// Subscription answer
		case(MaCaco_SUBSCRANS) :
		
			// Look for the subcription index
			i = 0;
			while ((subscr_outaddr[i] != addr) && i < MaCaco_INMAXSUBSCR)
				i++;
			
			// If no entry was found
			if(i == MaCaco_INMAXSUBSCR)
				return MaCaco_FUNCODE_ERR;
			
			// Set the subcription status flag
			if(subscr_outaddr[i] == addr)
				subscr_status[i] = 1;
			
			// Check the writing permission and store received data			
			#if(MaCaco_USERMODE && !MaCaco_PASSTHROUGH)
				if (((rx->putin >= &memory_map[MaCaco_WRITE_s] && rx->putin <= &memory_map[MaCaco_WRITE_f]) || (rx->putin >= &memory_map[MaCaco_EXT_WRITE_s] && rx->putin <= &memory_map[MaCaco_EXT_WRITE_f])) && (rx->numberof > 0))				
				{
					// Collect data in putin address
					memmove(rx->putin, rx->data, rx->numberof); 
					
					// Send the data to all subscribers
					MaCaco_UserMode_subAnswer(memory_map, 0); 
				}
				else
					return MaCaco_FUNCODE_ERR;
			#elif(MaCaco_PASSTHROUGH)	
				if (rx->putin == 0)		// This flag data subscribed by us as passtrough for the User Interface
				{
					// Identify the node index
					U8 nodeindex;
					for(nodeindex=1; nodeindex<MaCaco_NODES ; nodeindex++)
						if(addr == (*(U16*)(memory_map+MaCaco_ADDRESSES_s+2*nodeindex)))
							break;
					
					// Send the data to all subscribers
					MaCaco_PassThrough_subAnswer(nodeindex, rx->numberof, rx->data);
					
					return MaCaco_FUNCODE_OK;
				}	
				else					// This flag data subscribed for local use
				{
					if ((rx->putin >= &memory_map[MaCaco_WRITE_s] && rx->putin <= &memory_map[MaCaco_WRITE_f]) && (rx->numberof > 0))				
						memmove(rx->putin, rx->data, rx->numberof); // data collected in putin address
					else
						return MaCaco_FUNCODE_ERR;
				}			
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
			#if(MaCaco_USERMODE && !MaCaco_PASSTHROUGH)
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
		status = MaCaco_UserMode_subAnswer(memory_map, data_chg);	
		
	return status;	
}

/**************************************************************************/
/*!
    Senda data to all subscribers nodes, when working in PassThrough UserMode
*/
/**************************************************************************/
U8 MaCaco_PassThrough_subAnswer(U8 startoffset, U8 numberof, U8 *data)
{
	U8 status=0, i=0, j=0;

	// Send data to all subscribers
	while (i < MaCaco_INMAXSUBSCR)
	{
		if(subscr_addr[i]!=0x0000)
		{		
			// Send data to subscribers
			status = MaCaco_send(subscr_addr[i], MaCaco_STATEANS, subscr_putin[i], startoffset, numberof, data);
			
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
			// Next subscription
			i++;
			j=0;
		}
	}
	
	// Return the completition status
	return status;
}

/**************************************************************************/
/*!
    Senda data to all subscribers nodes, when working in UserMode
*/
/**************************************************************************/
U8 MaCaco_UserMode_subAnswer(U8* memory_map, U8* data_chg)
{
	U8 status=0, i=0, j=0;

	// Send data to all subscribers
	while (i < MaCaco_INMAXSUBSCR)
	{
		if(subscr_addr[i]!=0x0000)
		{
			// answer to subscription
			if(subscr_funcode[i] == MaCaco_SUBSCRREQ)
				status = MaCaco_send(subscr_addr[i], MaCaco_SUBSCRANS, subscr_putin[i], subscr_startoffset[i], subscr_numberof[i], (subscr_startoffset[i] + memory_map));				
			else if(subscr_funcode[i] == MaCaco_STATEREQ)
			{			
				U16 nodeoffest, len, u_nodeoffest, u_len;
				
				// These points the local data
				nodeoffest = MaCaco_OUT_s;
				len = MaCaco_OUTLENGHT;			
				
				// Offset and lenght are calculated automatically while processing the request
				#if(MaCaco_USERMODE && !MaCaco_PASSTHROUGH)
					// We need to identify if data from local node are requested or not, local node has alway index equal to zero
					if((subscr_startoffset[i] == 0) && (subscr_numberof[i] > 1))
					{
						// We want either local and remote data
						u_nodeoffest = MaCacoUserMode_OUT_s;
						u_len = ((subscr_numberof[i]-1) * MaCaco_OUTLENGHT);
					
						// Send the data for etiher local and remote nodes
						status = MaCacoUserMode_send(subscr_addr[i], MaCaco_STATEANS, subscr_putin[i], subscr_startoffset[i], len + u_len, len, u_len, (nodeoffest + memory_map), (u_nodeoffest + memory_map));				
					}
					else if(subscr_startoffset[i] != 0)	// Only data relevant to other nodes (remote) are requested
					{
						// We want either local and remote data
						nodeoffest = MaCacoUserMode_OUT_s + ((subscr_startoffset[i]-1) * MaCaco_OUTLENGHT);
						len = (subscr_numberof[i] * MaCaco_OUTLENGHT);
						
						// Send data for remote nodes
						status = MaCaco_send(subscr_addr[i], MaCaco_STATEANS, subscr_putin[i], subscr_startoffset[i], len, (nodeoffest + memory_map));				
					}
					else if((subscr_startoffset[i] == 0) && (subscr_numberof[i] == 1))	// Only local data are requested
						status = MaCaco_send(subscr_addr[i], MaCaco_STATEANS, subscr_putin[i], subscr_startoffset[i], len, (nodeoffest + memory_map));
			#elif(MaCaco_PASSTHROUGH)
				// The nodes contains only local data, send them
						status = MaCaco_send(subscr_addr[i], MaCaco_STATEANS, subscr_putin[i], subscr_startoffset[i], len, (nodeoffest + memory_map));
			#endif
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
    Count for how many nodes typicals values was requested, this is set
	when the User Interface request the typicals values and count for
	the number of nodes to be requested.
*/
/**************************************************************************/
U8 MaCaco_reqtyp()
{	
	if(reqtyp_times)
		return reqtyp_times--;
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
	if(subscr_chnl >= MaCaco_INMAXSUBSCR)
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
				if ((*healty > 0x00) && (MaCaco_send(addr, MaCaco_SUBSCRREQ, putin, startoffset, numberof, 0x00))) 
					(*healty)-=MaCaco_SUBSDECREASE;
								
				*count = MaCaco_SUBMAXHEALTY;	// Delay the next try
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

/**************************************************************************/
/*!
    Return the subscription state, if true a gateway node has subscribed 
	this node
*/
/**************************************************************************/
U8 MaCaco_IsSubscribed()
{
	U8 i=0;
	
	// stops at first subcription
	while((subscr_addr[i]==0) && (i<(MaCaco_INMAXSUBSCR-1)))
		i++;
	
	// returns subscription state
	return (subscr_addr[i]!=0);	
	
}