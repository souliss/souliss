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

/**************************************************************************/
/*!
    This library is designed to use part of the ENC28J60 RAM as extension of
	the data space of the microcontroller
*/
/**************************************************************************/

#include "ENC28J60.h"
#include "spi.h"


/**************************************************************************/
/*!
	Init the chip
*/
/**************************************************************************/
#define vram_init()	;

/**************************************************************************/
/*!
	Write data into the chip
*/
/**************************************************************************/
void vram_write(U8 pointer, U8 *data, U8 len)
{
	// Check the lenght
	if(len >= vramBUF_len)
		return;
		
	// Set the write pointer to start of transmit buffer area
	enc28j60WriteWord(EWRPTL, vramBUF_start + pointer);

	// Load data into the ENC28J60 RAM
	enc28j60WriteBuffer(len, data);
}

/**************************************************************************/
/*!
	Read data from the chip
*/
/**************************************************************************/
U8 vram_read(U8 pointer)
{
	U8 data;
	
	// Set the write pointer to start of transmit buffer area
	enc28j60WriteWord(ERDPTL, vramBUF_start + pointer);

	// Read a byte
	enc28j60ReadBuffer(1, &data);
	return data;
}

/**************************************************************************/
/*!
	Read multiple bytes of data from the chip
*/
/**************************************************************************/
void vram_read(U8 pointer, U8 *data, U8 len)
{
	// Set the write pointer to start of transmit buffer area
	enc28j60WriteWord(ERDPTL, vramBUF_start + pointer);

	// Read bytes
	enc28j60ReadBuffer(len, data);
}