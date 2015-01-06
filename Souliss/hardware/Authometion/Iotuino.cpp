/*
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */

/*  * * * * * * * * * * * * * * * * * * * * * * * * * * *
 Code by Pietro Moscetta
 http://www.authometion.com
 Version 1.3
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// For Arduino 1.0 and earlier
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <SPI.h>
#include "Iotuino.h"

/* Initialization function */
void IOTUINO::initialize (void)
{
    csPin = 10;
    rstPin = 4;
    ADDR_A=1;
    ADDR_B=1;
    PAR=1;
    COMMAND=0;
    MODE=0;
    PACK=0;
    
    //Default radio transmission number for LYT
    radioTransmission=100;
    
    SPI.setDataMode(SPI_MODE1);
    //SPI.setClockDivider(SPI_CLOCK_DIV64);
    SPI.setClockDivider(SPI_CLOCK_DIV128);
    SPI.setBitOrder(MSBFIRST);
	
    //initialize SPI:
    SPI.begin();
    
    pinMode(csPin, OUTPUT);
    pinMode(rstPin, OUTPUT);
    
    //Reset 2.4Ghz Transceiver
    digitalWrite(rstPin,HIGH);
    delay(10);
    digitalWrite(rstPin,LOW);
    delay(5);
    digitalWrite(rstPin,HIGH);
    delay(10);
    
    initRadioModule();
    
    delay(50);
    writeHead();
    delayMicroseconds(5);
    
}

void IOTUINO::initialize (byte chipselect)
{
    csPin = chipselect;
    rstPin = 4;
    ADDR_A=1;
    ADDR_B=1;
    PAR=1;
    COMMAND=0;
    MODE=0;
    PACK=0;
    
    //Default radio transmission number for LYT
    radioTransmission=100;
/*    
    SPI.setDataMode(SPI_MODE1);
    //SPI.setClockDivider(SPI_CLOCK_DIV64);
    SPI.setClockDivider(SPI_CLOCK_DIV128);
    SPI.setBitOrder(MSBFIRST);
*/	
    //initialize SPI:
    SPI.begin();
    
    pinMode(csPin, OUTPUT);
    pinMode(rstPin, OUTPUT);
    
    //Reset 2.4Ghz Transceiver
    digitalWrite(rstPin,HIGH);
    delay(10);
    digitalWrite(rstPin,LOW);
    delay(5);
    digitalWrite(rstPin,HIGH);
    delay(10);
    
    initRadioModule();
    
    delay(50);
    writeHead();
    delayMicroseconds(5);
    
}

/*
 *      LYT COMMANDS
 *
 */


void IOTUINO::setRadioTransmission (byte Val)
{
    radioTransmission=Val;
}

void IOTUINO::rgbSetAddress (byte ADDRA, byte ADDRB)
{
    sendcmd(ADDRA,ADDRB,5,1);
}

void IOTUINO::rgbwSetAddress (byte ADDRA, byte ADDRB)
{
    sendcmdnew(0,ADDRA,ADDRB,3,1,1,1);
}

void IOTUINO::whiteSetAddress (byte ADDRA, byte ADDRB)
{
    sendcmdnew(1,ADDRA,ADDRB,8,1,1,1);
}

void IOTUINO::rgbClearAddress (byte ADDRA, byte ADDRB)
{
    //SEND 3 SHORT DEL COMMANDS + 2 LONG DEL COMMANDS
    for(int i=0;i<5;i++)
    {
        if(i>=0 && i<3)
            sendcmd(ADDRA,ADDRB,5,1);
        
        if(i>2)
            sendcmd(ADDRA,ADDRB,5,1);
        
        delay(200);
    }
}

void IOTUINO::rgbwClearAddress (byte ADDRA, byte ADDRB)
{
    //SEND 3 SHORT DEL COMMANDS + 2 LONG DEL COMMANDS
    for(int i=0;i<5;i++)
    {
        if(i>=0 && i<3)
            sendcmdnew(0,ADDRA,ADDRB,3,1,1,i+1);
        
        if(i>2)
            sendcmdnew(0,ADDRA,ADDRB,19,1,1,i+1);
        
        delay(200);
    }
}

void IOTUINO::whiteClearAddress (byte ADDRA, byte ADDRB)
{
    //SEND 5 SHORT DEL COMMANDS
    for(int i=0;i<5;i++)
    {
        if(i>=0 && i<5)
            sendcmdnew(1,ADDRA,ADDRB,8,1,i+1,1);
        delay(200);
    }
}

void IOTUINO::rgbOn (byte ADDRA, byte ADDRB)
{
    sendcmd(ADDRA,ADDRB,2,1);
}

void IOTUINO::rgbwOn (byte ADDRA, byte ADDRB)
{
    sendcmdnew(0,ADDRA,ADDRB,3,1,1,1);
}

void IOTUINO::whiteOn (byte ADDRA, byte ADDRB)
{
    sendcmdnew(1,ADDRA,ADDRB,8,1,1,1);
}

void IOTUINO::rgbOff (byte ADDRA, byte ADDRB)
{
    sendcmd(ADDRA,ADDRB,1,1);
}

void IOTUINO::rgbwOff (byte ADDRA, byte ADDRB)
{
    sendcmdnew(0,ADDRA,ADDRB,4,1,1,1);
}

void IOTUINO::whiteOff (byte ADDRA, byte ADDRB)
{
    sendcmdnew(1,ADDRA,ADDRB,11,1,1,1);
}

void IOTUINO::rgbRgb (byte ADDRA, byte ADDRB, byte VALUE)
{
    sendcmd(ADDRA,ADDRB,0,VALUE);
}

void IOTUINO::rgbwRgb (byte ADDRA, byte ADDRB, byte VALUE)
{
    sendcmdnew(0,ADDRA,ADDRB,15,VALUE,1,0);
}

void IOTUINO::rgbMaxWhite (byte ADDRA, byte ADDRB)
{
    sendcmd(ADDRA,ADDRB,24,1);
}

void IOTUINO::rgbwMaxWhite(byte ADDRA, byte ADDRB)
{
    sendcmdnew(0,ADDRA,ADDRB,19,1,1,1);
}

void IOTUINO::whiteMaxWhite (byte ADDRA, byte ADDRB)
{
    sendcmdnew(1,ADDRA,ADDRB,24,1,1,1);
}

void IOTUINO::whiteSetTemperature (byte ADDRA, byte ADDRB, byte VALUE)
{
    int i;
    
    if(VALUE>0 && VALUE<13)
    {
        for(i=0;i<VALUE;i++)
            sendcmdnew(1,ADDRA,ADDRB,14,1,0,0);
    }
    else if(VALUE>12 && VALUE<24)
    {
        for(i=0;i<VALUE-11;i++)
            sendcmdnew(1,ADDRA,ADDRB,15,1,0,0);
    }
}

void IOTUINO::whiteSetBrightness (byte ADDRA, byte ADDRB, byte VALUE)
{
    int i;
    
    if(VALUE>0 && VALUE<13)
    {
        for(i=0;i<VALUE;i++)
            sendcmdnew(1,ADDRA,ADDRB,12,1,0,0);
    }
    else if(VALUE>12 && VALUE<24)
    {
        for(i=0;i<VALUE-12;i++)
            sendcmdnew(1,ADDRA,ADDRB,4,1,0,0);
    }
}

void IOTUINO::whiteSetNightMode (byte ADDRA, byte ADDRB)
{
    sendcmdnew(1,ADDRA,ADDRB,27,1,0,0);
}

void IOTUINO::rgbwSetBrightness (byte ADDRA, byte ADDRB, byte VALUE)
{
    if(VALUE>=0 && VALUE<18)
    {
        if(VALUE!=17) VALUE=16-VALUE;
        else VALUE=23;
        
        VALUE=1+(VALUE*8);
        sendcmdnew(0,ADDRA,ADDRB,14,1,VALUE,1);
    }
}



//****PRIVATE FUNCTIONS*********
//DO NOT MODIFY THIS CODE!!

void IOTUINO::regWrite16(byte ADDR, byte V1, byte V2, byte WAIT)
{
    digitalWrite(csPin,LOW);
    SPI.transfer(ADDR);
    SPI.transfer(V1);
    SPI.transfer(V2);
    digitalWrite(csPin,HIGH);
    delayMicroseconds(WAIT);
}

void IOTUINO::sendcmd(byte ADDRA, byte ADDRB, byte CMD, byte P1)
{
    SPI.setDataMode(SPI_MODE1);

	ADDR_A=ADDRA;
	ADDRB=ADDRB;
  	COMMAND=CMD;
  	PAR=P1;
  	
  	writeHead();
    delayMicroseconds(350);
    for(int i=0;i<radioTransmission;i++)
    {
        writeFirstPacket();
        delayMicroseconds(350);
        
    }

    
	PACK++;
	if(PACK>254)
		PACK=0;

	SPI.setDataMode(SPI_MODE0);
	
}

void IOTUINO::writeHead(void)
{
    
    regWrite16(7,0,0,1);
    regWrite16(36,154,171,1);
    regWrite16(39,188,205,5);
    regWrite16(7,0,0,1);
    regWrite16(52,128,128,1);
    
    digitalWrite(csPin,LOW);
    SPI.transfer(50);
    SPI.transfer(6);
    SPI.transfer(164);
    SPI.transfer(ADDR_A);
    SPI.transfer(ADDR_B);
    SPI.transfer(PAR);
    SPI.transfer(COMMAND);
    SPI.transfer(PACK); //P2
    digitalWrite(csPin,HIGH);
    delayMicroseconds(10);
    
    regWrite16(7,1,3,1);
    
}

void IOTUINO::writeFirstPacket(void)
{
    SPI.setDataMode(SPI_MODE1);

    regWrite16(7,0,0,1);
    regWrite16(52,128,128,1);
    
    
    digitalWrite(csPin,LOW);
    SPI.transfer(50);
    SPI.transfer(6);
    SPI.transfer(164);
    SPI.transfer(ADDR_A);
    SPI.transfer(ADDR_B);
    SPI.transfer(PAR);
    SPI.transfer(COMMAND);
    SPI.transfer(PACK);
    digitalWrite(csPin,HIGH);
    delayMicroseconds(10);
    
    regWrite16(7,1,3,1);
    SPI.setDataMode(SPI_MODE0);
    
}

void IOTUINO::sendcmdnew(byte Mode, byte ADDRA, byte ADDRB, byte CMD, byte P1, byte P2, byte P3)
{
    SPI.setDataMode(SPI_MODE1);

    int cbit=0;
    
	MODE=Mode;
	ADDR_A=ADDRA;
	ADDR_B=ADDRB;
  	COMMAND=CMD;
  	PAR=P1;
	PAR2=P2;
	PAR3=P3;
	
  	
  	writeHeadnew();
    delayMicroseconds(350);
    for(int i=0;i<radioTransmission;i++)
    {
        writeFirstPacketnew(cbit);
        delayMicroseconds(350);
        
        cbit++;
        if(cbit>2) cbit=0;
    }

    
	PACK++;
	if(PACK>254)
		PACK=0;
    SPI.setDataMode(SPI_MODE0);
		
}

void IOTUINO::writeHeadnew(void)
{
    regWrite16(7,0,0,1);
    
    digitalWrite(csPin,LOW);
    SPI.transfer(36);
    if(MODE==0)
    {
        SPI.transfer(20);
        SPI.transfer(122);
    }
    else
    {
        SPI.transfer(5);
        SPI.transfer(10);
    }
    
    digitalWrite(csPin,HIGH);
    delayMicroseconds(1);
    
    digitalWrite(csPin,LOW);
    SPI.transfer(39);
    if(MODE==0)
    {
        SPI.transfer(37);
        SPI.transfer(139);
    }
    else
    {
        SPI.transfer(85);
        SPI.transfer(170);
    }
    
    digitalWrite(csPin,HIGH);
    delayMicroseconds(5);
    
    regWrite16(7,0,0,1);
    regWrite16(52,128,128,1);
    
    digitalWrite(csPin,LOW);
    SPI.transfer(50);
    SPI.transfer(7);
    if(MODE==0)
        SPI.transfer(184);
    else
        SPI.transfer(90);
    
    SPI.transfer(ADDR_A);
    SPI.transfer(ADDR_B);
    SPI.transfer(PAR);
    
    if(MODE==0)
    {
        SPI.transfer(PAR2);
        SPI.transfer(COMMAND);
        
    }
    else
    {
        SPI.transfer(COMMAND);
        SPI.transfer(PACK);
        
    }
    
    SPI.transfer(PACK); //P3
    digitalWrite(csPin,HIGH);
    delayMicroseconds(10);
    
    digitalWrite(csPin,LOW);
    SPI.transfer(7);
    SPI.transfer(1);
    if(MODE==0)
        SPI.transfer(9);
    else
        SPI.transfer(4);
    
    digitalWrite(csPin,HIGH);
    
}

void IOTUINO::writeFirstPacketnew(int cbit)
{
    
	regWrite16(7,0,0,1);
    regWrite16(52,128,128,1);
    
    digitalWrite(csPin,LOW);
    SPI.transfer(50);
    SPI.transfer(7);
    if(MODE==0)
        SPI.transfer(184);
    else
        SPI.transfer(90);
    
    SPI.transfer(ADDR_A);
    SPI.transfer(ADDR_B);
    SPI.transfer(PAR);
    
    if(MODE==0)
    {
        SPI.transfer(PAR2);
        SPI.transfer(COMMAND);
        
    }
    else
    {
        SPI.transfer(COMMAND);
        SPI.transfer(PACK);
        
    }
    
    SPI.transfer(PACK); //P2
    digitalWrite(csPin,HIGH);
    delayMicroseconds(10);
    
    digitalWrite(csPin,LOW);
    SPI.transfer(7);
    SPI.transfer(1);
    if(MODE==0)
    {
        if(cbit==0) SPI.transfer(40);
        if(cbit==1) SPI.transfer(71);
        if(cbit==2) SPI.transfer(9);
        
    }
    else
    {
        if(cbit==0) SPI.transfer(39);
        if(cbit==1) SPI.transfer(74);
        if(cbit==2) SPI.transfer(4);
        
    }

    
    digitalWrite(csPin,HIGH);

}

void IOTUINO::initRadioModule(void)
{
    
    regWrite16(0,111,224,7);
    regWrite16(1,86,129,7);
    regWrite16(2,102,23,7);
    regWrite16(4,156,201,7);
    regWrite16(5,102,55,7);
    regWrite16(7,0,76,7);
    regWrite16(8,108,144,7);
    regWrite16(9,72,0,7);
    regWrite16(10,127,253,7);
    regWrite16(11,0,8,7);
    regWrite16(12,0,0,7);
    regWrite16(13,72,189,7);
    regWrite16(22,0,255,7);
    regWrite16(23,128,5,7);
    regWrite16(24,0,103,7);
    regWrite16(25,22,89,7);
    regWrite16(26,25,224,7);
    regWrite16(27,19,0,7);
    regWrite16(28,24,0,7);
    regWrite16(32,72,0,7);
    regWrite16(33,63,199,7);
    regWrite16(34,32,0,7);
    regWrite16(35,3,0,7);
    regWrite16(40,68,2,7);
    regWrite16(41,176,0,7);
    regWrite16(42,253,176,7);
    regWrite16(43,0,15,7);
    delay(200);
    regWrite16(128,0,0,7);
    regWrite16(129,255,255,7);
    regWrite16(130,0,0,7);
    regWrite16(132,0,0,7);
    regWrite16(133,255,255,7);
    regWrite16(135,255,255,7);
    regWrite16(136,0,0,7);
    regWrite16(137,255,255,7);
    regWrite16(138,0,0,7);
    regWrite16(139,255,255,7);
    regWrite16(140,0,0,7);
    regWrite16(141,255,255,7);
    regWrite16(150,0,0,7);
    regWrite16(151,255,255,7);
    regWrite16(152,0,0,7);
    regWrite16(153,255,255,7);
    regWrite16(154,0,0,7);
    regWrite16(155,255,255,7);
    regWrite16(156,0,0,7);
    regWrite16(160,0,0,7);
    regWrite16(161,255,255,7);
    regWrite16(162,0,0,7);
    regWrite16(163,255,255,7);
    regWrite16(168,0,0,7);
    regWrite16(169,255,255,7);
    regWrite16(170,0,0,7);
    regWrite16(171,255,255,7);
    regWrite16(7,0,0,7);
    
    
}

/* 
 Use to calcolate % to color 
*/

int CalculareValueColor (int val1 ,int  val2 , int value )
{
  
  float CalculatePercent ;
  int  ValColor;
  
  if (val1>=val2) 
  {
    CalculatePercent= (( 100.0/  val1 ) *val2);
  }     
  else
  {
    CalculatePercent= 100.0- ((  100.0/val2 ) *val1);
  }
  
  ValColor=((value/2)*CalculatePercent)/100.0;
  return ValColor;
}

/* 
Convert Value RGB to value data
*/
int ConverColorRgbToColorVal(int R, int G, int B)
{
  float color, r, g, b, lum;
  
  int offsetZeroRB  =36;
  int offsetZeroGB  =60;
  int BaseR,BaseG,BaseB ;
  //int r,g,b ;
  int ValueColor ;
  int ColorRGB ;
  int offsetZero =6;
  BaseR=30;
  BaseG=110;
  BaseB=190;
  
  
  if ((R>=G) && (R>=B)) 
  {  
    lum=R*0.034;
    if (G>B)
    { 
      ValueColor=BaseG-BaseR;
      ValueColor=CalculareValueColor(R,G,ValueColor);
      ColorRGB=BaseR+ValueColor;
    }
    else
    {
      ValueColor=offsetZeroRB+255-BaseB+BaseR; 
      ValueColor=CalculareValueColor(R,B,ValueColor);
      if (ValueColor>offsetZero)
        ColorRGB=BaseR-ValueColor+offsetZeroRB;
      else
        ColorRGB=BaseR-ValueColor;
      
      
      if (ColorRGB<0) 
        ColorRGB=255+ColorRGB;
     
    }
  }
  else if ( (G>=R)  && (G>=B) )
  {  
    
    lum=G*0.034;
    if (R>B) 
    {
      ValueColor=BaseG-BaseR;
      ValueColor=CalculareValueColor(G,R,ValueColor);
      ColorRGB=BaseG-ValueColor;
      if (ColorRGB<0) 
        ColorRGB=255+ColorRGB;
     
    }
    else
    {
      ValueColor=BaseB-BaseG-offsetZeroGB;
      ValueColor=CalculareValueColor(G,B,ValueColor);
      if (ValueColor>offsetZero)
        ColorRGB=BaseG+ValueColor;
      else
        ColorRGB=BaseG+ValueColor-10;
      
     }
   
  } 
  else if ((B>=G) && (B>=R))
  {
    lum=B*0.034;
    if (G>R) 
    {
      ValueColor=BaseB-BaseG+offsetZeroGB;
      ValueColor=CalculareValueColor(B,G,ValueColor);
      ColorRGB=BaseB-ValueColor;
      if (ColorRGB<0) 
        ColorRGB=255+ColorRGB;
      
    }
    else
    {
      ValueColor=offsetZeroRB+255-BaseB+BaseR;
      ValueColor=CalculareValueColor(B,R,ValueColor);
      ColorRGB=BaseB+ValueColor;
    }
    
  }
  
 return ColorRGB;
  
}