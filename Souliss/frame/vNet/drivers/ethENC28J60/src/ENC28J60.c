/**************************************************************************
	Souliss - vNet Virtualized Network
    
	Porting in vNet for Souliss project by Dario Di Maio
	
***************************************************************************/
/*********************************************
 * vim:sw=8:ts=8:si:et
 * To use the above modeline in vim you must have "set modeline" in your .vimrc
 * Author: Guido Socher 
 * Copyright: GPL V2
 * http://www.gnu.org/licenses/gpl.html
 *
 * Based on the enc28j60.c file from the AVRlib library by Pascal Stang.
 * For AVRlib See http://www.procyonengineering.com/
 * Used with explicit permission of Pascal Stang.
 *
 * Title: Microchip ENC28J60 Ethernet Interface Driver
 * Chip type           : ATMEGA88 with ENC28J60
 *********************************************/
// Updated 16/06/2010 ADL Added configurable CS, 16 bit writes and reads.

#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>

#include "enc28j60.h"
#include "Arduino.h"  //all things wiring / arduino
#include "spi.h"

static uint8_t Enc28j60Bank;
static uint16_t gNextPacketPtr;

// Enable ENC28J60 after disabling interupts
static void enableChip() {
    cli();
    ENC28J60_CSoff();
}

// Disable ENC28J60 then enable interupts
static void disableChip() {
    ENC28J60_CSon();
    sei();
}

static void sendSPI(byte data) {
    SPDR = data;
    while (!(SPSR&(1<<SPIF)))
        ;
}

uint8_t enc28j60ReadOp(uint8_t op, uint8_t address)
{
        enableChip();
        // issue read command
        sendSPI(op | (address & ADDR_MASK));
        sendSPI(0x00);
        if (address & 0x80)
            sendSPI(0x00);

        byte result = SPDR;
        // release CS
        disableChip();
        return result;
}

void enc28j60WriteOp(uint8_t op, uint8_t address, uint8_t data)
{
    enableChip();
    sendSPI(op | (address & ADDR_MASK));
    sendSPI(data);
    disableChip();
}

void enc28j60ReadBuffer(uint16_t len, uint8_t* data)
{
    enableChip();
    sendSPI(ENC28J60_READ_BUF_MEM);
    while (len--) {
        sendSPI(0x00);
        *data++ = SPDR;
    }
    disableChip();
    *data='\0';
}

static word enc28j60ReadBufferWord() {
    word result;
    enc28j60ReadBuffer(2, (byte*) &result);
    return result;
}


void enc28j60WriteBuffer(uint16_t len, uint8_t* data)
{
    enableChip();
    sendSPI(ENC28J60_WRITE_BUF_MEM);
    while (len--)
        sendSPI(*data++);

    disableChip();
}

void enc28j60SetBank(uint8_t address)
{
    if ((address & BANK_MASK) != Enc28j60Bank) {
        enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_BSEL1|ECON1_BSEL0);
        Enc28j60Bank = address & BANK_MASK;
        enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, Enc28j60Bank>>5);
    }
}

uint8_t enc28j60Read(uint8_t address)
{
        // set the bank
        enc28j60SetBank(address);
        // do the read
        return enc28j60ReadOp(ENC28J60_READ_CTRL_REG, address);
}
/*
static void enc28j60WriteByte(byte address, byte data) {
    enc28j60SetBank(address);
    enc28j60WriteOp(ENC28J60_WRITE_CTRL_REG, address, data);
}
*/
void enc28j60WriteWord(byte address, word data) {
//    enc28j60WriteByte(address, data & 0xff);
//    enc28j60WriteByte(address + 1, data >> 8);
    enc28j60Write(address, data & 0xff);
    enc28j60Write(address + 1, data >> 8);
}

// read upper 8 bits
uint16_t enc28j60PhyReadH(uint8_t address)
{
	// Set the right address and start the register read operation
	enc28j60Write(MIREGADR, address);
	enc28j60Write(MICMD, MICMD_MIIRD);
        delayMicroseconds(15);

	// wait until the PHY read completes
	while(enc28j60Read(MISTAT) & MISTAT_BUSY);

	// reset reading bit
	enc28j60Write(MICMD, 0x00);
	
	return (enc28j60Read(MIRDH));
}


void enc28j60Write(uint8_t address, uint8_t data)
{
        // set the bank
        enc28j60SetBank(address);
        // do the write
        enc28j60WriteOp(ENC28J60_WRITE_CTRL_REG, address, data);
}


void enc28j60PhyWrite(uint8_t address, uint16_t data)
{
        // set the PHY register address
        enc28j60Write(MIREGADR, address);
        // write the PHY data
        enc28j60Write(MIWRL, data);
        enc28j60Write(MIWRH, data>>8);
        // wait until the PHY write completes
        while(enc28j60Read(MISTAT) & MISTAT_BUSY){
                delayMicroseconds(15);
        }
}
/*
static void enc28j60PhyWriteWord(byte address, word data) {
    enc28j60Write(MIREGADR, address);
    //enc28j60WriteByte(MIREGADR, address);
    enc28j60WriteWord(MIWRL, data);
    while (enc28j60ReadByte(MISTAT) & MISTAT_BUSY)
        ;
}
*/
void enc28j60clkout(uint8_t clk)
{
        //setup clkout: 2 is 12.5MHz:
	enc28j60Write(ECOCON, clk & 0x7);
}

void enc28j60Init( uint8_t* macaddr)
{
	ENC28J60_CSInit();

	ENC28J60_SpiInit();
    
    // use clk/8 (2x 1/16th) to avoid exceeding RF12's SPI specs of 2.5 MHz
    SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR0);
    SPSR |= _BV(SPI2X);
            
	// perform system reset
	enc28j60WriteOp(ENC28J60_SOFT_RESET, 0, ENC28J60_SOFT_RESET);
	delay(50);
	// check CLKRDY bit to see if reset is complete
        // The CLKRDY does not work. See Rev. B4 Silicon Errata point. Just wait.
	//while(!(enc28j60Read(ESTAT) & ESTAT_CLKRDY));
	// do bank 0 stuff
	// initialize receive buffer
	// 16-bit transfers, must write low byte first
	// set receive buffer start address
	gNextPacketPtr = RXSTART_INIT;
        // Rx start
	enc28j60WriteWord(ERXSTL, RXSTART_INIT);
	// set receive pointer address
	enc28j60WriteWord(ERXRDPTL, RXSTART_INIT);
	// RX end
	enc28j60WriteWord(ERXNDL, RXSTOP_INIT);
	// TX start
	enc28j60WriteWord(ETXSTL, TXSTART_INIT);
	// TX end
	enc28j60WriteWord(ETXNDL, TXSTOP_INIT);
	// do bank 1 stuff, packet filter:
        // For broadcast packets we allow only ARP packtets
        // All other packets should be unicast only for our mac (MAADR)
        //
        // The pattern to match on is therefore
        // Type     ETH.DST
        // ARP      BROADCAST
        // 06 08 -- ff ff ff ff ff ff -> ip checksum for theses bytes=f7f9
        // in binary these poitions are:11 0000 0011 1111
        // This is hex 303F->EPMM0=0x3f,EPMM1=0x30
	enc28j60Write(ERXFCON, ERXFCON_UCEN|ERXFCON_CRCEN|ERXFCON_PMEN);
	enc28j60WriteWord(EPMM0, 0x303f);
	enc28j60WriteWord(EPMCSL, 0xf7f9);
        //
	// do bank 2 stuff
	// enable MAC receive
	enc28j60Write(MACON1, MACON1_MARXEN|MACON1_TXPAUS|MACON1_RXPAUS);
	// bring MAC out of reset
	enc28j60Write(MACON2, 0x00);
	// enable automatic padding to 60bytes and CRC operations
	enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, MACON3, MACON3_PADCFG0|MACON3_TXCRCEN|MACON3_FRMLNEN);  //|MACON3_FULDPX);
//	enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, MACON3, MACON3_PADCFG2|MACON3_PADCFG1|MACON3_PADCFG0|MACON3_TXCRCEN|MACON3_FRMLNEN);
	//enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, MACON3, MACON3_PADCFG0|MACON3_TXCRCEN|MACON3_FRMLNEN);
	// set inter-frame gap (non-back-to-back)
	enc28j60WriteWord(MAIPGL, 0x0C12);
	// set inter-frame gap (back-to-back)
	enc28j60Write(MABBIPG, 0x12);
	// Set the maximum packet size which the controller will accept
        // Do not send packets longer than MAX_FRAMELEN:
	enc28j60WriteWord(MAMXFLL, MAX_FRAMELEN);	
	// do bank 3 stuff
        // write MAC address
        // NOTE: MAC address in ENC28J60 is byte-backward	
        enc28j60Write(MAADR5, macaddr[0]);
        enc28j60Write(MAADR4, macaddr[1]);
        enc28j60Write(MAADR3, macaddr[2]);
        enc28j60Write(MAADR2, macaddr[3]);
        enc28j60Write(MAADR1, macaddr[4]);
        enc28j60Write(MAADR0, macaddr[5]);		
		
	// no loopback of transmitted frames
	enc28j60PhyWrite(PHCON2, PHCON2_HDLDIS);
	// switch to bank 0
	enc28j60SetBank(ECON1);
	// enable interrutps
	enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, EIE, EIE_INTIE|EIE_PKTIE);
	// enable packet reception
	enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN);
}

// read the revision of the chip:
uint8_t enc28j60getrev(void)
{
	return(enc28j60Read(EREVID));
}

// link status
uint8_t enc28j60linkup(void)
{
        // bit 10 (= bit 3 in upper reg)
	return(enc28j60PhyReadH(PHSTAT2) && 4);
}

void enc28j60PacketSend(uint16_t len, uint8_t* packet)
{
        // Check no transmit in progress
        while (enc28j60ReadOp(ENC28J60_READ_CTRL_REG, ECON1) & ECON1_TXRTS)
        {
                // Reset the transmit logic problem. See Rev. B4 Silicon Errata point 12.
                if( (enc28j60Read(EIR) & EIR_TXERIF) ) {
                        enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRST);
                        enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRST);
                }
        }

	// Set the write pointer to start of transmit buffer area
	enc28j60WriteWord(EWRPTL, TXSTART_INIT);
	// Set the TXND pointer to correspond to the packet size given
	enc28j60WriteWord(ETXNDL, (TXSTART_INIT+len));
	// write per-packet control byte (0x00 means use macon3 settings)
	enc28j60WriteOp(ENC28J60_WRITE_BUF_MEM, 0, 0x00);
	// copy the packet into the transmit buffer
	enc28j60WriteBuffer(len, packet);
	// send the contents of the transmit buffer onto the network
	enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRTS);
        // Reset the transmit logic problem. See Rev. B4 Silicon Errata point 12.
}

/*
// just probe if there might be a packet
uint8_t enc28j60hasRxPkt(void)
{
       return enc28j60ReadByte(EPKTCNT) > 0;
}
*/

// Gets a packet from the network receive buffer, if one is available.
// The packet will by headed by an ethernet header.
//      maxlen  The maximum acceptable length of a retrieved packet.
//      packet  Pointer where packet data should be stored.
// Returns: Packet length in bytes if a packet was retrieved, zero otherwise.
uint16_t enc28j60PacketReceive(uint16_t maxlen, uint8_t* packet)
{
        	uint16_t rxstat;
	uint16_t len;
	// check if a packet has been received and buffered
	//if( !(enc28j60Read(EIR) & EIR_PKTIF) ){
        // The above does not work. See Rev. B4 Silicon Errata point 6.
	if( enc28j60Read(EPKTCNT) ==0 ){
		return(0);
        }

	// Set the read pointer to the start of the received packet
	enc28j60WriteWord(ERDPTL, gNextPacketPtr);
	//enc28j60Write(ERDPTL, (gNextPacketPtr &0xFF));
	//enc28j60Write(ERDPTH, (gNextPacketPtr)>>8);
	// read the next packet pointer
	gNextPacketPtr  = enc28j60ReadBufferWord();
	//gNextPacketPtr  = enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0);
	//gNextPacketPtr |= enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0)<<8;
	// read the packet length (see datasheet page 43)
	len = enc28j60ReadBufferWord() - 4;
	//len = enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0);
	//len |= enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0)<<8;
        //len-=4; //remove the CRC count
	// read the receive status (see datasheet page 43)
	rxstat  = enc28j60ReadBufferWord();
	//rxstat  = enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0);
	//rxstat |= ((uint16_t)enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0))<<8;
	// limit retrieve length
        if (len>maxlen-1){
                len=maxlen-1;
        }
        // check CRC and symbol errors (see datasheet page 44, table 7-3):
        // The ERXFCON.CRCEN is set by default. Normally we should not
        // need to check this.
        if ((rxstat & 0x80)==0){
                // invalid
                len=0;
        }else{
                // copy the packet from the receive buffer
                enc28j60ReadBuffer(len, packet);
        }
	// Move the RX read pointer to the start of the next received packet
	// This frees the memory we just read out
	enc28j60WriteWord(ERXRDPTL, gNextPacketPtr );
	//enc28j60Write(ERXRDPTL, (gNextPacketPtr &0xFF));
	//enc28j60Write(ERXRDPTH, (gNextPacketPtr)>>8);
        // However, compensate for the errata point 13, rev B4: enver write an even address!
        if ((gNextPacketPtr - 1 < RXSTART_INIT)
                || (gNextPacketPtr -1 > RXSTOP_INIT)) {
                enc28j60WriteWord(ERXRDPTL, RXSTOP_INIT);
                //enc28j60Write(ERXRDPTL, (RXSTOP_INIT)&0xFF);
                //enc28j60Write(ERXRDPTH, (RXSTOP_INIT)>>8);
        } else {
                enc28j60WriteWord(ERXRDPTL, (gNextPacketPtr-1));
                //enc28j60Write(ERXRDPTL, (gNextPacketPtr-1)&0xFF);
                //enc28j60Write(ERXRDPTH, (gNextPacketPtr-1)>>8);
        }
	// decrement the packet counter indicate we are done with this packet
	enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PKTDEC);
	return(len);

/*
  uint16_t rxstat;
	uint16_t len;
	// check if a packet has been received and buffered
	//if( !(enc28j60Read(EIR) & EIR_PKTIF) ){
        // The above does not work. See Rev. B4 Silicon Errata point 6.
	if( enc28j60Read(EPKTCNT) ==0 ){
		return(0);
        }

	// Set the read pointer to the start of the received packet
	enc28j60WriteWord(ERDPTL, gNextPacketPtr);
	// read the next packet pointer
	gNextPacketPtr  = enc28j60ReadBufferWord();
	// read the packet length (see datasheet page 43)
	len  = enc28j60ReadBufferWord() - 4;
	// read the receive status (see datasheet page 43)
	rxstat  = enc28j60ReadBufferWord();
	// limit retrieve length
        if (len>maxlen-1){
                len=maxlen-1;
        }
        // check CRC and symbol errors (see datasheet page 44, table 7-3):
        // The ERXFCON.CRCEN is set by default. Normally we should not
        // need to check this.
        if ((rxstat & 0x80)==0){
                // invalid
                len=0;
        }else{
                // copy the packet from the receive buffer
                enc28j60ReadBuffer(len, packet);
        }
	// Move the RX read pointer to the start of the next received packet
	// This frees the memory we just read out
//	enc28j60WriteWord(ERXRDPTL, gNextPacketPtr );
        // However, compensate for the errata point 13, rev B4: enver write an even address!
        if ((gNextPacketPtr - 1 < RXSTART_INIT)
                || (gNextPacketPtr -1 > RXSTOP_INIT)) {
                enc28j60WriteWord(ERXRDPTL, RXSTOP_INIT);
        } else {
                enc28j60WriteWord(ERXRDPTL, (gNextPacketPtr-1));
        }
	// decrement the packet counter indicate we are done with this packet
	enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PKTDEC);
	return(len);
*/
}

uint16_t enc28j60BeginPacketReceive(void)
{
	U16 rxstat;
	U16 len;

	// check if a packet has been received and buffered
	if( !(enc28j60Read(EIR) & EIR_PKTIF) )
		return 0;
	
	// Make absolutely certain that any previous packet was discarded	
	//if( WasDiscarded == FALSE)
	//	MACDiscardRx();

	// Set the read pointer to the start of the received packet
	enc28j60Write(ERDPTL, (gNextPacketPtr));
	enc28j60Write(ERDPTH, (gNextPacketPtr)>>8);
	// read the next packet pointer
	gNextPacketPtr  = enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0);
	gNextPacketPtr |= enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0)<<8;
	// read the packet length
	len  = enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0);
	len |= enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0)<<8;
	// read the receive status
	rxstat  = enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0);
	rxstat |= enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0)<<8;

	// limit retrieve length
	// (we reduce the MAC-reported length by 4 to remove the CRC)
//	if(len>maxlen) len=maxlen;
//	if(len<=0) return 0;
//	else return len;
	return len;
}

void enc28j60EndPacketReceive(void)
{
	// Move the RX read pointer to the start of the next received packet
	// This frees the memory we just read out
	enc28j60Write(ERXRDPTL, (gNextPacketPtr));
	enc28j60Write(ERXRDPTH, (gNextPacketPtr)>>8);

	// decrement the packet counter indicate we are done with this packet
	enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PKTDEC);
}

uint8_t enc28j60BufferedData()
{
	// check if a packet has been received and buffered
	if( enc28j60Read(EPKTCNT) ==0 )
		return(0);
    
	return 1;
}		