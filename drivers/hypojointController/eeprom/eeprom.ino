//EEPROM initial setup

#include <EEPROM.h>
#include "EEPROMAnything.h"

int addr = 0;

void setup()
{
}

void loop()
{  
  //Clear the eeprom
  // write a 0 to all 512 bytes of the EEPROM
  for (int i = 0; i < 512; i++)
    EEPROM.write(i, 0);
  
  //Write device id
  byte id = B1;
  addr += EEPROM_writeAnything(addr, id); 
  
  //Write potentiometer pin
  int potPin = A3;
  addr += EEPROM_writeAnything(addr, potPin);

  //Write direction pin
  int dirPin = 8;
  addr += EEPROM_writeAnything(addr, dirPin);
  
  //Write stepper motor pin
  int stepPin = A3;
  addr += EEPROM_writeAnything(addr, stepPin);
  
  //Write minFrequency
  int minFreq = 0;
  addr += EEPROM_writeAnything(addr, minFreq);
  
  //Write maxFrequency
  long maxFreq = 3500;
  addr += EEPROM_writeAnything(addr, potPin);
  
  //Write minPosition
  int minPos = 0;
  addr += EEPROM_writeAnything(addr, minPos);
  
  //Write maxPosition
  long maxPos = 1024;
  addr += EEPROM_writeAnything(addr, maxPos);

  //Return address to 0
  addr = 0;
  // advance to the next address.  there are 512 bytes in 
  // the EEPROM, so go back to 0 when we hit 512.
  //addr = addr + 1;
  //if (addr == 512)
    //addr = 0;
}
