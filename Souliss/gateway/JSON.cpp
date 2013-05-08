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
#include "frame/MaCaco/MaCaco.h"
#include "frame/vNet/vNet.h"

#include "JSON.h"

#if(JSONSERVER && VNET_MEDIA1_ENABLE && ETH_W5100)

String incomingURL = String(JSON_MAXBYTES);			// The GET request is stored in incomingURL
char buf[JSON_MAXBYTES];							// Used for temporary operations
uint8_t *buff = (uint8_t *)buf;
uint8_t nodes, indata=0, bufferlen, data_len;		// End of HTML request

// Send JSON string
const char* json[] = {"({[", "{\"id\" : \"", "\", \"hlt\" : \"", "\", \"slot\" : [", 
					"{ \"typ\" : \"", "\", \"val\" : \"", "\"}", ",", "]}", "]]})", 
					"({\"id\" : [", "({})", "{\"hlt\" : \""};

/**************************************************************************
/*!
	Parse incoming HTTP GET request and reply with a JSON strings containing
	the database structure and relevant data
	
	Use 
	-	/status, /status?all, /status?id	to get data
	-	/force, /typ, /reset				to force data
*/	
/**************************************************************************/
void JSONServer(U8 *memory_map)
{	
	U8 typ_mask=0xFF;
	
	// Set the socket
	srvcln_setsocket(SRV_SOCK1);
	
	// Look for available data on the listening port
	data_len = srvcln_dataavailable(ETH_JSON);
	
	// If there are incoming data on the listened port
	if(data_len) 
	{	
		// If the socket is connected and the data size doesn't exceed the buffer
		if (srvcln_connected(ETH_JSON))													// If data are available
		{
			// Retrieve data from the Ethernet controller
			if (data_len > JSON_MAXBYTES) 
				data_len = JSON_MAXBYTES;
				
			srvcln_retrieve(buff, data_len);	
				
			// Flag the incoming data
			if(data_len)
				indata=1;
			
			// Move data into a string for parsing
			incomingURL = "";
			for(U8 i=0;i<data_len;i++)
			{
				// Stop at next space after GET
				if(incomingURL.startsWith("GET /") && buf[i] == 32)
					break;
					
				incomingURL = incomingURL + buf[i];	
			}
		}	
		else
		{
			// Empty the controller buffer and discard the data
			srvcln_retrieve(buff, JSON_MAXBYTES);
		}	
	}	
	
	// Parse the incoming data
	if(indata)
    {
		// Count the number of nodes
		nodes = 0;	
		while(*(U16*)(memory_map+(MaCaco_ADDRESSES_s+nodes*2)) != 0x0000)
			nodes++;
	
		// Define HTTP Hello	
		char* str[] = {"HTTP/1.1 200 OK\n\rContent-Type: text/html\n\r\n\r"};
		srvcln_send((uint8_t*)str[0], strlen(str[0])); 
	
		// Handler1, if /status?id=... or /status?all is requested from client
		if((incomingURL.startsWith("GET /status?") || ((incomingURL.indexOf("GET /status?",0) > 0))))
		{
			U8 id_for,nodes_for;
					
			// Find start and end index request
			U8 status  = incomingURL.indexOf("status",0);
			if(incomingURL.indexOf("?id=",status) > 0)
			{						
				U8 id   = incomingURL.indexOf("?id=",status);		
				U8 id_f  = incomingURL.indexOf(" ", id);						// Command should end with a space	
				
				// If one ID is requested
				id_for    = incomingURL.substring(id+4, id_f).toInt();			// Sum lenght of "?id="
				nodes_for = id_for + 1; 
			}					
			else if(incomingURL.indexOf("?all",status) > 0)
			{						
				// If no ID is specified, returns all the IDs
				id_for = 0;
				nodes_for = nodes;
			}										
		
			// Print "({["
			srvcln_send((uint8_t*)json[0], strlen(json[0])); 
			
			for(U8 id=id_for;id<nodes_for;id++)
			{
				bufferlen=0;
				
				// Print "({[", "{\"id\" : \""
				memmove(&buf[bufferlen],json[1],strlen(json[1]));
				bufferlen += strlen(json[1]);
				
				// Print ID
				*(unsigned long*)(buf+bufferlen) = (unsigned long)id;
				srvcln_num2str((uint8_t*)(buf+bufferlen), DEC, &bufferlen);

				// Print "\", \"hlt\" : \""
				memmove(&buf[bufferlen],json[2],strlen(json[2]));
				bufferlen += strlen(json[2]);
				
				if(id != MaCaco_LOCNODE)
				{
					// If remote node, publish healty data	
					*(unsigned long*)(buf+bufferlen) = (unsigned long)memory_map[MaCaco_HEALTY_s+id];
					srvcln_num2str((uint8_t*)(buf+bufferlen), DEC, &bufferlen);
				}
				else
				{
					// If local node, publish max healty as default	
					*(unsigned long*)(buf+bufferlen) = (unsigned long)MaCaco_SUBMAXHEALTY;
					srvcln_num2str((uint8_t*)(buf+bufferlen), DEC, &bufferlen);						
				}
				
				// Print "\", \"slot\" : ["
				memmove(&buf[bufferlen],json[3],strlen(json[3]));
				bufferlen += strlen(json[3]);

				// The frame is now assembled, send the data
				srvcln_send((uint8_t*)buf, bufferlen); 
				
				// Prepare the frame with data from nodes
				for(U8 slot=0;slot<MaCaco_SLOT;slot++)
				{	
					bufferlen=0;
					
					// Print "{ \"typ\" : \""
					memmove(&buf[bufferlen],json[4],strlen(json[4]));
					bufferlen += strlen(json[4]);	
					
					// Print "typ" value
					*(unsigned long*)(buf+bufferlen) = (unsigned long)memory_map[id*MaCaco_TYPLENGHT+MaCaco_TYP_s+slot];
					srvcln_num2str((uint8_t*)(buf+bufferlen), DEC, &bufferlen);						
					
					// Print "\", \"val\" : \""
					memmove(&buf[bufferlen],json[5],strlen(json[5]));
					bufferlen += strlen(json[5]);
						
					// Print "val" value
					*(unsigned long*)(buf+bufferlen) = (unsigned long)memory_map[id*MaCaco_OUTLENGHT+MaCaco_OUT_s+slot];
					srvcln_num2str((uint8_t*)(buf+bufferlen), DEC, &bufferlen);						
				
					// Print "\"}", ","
					memmove(&buf[bufferlen],json[6],strlen(json[6]));
					bufferlen += strlen(json[6]);				

					if(slot+1<MaCaco_SLOT) 
					{
						// Print ","
						memmove(&buf[bufferlen],json[7],strlen(json[7]));
						bufferlen += strlen(json[7]);						
					}
					
					// The frame is now assembled, send the data					
					srvcln_send((uint8_t*)buf, bufferlen); 
				}
					
				bufferlen=0;
					
				// Print "]}"
				memmove(&buf[bufferlen],json[8],strlen(json[8]));
				bufferlen += strlen(json[8]);

				if(id+1<nodes) 
				{
					// Print ","
					memmove(&buf[bufferlen],json[7],strlen(json[7]));
					bufferlen += strlen(json[7]);
				}
				
				// The frame is now assembled, send the data
				srvcln_send((uint8_t*)buf, bufferlen); 
			}
				
			bufferlen=0;
				
			// Print "]})"
			memmove(&buf[bufferlen],json[9],strlen(json[9]));
			bufferlen += strlen(json[9]);	
			
			// The frame is now assembled, send the data
			srvcln_send((uint8_t*)buf, bufferlen); 
		}	

		// Handler2, if /force is requested from client	
		// an example command is "GET /force?id=4&slot=2&val=1 "
		if(incomingURL.startsWith("GET /force") || ((incomingURL.indexOf("GET /force",0) > 0)))
		{			
			// Find start and end index for callback request
			U8 force  = incomingURL.indexOf("force",0);
			if(incomingURL.indexOf("?id=",force) > 0)
			{						
				U8 id   = incomingURL.indexOf("?id=",force);		
				U8 slot = incomingURL.indexOf("&slot=", id);		
				U8 val_s  = incomingURL.indexOf("&val=", slot);		
				U8 val_f  = incomingURL.indexOf(" ", val_s);			// Command should end with a space	
					
				id = incomingURL.substring(id+4, slot).toInt();			// Sum lenght of "?id="
				slot = incomingURL.substring(slot+6, val_s).toInt();	// Sum lenght of "&slot="
				val_s = incomingURL.substring(val_s+5, val_f).toInt();	// Sum lenght of "&val="						
						
				// Send a command to the node	
				if((id < nodes) && (id != MaCaco_LOCNODE) && (U16)(memory_map[MaCaco_ADDRESSES_s+2*id] != 0x0000))	// If is a remote node, the command act as remote input				
					Souliss_RemoteInput(*(U16 *)(memory_map + MaCaco_ADDRESSES_s + 2*id), slot, val_s);
				else if (id == MaCaco_LOCNODE)								// If is a local node (me), the command is written back
					memory_map[MaCaco_IN_s+slot] = val_s;

				// Send a blank reply - Print "({})"
				srvcln_send((uint8_t*)json[11], strlen(json[11])); 
			}
			else if(incomingURL.indexOf("?typ=",force) > 0)
			{
				U8 typ   = incomingURL.indexOf("?typ=",force);
				U8 val_s  = incomingURL.indexOf("&val=", typ);		
				U8 val_f  = incomingURL.indexOf(" ", val_s);				// Command should end with a space	
				typ = incomingURL.substring(typ+5, val_s).toInt();			// Sum lenght of "?typ="
				val_s = incomingURL.substring(val_s+5, val_f).toInt();		// Sum lenght of "&val="								
				
				// Send a multicast command to all typicals, first identify if the command is issued
				// for a typical or a typical class
				if((typ & 0x0F) == 0x00)
					typ_mask = 0xF0;	// We look only to the typical class value
				else
					typ_mask = 0xFF;	// We look to whole typical value
				
				for(U8 id=0;id<nodes;id++)
				{
					for(U8 slot=0;slot<MaCaco_SLOT;slot++)
					{						
						if(memory_map[id*MaCaco_TYPLENGHT+MaCaco_TYP_s+slot] & typ_mask == typ & typ_mask)	
						{									
							// Send a command to the node	
							if((id != MaCaco_LOCNODE) && ((*(U16 *)(memory_map + MaCaco_ADDRESSES_s + 2*id)) != 0x0000))	// If is a remote node, the command act as remote input
								Souliss_RemoteInput((*(U16 *)(memory_map + MaCaco_ADDRESSES_s + 2*id)), slot, val_s);			
							else if (id == MaCaco_LOCNODE)												// If is a local node (me), the command is written back
								memory_map[MaCaco_IN_s+slot] = val_s;	
						}
					}			
				}	
			}
		}
	
		// Close the connection
		indata=0;
		srvcln_stop();
	}		
}
#endif
