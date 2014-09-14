/**************************************************************************
	Souliss - vNet Virtualized Network
    Copyright (C) 2014  Veseo

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

#include "lpt200.h"
#include "interfaces/ASCIItools.c"

static uint8_t usartframe[L200_FRAME_LEN], l=0, junkdata=0;	
static uint16_t myaddress=0;
static uint8_t dhcpip[4];

#if (LPT200_DEBUG)
	#include "SoftwareSerial.h"
	extern SoftwareSerial myUSARTDRIVER; // RX, TX
	
	#define USART_LOG 	myUSARTDRIVER.print
#endif

/**************************************************************************/
/*!
    Init the LPT200 chip
*/
/**************************************************************************/
uint8_t lpt200_init()
{
	// Set LPT200 WiFi ON/OFF pin
	pinMode(2, OUTPUT);
  
	// Power off the module
	digitalWrite(2, HIGH);
  
	// Start the USART
	USARTDRIVER.begin(USART_BAUD);

	// Power on the module
	digitalWrite(2, LOW);
	
	// Wait till the LPT200 got an address via DHCP
	delay(L200_BOOTTIME);
	
	// Clean the serial buffer
	while(USARTDRIVER.available())
		USARTDRIVER.read();
	
	USARTDRIVER.println("#GETMYIP$");
	
	// Wait the answer
	delay(L200_WAITTIME);
		
	l=0;
	while(USARTDRIVER.available() && (l < L200_FRAME_LEN))	
		usartframe[l++] = USARTDRIVER.read();
		
	// There are at least nine chars #GETMYIP,
	if(l>L200_TYPECODE)
	{
		l=9;	// Length of #GETMYIP,

		#if (LPT200_DEBUG)		
		USART_LOG("DHCP IP=");				
		#endif
		
		// Get the source IP address
		for(uint8_t i=0;i<4;i++)
		{
			dhcpip[i] = ASCII_str2num(usartframe+l, &l);	

			#if (LPT200_DEBUG)
			USART_LOG(dhcpip[i],HEX);
			USART_LOG(",");
			#endif
		}	
		
		#if (LPT200_DEBUG)
		USART_LOG("\r\n");
		#endif
	}
	else
		return L200_FAIL;
			
	// There is no more to init, the sockets (open/close/listen) are handlded 
	// directly by the WiFi module.
			
	return L200_SUCCESS;
}

/**************************************************************************/
/*!
    Retrieve the IP address provided by DHCP
*/
/**************************************************************************/
void getip(uint8_t * addr)
{

	#if (LPT200_DEBUG)
	USART_LOG(GETIP=);
	#endif

	for(uint8_t i=0;i<4;i++)
	{
		addr[i]=dhcpip[i];

		#if (LPT200_DEBUG)
		USART_LOG(addr[i],HEX);
		USART_LOG(",");
		#endif

	}
	#if (LPT200_DEBUG)
	USART_LOG("\r\n");
	#endif
}

/**************************************************************************/
/*!
    Send an UDP frame
*/
/**************************************************************************/
uint8_t sendUDP(const uint8_t * buf, uint16_t len, uint8_t * addr, uint16_t port)
{ 
	uint8_t i=0;
	uint8_t header[L200_HEADER_LEN] = "#SNDUDP,";	// Send UDP request in ASCII
	uint8_t hedlen = 8;								// Starting position after the #SNDUDP,
	oFrame* frame=0;
	
	// Insert the IP address and convert it in ASCII
	for(i=0;i<4;i++)
	{
		// Inser the IP byte value
		*(unsigned long*)(header+hedlen) = (unsigned long)(addr[i]);
		ASCII_num2str((uint8_t*)(header+hedlen), DEC, &hedlen);	
		
		// Insert dot separator
		header[hedlen++] = '.';
		
	}
	
	// Last separator is a comma and not a dot
	header[--hedlen] = ',';
	hedlen++;
	
	// Insert the port number
	*(unsigned long*)(header+hedlen) = (unsigned long)(port);
	ASCII_num2str((uint8_t*)(header+hedlen), DEC, &hedlen);
	
	// At this point we have something like #SNDUDP,192.168.1.17,2300 add the comma
	header[hedlen++] = ',';
	
	// If there are no pending data to be processed
	if((l==0) && !(USARTDRIVER.available()))
	{
		// Define the oFrame and look for data to send
		frame = (oFrame*)buf;
		oFrame_Define(frame);
		
		if(oFrame_Available() && (oFrame_GetLenght() < (L200_FRAME_LEN - hedlen)))
		{
			// Insert the number of bytes
			*(unsigned long*)(header+hedlen) = (unsigned long)(oFrame_GetLenght());
			ASCII_num2str((uint8_t*)(header+hedlen), DEC, &hedlen);
			header[hedlen++] = ',';
		
			// Send the header
			i=0;
			while(i < hedlen)
			{
				USARTDRIVER.write(header[i]);
				i++;
			}
			USARTDRIVER.flush();					// Wait data send

			// If len is zero buf points to an oFrame
			if(len==0)
			{		
				// Send the message content
				while(oFrame_Available())
				{
					//USARTDRIVER.write(oFrame_GetByte());
					uint8_t databyte= oFrame_GetByte();
					USARTDRIVER.write(databyte);					
				}
				USARTDRIVER.flush();					// Wait data send

			}
			else
			{
				// Send the message content
				i=0;
				while(i < len)
				{
					USARTDRIVER.write(buf[i]);
					i++;
				}
				USARTDRIVER.flush();					// Wait data send

			}
				
			// Send $ to close the frame
			USARTDRIVER.print("$");
			USARTDRIVER.flush();					// Wait data send

			return L200_SUCCESS;
		}
		else
			return L200_FAIL;	
	}
	else
		return L200_FAIL;
	
}

/**************************************************************************/
/*!
    Check if data on the USART are available
*/
/**************************************************************************/
uint8_t dataaval()
{
	uint8_t i=0, payload=0;

	// Check if buffer is yet full and hasn't been parsed or if contain unparsed data from long time
	if((l == (L200_FRAME_LEN-1)) || (junkdata > L200_JUNK))
	{	
		// Reset data, this avoid fake reads due to old data
		for(i=0;i<L200_FRAME_LEN;i++)
			usartframe[i]=0;
	
		l=0;		// Buffer is full just before retrieve data, remove junk
		junkdata=0;
	}
	else if (l > 0)
		junkdata++;
	
	// From the USART we get one byte per time, so before has to be identified
	// if there is an incoming frame	
	while(USARTDRIVER.available() && (l < L200_FRAME_LEN))	
		usartframe[l++] = USARTDRIVER.read();

		#if (LPT200_DEBUG)
		if(l)
		{
			USART_LOG("\r\nData in\r\nl=");
			USART_LOG(l,DEC);
	
			USART_LOG("\r\nASCII >");
			i=0;
			while(i<l)
			{
				USART_LOG((char)usartframe[i]);
				i++;
			}
		
			USART_LOG("\r\nHEX >");
			i=0;
			while(i<l)
			{
				USART_LOG(usartframe[i],HEX);
				USART_LOG(",");
				i++;
			}
		}
		#endif
/*	
	// The LPT200 module can receive binary data on the UDP socket or ASCII data
	// on all the other opened sockets. In case of binary data is always used #RCVUDP
	// as starting string of the frame, the # character is considered special and cannot
	// be used in ASCII data.
	
	// Look for the # special character, if is found is a binary data and we can remove the junk
	for(i=0;i<l;i++)
	{
		// Look for starting character
		if(usartframe[i] == '#')	//isn't enough, the # could be also a char into the frame
		{
			// The payload
			if(i < l) payload = l-i;
	
			if((payload) && (i!=0))
			{			
				// The frame is valid remove junk
				memmove(usartframe, (usartframe+i), payload);		
				l = payload;
			}
		}
	}	
*/
	// If there are no incoming data
	if((l == 0))
		return L200_FAIL;
	else
		return l;	// Return the availale bytes	
}

/**************************************************************************/
/*!
    Return length of the last received frame
*/
/**************************************************************************/
uint8_t getlen()
{
	return l;
}

/**************************************************************************/
/*!
    Parse and get data (binary) from UDP socket
*/
/**************************************************************************/
uint8_t recvUDP(uint8_t * buf, uint16_t len, uint8_t * s_addr, uint16_t *s_port, uint8_t * d_addr, uint16_t * d_port)
{
	uint8_t i=0, datalen=0;
	uint8_t header[L200_HEADER_LEN] = "#RCVUDP,";	// Send UDP request in ASCII
	uint8_t hedlen = 8;								// Starting position after the #RCVUDP,

	// Parse the frame till the #RCVUDP is found
	for(uint8_t p=0;p<l-L200_HEADER_LEN;p++)
	{	
		uint8_t* uframe = usartframe+p;				// Move the pointer
	
		// Define the frame type
		if(compare_string((char*)uframe, (char*)header, hedlen))
		{
			// Check the lenght of the header
			if(l < L200_HEADER_MIN)
				return L200_FAIL;

			// Check if the received frame has a complete header like #RCVUDP,192.168.1.17,23000,192.168.1.18,230,12,....			
			if(nof_string((char*)uframe, ',', L200_HEADER_LEN) < 6)
				return L200_FAIL;
				
			// From this point the header is supposed to be complete, parse it
		
			// Get the source IP address
			for(i=0;i<4;i++)
				s_addr[i] = ASCII_str2num(uframe+hedlen, &hedlen);
				
			// Get the source port number
			*s_port = ASCII_str2num(uframe+hedlen, &hedlen);

			// Get the destination IP address
			for(i=0;i<4;i++)
				d_addr[i] = ASCII_str2num(uframe+hedlen, &hedlen);
			
			// Get the destination port number
			*d_port = ASCII_str2num(uframe+hedlen, &hedlen);		 

			// The lenght of the payload is the last parameter
			datalen = ASCII_str2num(uframe+hedlen, &hedlen);
			
			// Retreive the data
			i=0;
			while((i < datalen) && ((uframe+hedlen) < (usartframe+L200_FRAME_LEN)))
			{
				// Copy data in the buffer
				buf[i] = uframe[hedlen];
				
				i++;
				hedlen++;
			}
			
			// Flag data as read
			if(hedlen < l)
			{
				hedlen++;		// Remove the last byte, the $ end of line
				l=l-hedlen;		// Remove the data that has been read
					
				// If there are not used data in the buffer, move them at the begin of the buffer
				if(l)	memmove(uframe, (uframe+hedlen), l);
			}	
			else
				l=0;
				
			// Reset the junk timer
			junkdata=0;
				
			// Return the amount of data
			return i;
		}
	}		
}

/**************************************************************************/
/*!
    Parse and get data (ASCII) from all other socket
*/
/**************************************************************************/
uint8_t recv(uint8_t * buf, uint16_t len)
{
	uint8_t i=0, datalen=l;
	uint8_t header[L200_HEADER_LEN] = "#RCVUDP,";	// Send UDP request in ASCII
	uint8_t hedlen = 8;								// Starting position after the #RCVUDP,
	
	// We should not process here UDP frames
	if(compare_string((char*)usartframe, (char*)header, hedlen))
		return L200_FAIL;

	// Retreive the data
	if(datalen)
	{	
		while((i < datalen) && (i < L200_FRAME_LEN) && (i < len))
		{
			// Copy data in the buffer
			buf[i] = usartframe[i];
			
			i++;
		}

		// Flag data as read
		if(i < l) 
		{
			l=l-i;
			
			// If there are not used data in the buffer, move them at the begin of the buffer
			if(l)	memmove(usartframe, (usartframe+hedlen), l);
		}
		else
			l=0;
			
		// Reset the junk timer
		junkdata=0;
		
		// Return the amount of data	
		return i;
	}
	else
		return L200_FAIL;
}