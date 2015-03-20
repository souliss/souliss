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
#ifndef CHB_H
#define CHB_H

#include "GetConfig.h"				// need : chibiUsrCfg.h
#include "src/types.h"

#define CHB_HDR_SZ        9    // FCF + seq + pan_id + dest_addr + src_addr (2 + 1 + 2 + 2 + 2)
#define CHB_FCS_LEN       2

// frame_type = data
// security enabled = false
// frame pending = false
// ack request = false
// pan ID compression = true
#define CHB_FCF_BYTE_0    0x41    

// dest addr = 16-bit
// frame version = 802.15.4 (not 2003)
// src addr = 16-bit
#define CHB_FCF_BYTE_1    0x98

#define CHB_ACK_REQ_POS   5

enum
{
    CHB_SUCCESS                 = 0,
    CHB_SUCCESS_DATA_PENDING    = 1,
    CHB_CHANNEL_ACCESS_FAILURE  = 3,
    CHB_NO_ACK                  = 5,
    CHB_INVALID                 = 7
};

typedef struct
{
    U16 src_addr;
    U8 seq;
    volatile bool data_rcv;
    volatile bool trx_end;

    // stats
    U16 rcvd_xfers;
    U16 txd_success;
    U16 txd_noack;
    U16 txd_channel_fail;
    U16 overflow;
    U16 underrun;
    U8 battlow;
    U8 status;
    U8 ed;
    U8 crc;
} pcb_t;

typedef struct
{
    U8 len;
    U16 src_addr;
    U16 dest_addr;
    U8 *data;
} chb_rx_data_t;

void chb_init();
pcb_t *chb_get_pcb();
U8 chb_write(U16 addr, oFrame *frame, U8 len);
U8 chb_read(chb_rx_data_t *rx);

#endif
