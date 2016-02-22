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
    User Mode is an embed functionality in vNet over IP that allow 
	communication with devices that are out of the LAN and/or use
	two different port while writing and receiving vNet frames.
		
	Using the User Mode, the entire IP address is stored to allow
	communication over NAT.
*/
/**************************************************************************/

#include "UserMode.h"
#include "GetConfig.h"				// need : ethUsrCfg.h

#if(UMODE_ENABLE)

#define	IPADDRESS_BYTES		4
#define PPORT_BYTES			2

U16 in_vNet_Addresses[UMODE_USERS];
U8  last_entry, last_entry_blacklist,
	in_IP_Addresses[UMODE_USERS*IPADDRESS_BYTES],
	blacklist_IP_Addresses[UMODE_USERS*IPADDRESS_BYTES], in_P_Port[UMODE_USERS*PPORT_BYTES];

#if (VNET_DEBUG)
	#define VNET_LOG LOG.print	
#endif

/**************************************************************************/
/*!
    Initialization
*/
/**************************************************************************/
void UserMode_Init()
{
	for(U8 i=0;i<(UMODE_USERS*IPADDRESS_BYTES);i++)
	{
		in_IP_Addresses[i]=0;
		blacklist_IP_Addresses[i]=0;
	}

	// If there were previous saved vNet address from User Interfaces
	#if(USEEEPROM)
	if(Return_ID()==STORE__DEFAULTID)
		Return_UserModeAddresses(in_vNet_Addresses, UMODE_USERS);
	#endif

	#if(VNET_DEBUG)
	VNET_LOG(F("(vNet)<USERMODE><0x"));
	for(U8 i=0;i<(UMODE_USERS);i++)
	{
		VNET_LOG(in_vNet_Addresses[i],HEX);
		VNET_LOG(F("|0x"));
	}
	VNET_LOG(F(">\r\n"));
	#endif	
}

/**************************************************************************/
/*!
    Record a vNet address manually, this allow a bypass of the USERLOCKDOWN
	at compile time
*/
/**************************************************************************/
void UserMode_ManualRecord(U16 addr)
{
	U8 i=0;

	// Verify that the entry was not yet saved or find the first available index
	for(i=0;i<UMODE_USERS;i++)
		if((in_vNet_Addresses[i]==addr) || (in_vNet_Addresses[i]==0x0000))
			break;

	if(i==UMODE_USERS)
		return;
	
	in_vNet_Addresses[i] = addr;
}

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
		if((in_vNet_Addresses[i]==addr) || (in_vNet_Addresses[i]==0x0000))
			break;


	// Accept new incoming connection only just after the boot
	#if(VNET_USERLOCKDOWN)
	if(!JustBooted())
	{
		// Update the IP address for existing entries
		if(in_vNet_Addresses[i]==addr)
		{
			// Refresh the IP address
			memmove(in_IP_Addresses + i*(IPADDRESS_BYTES), ip_addr, IPADDRESS_BYTES);	// Store the IP address
			memmove(in_P_Port + i*(PPORT_BYTES), p_port, PPORT_BYTES);					// Store the IP port
		}
		else
		{
			//Record the IP address in the black list
			for(i=0;i<(UMODE_USERS);i++)
				if((blacklist_IP_Addresses[i*4] == 0) || 
					((*(ip_addr) == *(blacklist_IP_Addresses+i)) && (*(ip_addr+1) == *(blacklist_IP_Addresses+i+1)) &&
					(*(ip_addr+2) == *(blacklist_IP_Addresses+i+2)) && (*(ip_addr+3) == *(blacklist_IP_Addresses+i+3))))

					break;
			
			// If the table is full start from the first entry
			if(i==UMODE_USERS)
				i=(last_entry_blacklist+1)%UMODE_USERS;
			
			last_entry_blacklist=i;

			// Store the IP address in the blacklist
			blacklist_IP_Addresses[i]	= ip_addr[0];
			blacklist_IP_Addresses[i+1] = ip_addr[1];
			blacklist_IP_Addresses[i+2] = ip_addr[2];
			blacklist_IP_Addresses[i+3] = ip_addr[3];
		}

		#if(VNET_DEBUG)
		VNET_LOG(F("(vNet)<Lockdwn>"));
		#endif			

		return;
	}
	#endif

	// If the table is full, use the oldest entry
	if(i==UMODE_USERS)
		i=(last_entry+1)%UMODE_USERS;
	
	// Record the last entry index
	last_entry = i;
	
	// If is a new vNet address, record it
	if((in_vNet_Addresses[i]!=addr))
	{
		in_vNet_Addresses[i] = addr;
		
		#if(USEEEPROM)
		Store_UserModeAddresses(in_vNet_Addresses, UMODE_USERS);
		Store_Commit();
		#endif	

		#if(VNET_DEBUG)
		VNET_LOG(F("(vNet)<USERMODE><0x"));
		for(U8 i=0;i<(UMODE_USERS);i++)
		{
			VNET_LOG(in_vNet_Addresses[i],HEX);
			VNET_LOG(F("|0x"));
		}
		VNET_LOG(F(">\r\n"));
		#endif		
	}

	// Refresh the IP address
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
uint8_t UserMode_Get(U16 addr, U8* ip_addr, U8* p_port)
{
	U8 i=0;

	// Verify that the entry was not yet saved
	for(i=0;i<UMODE_USERS;i++)
		if(in_vNet_Addresses[i]==addr)
			break;

	// If the vNet address wasn't found, return
	if(i==UMODE_USERS)
		return 0;			
	
	// Return the IP address pointer
	memmove(ip_addr, in_IP_Addresses + i*(IPADDRESS_BYTES), IPADDRESS_BYTES);
	memmove(p_port, in_P_Port + i*(PPORT_BYTES), PPORT_BYTES);	

	// If the IP address is in black list skip
	#if(VNET_USERLOCKDOWN)
	for(i=0;i<UMODE_USERS;i++)
	{
		if((*(ip_addr) == *(blacklist_IP_Addresses+i)) && (*(ip_addr+1) == *(blacklist_IP_Addresses+i+1)) &&
			(*(ip_addr+2) == *(blacklist_IP_Addresses+i+2)) && (*(ip_addr+3) == *(blacklist_IP_Addresses+i+3)))
		{
			#if(VNET_DEBUG)
			VNET_LOG(F("(vNet)<Blacklist>\r\n"));
			#endif			

			return 0;
		}
	}
	#endif

	return 1;
}

#endif