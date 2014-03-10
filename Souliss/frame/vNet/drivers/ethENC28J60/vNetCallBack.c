/**************************************************************************
	Souliss - vNet Virtualized Network
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
/**************************************************************************/

#include "vNetCallBack.h"

#define ETHBUF ((struct uip_eth_hdr *)&uip_buf[0])

extern uint8_t *appdata;									// Pointer of the incoming packet
extern uint8_t *ripadrr;									// Pointer of remote IP address
extern uint8_t vnetlenght;									// Lenght of the incoming packet
extern uint16_t portnumber;									// Destination Port number of the incoming packet
extern uint16_t sportnumber;								// Source Port number of the incoming packet
extern uint8_t arptimer;									// Timeout for ARP table
extern oFrame vNetM1_oFrame;								// Data structure for output frame
extern uint8_t rawdata;										// Flag if there are raw data (not UDP/IP, TCP/IP, ICMP, ARP)

/**************************************************************************/
/*!
	This method is called from the uIP stack when the upper layer are 
	requested to perform some action for TCP connection.
*/
/**************************************************************************/
void vNet_callback()
{
	/* Following the list of cases where the upper layer maybe requested
		to act:
		- uip_flags & UIP_TIMEDOUT
		- uip_flags & UIP_POLL
		- uip_flags & UIP_REXMIT
		- uip_flags & UIP_CLOSE
		- uip_flags & UIP_NEWDATA
	*/	

	if((uip_flags & UIP_POLL) || (uip_flags & UIP_REXMIT))
	{
		if(uip_conn->appstate & SENDDATA)
		{	
			// Set pointer and lenght to send data
			uip_send((uint8_t*)(&vNetM1_oFrame), oFrame_GetLenght());
		
			// Flag that send is requested
			uip_conn->appstate &= ~SENDDATA;
		}
		else if(uip_conn->appstate & CLOSECLIENT)
			uip_flags = UIP_CLOSE;
	}
	else if(uip_flags & UIP_NEWDATA)
	{
		// Save the pointer, lenght and port of incoming data
		appdata = (uint8_t*)uip_appdata;	
		vnetlenght = uip_len;
		portnumber  = uip_conn->lport;
		sportnumber = uip_conn->rport;		
		ripadrr = (uint8_t*)uip_conn->ripaddr;
	}
	else if(uip_flags & UIP_TIMEDOUT)
	{
		// Reset the connection and the data buffer
		uip_conn->appstate = RESET;
		oFrame_Reset();
	}
	
}

/**************************************************************************/
/*!
	This method is called from the uIP stack when the upper layer are 
	requested to perform some action for UDP connection.
*/
/**************************************************************************/
void vNet_UDP_callback()
{
	/* Following the list of cases where the upper layer maybe requested
		to act:
		- uip_flags & UIP_POLL
		- uip_flags & UIP_NEWDATA
	*/	

	if((uip_flags & UIP_POLL))
	{
		// At this stage the uIP is ready to build an UDP frame to be sent out
		// uip_slen is used to flag that there are waiting data

		oFrame_Define((&vNetM1_oFrame));
		uip_slen = oFrame_GetLenght();		

		uip_udp_conn->appstate &= ~SENDDATA;	
		uip_flags = 0;
	}
	else if(uip_flags & UIP_NEWDATA)
	{
		// Save the pointer, lenght and port of incoming data
		appdata = (uint8_t*)uip_appdata;	
		vnetlenght = uip_len;
		portnumber  = uip_udp_conn->lport;
		sportnumber = uip_udp_conn->rport;		
		ripadrr = (uint8_t*)uip_udp_conn->ripaddr;
		
		uip_flags = 0;
	}
	
}

/**************************************************************************/
/*!
	Retrieve data from the ethernet controller and process data using uIP 
	stack.
*/
/**************************************************************************/
uint8_t vNet_uIP()
{
	/*	If there are incoming data, the uip_input() issue a callback that set 
		the vnetlenght value to the lenght of the vNet frame, then the value
		is returned and the retrieve function provide to get the data from the
		buffer */	

	// Retreive data from the EN28J60
	rawdata = nic_poll();
	if(rawdata==0)
	{
		// If there are no available data
		
		// Verify if there data to be sent on the UDP socket
		uip_udp_periodic(UIP_UDP_SEND_CONN);

		// Transmit a packet, if one is ready
		if(uip_len > 0)
		{
			/* Look for the MAC addrress, if not available drop the message and issue
				an ARP request */
			uip_arp_out();
			nic_send();
			uip_len = 0;
			
			// No incoming data
			return ETH_FAIL;
		}
		
		// Verify all the TCP sockets
		for(U8 i=0;i<MAX_SOCK_NUM;i++)
        {
			// Execute the TCP stack periodically (listen ports, send data, ...)
			uip_periodic(i);
		
			// Transmit a packet, if one is ready
			if(uip_len > 0)
			{
				// Look for the MAC addrress, if not available drop the message and issue
				// an ARP request 
				uip_arp_out();
				nic_send();
				uip_len = 0;
				
				break;
			}
        }

        // Periodically reset for the ARP table
        if(++arptimer == 20)
        {	
			uip_arp_timer();			// Reset the ARP table
			arptimer = 0;
        }
	}
	else
	{
		// If there are available data
		
		// Process an IP packet
		if(ETHBUF->type == htons(UIP_ETHTYPE_IP))
		{
			// Add the source to the ARP cache
			uip_arp_ipin();
			
			// Process the incoming frame
			uip_input();

			// Transmit a packet, if one is ready
			if(uip_len > 0)
			{
				/* Look for the MAC addrress, if not available drop the message and issue
					an ARP request */
				uip_arp_out();
				nic_send();
			}
			
			// Data are not RAW
			rawdata = 0;
		}
		// Process an ARP packet
		else if(ETHBUF->type == htons(UIP_ETHTYPE_ARP))
		{
			uip_arp_arpin();

			// Transmit a packet, if one is ready
			if(uip_len > 0)
				nic_send();
				
			// Data are not RAW
			rawdata = 0;	
		}

	}
	
	return ETH_FAIL;
}
