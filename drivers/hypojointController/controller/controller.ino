
// Motor Controller
// this is the code for the individual joints..... the master is located on the main body
// and will have a different code uploaded to it.
//
//
//CRC code from - http://www.barrgroup.com/Embedded-Systems/How-To/CRC-Calculation-C-Code
//
//
//32000 TONE @ 16th Steps

//EEPROM bytes 0-34 are taken for configuration
//To set default config, simple set _DEFAULT to a non-zero value


#include <Wire.h>
#include <EEPROM.h>
#include "NewTone.h"
#include "typedef.h"
#include "EEPROMAnything.h"
#include "default.h"

const int totalBytes = 34;
const int cutoff=10;

float ratio = 32000/1024;

//CRC stuffs
crc crcTable[256];
#define WIDTH  (8 * sizeof(crc))
#define TOPBIT (1 << (WIDTH - 1))
#define POLYNOMIAL 0xD8

//Register set
//first 17 registers are data from the EEPROM
const int maxRegisters = 32;
uint16_t registers[maxRegisters];

#define ID registers[0] 
#define POTPIN registers[1]
#define DIRPIN registers[2]
#define STEPPIN registers[3]
#define ENABLEPIN registers[4]
#define LEDPIN registers[5]
#define MINFREQ registers[6]
#define MAXFREQ registers[7]
#define MINPOS registers[8]
#define MAXPOS registers[9]
#define MAXTEMP registers[10]
#define KP registers[12]
#define KI registers[16]
#define KD registers[20]

#define TEMP registers[24]
#define FREQ registers[25]
#define DIR registers[26]
#define POSITION registers[27]
#define GOAL registers[28]
#define REBOOT registers[29]
#define SAVE registers[30]
#define _DEFAULT registers[31]

float frequencyVelocity = 0;
float goalFrequency = 0;
int  wait      = 100;
int  addr      = 0;          //eeprom starting memory address

//A struct to hold the response from the joint
struct Response{
  uint16_t pos;
  uint16_t temp;
  byte checksum;
} response;

//A struct to receive a message over the Wire
struct Step{
 uint16_t goal;
 byte checksum;
} step;

bool checksum();
void configure();
void crcInit();
crc crcFast(const crc message[], int nBytes);

unsigned long t;

void setup()
{
   crcInit();
   
   if(!checksum()) {
    Serial.print("Checksum does not match...\nDefaulting...\n");
    defaultConfigure();    
   }
   
   addr+=EEPROM_readAnything(addr, registers[addr]);   //Read ID
   addr+=EEPROM_readAnything(addr, registers[addr]);   //Read potPin
   addr+=EEPROM_readAnything(addr, registers[addr]);   //Read dirPin
   addr+=EEPROM_readAnything(addr, registers[addr]);   //Read stepPin
   addr+=EEPROM_readAnything(addr, registers[addr]);   //Read enablePin
   addr+=EEPROM_readAnything(addr, registers[addr]);   //Read stepPin
   addr+=EEPROM_readAnything(addr, registers[addr]);   //Read minFrequency
   addr+=EEPROM_readAnything(addr, registers[addr]);   //Read maxFrequency
   addr+=EEPROM_readAnything(addr, registers[addr]);   //Read minPosition
   addr+=EEPROM_readAnything(addr, registers[addr]);   //Read maxPosition

   Serial.print(ID);
   Serial.print("\n");
   Serial.print(POTPIN);
   Serial.print("\n");
   Serial.print(DIRPIN);
   Serial.print("\n");
   Serial.print(STEPPIN);
   Serial.print("\n");
   Serial.print(ENABLEPIN);
   Serial.print("\n");
   Serial.print(LEDPIN);
   Serial.print("\n");
   Serial.print(MINFREQ);
   Serial.print("\n");
   Serial.print(MAXFREQ);
   Serial.print("\n");
   Serial.print(MINPOS);
   Serial.print("\n");
   Serial.print(MAXPOS);
   Serial.print("\n");
     
   Wire.begin((int)ID);                                  // join i2c bus with address read from above
   Wire.onRequest(requestEvent);
   Wire.onReceive(wireReceiver);
   pinMode(DIRPIN,OUTPUT);
   pinMode(STEPPIN,OUTPUT);
   pinMode(ENABLEPIN, OUTPUT);
   pinMode(LEDPIN, OUTPUT);
   digitalWrite(STEPPIN,LOW);
   digitalWrite(ENABLEPIN, LOW);
   
   FREQ = 0;
   DIR = 0;
   t=millis();
}

void requestEvent()
{
  //Wire.write("Address");
}

void wireReceiver(int count)
{
  byte * b = (byte*) calloc(count, sizeof(byte));
  for(int i=0; i < count; ++i)
  {
    b[i] = Wire.read();
  }
  
  crc checksum = crcFast(b, totalBytes);
  if(checksum == b[count-1])
  {
   for(int j=0; j < count-1; j += 3)
   {
    //Is this correct?
    registers[b[j]] = (uint16_t) b[j+1];
   } 
  }
}

void loop()
{
  //Check registers
  if(_DEFAULT)
  {
    defaultConfigure();
    _DEFAULT = 0;
  }
  if(SAVE)
  {
    writeConfig();
    SAVE = 0;
  }
  if(REBOOT)
  {
   //handle reboot somehow. 
  }
  
  POSITION = (uint16_t) analogRead(POTPIN);
  
//  if (fabs(position-step.goal) < 4) {
//    goalFrequency = 0;
//  } else if (position-step.goal < 0) {
//    goalFrequency = maxFrequency;
//  } else {
//    goalFrequency = -maxFrequency;
//  }
  
//  frequencyVelocity = frequencyVelocity + dt*((-0.01) * frequencyVelocity +(0.001)/0.005*(position-step.goal));
//  frequency = frequency + dt*(0.001)*frequencyVelocity;
//  if (frequency > maxFrequency) {
//    frequency = maxFrequency;
//    frequencyVelocity = 0;
//  }
//  if (frequency < -maxFrequency) {
//    frequency = -maxFrequency;
//    frequencyVelocity = 0;
//  }
//  Serial.print("f="); Serial.print(frequency); Serial.print("v="); Serial.print(frequencyVelocity); Serial.println();
//  int toneGoal = fabs(frequency);
//  if(toneGoal > maxFrequency) toneGoal = maxFrequency;
//  if(toneGoal < minFrequency) toneGoal = 0;

  FREQ = (1.0)*fabs(POSITION-GOAL)*ratio;

  Serial.print(FREQ);
  Serial.print('\n');

  if(FREQ > MAXFREQ) FREQ = MAXFREQ;
  if(FREQ < MINFREQ) FREQ = 0;
   
  digitalWrite(DIRPIN,(POSITION>GOAL));
  digitalWrite(ENABLEPIN,FREQ != 0);
  NewTone((uint8_t)STEPPIN, (long)FREQ);
  
}

bool checksum(){
  byte b[totalBytes];
  
  for(int i = 0; i < totalBytes; ++i){
   b[i] = EEPROM.read(i);
  }
  
  crc checksum = crcFast(b, totalBytes);
  
  Serial.print("Checksum is: ");
  Serial.print(checksum);
  Serial.print('\n');
  
  crc c = EEPROM.read(totalBytes);
  if(checksum == c){
    return true;
  } else {
    return false;
  }
}

bool writeConfig(){
  Serial.print("Writing configuration to memory...\n");
  digitalWrite(_ledPin.val, HIGH);

  addr += EEPROM_writeAnything(_id.address, ID);
  addr += EEPROM_writeAnything(_potPin.address, POTPIN);
  addr += EEPROM_writeAnything(_dirPin.address, DIRPIN);
  addr += EEPROM_writeAnything(_stepPin.address, STEPPIN);
  addr += EEPROM_writeAnything(_enablePin.address, ENABLEPIN);
  addr += EEPROM_writeAnything(_ledPin.address, LEDPIN);
  addr += EEPROM_writeAnything(_minFreq.address, MINFREQ);
  addr += EEPROM_writeAnything(_maxFreq.address, MAXFREQ);
  addr += EEPROM_writeAnything(_minPos.address, MINPOS);
  addr += EEPROM_writeAnything(_maxPos.address, MAXPOS);
  addr += EEPROM_writeAnything(_maxTemp.address, MAXTEMP);
  
  addr += EEPROM_writeAnything(_Kp.address, KP);
  addr += EEPROM_writeAnything(_Kp.address + 2, ++KP);
  
  addr += EEPROM_writeAnything(_Ki.address, KI);
  addr += EEPROM_writeAnything(_Ki.address + 2, KI);
  
  addr += EEPROM_writeAnything(_Kd.address, KD);
  addr += EEPROM_writeAnything(_Kd.address + 2, KD);
  
  //Return address to 0
  addr = 0;
  
  //Do CRC
  byte b[totalBytes];
  for(int i = 0; i < totalBytes; ++i){
   b[i] = EEPROM.read(i);
  }
  crc checksum = crcFast(b, totalBytes);
  
  Serial.print("Checksum is: ");
  Serial.print(checksum);
  Serial.print('\n');
  EEPROM.write(totalBytes, checksum);
  
  Serial.print("Done writing values to memory");
  delay(1000);
  digitalWrite(_ledPin.val, LOW);
}

void defaultConfigure(){
  Serial.print("Writing default values to memory...\n");
  digitalWrite(_ledPin.val, HIGH);
  
  //Write device id
  Serial.print("Address is: ");
  Serial.print(addr);
  addr += EEPROM_writeAnything(_id.address, _id.val);
  Serial.print("   Value is: ");
  Serial.print(_id.val);
  Serial.print('\n'); 
  
  //Write potentiometer pin
  Serial.print("Address is: ");
  Serial.print(addr);
  addr += EEPROM_writeAnything(_potPin.address, _potPin.val);
  Serial.print("   Value is: ");
  Serial.print(_potPin.val);
  Serial.print('\n');

  //Write direction pin
  Serial.print("Address is: ");
  Serial.print(addr);
  addr += EEPROM_writeAnything(_dirPin.address, _dirPin.val);
  Serial.print("   Value is: ");
  Serial.print(_dirPin.val);
  Serial.print('\n');
  
  //Write stepper motor pin
  Serial.print("Address is: ");
  Serial.print(addr);
  addr += EEPROM_writeAnything(_stepPin.address, _stepPin.val);
  Serial.print("   Value is: ");
  Serial.print(_stepPin.val);
  Serial.print('\n');
  
  //Write enable pin
  Serial.print("Address is: ");
  Serial.print(addr);
  addr += EEPROM_writeAnything(_enablePin.address, _enablePin.val);
  Serial.print("   Value is: ");
  Serial.print(_enablePin.val);
  Serial.print('\n');
  
  //Write led pin
  Serial.print("Address is: ");
  Serial.print(addr);
  addr += EEPROM_writeAnything(_ledPin.address, _ledPin.val);
  Serial.print("   Value is: ");
  Serial.print(_ledPin.val);
  Serial.print('\n');
  
  //Write minFrequency
  Serial.print("Address is: ");
  Serial.print(addr);
  addr += EEPROM_writeAnything(_minFreq.address, _minFreq.val);
  Serial.print("   Value is: ");
  Serial.print(_minFreq.val);
  Serial.print('\n');
  
  //Write maxFrequency
  Serial.print("Address is: ");
  Serial.print(addr);
  addr += EEPROM_writeAnything(_maxFreq.address, _maxFreq.val);
  Serial.print("   Value is: ");
  Serial.print(_maxFreq.val);
  Serial.print('\n');
  
  //Write minPosition
  Serial.print("Address is: ");
  Serial.print(addr);
  addr += EEPROM_writeAnything(_minPos.address, _minPos.val);
  Serial.print("   Value is: ");
  Serial.print(_minPos.val);
  Serial.print('\n');
  
  //Write maxPosition
  Serial.print("Address is: ");
  Serial.print(addr);
  addr += EEPROM_writeAnything(_maxPos.address, _maxPos.val);
  Serial.print("   Value is: ");
  Serial.print(_maxPos.val);
  Serial.print('\n');
  
  //Write maxTemp
  Serial.print("Address is: ");
  Serial.print(addr);
  addr += EEPROM_writeAnything(_maxTemp.address, _maxTemp.val);
  Serial.print("   Value is: ");
  Serial.print(_maxTemp.val);
  Serial.print('\n');
  
  //Write Kp
  Serial.print("Address is: ");
  Serial.print(addr);
  addr += EEPROM_writeAnything(_Kp.address, _Kp.val);
  Serial.print("   Value is: ");
  Serial.print(_Kp.val);
  Serial.print('\n');
  
  //Write Ki
  Serial.print("Address is: ");
  Serial.print(addr);
  addr += EEPROM_writeAnything(_Ki.address, _Ki.val);
  Serial.print("   Value is: ");
  Serial.print(_Ki.val);
  Serial.print('\n');
  
  //Write Kd
  Serial.print("Address is: ");
  Serial.print(addr);
  addr += EEPROM_writeAnything(_Kd.address, _Kd.val);
  Serial.print("   Value is: ");
  Serial.print(_Kd.val);
  Serial.print('\n');

  //Return address to 0
  addr = 0;
  
  //Do CRC
  byte b[totalBytes];
  for(int i = 0; i < totalBytes; ++i){
   b[i] = EEPROM.read(i);
  }
  crc checksum = crcFast(b, totalBytes);
  
  Serial.print("Checksum is: ");
  Serial.print(checksum);
  Serial.print('\n');
  EEPROM.write(totalBytes, checksum);
  
  Serial.print("Done writing values to memory");
  delay(1000);
  digitalWrite(_ledPin.val, LOW);
}


//CRC lookupTable
void
crcInit(void)
{
    crc remainder;


    /*
     * Compute the remainder of each possible dividend.
     */
    for (int dividend = 0; dividend < 256; ++dividend)
    {
        /*
         * Start with the dividend followed by zeros.
         */
        remainder = dividend << (WIDTH - 8);

        /*
         * Perform modulo-2 division, a bit at a time.
         */
        for (uint8_t bit = 8; bit > 0; --bit)
        {
            /*
             * Try to divide the current data bit.
             */			
            if (remainder & TOPBIT)
            {
                remainder = (remainder << 1) ^ POLYNOMIAL;
            }
            else
            {
                remainder = (remainder << 1);
            }
        }

        /*
         * Store the result into the table.
         */
        crcTable[dividend] = remainder;
    }

}   /* crcInit() */

crc
crcFast(uint8_t const message[], int nBytes)
{
    crc data;
    crc remainder = 0;


    /*
     * Divide the message by the polynomial, a byte at a time.
     */
    for (int byte = 0; byte < nBytes; ++byte)
    {
        data = message[byte] ^ (remainder >> (WIDTH - 8));
        remainder = crcTable[data] ^ (remainder << 8);
    }

    /*
     * The final remainder is the CRC.
     */
    return (remainder);

}   /* crcFast() */











