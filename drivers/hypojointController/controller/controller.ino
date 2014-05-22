
// Motor Controller
// this is the code for the individual joints..... the master is located on the main body
// and will have a different code uploaded to it.
//
//
//CRC code from - http://www.barrgroup.com/Embedded-Systems/How-To/CRC-Calculation-C-Code
//
//
//32000 TONE @ 16th Steps


#include <Wire.h>
#include <EEPROM.h>
#include "NewTone.h"
#include "typedef.h"
#include "EEPROMAnything.h"
#include "default.h"

//These 2 will be deprecated soon
const int maxbuffer=4;
char serialInput[maxbuffer];
/////////////////////////////////

const int totalBytes = 35;
const int cutoff=10;

float ratio = 32000/1024;

//CRC stuffs
crc crcTable[256];
#define WIDTH  (8 * sizeof(crc))
#define TOPBIT (1 << (WIDTH - 1))
#define POLYNOMIAL 0xD8

//Items to be read from eeprom in setup()
byte id;
int  potPin;
int  dirPin;
int  stepPin;
int  enablePin;
int  ledPin;
int  minFrequency;
long maxFrequency;
int  minPosition;
long maxPosition;

//Receive buffer and length
const int byteBuffer = 6;
byte bytes[byteBuffer];

float frequency = 0;
float frequencyVelocity = 0;
float goalFrequency = 0;
int  position  = 0;
int  dir       = 0;          // 1 counter clockwise/0 stop/-1 clockwise
int  wait      = 100;
int  addr      = 0;          //eeprom starting memory address

//A struct to hold the response from the joint
struct Response{
  int pos;
  int temp;
  int checksum;
} response;

//A struct to receive a message over the Wire
struct Step{
 int id;
 int goal;
 int checksum;
} step;

bool checksum();
void configure();
void crcInit();
crc crcFast(const uint8_t message[], int nBytes);

unsigned long t;

void setup()
{
   Serial.begin(9600);
   crcInit();
   delay(1000);
   
   if(!checksum()) {
    Serial.print("Checksum no matchy...\nDefaulting...\n");
    defaultConfigure();    
   }
   
   addr+=EEPROM_readAnything(addr, id);             //Read ID
   addr+=EEPROM_readAnything(addr, potPin);         //Read potPin
   addr+=EEPROM_readAnything(addr, dirPin);         //Read dirPin
   addr+=EEPROM_readAnything(addr, stepPin);        //Read stepPin
   addr+=EEPROM_readAnything(addr, enablePin);      //Read enablePin
   addr+=EEPROM_readAnything(addr, ledPin);         //Read stepPin
   addr+=EEPROM_readAnything(addr, minFrequency);   //Read minFrequency
   addr+=EEPROM_readAnything(addr, maxFrequency);   //Read maxFrequency
   addr+=EEPROM_readAnything(addr, minPosition);    //Read minPosition
   addr+=EEPROM_readAnything(addr, maxPosition);    //Read maxPosition
    
   Serial.print(id);
   Serial.print("\n");
   Serial.print(potPin);
   Serial.print("\n");
   Serial.print(dirPin);
   Serial.print("\n");
   Serial.print(stepPin);
   Serial.print("\n");
   Serial.print(enablePin);
   Serial.print("\n");
   Serial.print(ledPin);
   Serial.print("\n");
   Serial.print(minFrequency);
   Serial.print("\n");
   Serial.print(maxFrequency);
   Serial.print("\n");
   Serial.print(minPosition);
   Serial.print("\n");
   Serial.print(maxPosition);
   Serial.print("\n");
   
     
   Wire.begin(id);                                  // join i2c bus with address read from above
   Wire.onRequest(requestEvent);
   pinMode(dirPin,OUTPUT);
   pinMode(stepPin,OUTPUT);
   pinMode(enablePin, OUTPUT);
   pinMode(ledPin, OUTPUT);
   digitalWrite(stepPin,LOW);
   digitalWrite(enablePin, LOW);
   
   frequency = 0;
   dir = 0;
   step.goal = 250;
   t=millis();
}


void loop()
{  
   //Read six incoming bytes, 
   if(Wire.available() % byteBuffer == 0) {
    for(int i = 0; i < byteBuffer; ++i){
     bytes[i] = Wire.read();
    }
    //Check validity of packet
    //if(valid) {
    memcpy(&step, bytes, byteBuffer); 
   }
   
//  long position=analogRead(potPin);
//  
//  if (abs(position-512)<16) 
//    digitalWrite(enablePin,LOW);
//  else 
//    digitalWrite(enablePin,HIGH);
//  
//  if(position<512) 
//    digitalWrite(dirPin,LOW);
//  else
//    digitalWrite(dirPin,HIGH);
//  
//  frequency = abs(position-512)*ratio;
//  if(frequency > maxFrequency) frequency = maxFrequency;
//  if(frequency < minFrequency) frequency = minFrequency;
//  
//  tone(stepPin,frequency);

  position = analogRead(potPin);
  
  if (fabs(position-step.goal) < 4) {
    goalFrequency = 0;
  } else if (position-step.goal < 0) {
    goalFrequency = maxFrequency;
  } else {
    goalFrequency = -maxFrequency;
  }
  
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
  
  Serial.print(frequency);
  Serial.print('\n');

  frequency = (1.0)*fabs(position-step.goal)*ratio;

  if(frequency > maxFrequency) frequency = maxFrequency;
  if(frequency < minFrequency) frequency = 0;
   
  digitalWrite(dirPin,(position>step.goal));
  digitalWrite(enablePin,frequency != 0);
  NewTone(stepPin,(int)frequency);
  
}

void requestEvent()
{
  Wire.write("Address "); // respond with message of 6 bytes
                          // as expected by master
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
  int checksum = crcFast(b, totalBytes);
  
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











