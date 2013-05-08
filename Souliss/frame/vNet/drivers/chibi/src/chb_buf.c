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
#include "chb_buf.h"

static U8 chb_buf[CHB_BUF_SZ];
static U8 rd_ptr, wr_ptr, len;

/**************************************************************************/
/*!

*/
/**************************************************************************/
void chb_buf_init()
{
    rd_ptr = 0;
    wr_ptr = 0;
    len = 0;
}

/**************************************************************************/
/*!

*/
/**************************************************************************/
void chb_buf_write(U8 data)
{
    chb_buf[wr_ptr] = data;
    wr_ptr = (wr_ptr + 1) % CHB_BUF_SZ;
    len++;
}

/**************************************************************************/
/*!

*/
/**************************************************************************/
U8 chb_buf_read()
{
    U8 data;

    data = chb_buf[rd_ptr];
    rd_ptr = (rd_ptr + 1) % CHB_BUF_SZ;
    len--;
    return data;
}

/**************************************************************************/
/*!

*/
/**************************************************************************/
U8 chb_buf_get_len()
{
    return len;
}
