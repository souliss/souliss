/**************************************************************************
	Souliss - MaCaco Communication Protocol
    Copyright (C) 2011  Veseo

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
#ifndef MACACO_H
#define MACACO_H

#include "src/types.h"
#include "GetConfig.h"			// need : MaCaco_Config.h

typedef struct
{
	U8  datain;			// Number of communication media with incoming data
    U8  funcode;		// Functional Code
    U8  *putin;			// Put in, where data will be placed
    U8  startoffset;	// Start offset
    U8  numberof;		// Number of bytes
	U8  *data;			// Message Data
} MaCaco_rx_data_t;

U8 MaCaco_subscribe(U16 addr, U8 *memory_map, U8 *putin, U8 startoffset, U8 numberof, U8 subscr_chnl);
U8 MaCaco_UserMode_subAnswer(U8* memory_map, U8* data_chg);
U8 MaCaco_PassThrough_subAnswer(U8 startoffset, U8 numberof, U8 *data);
U8 MaCaco_reqtyp();
void MaCaco_DataIn();
U8 MaCaco_retrieve(U8* memory_map, U8* data_chg);
U8 MaCaco_send(U16 addr, U8 funcode, U8 *putin, U8 startoffset, U8 numberof, U8 *data);
U8 MaCacoUserMode_send(U16 addr, U8 funcode, U8 *putin, U8 startoffset, U8 numberof, U8 l_numberof, U8 u_numberof, U8 *l_data, U8 *u_data);
U8 MaCaco_getfuncode();
U16 MaCaco_getputin();
U8 MaCaco_getstartoffset();
U8 MaCaco_getnumberof();
U8 MaCaco_getdatain();
U16 MaCaco_getdatabuffer();
U8 MaCaco_IsSubscribed();
#endif
