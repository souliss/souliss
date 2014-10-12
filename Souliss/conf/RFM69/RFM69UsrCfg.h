/**************************************************************************
	Souliss - vNet Virtualized Network
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
/**************************************************************************/
/*!
    \file 
    \ingroup
 
    This is the user configurable file. It contains parameters that users
    can change to suit their needs. 

*/
/**************************************************************************/
#ifndef RFM69_USR_CFG_H
#define RFM69_USR_CFG_H

/**************************************************************************/
/*!
	Select the wireless controller used for the board
	        
		Value       
        0x0         Disable (Default)
        0x1         Enable
		
		HOPERF_RFM69	-  HopeRF RFM69CW/RFM69HCW radio
*/
/**************************************************************************/
#if(!(QC_ENABLE))					// Define manually only in Detailed Configuration Mode
#	define HOPERF_RFM69  	0x0
#endif
/**************************************************************************/

/**************************************************************************/
/*!
	Select operational frequency of the used module
	        
		Value       
        0x0         Disable (Default)
        0x1         Enable
		
		RFM69_315MHZ	-	HopeRF RFM69CW/RFM69HCW radio at 315 MHz
		RFM69_433MHZ	-	HopeRF RFM69CW/RFM69HCW radio at 433 MHz
		RFM69_868MHZ	-	HopeRF RFM69CW/RFM69HCW radio at 868 MHz
		RFM69_915MHZ	-	HopeRF RFM69CW/RFM69HCW radio at 915 MHz
			
*/
/**************************************************************************/
#define	RFM69_315MHZ		0
#define	RFM69_433MHZ		0
#define	RFM69_868MHZ		1
#define	RFM69_915MHZ		0

/**************************************************************************/
/*!
	Enable the High Power transmission mode (only RFM69HCW)
	        
		Value       
        0x0         Disable (Default)
        0x1         Enable
*/
/**************************************************************************/
#define	RFM69_HIGHPOWER		0

/**************************************************************************/
/*!
	Insert the network ID (1 byte)
*/
/**************************************************************************/
#define RFM69_NETID    		0xAA

/**************************************************************************/
/*!
	Insert the encryption key, length must be 16 characters
*/
/**************************************************************************/
#define RFM69_ENCRYPTKEY    "sampleEncryptKey" 

#define RFM69_SUCC			1
#define RFM69_FAIL			0

#endif
