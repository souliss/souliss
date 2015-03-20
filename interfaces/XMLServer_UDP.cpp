/**************************************************************************
	Souliss 
    Copyright (C) 2014  

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
	
	Originally developed by Fulvio Spelta and Dario Di Maio
	
***************************************************************************/
/*!
    \file 
    \ingroup
*/

#include "Typicals.h"
#include "Souliss.h"
#include "frame/MaCaco/MaCaco.h"
#include "frame/vNet/vNet.h"

#include "XMLServer.h"

#if(XMLSERVER && VNET_MEDIA1_ENABLE && ( ETH_W5100 || ETH_W5200 || ETH_W5500))

#include "ASCIItools.c"
#include "BUFFERtools.c"

String incomingURL = String(XML_REQBYTES);			// The GET request is stored in incomingURL
char buf[XML_BUFBYTES];								// Used for temporary operations
uint8_t i, *buff = (uint8_t *)buf;
uint8_t indata=0, bufferlen, data_len;				// End of HTML request

uint8_t	id_for, idx, incoming_ip[4];				// Incoming IP address on the listening UDP port
uint16_t incoming_port;								// Incoming port on the listening UDP port

// Send JSON string
const char* xml[] = {"<s", ">", "</s", "<id", "</id"};

/**************************************************************************
/*!
	Init the interface
*/	
/**************************************************************************/
void XMLSERVERInit(U8 *memory_map)
{
	// Set an internal subscription in order to collect data from other
	// nodes in the network
	
	MaCaco_InternalSubcription(memory_map);
	
	// Open an UDP socket
	if(W5x00.readSnSR(SRV_SOCK1) == SnSR::CLOSED)
	{
		socket(SRV_SOCK1, SnMR::UDP, udpXML_inPORT, 0);	
	}	
}

/**************************************************************************
/*!
	Parse incoming HTTP GET for incoming commands
*/	
/**************************************************************************/
void XMLSERVERInterface(U8 *memory_map)
{
	data_len = W5x00.getRXReceivedSize(SRV_SOCK1);

	// If there are incoming data on the listened port
	if(data_len) 
	{	
		// Include debug functionalities, if required
		#if(XMLSERVER_DEBUG)
			XMLSERVER_LOG("(UDP/XML)Indata from controller<0x");
			XMLSERVER_LOG(data_len,HEX);
			XMLSERVER_LOG(">\r\n");		
		#endif
			
		// Retrieve data from the UDP socket
		data_len = recvfrom(SRV_SOCK1, buff, XML_REQBYTES, incoming_ip, &incoming_port);
		if(!data_len)
		{
			// Discard
			data_len = 0;
		
			// Reset the socket
			close(SRV_SOCK1);
			while(W5x00.readSnSR(SRV_SOCK1) != SnSR::CLOSED);
		
			// Init the socket
			socket(SRV_SOCK1, SnMR::UDP, ETH_PORT, 0);		// Open the socket		
		}
		
		// Include debug functionalities, if required
		#if(XMLSERVER_DEBUG)
			XMLSERVER_LOG("(UDP/XML)Indata <0x");
			XMLSERVER_LOG(data_len,HEX);
			XMLSERVER_LOG(">\r\n");		
		#endif
			
		#if(XMLSERVER_DEBUG)
			XMLSERVER_LOG("(UDP/XML)Instring buffer<");
			for(i=0;i<data_len;i++)
				XMLSERVER_LOG(buf[i]);

			XMLSERVER_LOG(">\r\n");					
		#endif
			
		// Move data into a string for parsing
		incomingURL = "";
		
		// 
		if(data_len < XML_REQBYTES)
		{
			for(i=0;i<data_len && buf[i] !=0;i++)						
				incomingURL = incomingURL + buf[i];	
				
			#if(XMLSERVER_DEBUG)
				XMLSERVER_LOG("(UDP/XML)incomingURL<");
				XMLSERVER_LOG(incomingURL);
				XMLSERVER_LOG(">\r\n");					
			#endif
			
			// Flag the incoming data and quit
			indata=1;	
		}	
		else
		{
			indata=0;
			
			#if(XMLSERVER_DEBUG)
			XMLSERVER_LOG("(UDP/XML)Bad request");
			#endif
		}
	}
	else
	{
		// If needed, restart the socket
		if(W5x00.readSnSR(SRV_SOCK1) != SnSR::UDP)
		{
			socket(SRV_SOCK1, SnMR::UDP, udpXML_inPORT, 0);	
		}		
	
		// Reset
		indata=0;
		
		// Send buffered commands
		command_send();			
	}
	
	// Look for data available from the LASTIN buffer
	if(MaCaco_isLastIn(memory_map))
	{
		#if(XMLSERVER_DEBUG)
			XMLSERVER_LOG("(UDP/XML)<LASTIN");
			XMLSERVER_LOG(">\r\n");								
		#endif		
	
		incomingURL = "GET /status";
		indata=1;
	}
	
	// Parse the incoming data
	if(indata)
    {	
		// Handler1, if /statusrequested from client	
		// an example command is "GET /status "
		if(incomingURL.startsWith("GET /status") || ((incomingURL.indexOf("GET /status",0) > 0)))
		{			
			// Init the buffer
			bufferlen=0;
		
		#if(XMLSERVER_DEBUG)
			XMLSERVER_LOG("(UDP/XML)<GET /status");
			XMLSERVER_LOG(">\r\n");								
		#endif			
	
			// Indentify if there are available data from LASTIN
			if(MaCaco_isLastIn(memory_map))
			{
				id_for = MaCaco_GetLastIn(memory_map);
				idx = MaCaco_GetLastIndex(memory_map, MaCaco_GetLastIn(memory_map));
			}
			else	// No data from LASTIN, load local data
			{
				// Store the node index
				*(memory_map+MaCaco_L_IDX_s) = 0;											// Local node has index 0
								
				// Store the data
				memmove((memory_map+MaCaco_L_OUT_s), (memory_map+MaCaco_OUT_s), MaCaco_SLOT);
			
				id_for = MaCaco_GetLastIn(memory_map);
				idx = MaCaco_GetLastIndex(memory_map, MaCaco_GetLastIn(memory_map));
			}
			
			// Print "<id"
			memmove(&buf[bufferlen],xml[3],strlen(xml[3]));
			bufferlen += strlen(xml[3]);

			// Print id number
			*(unsigned long*)(buf+bufferlen) = (unsigned long)id_for;
			ASCII_num2str((uint8_t*)(buf+bufferlen), DEC, &bufferlen);						
			
			// Print ">"
			memmove(&buf[bufferlen],xml[1],strlen(xml[1]));
			bufferlen += strlen(xml[1]);
			
			for(U8 slot=0;slot<MaCaco_SLOT;slot++)
			{	
					// Print "<s"
					memmove(&buf[bufferlen],xml[0],strlen(xml[0]));
					bufferlen += strlen(xml[0]);
					
					// Print slot number
					*(unsigned long*)(buf+bufferlen) = (unsigned long)slot;
					ASCII_num2str((uint8_t*)(buf+bufferlen), DEC, &bufferlen);						
					
					// Print ">"
					memmove(&buf[bufferlen],xml[1],strlen(xml[1]));
					bufferlen += strlen(xml[1]);

					/* 
						Typicals in group Souliss_T5n need conversion from half-precision float to string
					
							Using LASTIN all typicals in the 5n group shall be consecutive, only the first
							and the last 5n in the node is stored. Mixing different typical between 5n ones
							will result in floating point conversion of not floating point data.
					*/
					if((slot >= *(memory_map+MaCaco_L_TYP5n_s+2*id_for)) && (slot <= *(memory_map+MaCaco_L_TYP5n_s+2*id_for+1)))	
					{
						float f_val;
						
						float32((U16*)(memory_map+(MaCaco_G_OUT_s+(idx*MaCaco_OUTLENGHT)+slot)), &f_val);
						ASCII_float2str((float)f_val, 2, &buf[bufferlen], HTTP_BUFBYTES);
						bufferlen = strlen(buf); 
					} 
					else // All others values are stored as unsigned byte
					{
						// Print "val" value
						*(unsigned long*)(buf+bufferlen) = (unsigned long)memory_map[MaCaco_G_OUT_s+(idx*MaCaco_OUTLENGHT)+slot];
						ASCII_num2str((uint8_t*)(buf+bufferlen), DEC, &bufferlen);						
					}
							
					// Print "</slot"
					memmove(&buf[bufferlen],xml[2],strlen(xml[2]));
					bufferlen += strlen(xml[2]);
					
					// Print slot number
					*(unsigned long*)(buf+bufferlen) = (unsigned long)slot;
					ASCII_num2str((uint8_t*)(buf+bufferlen), DEC, &bufferlen);						
					
					// Print ">"
					memmove(&buf[bufferlen],xml[1],strlen(xml[1]));
					bufferlen += strlen(xml[1]);
			}

			// Print "</id"
			memmove(&buf[bufferlen],xml[4],strlen(xml[4]));
			bufferlen += strlen(xml[4]);

			// Print id number
			*(unsigned long*)(buf+bufferlen) = (unsigned long)id_for;
			ASCII_num2str((uint8_t*)(buf+bufferlen), DEC, &bufferlen);						
			
			// Print ">"
			memmove(&buf[bufferlen],xml[1],strlen(xml[1]));
			bufferlen += strlen(xml[1]);
									
			// Send the UDP/XML frame
			sendto(SRV_SOCK1, buff, bufferlen, incoming_ip, udpXML_outPORT);
			
			// At this point we reset all the subscription channels, in this way next time
			// there will be fresh data
			MaCaco_subscribe_reset();
		}
	
		// Handler2, if /force is requested from client	
		// an example command is "GET /force?id=4&slot=2&val=1 "
		if(incomingURL.startsWith("GET /force") || ((incomingURL.indexOf("GET /force",0) > 0)))
		{			
			// Include debug functionalities, if required
			#if(XMLSERVER_DEBUG)
				XMLSERVER_LOG("(UDP/XML)<GET /force");
				XMLSERVER_LOG(data_len,HEX);
				XMLSERVER_LOG(">\r\n");		
			#endif		
		
			// Find start and end index for callback request
			U8 force  = incomingURL.indexOf("force",0);
			if(incomingURL.indexOf("?id=",force) > 0)
			{						
				U8 id   = incomingURL.indexOf("?id=",force);		
				U8 slot = incomingURL.indexOf("&slot=", id);		
				U8 val_s  = incomingURL.indexOf("&val=", slot);		
				id = incomingURL.substring(id+4, slot).toInt();				// Sum lenght of "?id="
				slot = incomingURL.substring(slot+6, val_s).toInt();		// Sum lenght of "&slot="
								
				// This buffer is used to load values
				U8 valf[MAXVALUES];
				U8 vals[MAXVALUES];
				for(i=0;i<MAXVALUES;i++)	vals[i]=0;
				
				// Identify how many values (up to 5) are provided
				valf[0]  = incomingURL.indexOf(",", val_s);
				if(valf[0] != 0xFF)
				{
					// Buffer used to store offset of values
					for(i=0;i<MAXVALUES;i++)	valf[i]=0;
					
					// Get the offset of all values
					for(i=1;i<MAXVALUES;i++)
						valf[i]  = incomingURL.indexOf(",", valf[i-1]+1);
					
					vals[0] = incomingURL.substring(val_s+5, valf[0]).toInt();			// Sum lenght of "&val="						
					for(i=1;i<MAXVALUES;i++)
						vals[i] = incomingURL.substring(valf[i-1]+1, valf[i]).toInt();	// Sun the lenght of ","
				}
				else
				{		
					// Just one values
					U8 val_f  = incomingURL.indexOf(" ", val_s);					// Command should end with a space	
					
					// Convert to number
					vals[0] = incomingURL.substring(val_s+5, val_f).toInt();	// Sum lenght of "&val="						
				}
			
			
			#if(XMLSERVER_DEBUG)
				XMLSERVER_LOG("(UDP/XML)<GET /force");
				XMLSERVER_LOG(">\r\n");		
				
				XMLSERVER_LOG("(UDP/XML)<id=");
				XMLSERVER_LOG(id,DEC);
				XMLSERVER_LOG(">\r\n");		
				
				XMLSERVER_LOG("(UDP/XML)<slot=");
				XMLSERVER_LOG(slot,DEC);
				XMLSERVER_LOG(">\r\n");		
				
				XMLSERVER_LOG("(UDP/XML)<val=");
				XMLSERVER_LOG(val_s,DEC);
				XMLSERVER_LOG(">\r\n");		
			
				for(i=0;i<MAXVALUES;i++)
				{				
					XMLSERVER_LOG("<");		
					XMLSERVER_LOG(vals[i]);
					XMLSERVER_LOG(">\r\n");		
				}
				XMLSERVER_LOG("id=<");
				XMLSERVER_LOG(id);
				XMLSERVER_LOG(">\r\n");		
				XMLSERVER_LOG("slot=<");
				XMLSERVER_LOG(slot);
				XMLSERVER_LOG(">\r\n");		
			#endif				
			
				// Send a command to the node	
				if((id < MaCaco_NODES) && (id != MaCaco_LOCNODE) && (*(U16 *)(memory_map + MaCaco_ADDRESSES_s + 2*id) != 0x0000))	// If is a remote node, the command act as remote input				
					command_buffer(*(U16 *)(memory_map + MaCaco_ADDRESSES_s + 2*id), slot, vals);
				else if (id == MaCaco_LOCNODE)								// If is a local node (me), the command is written back
				{
					i = 0;
					while((vals[i] != 0) && (i < MAXVALUES))
					{
						#if(XMLSERVER_DEBUG)
							XMLSERVER_LOG("slot-vals=<");
							XMLSERVER_LOG(slot);
							XMLSERVER_LOG(" ");
							XMLSERVER_LOG(vals[i]);
							XMLSERVER_LOG(">\r\n");
						#endif		
						memory_map[MaCaco_IN_s+slot] = vals[i];
						slot++;
						i++;
					}
				}					
			}
			else if(incomingURL.indexOf("?typ=",force) > 0)
			{
				U8 typ_mask;
				U8 typ   = incomingURL.indexOf("?typ=",force);
				U8 val_s  = incomingURL.indexOf("&val=", typ);		
				U8 val_f  = incomingURL.indexOf(" ", val_s);				// Command should end with a space	
				typ = incomingURL.substring(typ+5, val_s).toInt();			// Sum lenght of "?typ="
				val_s = incomingURL.substring(val_s+5, val_f).toInt();		// Sum lenght of "&val="								

			#if(XMLSERVER_DEBUG)
				XMLSERVER_LOG("(UDP/XML)<GET /typ");
				XMLSERVER_LOG(">\r\n");		
			
				XMLSERVER_LOG("(UDP/XML)<val=");
				XMLSERVER_LOG(val_s,DEC);
				XMLSERVER_LOG(">\r\n");		
			#endif		
				
				U8* val_sp = &val_s;
				
				// Send a multicast command to all typicals, first identify if the command is issued
				// for a typical or a typical class
				if((typ & 0x0F) == 0x00)
					typ_mask = 0xF0;	// We look only to the typical class value
				else
					typ_mask = 0xFF;	// We look to whole typical value
				
				// Look for all slot assigned to this typical and put value in
				for(U8 id=0;id<MaCaco_NODES;id++)
				{						
					// Send a command to the node	
					if((id != MaCaco_LOCNODE) && ((*(U16 *)(memory_map + MaCaco_ADDRESSES_s + 2*id)) != 0x0000))	// If is a remote node, the command act as remote input								
						MaCaco_send(*(U16 *)(memory_map + MaCaco_ADDRESSES_s + 2*id), MaCaco_TYP, 0, typ, 1, val_sp);		
					else if (id == MaCaco_LOCNODE)																	// If is a local node (me), the command is written back
					{
						U8 typ_mask;
						
						// Identify if the command is issued for a typical or a typical class
						if((typ & 0x0F) == 0x00)
							typ_mask = 0xF0;	// we look only to the typical class value
						else
							typ_mask = 0xFF;	// we look to whole typical value
					
						for(i=0; i<MaCaco_SLOT; i++)		
							if((*(memory_map + MaCaco_TYP_s + i) & typ_mask) == typ)	// Start offset used as typical logic indication
								*(memory_map+MaCaco_IN_s + i) = val_s;
					}
				}
			}
		}	
		
		// Done
		indata=0;
		incomingURL="";
	}		
}

#endif
