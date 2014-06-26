/**************************************************************************
	Souliss - User Mode
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

/**************************************************************************/
/*!
    User Mode is an embeed functionality in vNet over IP that allow 
	communication with devices that are out of the LAN and/or use
	two different port while writing and receiving vNet frames.
		
	Using the User Mode, the entiere IP address is stored to allow
	communication over NAT.
*/
/**************************************************************************/

#include "UserMode.h"
#include "GetConfig.h"				// need : ethUsrCfg.h

#if(UMODE_ENABLE)

#define UMODE_LOG 			Serial.print
#define	IPADDRESS_BYTES		4
#define PPORT_BYTES			2

U16 in_vNet_Addresses[UMODE_USERS];
U8  last_entry, in_IP_Addresses[UMODE_USERS*IPADDRESS_BYTES], 
	in_P_Port[UMODE_USERS*PPORT_BYTES];

/**************************************************************************/
/*!
    Record the incoming IP address and link it with a vNet address
*/
/**************************************************************************/
void UserMode_Record(U16 addr, U8* ip_addr, U8* p_port)
{
	U8 i=0;

	// Verify that the entry was not yet saved or find the first available index
	for(i=0;i<UMODE_USERS;i++)
		if(in_vNet_Addresses[i]==addr)
			break;
		else if(in_vNet_Addresses[i]==0x0000)
			break;
	
	// If the table is full, use the oldest entry
	if(i==UMODE_USERS)
		i=(last_entry+1)%UMODE_USERS;
	
	// Record the last entry index
	last_entry = i;
	
	// Store the IP address
	in_vNet_Addresses[i] = addr;
	memmove(in_IP_Addresses + i*(IPADDRESS_BYTES), ip_addr, IPADDRESS_BYTES);	// Store the IP address
	memmove(in_P_Port + i*(PPORT_BYTES), p_port, PPORT_BYTES);					// Store the IP port
}

/**************************************************************************/
/*!
    Remove an IP address from the table starting from the vNet one
*/
/**************************************************************************/
void UserMode_Remove(U16 addr)
{
	U8 i=0;

	// Verify that the entry was not yet saved
	for(i=0;i<UMODE_USERS;i++)
		if(in_vNet_Addresses[i]==addr)
			break;
			
	// If the vNet address wasn't found, return
	if(i==UMODE_USERS)
		return;			
		
	// If the vNet address was found, delete the record	
	in_vNet_Addresses[i]=0x0000;	
	
}

/**************************************************************************/
/*!
    Remove all the IP addresses from the table
*/
/**************************************************************************/
void UserMode_Reset()
{
	// Verify that the entry was not yet saved
	for(U8 i=0;i<UMODE_USERS;i++)		
		in_vNet_Addresses[i]=0x0000;	
}

/**************************************************************************/
/*!
    Get an IP address from the table starting from the vNet one
*/
/**************************************************************************/
void UserMode_Get(U16 addr, U8* ip_addr, U8* p_port)
{
	U8 i=0;

	// Verify that the entry was not yet saved
	for(i=0;i<UMODE_USERS;i++)
		if(in_vNet_Addresses[i]==addr)
			break;

	// If the vNet address wasn't found, return
	if(i==UMODE_USERS)
		return;			
	
	// Return the IP address pointer
	memmove(ip_addr, in_IP_Addresses + i*(IPADDRESS_BYTES), IPADDRESS_BYTES);
	memmove(p_port, in_P_Port + i*(PPORT_BYTES), PPORT_BYTES);	
}

#endif