/**************************************************************************
	Souliss - Output Frame
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
    This library help to build composed frames without waste memory, create
	only the new part of the frame and add it to the structure.
*/
/**************************************************************************/

#include "oFrame.h"
#define oFRAME_LOG 		Serial.print

#define MAX_BUSYCYCLE	5				// Max on number of cycle before auto-reset of the frame

U8 i_header, i_payload, i_busy;
oFrame *actualframe;

/**************************************************************************/
/*!
    Set the starting frame, than the cascaded ones will be retrieved
	automatically
*/
/**************************************************************************/
void oFrame_Define(oFrame *frame)
{
	// Store the pointer for the frame
	actualframe = frame;
	
	// Init the index
	i_header  = 0;
	i_payload = 0;
	i_busy = 0;
}

/**************************************************************************/
/*!
    Set the internal values and pointer for the frame
*/
/**************************************************************************/
void oFrame_Set(U8* header, U8* payload, U8 header_len, U8  payload_len, oFrame *next)
{
	actualframe->header = header;
	actualframe->payload = payload;
	actualframe->header_len = header_len;
	actualframe->payload_len = payload_len;
	actualframe->next = (U8*)next;
}

/**************************************************************************/
/*!
    Reset the actual frame
*/
/**************************************************************************/
void oFrame_Reset()
{
	actualframe->header = 0;
	actualframe->payload = 0;
	actualframe->header_len = 0;
	actualframe->payload_len = 0;
	actualframe->next = 0;
}

/**************************************************************************/
/*!
    Return if a frame is busy, if is it from too many cycle it reset
	the frame
*/
/**************************************************************************/
U8 oFrame_isBusy()
{
	// If the frame is available, increase the busy index
	if(oFrame_Available())
	{
		i_busy++;
		return 1;		
	}
		
	// If the frame is busy from too many cycles, reset it
	if(i_busy > MAX_BUSYCYCLE)
	{
		i_busy = 0;
		oFrame_Reset();
	}	
}

/**************************************************************************/
/*!
    Return the number of bytes in the actual frame
*/
/**************************************************************************/
U8 oFrame_Available()
{
	// If a frame is defined
	if(actualframe)
		if (oFrame_GetLenght())
			return 1;
	else
		return 0;
}

/**************************************************************************/
/*!
    Get the total lenght
*/
/**************************************************************************/
U8 oFrame_GetLenght()
{
	U8 len=0;
	oFrame* frame = actualframe;
	
	// While the frame pointer is not null
	while(frame)
	{

		len+=frame->header_len+frame->payload_len;		// Get the frame lenght
		frame = (oFrame*)frame->next;					// Move to next frame
	}	
	return len;
}
/**************************************************************************/
/*!
    Get the next byte from the output frame
*/
/**************************************************************************/
U8 oFrame_GetByte()
{
	U8 outbyte;
	oFrame*	previousframe;
	
	// If the actual frame is empty, look for the next
	if(!(actualframe->header_len) && !(actualframe->payload_len))
	{
		// Store the pointer for the next frame
		previousframe = actualframe;
		actualframe = (oFrame *)actualframe->next;
		
		// Delete the connection with the previous frame
		previousframe->next=0;
	
		// Init the index
		i_header  = 0;
		i_payload = 0;	
	}	
	
	if(actualframe->header_len)					// Look for header's bytes
	{
		actualframe->header_len--;
		outbyte = actualframe->header[i_header++];
	}	
	else if(actualframe->payload_len)			// Look for payload's bytes
	{
		actualframe->payload_len--;
		outbyte = actualframe->payload[i_payload++];	
	}
	
	return outbyte;
}

/**************************************************************************/
/*!
    Make a copy of a source oFrame into a destination one
*/
/**************************************************************************/
void oFrame_Copy(oFrame *destination, oFrame *source)
{
	destination->header = source->header;				
	destination->payload = source->payload;			
	destination->header_len = source->header_len;		
	destination->payload_len = source->payload_len;		
	destination->next = source->next;				
}

/**************************************************************************/
/*!
    Print the oFrame, use for debug purpose
*/
/**************************************************************************/
#if(VNET_DEBUG)
void oFrame_Debug()
{
	oFrame* frame = actualframe;
	
	// While the frame pointer is not null
	while(frame)
	{
		oFRAME_LOG("\nheader ");
		oFRAME_LOG((U16)frame->header,HEX);
		oFRAME_LOG("\npayload ");
		oFRAME_LOG((U16)frame->payload,HEX);		
		oFRAME_LOG("\nheader_len ");
		oFRAME_LOG((U16)frame->header_len,HEX);
		oFRAME_LOG("\npayload_len ");
		oFRAME_LOG((U16)frame->payload_len,HEX);			
		oFRAME_LOG("\nnext ");
		oFRAME_LOG((U16)frame->next,HEX);		
		oFRAME_LOG("\r\n");
		frame = (oFrame*)frame->next;					// Move to next frame
	}	

}
#endif