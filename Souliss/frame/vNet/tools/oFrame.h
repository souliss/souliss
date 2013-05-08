/**************************************************************************
	Souliss - Output Frame
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
#ifndef oFRAME_H
#define oFRAME_H

#include <Arduino.h>
#include <stdbool.h>
#include <stdint.h>

#include "src/types.h"

// Data structure to handle multiple headers and payload
typedef struct 
{
	U8*	header;				// Pointer to header data area
	U8*	payload;			// Pointer to payload data area
	U8	header_len;			// Lenght of header
	U8 	payload_len;		// Lenght of payload
	U8* next;				// Pointer to next oFrame struct
	
} oFrame;					// Output frame

void oFrame_Define(oFrame *frame);
void oFrame_Set(U8* header, U8* payload, U8 header_len, U8  payload_len, oFrame *next);
void oFrame_Reset();
U8 oFrame_isBusy();
U8 oFrame_Available();
U8 oFrame_GetLenght();
U8 oFrame_GetByte();
void oFrame_Copy(oFrame *destination, oFrame *source);
void oFrame_Debug();
#endif
