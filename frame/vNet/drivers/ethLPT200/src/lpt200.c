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
#include "SerialPort.cpp"
#include "interfaces/ASCIItools.c"

SerialPort<0, L200_USARTBUFFER, 0> USARTDRIVER;

static uint8_t l=0, ll=0, junkdata=0;	
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
	
	#if (LPT200_DEBUG)		
	USART_LOG("GETIP\r\n");				
	#endif
			
	// Clean the serial buffer
	while(USARTDRIVER.available())
		USARTDRIVER.discard();
	
	// Request the IP address from the Wifi module (assigned via DHCP)
	USARTDRIVER.write(FRAMESTART);
	USARTDRIVER.write(GETMYIP);
	USARTDRIVER.write(FRAMESTOP);
	
	// Wait the answer
	delay(L200_WAITTIME);
		
	l=0;
	uint8_t tbuf[10];
	while(USARTDRIVER.available() && (l < 10))
	{
		tbuf[l++] = USARTDRIVER.read();
	}
			
	// Look for the IP address, the frame will look like 
	// [FRAMESTART GETMYIP FRAMESPACE 0x69 0x01 0xa8 0xc0 FRAMESTOP]
	if((l) && (tbuf[0] == FRAMESTART) && (tbuf[1] == GETMYIP) && (tbuf[2] == FRAMESPACE))
	{	
		#if (LPT200_DEBUG)		
		USART_LOG("DHCP IP=");				
		#endif
		
		// Get the source IP address
		for(uint8_t i=0;i<4;i++)
		{
			// The IP address is provided in reverse order
			dhcpip[3-i] = tbuf[3+i];	 	

			#if (LPT200_DEBUG)
			USART_LOG(dhcpip[i],HEX);
			USART_LOG(",");
			#endif
		}	
		
		#if (LPT200_DEBUG)
		USART_LOG("\r\n");
		#endif

		// There is no more to init, the sockets (open/close/listen) are handled 
		// directly by the WiFi module.
				
		return L200_SUCCESS;		
	}

	// If we are there, the module hasn't provided a valid IP address or it wasn't able 
	// to get an IP address from the DHCP server.
	
}

/**************************************************************************/
/*!
    Set manually the IP that the module got via DHCP, to be used only in case
	of problem or troubleshooting
*/
/**************************************************************************/
void setip(uint8_t * addr)
{
	// Return the IP address
	for(uint8_t i=0;i<4;i++)
		dhcpip[i]=addr[i];
}


/**************************************************************************/
/*!
    Retrieve the IP address provided by DHCP
*/
/**************************************************************************/
void getip(uint8_t * addr)
{
	#if (LPT200_DEBUG)
	USART_LOG("IP=");
	#endif
	
	// Return the IP address
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
	#if (LPT200_DEBUG)
	USART_LOG("SNDUDP\r\n");
	#endif
 
	uint8_t i=0;
	oFrame* frame=0;

	// A frame looks like
	// [FRAMESTART SNDUDP FRAMESPACE IP3 IP2 IP1 IP0 FRAMESPACE PORT1 PORT0 ...
	//  0x00 0x00 FRAMESPACE LENGTH DATA0 DATA1 ... FRAMESTOP]
	
	// Request a send UDP frame
	USARTDRIVER.write(FRAMESTART);
	USARTDRIVER.write(SNDUDP);	
	USARTDRIVER.write(FRAMESPACE);
	
	// Write the IP address and convert it in ASCII
	for(i=0;i<4;i++)
		USARTDRIVER.write(addr[3-i]);
	USARTDRIVER.write(FRAMESPACE);
	
	// Write the port number
	uint8_t *p_port = (uint8_t *)(&port);
	USARTDRIVER.write(*(p_port+0));
	USARTDRIVER.write(*(p_port+1));
	
	// Write two zeros (source port address isn't required) and a space
	USARTDRIVER.write((uint8_t)0x00);
	USARTDRIVER.write((uint8_t)0x00);
	USARTDRIVER.write(FRAMESPACE);

	// If the length is zero, is an oFrame
	if(len==0)
	{
		frame = (oFrame*)buf;
		oFrame_Define(frame);
	
		if(oFrame_Available())
		{		
			// Write the message length (max 255)
			USARTDRIVER.write(oFrame_GetLenght());
			USARTDRIVER.write(FRAMESPACE);
	
			// Write payload
			uint8_t databyte;
			while(oFrame_Available())
			{
				databyte = oFrame_GetByte();
				USARTDRIVER.write(databyte);		
			}
			
			USARTDRIVER.write(FRAMESTOP);			// Frame ends here
		}
		
		return L200_SUCCESS;
	}
	else
	{
		// Write the message length (max 255)
		USARTDRIVER.write(len);
		USARTDRIVER.write(FRAMESPACE);
		
		// Write payload
		i=0;
		while(i < len)
		{
			USARTDRIVER.write(buf[i]);
			i++;
		}
		
		USARTDRIVER.write(FRAMESTOP);			// Frame ends here		
		//USARTDRIVER.flush();					// Wait data send
		
		return L200_SUCCESS;
	}
	
	return L200_FAIL;
}

/**************************************************************************/
/*!
    Check if data on the USART are available
*/
/**************************************************************************/
uint8_t dataaval()
{
	// Get the number of available bytes
	l = USARTDRIVER.available();
	
	// If new data has been received, give a try more
	if(junkdata && (l>ll))
		junkdata--;
	
	// Store the actual number of bytes
	ll = l;
	
	// Print buffer contents
	#if (LPT200_DEBUG)
	if(l)
	{
		USART_LOG("\r\nDATA(0x");
		USART_LOG(l,HEX);
		USART_LOG(")=");
		for(uint8_t i=0;i<l;i++)
		{
			USART_LOG(USARTDRIVER.inspect(i),HEX);
			USART_LOG(",");
		}
		USART_LOG(";\r\n");
	}
	#endif

	// Prevent to loop indefinitely
	if(l && (junkdata<L200_JUNK))
		junkdata++;
	else if(junkdata>=L200_JUNK)
	{
		// Flush data and reset the junk counter
		l=0;
		junkdata=0;
		USARTDRIVER.flushRx();
					
		#if (LPT200_DEBUG)
		USART_LOG("RCVUDP Flush \r\n");
		#endif
	}	
	
	return l;
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
	uint8_t i=0, p=0, datalen=0;
	
	// If the header isn't completed loaded, return
	if(l<L200_HEADER_LEN)
		return L200_FAIL;

	#if (LPT200_DEBUG)
	USART_LOG("RCVUDP\r\n");
	#endif
	
	// Parse the frame looking for UDP data
	for(p=0;p<l-L200_HEADER_LEN;p++)
	{
			
		#if (LPT200_DEBUG)
		USART_LOG("p=");
		USART_LOG(p,HEX);
		USART_LOG(";\r\n");
		#endif
		
		// An UDP frame starts like [FRAMESTART RCVUDP FRAMESPACE IP3 IP2 IP1 IP0 FRAMESPACE PORT1 PORT0 0x00 0x00 ...
		//							 FRAMESPACE IP3 IP2 IP1 IP0 FRAMESPACE PORT1 PORT0 0x00 0x00 FRAMESPACE LENGTH ...
		//							 FRAMESPACE DATA0 DATA1 ... FRAMESTOP
		if( (USARTDRIVER.inspect(p+0)  == FRAMESTART) 	&&
			(USARTDRIVER.inspect(p+1)  == RCVUDP) 		&&
			(USARTDRIVER.inspect(p+2)  == FRAMESPACE) 	&&
			(USARTDRIVER.inspect(p+7)  == FRAMESPACE) 	&&
			(USARTDRIVER.inspect(p+12) == FRAMESPACE) 	&&
			(USARTDRIVER.inspect(p+17) == FRAMESPACE) 	&&
			(USARTDRIVER.inspect(p+22) == FRAMESPACE) )
		{
			#if (LPT200_DEBUG)
			USART_LOG("RCVUDP Header OK\r\n");
			#endif
		
			// If we are here the frame header is an UDP one
			datalen = USARTDRIVER.inspect(p+23);
			
			// Verify the frame stop byte
			if((l>=p+L200_HEADER_LEN+datalen) && (USARTDRIVER.inspect(p+L200_HEADER_LEN+datalen) == FRAMESTOP))
			{
				#if (LPT200_DEBUG)
				USART_LOG("RCVUDP Frame OK\r\n");
				#endif
			
				// The frame is complete, retrieve data and notify the upper layer
				
				#if (LPT200_DEBUG)
				USART_LOG("IP=");
				#endif
				
				// Get the source and destination IP address
				for(i=0;i<4;i++)
				{
					s_addr[i] = USARTDRIVER.inspect(p+6-i);
					d_addr[i] = USARTDRIVER.inspect(p+16-i);
					
						
					#if (LPT200_DEBUG)
					USART_LOG(s_addr[i],HEX);
					USART_LOG(",");
					#endif
				}
				
				#if (LPT200_DEBUG)
				USART_LOG("\r\n");
				#endif
				
				
				// Get the source and destination port
				for(i=0;i<2;i++)
				{
					*((uint8_t*)s_port+i) = USARTDRIVER.inspect(p+8+i);
					*((uint8_t*)d_port+i) = USARTDRIVER.inspect(p+18+i);
				}

					#if (LPT200_DEBUG)
					USART_LOG(*s_port,HEX);
					USART_LOG(",");
					#endif
					
				#if (LPT200_DEBUG)
				USART_LOG("\r\n");
				#endif
				
				// Check the maximum length of the destination buffer
				if(len < datalen) datalen = len;
			
				// Remove the header
				for(i=0;i<p+25;i++)
					USARTDRIVER.discard();
				
				// Get data
				for(i=0;i<datalen;i++)
					*(buf+i) = USARTDRIVER.read();
				
				// Update length
				l = USARTDRIVER.available();
				
				// The last byte is the FRAMESTOP, delete it
				USARTDRIVER.read();
				
				// Flag data as valid
				junkdata=0;
				
				// Return the amount of data
				return datalen;				
			}
			else
			{
				#if (LPT200_DEBUG)
				USART_LOG("RCVUDP Incomplete\r\n");
				#endif
			
				// Notify that data aren't complete and delete them if too old
				if(junkdata<L200_JUNK)
					junkdata++;
				else
				{
					// Flush data and reset the junk counter
					l=0;
					junkdata=0;
					USARTDRIVER.flushRx();
					
					#if (LPT200_DEBUG)
					USART_LOG("RCVUDP Flush \r\n");
					#endif
				}	

				break;	
			}	
		}
	}

	// There is no a valid frame, free space in the buffer
	for(i=0;i<p;i++)
		USARTDRIVER.discard();
	
	// Frame isn't complete, try next
	return L200_FAIL;
}

/**************************************************************************/
/*!
    Parse and get data (ASCII) from all other socket
*/
/**************************************************************************/
uint8_t recv(uint8_t * buf, uint16_t len)
{
}