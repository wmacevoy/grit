//EEPROM initial setup

#include <EEPROM.h>
#include "EEPROMAnything.h"

int addr = 0;

void setup()
{
  Serial.begin(9600);
  delay(1000);
}

void loop()
{  
  Serial.print("Writing values to memory...\n");
  //Clear the eeprom
  // write a 0 to all 512 bytes of the EEPROM
  for (int i = 0; i < 512; i++)
    EEPROM.write(i, 0);
  
  //Write device id
  byte id = 'a';
  addr += EEPROM_writeAnything(addr, id);
  Serial.print("Address is: ");
  Serial.print(addr);
  Serial.print('\n'); 
  
  //Write potentiometer pin
  int potPin = A3;
  addr += EEPROM_writeAnything(addr, potPin);
  Serial.print("Address is: ");
  Serial.print(addr);
  Serial.print('\n');

  //Write direction pin
  int dirPin = 8;
  addr += EEPROM_writeAnything(addr, dirPin);
  Serial.print("Address is: ");
  Serial.print(addr);
  Serial.print('\n');
  
  //Write stepper motor pin
  int stepPin = 9;
  addr += EEPROM_writeAnything(addr, stepPin);
  Serial.print("Address is: ");
  Serial.print(addr);
  Serial.print('\n');
  
  //Write minFrequency
  int minFreq = 0;
  addr += EEPROM_writeAnything(addr, minFreq);
  Serial.print("Address is: ");
  Serial.print(addr);
  Serial.print('\n');
  
  //Write maxFrequency
  long maxFreq = 3500;
  addr += EEPROM_writeAnything(addr, maxFreq);
  Serial.print("Address is: ");
  Serial.print(addr);
  Serial.print('\n');
  
  //Write minPosition
  int minPos = 0;
  addr += EEPROM_writeAnything(addr, minPos);
  Serial.print("Address is: ");
  Serial.print(addr);
  Serial.print('\n');
  
  //Write maxPosition
  long maxPos = 1024;
  addr += EEPROM_writeAnything(addr, maxPos);
  Serial.print("Address is: ");
  Serial.print(addr);
  Serial.print('\n');

  //Return address to 0
  addr = 0;
  
  Serial.print("Done writing values to memory");
  digitalWrite(13, HIGH);
  delay(1000);
  digitalWrite(13, LOW);
  exit(0);
}
