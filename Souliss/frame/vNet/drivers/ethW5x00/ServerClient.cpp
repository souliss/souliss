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

extern unsigned long start_time;
uint8_t usedsocket = SRV_SOCK1;

/**************************************************************************/
/*!
	Start a listening server
*/
/**************************************************************************/
void srvcln_setsocket(uint8_t sock)
{
	usedsocket = sock;
}

/**************************************************************************/
/*!
	Start a listening server
*/
/**************************************************************************/
void srv_listen(uint16_t port)
{
	if(W5x00.readSnSR(usedsocket) == SnSR::CLOSED)
	{
		socket(usedsocket, SnMR::TCP, port, 0);		// Open the socket
		listen(usedsocket);							// Start listen
	}
}

/**************************************************************************/
/*!
	Verify the connections status on the requested port, the sock variable
	is not used because is yet a fixed value and the port is associated 
	automatically by the W5100
*/
/**************************************************************************/
uint8_t srvcln_connected(uint16_t port)
{
	uint8_t s = W5x00.readSnSR(usedsocket);
  
	return !(s == SnSR::LISTEN || s == SnSR::CLOSED || s == SnSR::FIN_WAIT || 
		(s == SnSR::CLOSE_WAIT && !W5x00.getRXReceivedSize(usedsocket)));
}

/**************************************************************************/
/*!
	Send data for Server and Client connection
*/
/**************************************************************************/
uint8_t srvcln_send(uint8_t *data, uint8_t len) 
{
	// Write data
	if(!send(usedsocket, data, len))
	{	
		// Close the connection
		vNet_Stop_M1(usedsocket);
		return ETH_FAIL;
	}
	
	return ETH_SUCCESS;
}

/**************************************************************************/
/*!
	Look for available data on the requested socket and port
*/
/**************************************************************************/
uint8_t srvcln_dataavailable(uint16_t port) 
{
	uint8_t s;

	// Check socket status, reset socket if closed
	s = W5x00.readSnSR(usedsocket);
	
	// Accept closing and restart the socket
	if((s == SnSR::CLOSE_WAIT) && !W5x00.getRXReceivedSize(usedsocket))
		srvcln_stop();	
	else if(s == SnSR::CLOSED)
		srv_listen(port);
				 
	// Look for available data
	if((s == SnSR::ESTABLISHED || s == SnSR::CLOSE_WAIT))
		return W5x00.getRXReceivedSize(usedsocket);
	
	// If no data was available	
	return ETH_FAIL;	
}

/**************************************************************************/
/*!
	Retrieve data from the buffer
*/
/**************************************************************************/
uint8_t srvcln_retrieve(uint8_t *data, uint8_t len) 
{
	// Retrieve the complete message
	if(len>0 && len <= W5x00.getRXReceivedSize(usedsocket)+1)
		len = recv(usedsocket, data, len);
	else		
		return ETH_FAIL;

	return len;
}

/**************************************************************************/
/*!
	Close the socket
*/
/**************************************************************************/
void srvcln_stop() 
{
	start_time = millis();
	
	// Disconnect a TCP/IP connection
	disconnect(usedsocket);
	
	while((W5x00.readSnSR(usedsocket) != SnSR::CLOSED ) && (millis() - start_time < ETH_WAIT))
	    delay(1);
			
	if (W5x00.readSnSR(usedsocket) != SnSR::CLOSED)	
		close(usedsocket);
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
