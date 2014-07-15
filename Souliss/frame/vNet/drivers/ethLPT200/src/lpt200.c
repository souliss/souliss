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

static uint8_t usartframe[L200_FRAME_LEN], l=0, bootsuccess=0;	
static uint16_t myaddress=0;
static uint8_t dhcpip[4];

#if (LPT200_DEBUG)
	#include "SoftwareSerial.h"
	extern SoftwareSerial myUSARTDRIVER; // RX, TX
	
	#define USART_LOG 	myUSARTDRIVER.print
#endif

// The name of the class that refers to the USART, change it accordingly to the used device
#ifndef USARTDRIVER_INSKETCH
#	define	USARTDRIVER	Serial				
#endif

/**************************************************************************/
/*!
    Init the LPT200 chip
*/
/**************************************************************************/
uint8_t lpt200_init()
{
	// Start the USART
	USARTDRIVER.begin(USART_BAUD);
	
	// Wait till the LPT200 got an address via DHCP
	pinMode(L200_READYPIN,INPUT);
	delay(L200_BOOTTIME);
	
	// The pin goes LOW if boot has been completed
	if(digitalRead(L200_READYPIN))
		delay(L200_BOOTTIME/2);			// Give some more time
	
	// Check again if boot has been completed
	if(!digitalRead(L200_READYPIN))	
	{
		USARTDRIVER.println("#GETMYIP$");
		delay(L200_WAITTIME);
		
		l=0;
		while(USARTDRIVER.available() && (l < L200_FRAME_LEN))	
			usartframe[l++] = USARTDRIVER.read();
	
		// There are at least nine chars #GETMYIP,
		if(l>L200_TYPECODE)
		{
			l=9;	// Length of #GETMYIP,
			
			// Get the source IP address
			for(uint8_t i=0;i<4;i++)
				dhcpip[i] = ASCII_str2num(usartframe+l, &l);
		}
		else
			return L200_FAIL;
			
		return L200_SUCCESS;
	}	
	else
		return L200_FAIL;
}

/**************************************************************************/
/*!
    Retrieve the IP address provided by DHCP
*/
/**************************************************************************/
void getip(uint8_t * addr)
{
	for(uint8_t i=0;i<4;i++)
		addr[i]=dhcpip[i];
	
}

/**************************************************************************/
/*!
    Send an UDP frame
*/
/**************************************************************************/
uint8_t sendto(const uint8_t * buf, uint16_t len, uint8_t * addr, uint16_t port)
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
	header[hedlen--] = ',';
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
			// Send the header
			i=0;
			while(i < hedlen)
			{
				USARTDRIVER.write(header[i]);
				i++;
			}
			
			// If len is zero buf points to an oFrame
			if(len==0)
			{
				// Send the message content
				while(oFrame_Available())
					USARTDRIVER.write(oFrame_GetByte());
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
			}
				
			// Send $ to close the frame
			USARTDRIVER.print("$\r\n");
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
	uint8_t i=0,j=0, payload=0;

	// Check if buffer is yet full and hasn't been parsed
	if(l == L200_FRAME_LEN-1)
	{
		// Reset data, this avoid fake reads due to old data
		for(i=0;i<L200_FRAME_LEN;i++)
			usartframe[i]=0;
	
		l=0;		// Buffer is full just before retrieve data, remove junk
	}
	
	// From the USART we get one byte per time, so before has to be identified
	// if there is an incoming frame	
	while(USARTDRIVER.available() && (l < L200_FRAME_LEN))	
		usartframe[l++] = USARTDRIVER.read();	
	
	// If there are no incoming data
	if((l == 0))
		return L200_FAIL;

	#if(LPT200_DEBUG)	
	USART_LOG("(LPT200)<Read> Data aval\r\n");
	#endif	
	
	// If there are few bytes, the frame is still incomplete
	if(l < (L200_TYPECODE))
		return L200_FAIL;	// Nothing to parse
	
	// If the length exceed the buffer size
	if(l > L200_FRAME_LEN-1)
	{
		// Reset data, this avoid fake reads due to old data
		for(i=0;i<L200_FRAME_LEN;i++)
			usartframe[i]=0;
			
		l=0;		// Buffer is full just before retrieve data, remove junk
		
		return L200_FAIL;
	}
	
	// Analyse the retrieved frame 
	for(i=0;i<l;i++)
	{
		// Look for starting character
		if(usartframe[i] == '#')
		{
			// Look for the ending character
				
			for(j=0;j<(l-i);j++)						
				if(usartframe[i] == '$')
					payload = j;
			
			if(payload)
			{			
				// The frame is a valid remove junk
				memcpy(usartframe, &usartframe[i], payload);		
				l = payload;
				
				return payload;					// Return message length
			}
			else if((l-i)<(L200_TYPECODE))		// Frame is incomplete, give a try later
			{
				#if(LPT200_DEBUG)	
				USART_LOG("(LPT200)<Read> Incomplete\r\n");
				#endif	
				
				return L200_FAIL;	
			}
			else								// Missing start and ending character, cleanup
			{
				#if(LPT200_DEBUG)	
				USART_LOG("(LPT200)<Read> Corrupted\r\n");
				#endif				
				
				// Reset data, this avoid fake reads due to old data
				for(i=0;i<L200_FRAME_LEN;i++)
					usartframe[i]=0;
					
				l=0;		// Buffer is full just before retrieve data, remove junk
		
				return L200_FAIL;		
			}
		}
	}
	
	// Next try
	#if(LPT200_DEBUG)	
	USART_LOG("(LPT200)<Read> Next try\r\n");
	#endif		
	
	return L200_FAIL;		
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
    Parse and get data
*/
/**************************************************************************/
uint8_t recvUDP(uint8_t * buf, uint16_t len, uint8_t * s_addr, uint16_t *s_port, uint8_t * d_addr, uint16_t * d_port)
{
	uint8_t i=0;
	uint8_t header[L200_HEADER_LEN] = "#RCVUDP,";	// Send UDP request in ASCII
	uint8_t hedlen = 8;								// Starting position after the #SNDUDP,
	
	// Define the frame type
	if(compare_string((char*)usartframe, (char*)header, hedlen))
	{		
		// Get the source IP address
		for(i=0;i<4;i++)
		{
			s_addr[i] = ASCII_str2num(usartframe+hedlen, &hedlen);
			hedlen++;	// Skip the dot separator
		}

		// Get the source port number
		*s_port = ASCII_str2num(usartframe+hedlen, &hedlen);
		hedlen++;	// Skip the comma separator
		
		// Get the destination IP address
		for(i=0;i<4;i++)
		{
			d_addr[i] = ASCII_str2num(usartframe+hedlen, &hedlen);
			hedlen++;	// Skip the dot separator
		}
		
		// Get the destination port number
		*d_port = ASCII_str2num(usartframe+hedlen, &hedlen);		
		hedlen++;	// Skip the comma separator
 
		// Move till the end of the frame
		i=0;
		while((hedlen<l) && (usartframe[hedlen] != '$') && i < len)
		{
			// Copy data in the buffer
			buf[i] = usartframe[hedlen];
			
			i++;
			hedlen++;
		}
		
		// If there were available data
		if(i)
			return i;
	}
	else
		return L200_FAIL;
	
	
	
}