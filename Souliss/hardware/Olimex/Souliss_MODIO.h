/**************************************************************************
	Souliss
    Copyright (C) 2013  Veseo

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
#ifndef SOULISS_MODIO_H
#define SOULISS_MODIO_H

#include "MODIO.h"
#include "src/types.h"

U8 Souliss_MODIO_In();
U8 Souliss_MODIO_DigIn(U8 pin, U8 value, U8 *memory_map, U8 slot);
U8 Souliss_MODIO_LowDigIn(U8 pin, U8 value, U8 *memory_map, U8 slot);
U8 Souliss_MODIO_DigIn2State(U8 pin, U8 value_state_on, U8 value_state_off, U8 *memory_map, U8 slot);
U8 Souliss_MODIO_DigInHold(U8 pin, U8 value_state1, U8 value_state2, U8 *memory_map, U8 slot);
U8 Souliss_MODIO_LowDigInHold(U8 pin, U8 value_state1, U8 value_state2, U8 *memory_map, U8 slot);
void Souliss_MODIO_SetOut(U8 pin, U8 value, U8 *memory_map, U8 slot);
void Souliss_MODIO_nSetOut(U8 pin, U8 value, U8 *memory_map, U8 slot);
void Souliss_MODIO_SetOutToggle(U8 pin, U8 value, U8 *memory_map, U8 slot);
void Souliss_MODIO_SetOutLessThan(U8 pin, U8 value, U8 deadband, U8 *memory_map, U8 slot);
void Souliss_MODIO_SetOutGreaterThan(U8 pin, U8 value, U8 deadband, U8 *memory_map, U8 slot);
void Souliss_MODIO_DigOut();
void Souliss_MODIO_RelayOut();

#endif