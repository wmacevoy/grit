//EEPROM initial setup
//LED will stay on while writing, when program exits, LED will go off

#include <EEPROM.h>
#include "EEPROMAnything.h"

const int totalBytes = 21;
int addr   = 0;

struct Id{
 byte val;
 static const int address = 0;
 Id(){ val = 0;}
}id;
struct PotPin{
 int val;
 static const int address = 1;
 PotPin(){val = A3;}
}potPin; 
struct DirPin{
 int val;
 static const int address = 3;
 DirPin(){val = 8;}
}dirPin;
struct StepPin{
 int val;
 static const int address = 5;
 StepPin(){val = 9;}
}stepPin;
struct LedPin{
 int val;
 static const int address = 7;
 LedPin(){val = 13;}
}ledPin;
struct MinFreq{
 int val;
 static const int address = 9; 
 MinFreq(){val = 0;}
}minFreq;
struct MaxFreq{
 long val;
 static const int address = 11;
 MaxFreq(){val = 3500;}
}maxFreq;
struct MinPos{
 int val;
 static const int address = 15;
 MinPos(){val = 0;}
}minPos;
struct MaxPos{
 long val;
 static const int address = 17;
 MaxPos(){val = 1024;}
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
  
  byte b = 0;
  for(int i = 0; i < totalBytes; ++i){
   b ^= EEPROM.read(i);
  }
  Serial.print("Checksum is: ");
  Serial.print(b);
  Serial.print('\n');
  EEPROM.write(totalBytes, b);
  
  Serial.print("Done writing values to memory");
  delay(1000);
  digitalWrite(ledPin.val, LOW);
}

void loop()
{  
}
