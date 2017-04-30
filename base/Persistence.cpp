/**************************************************************************
	Souliss
    Copyright (C) 2015  Veseo

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

#include "Souliss.h"
#include "Typicals.h"
#include <Arduino.h>

#include "tools/types.h"
#include "GetConfig.h"
#include "Souliss.h"

#if(MaCaco_PERSISTANCE)
/**************************************************************************/
/*!
	Return if data are available from a node id
*/	
/**************************************************************************/
U8 Souliss_Persistence_IsData(U8 *memory_map, U8 id)
{
	return (MaCaco_GetPersistence(memory_map) == id);
}

/**************************************************************************/
/*!
	Clear data if available from a node id
*/	
/**************************************************************************/
void Souliss_Persistence_ClearData(U8 *memory_map, U8 id)
{
	MaCaco_ClearPersistenceIndex(memory_map, id);
}
#endif
