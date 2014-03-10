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

/**************************************************************************/
/*!
	Start a listening server
*/
/**************************************************************************/
void srv_listen(uint16_t port)
{
	// Check if a listening point was already open
	for(U8 i=0;i<MAX_SOCK_NUM;i++)
		if(uip_listenports[i] == HTONS(port))
			return;
	
	// Start listening	
	uip_listen(HTONS(port));
}

/**************************************************************************/
/*!
	Stop a listening server
*/
/**************************************************************************/
void srv_unlisten(uint16_t port)
{
	uip_unlisten(HTONS(port));
}

/**************************************************************************/
/*!
	Return the socket number of a server or client connected on the 
	requested port
*/
/**************************************************************************/
uint8_t srvcln_getsocket(uint16_t port)
{
	// Look for an established connection
	for(U8 sock=0;sock<MAX_SOCK_NUM;sock++)
		if(uip_conns[sock].lport == HTONS(port) && (uip_conns[sock].tcpstateflags == ESTABLISHED))
			return sock;	// Return the used socket number
	
	// No available connection on requested port	
	return MAX_SOCK_NUM;
}

/**************************************************************************/
/*!
	Verify the connections status on the requested port
*/
/**************************************************************************/
uint8_t srvcln_connected(uint16_t port, uint8_t sock)
{
	if(sock == MAX_SOCK_NUM)
		return ETH_FAIL;
		
	// Look for an estabilshed connection
	if((uip_conns[sock].lport == HTONS(port)) && (uip_conns[sock].tcpstateflags == ESTABLISHED))
		return ETH_SUCCESS;
	
	// No available connection on requested port	
	return ETH_FAIL;
}

/**************************************************************************/
/*!
	Start a client connection
*/
/**************************************************************************/
uint8_t cln_connect(uint8_t *ip, uint16_t port) 
{
	uint8_t sock;
	
	// Setup the connection, data will be sent using a callback function
	struct uip_conn * conn_sock = uip_connect((u16_t*)ip, HTONS(port));
	
	// Check if socket was opened
	if(conn_sock->tcpstateflags != SYN_SENT)
		return MAX_SOCK_NUM;

	// Retrieve the socket number
	for(uint8_t i=0;i<MAX_SOCK_NUM;i++)
		if(conn_sock == &uip_conns[i])
		{	
			sock = i;
			return sock;
		}
	
	// If a socket wasn't available
	if(conn_sock != &uip_conns[i])
	{
		return MAX_SOCK_NUM;
	}	
}

/**************************************************************************/
/*!
	Assign data to the oFrame, use this method before the send one
*/
/**************************************************************************/
void srvcln_load(uint8_t *data, uint8_t len) 
{
	// If the frame is not empty, there are waiting data 	
	oFrame_Define(&vNetM1_oFrame);
	if(oFrame_Available())
		return ;
	
	// Assign data to the oFrame	
	oFrame_Set(0, data, 0, len, 0);

}

/**************************************************************************/
/*!
	Prepare data sending for a server connection, verify that the connection
	is established before send the data mode
*/
/**************************************************************************/
void srv_send(uint8_t sock, uint8_t mode) 
{
	if ((sock != MAX_SOCK_NUM) && ((uip_conns[sock].tcpstateflags & TS_MASK) == ESTABLISHED))
	{	
		// Set the next action for the socket (SENDDATA or SENDCLOSE)
		uip_conns[sock].appstate |= mode;
	
		/* Data are not really sent with this method, the outgoing data are placed into
		the dedicated buffer and a connection request to a peer is flagged. Later a
		callback function from uip_input() will act on the ENC28J60 to send data. */
	}		
}

/**************************************************************************/
/*!
	Prepare data sending for a client connection. This method is tipically
	used just below the connection one, so there is no processing of the
	uIP stack to evaluate if the connection is estabilished or not.
*/
/**************************************************************************/
void cln_send(uint8_t sock, uint8_t mode) 
{	
		// Set the next action for the socket (SENDDATA or SENDCLOSE)
		uip_conns[sock].appstate |= mode;
	
		/* Data are not really sent with this method, the outgoing data are placed into
		the dedicated buffer and a connection request to a peer is flagged. Later a
		callback function from uip_input() will act on the ENC28J60 to send data. */	
}

/**************************************************************************/
/*!
	Runs the stack until sending and acks are completed, is used to avoid
	more than multiple writes get overrided
*/
/**************************************************************************/
void srv_waitsend(uint8_t sock) 
{	
	/* Process the stack until data where sent and no pending
		frames are waiting for ack
	*/
	do
	{	
		vNet_uIP();
		delay(uIP_DELAY);
	}
	while((uip_outstanding(&uip_conns[sock])) || (uip_conns[sock].appstate & SENDDATA));
	
	// Reset
	vnetlenght=0;

}

/**************************************************************************/
/*!
	Runs the stack until sending and acks are completed, is used to avoid
	more than multiple writes get overrided
*/
/**************************************************************************/
void cln_waitsend(uint8_t sock) 
{	
	/* Process the stack until data where sent and no pending
		frames are waiting for ack
	*/
	do
	{	
		vNet_uIP();
		delay(uIP_DELAY);		
	}
	while(uip_conns[sock].tcpstateflags != CLOSED);

	// Reset
	vnetlenght=0;

}

/**************************************************************************/
/*!
	Look for available data on the requested socket and port
*/
/**************************************************************************/
uint8_t srvcln_dataavailable(uint16_t port) 
{
	// If data are available on port
	if((vnetlenght > 0) && (portnumber == HTONS(port)))
		return vnetlenght;

	//vnetlenght = 0;
	return ETH_FAIL;	
}

/**************************************************************************/
/*!
	Look for available data on the requested socket and port
*/
/**************************************************************************/
uint8_t srvcln_lastport(uint16_t port) 
{
	return (portnumber == HTONS(port));
}

/**************************************************************************/
/*!
	Retrieve data from the buffer
*/
/**************************************************************************/
uint8_t srvcln_retrieve(uint8_t *data, uint8_t len) 
{
	// If there are incoming data
	if((vnetlenght>0) && (len <= vnetlenght))
	{
		memmove(data, appdata, len);
		vnetlenght -= len;				
	}
	else if((vnetlenght>0) && (len > vnetlenght))
	{
		memmove(data, appdata, vnetlenght);
		vnetlenght=0;				
	}
	else
		return ETH_FAIL;
	
	// Returns number of byte still in the buffer
	return vnetlenght;
	
}

/**************************************************************************/
/*!
	Discard the buffered data
*/
/**************************************************************************/
void srvcln_discard() 
{
	vnetlenght=0;	
}

/**************************************************************************/
/*!
	Close the requested socket
*/
/**************************************************************************/
void srvcln_stop(uint8_t sock) 
{
	if (sock == MAX_SOCK_NUM)
		return;

	uip_conns[sock].appstate = CLOSECLIENT;
}

/**************************************************************************/
/*!
	State for ther requested socket
*/
/**************************************************************************/
uint8_t srvcln_status(uint8_t sock) 
{
	if (sock == MAX_SOCK_NUM) return CLOSED;
	return uip_conns[sock].tcpstateflags;
}

/**************************************************************************/
/*!
	Convert number in string
*/
/**************************************************************************/
void srvcln_num2str(uint8_t *data, uint8_t base, uint8_t *len) 
{
	// Convert a number into a string
	unsigned long i = 0, n, nn;
	
	// Save the incoming byte
	n = *(unsigned long*)data;

	if (n == 0) 	// Print 0
	{
		data[0] = '0';
		*len += 1;
	} 
	else				// Print other chars
	{
		// Save the value to convert
		nn = n;
		
		while (nn > 0) 
		{
			i++;
			nn /= base;
		}

		*len += i;		// Store the size
		
		for (; i > 0; i--)
		{
			data[i-1] = n % base;
			n /= base;
			
			data[i-1] = (data[i-1] < 10 ? '0' + data[i-1] : 'A' + data[i-1] - 10);
		}
	}
}
