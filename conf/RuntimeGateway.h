/**************************************************************************
	Souliss
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
#ifndef bconf_ssGATEWAY_H
#define bconf_ssGATEWAY_H

#define	MaCacoCONF_INSKETCH

#define	MaCaco_USERMODE				1
#define MaCaco_SUBSCRIBERS			1	
#define MaCaco_PERSISTANCE			0
#define	MaCaco_LASTIN				0

/* Compared to a standard Gateway at compile time, in this case is not set by default the VNET_SUPERNODE
   this prevent nodes to route frames and process multi-hop broadcast messages.
 
   A node is considered a Runtime Gateway only if C8TO16(memory_map + MaCaco_ADDRESSES_s) returns a non zero
   value, this is set by the upper layer to define a node as Gateway.
*/



#endif
