/**************************************************************************
	Souliss - Virtual RAM
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

#include "w5x00.h"
#include "socket.h"

uint8_t baseaddr;

/**************************************************************************/
/*!
    This library is designed to use part of the W5100/W5200 RAM as extension 
	of the data space of the microcontroller
*/
/**************************************************************************/

/**************************************************************************/
/*!
	Init the chip
*/
/**************************************************************************/
void vram_init()
{
	// Set a socket of the W5x00, this will not be used for IP communication
	// rather we will only use the allocated RAM
	srvcln_setsocket(SRV_SOCK2);
	srv_listen(2323);
	
	baseaddr = TXBUF_BASE + W5x00.SSIZE * SRV_SOCK2;
}

/**************************************************************************/
/*!
	Write data into the chip
*/
/**************************************************************************/
void vram_write(U8 pointer, U8 *data, U8 len)
{
	// Check the lenght
	if(len >= W5x00.SSIZE)
		return;
			
	// Write data into the W5x00 RAM	
	W5x00.write(baseaddr + pointer, data, len);
}

/**************************************************************************/
/*!
	Read data from the chip
*/
/**************************************************************************/
U8 vram_read(U8 pointer)
{
	U8 data;
	
	// Read a byte
	W5x00.read(baseaddr + pointer, &data, 1);
	return data;
}

/**************************************************************************/
/*!
	Read multiple bytes of data from the chip
*/
/**************************************************************************/
void vram_read(U8 pointer, U8 *data, U8 len)
{
	// Read bytes
	W5x00.read(baseaddr + pointer, data, len);
}