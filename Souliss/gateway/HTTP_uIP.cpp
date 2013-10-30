/**************************************************************************
	Souliss
    Copyright (C) 2012  Veseo

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

#include "HTTP_uIP.h"

#if(HTTPSERVER && VNET_MEDIA1_ENABLE && ETH_ENC28J60)

String incomingURL = String(HTTP_MAXBYTES);			// The GET request is stored in incomingURL
char buf[HTTP_MAXBYTES];							// Used for temporary operations
uint8_t *buff = (uint8_t *)buf;
uint8_t nodes, indata=0, bufferlen, data_len;		// End of HTML request

/**************************************************************************
/*!
	Parse incoming HTTP GET for incoming commands
	
	Use 
	-	/force, /typ				to force data
*/	
/**************************************************************************/
void HTTPServer(U8 *memory_map)
{	
	// Listen the HTTP port
	srv_listen(HTTPPORT);
	
	// Get the actual used socket
	uint8_t sock = srvcln_getsocket(HTTPPORT);
	
	// Listen for incoming clients
	if (srvcln_connected(HTTPPORT, sock)) 
	{
		if(indata)
        {	
			// Count the number of nodes
			nodes = 0;	
			while(*(U16*)(memory_map+(MaCaco_ADDRESSES_s+nodes*2)) != 0x0000)
				nodes++;
		
			srvcln_discard();			
			
			// Define and send HTTP Hello	
			char* str[] = {"HTTP/1.1 200 OK\n\rContent-Type: text/html\n\r\n\r"};
			
			// Load and send data
			srvcln_load((uint8_t*)str[0], strlen(str[0])); 
			srv_send(sock, SENDDATA); 					// Send data
			srv_waitsend(sock);	
	
			// Handler1, if /force is requested from client	
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
					U8 val_f  = incomingURL.indexOf(" ", val_s);		// Command should end with a space	
					
					id = incomingURL.substring(id+4, slot).toInt();			// Sum lenght of "?id="
					slot = incomingURL.substring(slot+6, val_s).toInt();	// Sum lenght of "&slot="
					val_s = incomingURL.substring(val_s+5, val_f).toInt();	// Sum lenght of "&val="						
							
					// Send a command to the node		
					if((id < nodes) && (id != MaCaco_LOCNODE) && ((*(U16*)(memory_map+(MaCaco_ADDRESSES_s+id*2))) != 0x0000))	// If is a remote node, the command act as remote input				
						Souliss_RemoteInput(*(U16 *)(memory_map + MaCaco_ADDRESSES_s + 2*id), slot, val_s);
					else if (id == MaCaco_LOCNODE)								// If is a local node (me), the command is written back
						memory_map[MaCaco_IN_s+slot] = val_s;
				}
				else if(incomingURL.indexOf("?typ=",force) > 0)
				{
					U8 typ   = incomingURL.indexOf("?typ=",force);
					U8 val_s  = incomingURL.indexOf("&val=", typ);		
					U8 val_f  = incomingURL.indexOf(" ", val_s);				// Command should end with a space	
					typ = incomingURL.substring(typ+5, val_s).toInt();			// Sum lenght of "?typ="
					val_s = incomingURL.substring(val_s+5, val_f).toInt();		// Sum lenght of "&val="								

					U8* val_sp = &val_s;
					
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
						
							for(U8 i=0; i<MaCaco_SLOT; i++)		
								if((*(memory_map + MaCaco_TYP_s + i) & typ_mask) == typ)	// Start offset used as typical logic indication
									*(memory_map+MaCaco_IN_s + i) = val_s;
						}	
					}	
				}
			}	
			
			indata=0;
			srvcln_stop(sock);
			srv_waitsend(sock);
		}
		
		// If there were an incoming message on the HTTP port
		if(srvcln_lastport(HTTPPORT))
		{
			// Look for available data
			if (srvcln_dataavailable(HTTPPORT))
			{	
				// Move data into the temporary buffer
				srvcln_retrieve(buff, HTTP_MAXBYTES);
				
				incomingURL = "";

				// Copy data into the string
				indata=1;
				for(U8 i=0;i<HTTP_MAXBYTES;i++)
				{
					// Stop at next space after GET
					if(incomingURL.startsWith("GET /") && buf[i] == 32)
						break;
						
					incomingURL = incomingURL + buf[i];	
				}			
			}
		}
	}
}	
#endif
