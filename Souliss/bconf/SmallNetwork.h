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
#ifndef bconf_ssSMALLNTW_H
#define bconf_ssSMALLNTW_h

// If we are planning to use a small network, we can redefine the number of nodes
// and slots for nodes using the INSKETCH parameters and using values that are smaller
// than standard.

// Reduce the number of nodes in the network reduce the RAM usage in case of PERSISTANCE
// mode active, as when using the HTTP/XML polling interface (or similar).
// All nodes in the same network must share the same number of NODES and SLOT in their
// configuration.

#define	NODESIZE_INSKETCH
#define MaCaco_NODES			8				// Number of remote nodes
#define MaCaco_SLOT				0xF				// Number of slot

#endif
