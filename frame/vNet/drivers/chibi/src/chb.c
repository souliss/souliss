/*******************************************************************
    Copyright (C) 2009 FreakLabs
    All rights reserved.
    
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:
 
    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
    3. Neither the name of the the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software
       without specific prior written permission.
    
    THIS SOFTWARE IS PROVIDED BY THE THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS'' AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
    OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
    OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
    SUCH DAMAGE.
 
    Originally written by Christopher Wang aka Akiba.
    Please post support questions to the FreakLabs forum.

*******************************************************************/
#include <stdio.h>
#include <string.h>

#include "chb.h"
#include "chb_drvr.h"
#include "chb_buf.h"

static pcb_t pcb;

// these are for the duplicate checking and rejection
static U8 prev_seq = 0xFF;
static U16 prev_src_addr = 0xFFFE;

U8 hdr[CHB_HDR_SZ + 1];		// Buffer for header
oFrame chb_oFrame;			// Data structure for output frame

/**************************************************************************/
/*!

*/
/**************************************************************************/
void chb_init()
{
    memset(&pcb, 0, sizeof(pcb_t));
    pcb.src_addr = chb_get_short_addr();
    chb_drvr_init();
}

/**************************************************************************/
/*!

*/
/**************************************************************************/
pcb_t *chb_get_pcb()
{
    return &pcb;
}

/**************************************************************************/
/*! 
    Requires the dest addr, location to store data, and len of payload.
    Returns the length of the hdr. 
*/
/**************************************************************************/
static U8 chb_gen_hdr(U8 *hdr, U16 addr, U8 len)
{
    U8 *hdr_ptr = hdr;

    // calc frame size and put in 0 position of array
    // frame size = hdr sz + payload len + fcs len
    *hdr_ptr++ = CHB_HDR_SZ + len + CHB_FCS_LEN;

    // use default fcf byte 0 val but test for ack request. we won't request
    // ack if broadcast. all other cases we will.
    *hdr_ptr++ = CHB_FCF_BYTE_0 | ((addr != 0xFFFF) << CHB_ACK_REQ_POS);
    *hdr_ptr++ = CHB_FCF_BYTE_1;

    *hdr_ptr++ = pcb.seq++;

    // fill out dest pan ID, dest addr, src addr
    *(U16 *)hdr_ptr = CHB_PAN_ID;
    hdr_ptr += sizeof(U16);
    *(U16 *)hdr_ptr = addr;
    hdr_ptr += sizeof(U16);
    *(U16 *)hdr_ptr = pcb.src_addr;
    hdr_ptr += sizeof(U16);
    
    // return the len of the header
    return hdr_ptr - hdr;
}

/**************************************************************************/
/*!
    This is the main function that transmits the data array to the specified
    address.
*/
/**************************************************************************/
U8 chb_write(U16 addr, oFrame *frame, U8 len)
{
    U8 status, frm_len, hdr_len;
	
    while (len > 0)
    {
        // calculate which frame len to use. if greater than max payload, split
        // up operation.
        frm_len = (len > CHB_MAX_PAYLOAD) ? CHB_MAX_PAYLOAD : len;

        // gen frame header
        hdr_len = chb_gen_hdr(hdr, addr, frm_len);	
		
		// Build the complete frame
		oFrame_Define(&chb_oFrame);
		oFrame_Set(hdr, 0, hdr_len, 0, frame);
		
        // send data to chip
        //status = chb_tx(hdr, data, frm_len);
		status = chb_tx(&chb_oFrame);

        if (status != CHB_SUCCESS)
        {
            switch (status)
            {
            case RADIO_SUCCESS:
                // fall through
            case CHB_SUCCESS_DATA_PENDING:
                pcb.txd_success++;
                break;

            case CHB_NO_ACK:
                pcb.txd_noack++;
                break;

            case CHB_CHANNEL_ACCESS_FAILURE:
                pcb.txd_channel_fail++;
                break;

            default:
                break;
            }
			
			if (status == RADIO_SUCCESS) 	// Added by Dario to manage the return properly
				return 1;
			else
				return 0;
        }

        // adjust len and restart
        len = len - frm_len;
    }
	
    return CHB_SUCCESS;
}

/**************************************************************************/
/*!
    Read data from the buffer. Need to pass in a buffer of at leasts max frame
    size and two 16-bit containers for the src and dest addresses.
 
    The read function will automatically populate the addresses and the data with
    the frm payload. It will then return the len of the payload.
*/
/**************************************************************************/
U8 chb_read(chb_rx_data_t *rx)
{
    U8 i, len, seq, *data_ptr;

    data_ptr = rx->data;

	// this should not happen, if buffer is empty then exit
	if (!chb_buf_get_len())
	{
		pcb.data_rcv = false;

		return 0;		
	}
	
    // first byte is always len. check it to make sure 
    // we have a valid len byte.
	len = chb_buf_read();

	// a device couldn't send frame out of min and max lenght,
	// if frame lenght is out of these, discard and reset the
	// buffer
    if ((len > CHB_MAX_FRAME_LENGTH) || (len <= CHB_HDR_SZ))  
	{
		chb_buf_init();	
		pcb.data_rcv = false;

		return 0;		
	}
	
    *data_ptr++ = len;


    // load the rest of the data into buffer
    for (i=0; i<len; i++)
        *data_ptr++ = chb_buf_read();


    // we're using the buffer that's fed in as an argument as a temp
    // buffer as well to save resources.
    // we'll use it as temp storage to parse the frame. then move the frame
    // down so that only the payload will be in the buffer.

    // extract the sequence number
    data_ptr = rx->data + 3;    // location of sequence number
    seq = *data_ptr;

    // parse the buffer and extract the dest and src addresses
    data_ptr = rx->data + 6;                // location of dest addr
    rx->dest_addr = *(U16 *)data_ptr;
    data_ptr += sizeof(U16);
    rx->src_addr = *(U16 *)data_ptr;
    data_ptr += sizeof(U16);

    // if the data in the rx buf is 0, then clear the rx_flag. otherwise, keep it raised
    if (!chb_buf_get_len())
		pcb.data_rcv = false;


#if (CHIBI_PROMISCUOUS)
    // if we're in promiscuous mode, we don't want to do any duplicate rejection and we don't want to move the payload
    // to the front of the buffer. We want to capture the full frame so just keep the frame intact and return the length.
    return len;
#else
    // duplicate frame check (dupe check). we want to remove frames that have been already been received since they 
    // are just retries. 
    // note: this dupe check only removes duplicate frames from the previous transfer. if another frame from a different
    // node comes in between the dupes, then the dupe will show up as a received frame.
    if ((seq == prev_seq) && (rx->src_addr == prev_src_addr))
    {
        // this is a duplicate frame from a retry. the remote node thinks we didn't receive 
        // it properly. discard.
        return 0;
    }
    else
    {
        prev_seq = seq;
        prev_src_addr = rx->src_addr;
    }

    // move the payload down to the beginning of the data buffer
	memmove(rx->data, data_ptr, len - CHB_HDR_SZ);
	
    // finally, return the len of the payload
    return len - CHB_HDR_SZ - CHB_FCS_LEN;
#endif

}
