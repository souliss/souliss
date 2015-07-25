#include <SoftwareSerial.h>
#include "EmonLib.h"             // Include Emon Library 
EnergyMonitor emon1;             // Create an instance 


SoftwareSerial TinySerial(3, 4); // RX, TX
#define LED 1
#define CT  A1


void setup()  
{
 // Open serial communications and let us know we are connected
 TinySerial.begin(9600);  // SET TO 1200 ON RECEIVER, I DONT KNOW WHY
 pinMode(LED, OUTPUT);
 emon1.current(CT, 10);       // Current: input pin, calibration.
}


void loop()
{
  float current = emon1.calcIrms(1480);
  TinySerial.print(current);
  delay(100);
  digitalWrite(LED,!digitalRead(LED));
}
