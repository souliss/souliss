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

#include "IOdef.h"
#include "GetConfig.h"			// need : hwBoards.h

// Include drivers for Olimex MOD-IO
#if(IOBOARD_MODEL == 0x01)						
#	include "hardware/Olimex/MODIO.cpp"
#	include "hardware/Olimex/Souliss_MODIO.cpp"
#endif

// Include drivers for Olimex MOD-IO2
#if(IOBOARD_MODEL == 0x02)						
#	include "hardware/Olimex/MODIO2.cpp"
#	include "hardware/Olimex/Souliss_MODIO.cpp"
#endif

// Include drivers for Olimex MOD-RGB
#if(IOBOARD_MODEL == 0x03)						
#	include "hardware/Olimex/MODRGB.cpp"
#endif