
#ifndef DINOv2_HW_H
#define DINOv2_HW_H

#define ONE_WIRE_PIN 5
/*
#define RS485TXControlPin 0x03
#define RS485TXPin        0x01
#define RS485RXPin        0x00
#define RS485Transmit     HIGH
#define RS485Receive      LOW
*/


// List QuickCfg.h parameters that want to be configured as standard

void DINo_v2_HW_Setup()
{
	// Init the board
	InitDINo();
	
	// Set the inputs
	SetInput1();
        SetInput2();
        SetInput3();
        SetInput4();
	
	// Set the outputs
	SetRelay1();
	SetRelay2();
	SetRelay3();
	SetRelay4();
	
	// Set and turn ON the status LED
	SetLED();
	TurnOnLED();

}


#endif
