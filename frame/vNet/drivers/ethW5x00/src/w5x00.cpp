/*
 * Copyright (c) 2010 by Cristian Maglie <c.maglie@bug.st>
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#include <stdio.h>
#include <string.h>
#include <avr/interrupt.h>

#include "w5x00.h"

// W5x00 controller instance
W5x00Class W5x00;

#define TX_RX_MAX_BUF_SIZE 2048
#define TX_BUF 0x1100
#define RX_BUF (TX_BUF + TX_RX_MAX_BUF_SIZE)

#if(ETH_W5200)
#	define TXBUF_BASE 0x8000
#	define RXBUF_BASE 0xC000
#elif(ETH_W5100) 
#	define TXBUF_BASE 0x4000
#	define RXBUF_BASE 0x6000
#endif

void W5x00Class::init(void)
{
  delay(300);

  SPI.begin();
  initSS();
  
  writeMR(1<<RST);

#if(ETH_W5200)
  for (int i=0; i<MAX_SOCK_NUM; i++) {
    write((0x4000 + i * 0x100 + 0x001F), 2);
    write((0x4000 + i * 0x100 + 0x001E), 2);
  }
#elif(ETH_W5100) 
  writeTMSR(0x55);
  writeRMSR(0x55);
#endif  

  for (int i=0; i<MAX_SOCK_NUM; i++) {
    SBASE[i] = TXBUF_BASE + SSIZE * i;
    RBASE[i] = RXBUF_BASE + RSIZE * i;
  }
}

uint16_t W5x00Class::getTXFreeSize(SOCKET s)
{
  uint16_t val=0, val1=0;
  do {
    val1 = readSnTX_FSR(s);
    if (val1 != 0)
      val = readSnTX_FSR(s);
  } 
  while (val != val1);
  return val;
}

uint16_t W5x00Class::getRXReceivedSize(SOCKET s)
{
  uint16_t val=0,val1=0;
  do {
    val1 = readSnRX_RSR(s);
    if (val1 != 0)
      val = readSnRX_RSR(s);
  } 
  while (val != val1);
  
  return val;
}


void W5x00Class::send_data_processing(SOCKET s, const uint8_t *data, uint16_t len)
{
  // This is same as having no offset in a call to send_data_processing_offset
  send_data_processing_offset(s, 0, data, len);
}

void W5x00Class::send_data_processing_offset(SOCKET s, uint16_t data_offset, const uint8_t *data, uint16_t len)
{
  uint16_t ptr = readSnTX_WR(s);
  ptr += data_offset;
  uint16_t offset = ptr & SMASK;
  uint16_t dstAddr = offset + SBASE[s];
  oFrame* frame=0;
  
  // If len is equal to zero the buf pointer to a structure
  if(len==0)
  {	
	frame = (oFrame*)data;				// Pointer to output frame
	oFrame_Define(frame);				// Set the frame
	len = oFrame_GetLenght();			// Get the total lenght
  }
  
  if (offset + len > SSIZE) 
  {
    // Wrap around circular buffer
    uint16_t size = SSIZE - offset;
	
	if(frame==0)
	{
		write(dstAddr, data, size);
		write(SBASE[s], data + size, len - size);
	}
	else
	{
		// Write up the end of the buffer
		for(U8 i=0;i<size;i++)
			if(oFrame_Available())
				write(dstAddr++, oFrame_GetByte());
			else
				break;
		
		// Move to the start of the buffer
		dstAddr = SBASE[s];
		for(U8 i=0;i<(len - size);i++)
			if(oFrame_Available())
				write(dstAddr++, oFrame_GetByte());
			else
				break;
	}		
  } 
  else {
	if(frame==0)
		write(dstAddr, data, len);
	else
	{
		// Write down to the W5x00
		for(U8 i=0;i<len;i++)
			if(oFrame_Available())
				write(dstAddr++, oFrame_GetByte());
			else
				break;
	}
	
  }

  ptr += len;
  writeSnTX_WR(s, ptr);
}


void W5x00Class::recv_data_processing(SOCKET s, uint8_t *data, uint16_t len, uint8_t peek)
{
  uint16_t ptr;
  ptr = readSnRX_RD(s);
  read_data(s, (uint8_t *)ptr, data, len);
  if (!peek)
  {
    ptr += len;
    writeSnRX_RD(s, ptr);
  }
}

void W5x00Class::read_data(SOCKET s, volatile uint8_t *src, volatile uint8_t *dst, uint16_t len)
{
  uint16_t size;
  uint16_t src_mask;
  uint16_t src_ptr;

  src_mask = (uint16_t)src & RMASK;
  src_ptr = RBASE[s] + src_mask;

  if( (src_mask + len) > RSIZE ) 
  {
    size = RSIZE - src_mask;
    read(src_ptr, (uint8_t *)dst, size);
    dst += size;
    read(RBASE[s], (uint8_t *) dst, len - size);
  } 
  else
    read(src_ptr, (uint8_t *) dst, len);
}


uint8_t W5x00Class::write(uint16_t _addr, uint8_t _data)
{
  cli();  // turn off interrupts 
  setSS();  
  
#if(ETH_W5200)
  SPI.transfer(_addr >> 8);
  SPI.transfer(_addr & 0xFF);
  SPI.transfer(0x80);
  SPI.transfer(0x01);
#elif(ETH_W5100)
  SPI.transfer(0xF0);
  SPI.transfer(_addr >> 8);
  SPI.transfer(_addr & 0xFF);
#endif  
  
  SPI.transfer(_data);
  resetSS();
  sei();  // turn on interrupts 
  return 1;
}

uint16_t W5x00Class::write(uint16_t _addr, const uint8_t *_buf, uint16_t _len)
{
#if(ETH_W5200)
  cli();  // turn off interrupts 
  setSS();
  SPI.transfer(_addr >> 8);
  SPI.transfer(_addr & 0xFF);
  SPI.transfer((0x80 | ((_len & 0x7F00) >> 8)));
  SPI.transfer(_len & 0x00FF);

  for (uint16_t i=0; i<_len; i++)
    SPI.transfer(_buf[i]);
	
  sei();  // turn on interrupts 	
  resetSS();
#elif(ETH_W5100)
  for (uint16_t i=0; i<_len; i++)
  {
    cli();  // turn off interrupts 
	setSS();    
    SPI.transfer(0xF0);
    SPI.transfer(_addr >> 8);
    SPI.transfer(_addr & 0xFF);
    _addr++;
    SPI.transfer(_buf[i]);
    resetSS();
	sei();  // turn on interrupts 
  }
  return _len;
#endif

  return _len;
}

uint8_t W5x00Class::read(uint16_t _addr)
{
  cli();  // turn off interrupts 
  setSS();  
  
#if(ETH_W5200)
  SPI.transfer(_addr >> 8);
  SPI.transfer(_addr & 0xFF);
  SPI.transfer(0x00);
  SPI.transfer(0x01);
#elif(ETH_W5100)
  SPI.transfer(0x0F);
  SPI.transfer(_addr >> 8);
  SPI.transfer(_addr & 0xFF);
#endif  
  
  uint8_t _data = SPI.transfer(0);
  resetSS();
  sei();  // turn on interrupts 
  return _data;
}

uint16_t W5x00Class::read(uint16_t _addr, uint8_t *_buf, uint16_t _len)
{
#if(ETH_W5200)
  cli();  // turn off interrupts 
  setSS();
  SPI.transfer(_addr >> 8);
  SPI.transfer(_addr & 0xFF);
  SPI.transfer((0x00 | ((_len & 0x7F00) >> 8)));
  SPI.transfer(_len & 0x00FF);

  for (uint16_t i=0; i<_len; i++)
    _buf[i] = SPI.transfer(0);

  resetSS();
  sei();  // turn on interrupts 
#elif(ETH_W5100)
  for (uint16_t i=0; i<_len; i++)
  {
    cli();  // turn off interrupts 
    setSS();
    SPI.transfer(0x0F);
    SPI.transfer(_addr >> 8);
    SPI.transfer(_addr & 0xFF);
    _addr++;
    _buf[i] = SPI.transfer(0);
    resetSS();
	sei();  // turn on interrupts 
  }
#endif
  
  return _len;
}

void W5x00Class::execCmdSn(SOCKET s, SockCMD _cmd) {
  // Send command to socket
  writeSnCR(s, _cmd);
  // Wait for command to complete
  while (readSnCR(s))
    ;
}
