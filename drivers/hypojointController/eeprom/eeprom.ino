//EEPROM initial setup
//LED will stay on while writing, when program exits, LED will go off

#include <EEPROM.h>
#include "EEPROMAnything.h"

int addr   = 0;

struct Id{
 byte val;
 int   address;
 Id(){ val = 0; address = 0;}
}id;
struct PotPin{
 int val;
 int   address;
 PotPin(){val = A3; address = 1;}
}potPin; 
struct DirPin{
 int val;
 int   address;
 DirPin(){val = 8; address = 3;}
}dirPin;
struct StepPin{
 int val;
 int   address;
 StepPin(){val = 9; address = 5;}
}stepPin;
struct LedPin{
 int val;
 int   address;
 LedPin(){val = 13; address = 7;}
}ledPin;
struct MinFreq{
 int val;
 int   address;
 MinFreq(){val = 0; address = 9;}
}minFreq;
struct MaxFreq{
 long val;
 int   address;
 MaxFreq(){val = 3500; address = 11;}
}maxFreq;
struct MinPos{
 int val;
 int   address;
 MinPos(){val = 0; address = 15;}
}minPos;
struct MaxPos{
 long val;
 int   address;
 MaxPos(){val = 1024; address = 17;}
}maxPos;


void setup()
{
  Serial.begin(9600);
  pinMode(ledPin.val, OUTPUT);
  delay(1000);
  
  Serial.print("Writing values to memory...\n");
  digitalWrite(ledPin.val, HIGH);
  
  //Write device id
  Serial.print("Address is: ");
  Serial.print(addr);
  addr += EEPROM_writeAnything(id.address, id.val);
  Serial.print("   Value is: ");
  Serial.print(id.val);
  Serial.print('\n'); 
  
  //Write potentiometer pin
  Serial.print("Address is: ");
  Serial.print(addr);
  addr += EEPROM_writeAnything(potPin.address, potPin.val);
  Serial.print("   Value is: ");
  Serial.print(potPin.val);
  Serial.print('\n');

  //Write direction pin
  Serial.print("Address is: ");
  Serial.print(addr);
  addr += EEPROM_writeAnything(dirPin.address, dirPin.val);
  Serial.print("   Value is: ");
  Serial.print(dirPin.val);
  Serial.print('\n');
  
  //Write stepper motor pin
  Serial.print("Address is: ");
  Serial.print(addr);
  addr += EEPROM_writeAnything(stepPin.address, stepPin.val);
  Serial.print("   Value is: ");
  Serial.print(stepPin.val);
  Serial.print('\n');
  
  //Write led pin
  Serial.print("Address is: ");
  Serial.print(addr);
  addr += EEPROM_writeAnything(ledPin.address, ledPin.val);
  Serial.print("   Value is: ");
  Serial.print(ledPin.val);
  Serial.print('\n');
  
  //Write minFrequency
  Serial.print("Address is: ");
  Serial.print(addr);
  addr += EEPROM_writeAnything(minFreq.address, minFreq.val);
  Serial.print("   Value is: ");
  Serial.print(minFreq.val);
  Serial.print('\n');
  
  //Write maxFrequency
  Serial.print("Address is: ");
  Serial.print(addr);
  addr += EEPROM_writeAnything(maxFreq.address, maxFreq.val);
  Serial.print("   Value is: ");
  Serial.print(maxFreq.val);
  Serial.print('\n');
  
  //Write minPosition
  Serial.print("Address is: ");
  Serial.print(addr);
  addr += EEPROM_writeAnything(minPos.address, minPos.val);
  Serial.print("   Value is: ");
  Serial.print(minPos.val);
  Serial.print('\n');
  
  //Write maxPosition
  Serial.print("Address is: ");
  Serial.print(addr);
  addr += EEPROM_writeAnything(maxPos.address, maxPos.val);
  Serial.print("   Value is: ");
  Serial.print(maxPos.val);
  Serial.print('\n');

  //Return address to 0
  addr = 0;
  
  Serial.print("Done writing values to memory");
  delay(200);
  digitalWrite(ledPin.val, LOW);
}

void loop()
{  
}
