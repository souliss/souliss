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
#ifndef ACTIONMESSAGES_H
#define ACTIONMESSAGES_H

/**************************************************************************
/*!
	This macros connect a sender with a receiver that can do some action
	once received the notification, example of use:
	
	// Sender
	if(...)	broadcast_Alarm();
	
	// Receiver
	if(is_Alarm())
	{
		// Do something
	}
*/	
/**************************************************************************/	

/*** Alarms ***/

// Standard Alarm
#define	broadcast_Alarm()							Souliss_BroadcastAction(memory_map, 0x0001, 0x01)
#define	multicast_Alarm(maddress)					Souliss_BroadcastAction(maddress, memory_map, 0x0001, 0x01)
#define	broadcastd_Alarm(data, len)					Souliss_BroadcastActionMessage(memory_map, 0x0001, 0x01, data, len)
#define	multicastd_Alarm(maddress, data, len)		Souliss_BroadcastActionMessage(maddress, memory_map, 0x0001, 0x01, data, len)
#define	is_Alarm()									Souliss_GetAction(memory_map, 0x0001, 0x01)
#define	isd_Alarm(data, len)						Souliss_GetActionMessage(memory_map, 0x0001, 0x01, data, len)

// Fire Alarm
#define	broadcast_AlarmFire()						Souliss_BroadcastAction(memory_map, 0x0001, 0x02)
#define	multicast_AlarmFire(maddress)				Souliss_BroadcastAction(maddress, memory_map, 0x0001, 0x02)
#define	broadcastd_AlarmFire(data, len)				Souliss_BroadcastActionMessage(memory_map, 0x0001, 0x02, data, len)
#define	multicastd_AlarmFire(maddress, data, len)	Souliss_BroadcastActionMessage(maddress, memory_map, 0x0001, 0x02, data, len)
#define	is_AlarmFire()								Souliss_GetAction(memory_map, 0x0001, 0x02)
#define	isd_AlarmFire(data, len)					Souliss_GetActionMessage(memory_map, 0x0001, 0x02, data, len)

/*** Events ***/

// Door open 
#define	broadcast_DoorOpen()							Souliss_BroadcastAction(memory_map, 0x0002, 0x01)
#define	multicast_DoorOpen(maddress)					Souliss_BroadcastAction(maddress, memory_map, 0x0002, 0x01)
#define	broadcastd_DoorOpen(data, len)					Souliss_BroadcastActionMessage(memory_map, 0x0002, 0x01, data, len)
#define	multicastd_DoorOpen(maddress, data, len)		Souliss_BroadcastActionMessage(maddress, memory_map, 0x0002, 0x01, data, len)
#define	is_DoorOpen()									Souliss_GetAction(memory_map, 0x0002, 0x01)
#define	isd_DoorOpen(data, len)							Souliss_GetActionMessage(memory_map, 0x0002, 0x01, data, len)

// Door closed 
#define	broadcast_DoorClosed()							Souliss_BroadcastAction(memory_map, 0x0002, 0x02)
#define	multicast_DoorClosed(address)					Souliss_BroadcastAction(maddress, memory_map, 0x0002, 0x02)
#define	broadcastd_DoorClosed(data, len)				Souliss_BroadcastActionMessage(memory_map, 0x0002, 0x02, data, len)
#define	multicastd_DoorClosed(maddress, data, len)		Souliss_BroadcastActionMessage(maddress, memory_map, 0x0002, 0x02, data, len)
#define	is_DoorClosed()									Souliss_GetAction(memory_map, 0x0002, 0x02)
#define	isd_DoorClosed(data, len)						Souliss_GetActionMessage(memory_map, 0x0002, 0x02, data, len)

// Is raining 
#define	broadcast_Raining()								Souliss_BroadcastAction(memory_map, 0x0002, 0x10)
#define	multicast_Raining(maddress)						Souliss_BroadcastAction(maddress, memory_map, 0x0002, 0x10)
#define	broadcastd_Raining(data, len)					Souliss_BroadcastActionMessage(memory_map, 0x0002, 0x10, data, len)
#define	multicastd_Raining(maddress, data, len)			Souliss_BroadcastActionMessage(maddress, memory_map, 0x0002, 0x10, data, len)
#define	is_Raining()									Souliss_GetAction(memory_map, 0x0002, 0x10)
#define	isd_Raining(data, len)							Souliss_GetActionMessage(memory_map, 0x0002, 0x10, data, len)

// Rain stops 
#define	broadcast_RainStops()							Souliss_BroadcastAction(memory_map, 0x0002, 0x11)
#define	multicast_RainStops(maddress)					Souliss_BroadcastAction(maddress, memory_map, 0x0002, 0x11)
#define	broadcastd_RainStops(data, len)					Souliss_BroadcastActionMessage(memory_map, 0x0002, 0x11, data, len)
#define	multicastd_RainStops(maddress, data, len)		Souliss_BroadcastActionMessage(maddress, memory_map, 0x0002, 0x11, data, len)
#define	is_RainStops()									Souliss_GetAction(memory_map, 0x0002, 0x11)
#define	isd_RainStops(data, len)						Souliss_GetActionMessage(memory_map, 0x0002, 0x11, data, len)

// Sunny day
#define	broadcast_Sunny()								Souliss_BroadcastAction(memory_map, 0x0002, 0x12)
#define	multicast_Sunny(maddress)						Souliss_BroadcastAction(maddress, memory_map, 0x0002, 0x12)
#define	broadcastd_Sunny(data, len)						Souliss_BroadcastActionMessage(memory_map, 0x0002, 0x12, data, len)
#define	multicastd_Sunny(maddress, data, len)			Souliss_BroadcastActionMessage(maddress, memory_map, 0x0002, 0x12, data, len)
#define	is_Sunny()										Souliss_GetAction(memory_map, 0x0002, 0x12)
#define	isd_Sunny(data, len)							Souliss_GetActionMessage(memory_map, 0x0002, 0x12, data, len)

#endif
