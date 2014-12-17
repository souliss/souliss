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

#ifndef IOTUINO_H
#define IOTUINO_H

class IOTUINO
{

private:
    byte csPin, rstPin;  //CS and RST pins for LYT radio module
    byte ADDR_A, ADDR_B, PAR, PAR2, PAR3, COMMAND, MODE, PACK;
    //Number of radio transmission for LYT
    byte radioTransmission;
    
    void initRadioModule(void);
    void regWrite16(byte ADDR, byte V1, byte V2, byte WAIT);
    void writeFirstPacket(void);
    void writeHead(void);
    void writeFirstPacketnew(int cbit);
    void writeHeadnew(void);
    void sendcmd(byte ADDRA, byte ADDRB, byte CMD, byte PAR);
    void sendcmdnew(byte Mode, byte ADDRA, byte ADDRB, byte CMD, byte PAR, byte PAR2, byte PAR3);
    
public:
    
    
    //Init Iotuino board
    void initialize(void);

    //Init Iotuino board
    void initialize(byte chipselect);
    
    //Set radio transmission
    void setRadioTransmission(byte Val);
    
    //Power On LYT bulb
    void rgbOn(byte ADDRA, byte ADDRB);
    void rgbwOn(byte ADDRA, byte ADDRB);
    void whiteOn(byte ADDRA, byte ADDRB);
    
    //Power Off LYT bulb
    void rgbOff(byte ADDRA, byte ADDRB);
    void rgbwOff(byte ADDRA, byte ADDRB);
    void whiteOff(byte ADDRA, byte ADDRB);
    
    //Set the max white intensity
    void rgbMaxWhite(byte ADDRA, byte ADDRB);
    void rgbwMaxWhite(byte ADDRA, byte ADDRB);
    void whiteMaxWhite(byte ADDRA, byte ADDRB);
    
    //Set LYT address
    void rgbSetAddress(byte ADDRA, byte ADDRB);
    void rgbwSetAddress(byte ADDRA, byte ADDRB);
    void whiteSetAddress(byte ADDRA, byte ADDRB);
    
    //Clear LYT address
    void rgbClearAddress(byte ADDRA, byte ADDRB);
    void rgbwClearAddress(byte ADDRA, byte ADDRB);
    void whiteClearAddress(byte ADDRA, byte ADDRB);
    
    //Set color (only for RGB LYT)
    void rgbRgb(byte ADDRA, byte ADDRB, byte VALUE);
    void rgbwRgb(byte ADDRA, byte ADDRB, byte VALUE);
    
    //Set light temperature (only for White LYT)
    void whiteSetTemperature(byte ADDRA, byte ADDRB, byte VALUE);
    
    //Set brightness (only for White LYT)
    void whiteSetBrightness(byte ADDRA, byte ADDRB, byte VALUE);
    
    //Set brightness (only for RGB LYT)
    void rgbwSetBrightness(byte ADDRA, byte ADDRB, byte VALUE);
    
    //Set night mode on(only for White LYT)
    void whiteSetNightMode(byte ADDRA, byte ADDRB);
    
    
    
};

#endif
