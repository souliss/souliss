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
#ifndef ACTIONMESSAGES_H
#define ACTIONMESSAGES_H

/**************************************************************************
/*!
	This macros connect a sender with a receiver that can do some action
	once received the notification, example of use:
	
	// Sender
	if(...)	broadcast_Event(event_number);
	
	// Receiver
	if(is_Event(event_number))
	{
		// Do something
	}
	
	The followings methods are available:
		broadcast_##name()
		broadcastd_##name(data, len)
		multicast_##name(maddress)
		multicastd_##name(maddress, data, len)
		is_##name()
		isd_##name(data, len)
		
	where ##name can be:
		Alarm
		AlarmFire
		DoorOpen
		...
*/	
/**************************************************************************/	

#define	broadcast_Event(event)						Souliss_BroadcastAction(memory_map, 0xF001, event)
#define	is_Event(event)								Souliss_GetAction(memory_map, 0xF001, event)
#define	broadcastd_Event(event,data,len)			Souliss_BroadcastActionMessage(memory_map, 0xF001, event, data, len)
#define	isd_Event(event,data,len)					Souliss_GetActionMessage(memory_map, 0xF001, event, data, len)

#define __ACTION_MESSAGE(name, address1, address2)            									\
	static inline void broadcast_##name() { 													\
		Souliss_BroadcastAction(memory_map, address1, address2);								\
	}																							\
	static inline void broadcastd_##name(data, len) { 											\
		Souliss_BroadcastActionMessage(memory_map, address1, address2, data, len);				\
	}																							\
	static inline void multicast_##name(maddress) { 											\
		Souliss_BroadcastAction(maddress, memory_map, address1, address2);						\
	}																							\
	static inline void multicastd_##name(maddress, data, len) { 								\
		Souliss_BroadcastActionMessage(maddress, memory_map, address1, address2, data, len);	\
	}																							\
	static inline void is_##name() { 															\
		Souliss_GetAction(memory_map, address1, address2);										\
	}																							\
	static inline void isd_##name(data, len) { 													\
		Souliss_GetActionMessage(memory_map, address1, address2, data, len));					\
	}																							\		

/*** Event List ***/	
__ACTION_MESSAGE(Alarm,  		0x0001, 0x01);
__ACTION_MESSAGE(AlarmFire, 	0x0001, 0x02);
__ACTION_MESSAGE(DoorOpen, 		0x0002, 0x01);
__ACTION_MESSAGE(DoorClosed, 	0x0002, 0x02);
__ACTION_MESSAGE(Raining, 		0x0002, 0x10);
__ACTION_MESSAGE(RainStops, 	0x0002, 0x11);
__ACTION_MESSAGE(Sunny, 		0x0002, 0x12);

#endif