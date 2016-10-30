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
#ifndef PUBSUB_H
#define PUBSUB_H

/**************************************************************************/
/*!
	This macros connect a sender with a receiver that can do some action
	once received the notification, example of use:
	
	// Sender
	if(...)	publish(Alarm);
	
	// Receiver
	if(subscribe(Alarm))
	{
		// Do something
	}
	
	You can define your own topics as
		#define	your_topic_name		code1, code2
		
		where code1 is a two bytes value and code 2 is a single byte
*/	
/**************************************************************************/	

// Generic topics for publish/subscribe
#define	GeneralEvent									0xF001,0x01
#define	Alarm											0x0001,0x01
#define	Alarm_Fire										0x0001,0x02
#define	Alarm_WaterLeak									0x0001,0x03
#define	Alarm_GasLeak									0x0001,0x04
#define	DoorClosed										0x0002,0x01
#define	DoorOpen										0x0003,0x01
#define	Sunny											0x0004,0x01
#define	Raining											0x0004,0x02
#define	Cloudy											0x0004,0x03
#define Notify_GatewayAlive								0x0005,0x01

// General macros for publish/subscribe method
#define	pblsh(topic)								Souliss_Publish(memory_map, topic)
#define	m_pblsh(maddress, topic)					Souliss_MulticastPublish(maddress, memory_map, topic)
#define	pblshdata(topic, data, len)					Souliss_PublishData(memory_map, topic, data, len)
#define	m_pblshdata(maddress, topic, data, len)		Souliss_MulticastPublishData(maddress, memory_map, topic, data, len)
#define sbscrb(topic)								Souliss_Subscribe(memory_map, topic)
#define	sbscrbdata(topic, data, len)				Souliss_SubscribeData(memory_map, topic, data, len)

#endif
