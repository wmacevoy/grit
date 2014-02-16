// Motor Controller
// this is the code for the individual joints..... the master is located on the main body
// and will have a different code uploaded to it.
//
//
//CRC code from - http://www.barrgroup.com/Embedded-Systems/How-To/CRC-Calculation-C-Code
//
//


#include <Wire.h>
#include <EEPROM.h>
#include "typedef.h"
#include "EEPROMAnything.h"
#include "default.h"

//These 2 will be deprecated soon
const int maxbuffer=4;
char serialInput[maxbuffer];
/////////////////////////////////

const int totalBytes = 33;
const int cutoff=10;

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
int  ledPin;
int  minFrequency;
long maxFrequency;
int  minPosition;
long maxPosition;

//Receive buffer and length
const int byteBuffer = 6;
byte bytes[byteBuffer];

float frequency = 0;
int  position  = 0;
int  dir       = 0;          // 1 counter clockwise/0 stop/-1 clockwise
int  wait      = 100;
int  addr      = 0;          //eeprom starting memory address

//Time stuffs
unsigned long t1, t2, dt;

//PID stuffs
long error, previous_error;
long integral, derivative;
float Kp, Ki, Kd;

//A struct to hold the response from the joint
struct Response{
  int pos;
  int temp;
  int checksum;
} response;

//A struct to receive a message over the Wire
struct Step{
 int freq;
 int goal;
 int checksum;
} step;

bool checksum();
void configure();
void crcInit();
crc crcFast(const uint8_t message[], int nBytes);

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
   addr+=EEPROM_readAnything(addr, ledPin);         //Read stepPin
   addr+=EEPROM_readAnything(addr, minFrequency);   //Read minFrequency
   addr+=EEPROM_readAnything(addr, maxFrequency);   //Read maxFrequency
   addr+=EEPROM_readAnything(addr, minPosition);    //Read minPosition
   addr+=EEPROM_readAnything(addr, maxPosition);    //Read maxPosition
   addr+=EEPROM_readAnything(addr, Kp);             //Read Kp
   addr+=EEPROM_readAnything(addr, Ki);             //Read Ki
   addr+=EEPROM_readAnything(addr, Kd);             //Read Kd
     
   Wire.begin(id);                                  // join i2c bus with address read from above
   Wire.onRequest(requestEvent);
   pinMode(dirPin,OUTPUT);
   pinMode(stepPin,OUTPUT);
   pinMode(ledPin, OUTPUT);
   digitalWrite(dirPin,LOW);
   digitalWrite(stepPin,LOW);
   
   step.freq = maxFrequency;
   frequency = 0;
   
   t1 = millis();
   t2 = 0;
   dt = 0;
   
   previous_error = 0;
   integral = 0;
}


void loop()
{  
   //Serial will be used when a config manager connects to the board
   if (Serial.available() > 0) {
    Serial.readBytes(serialInput,maxbuffer);
    //Will be deprecated, for testing only
    step.goal=0;
    for(int i=0;i<maxbuffer-1;i++)
    {
      step.goal*=10;
      step.goal += serialInput[i]-'0';//Serial.print(goal);Serial.print(" ");delay(500);
    }
    //End testing
   }
  
   //Read six incoming bytes, 
   /*if(Wire.available() % byteBuffer == 0) {
    for(int i = 0; i < byteBuffer; ++i){
     bytes[i] = Wire.read();
    }
    //Check validity of packet
    //if(valid) {
    memcpy(&step, bytes, byteBuffer); 
   }*/
   
   if (step.goal < 250) step.goal=250;
   if (step.goal > 745) step.goal=750;
   
   position = analogRead(potPin);
   //Serial.print(position);
   //Serial.print('\n');

   //Calculate the change in time
   t2 = millis();
   dt = t2 - t1;
   t1 = t2;
   
   error = step.goal-position;
   integral = integral + error*dt;
   derivative = (error - previous_error)/dt;
   frequency = Kp*error + Ki*integral + Kd*derivative;
   frequency = (2900.0 - (frequency*frequency) / 86.2) + 100;
   if(frequency < 100) frequency=100;
   if(frequency > 3000) frequency=3000;
   previous_error = error;
   
   if (dir == 0 && abs(step.goal-position) <= cutoff) {
     return;
   }
   if (position < step.goal) {
      if (dir != 1) {
        digitalWrite(dirPin,0); 
      }
      dir = 1;
      tone(stepPin,(int)frequency);
   } else if (position > step.goal) {
      if (dir != -1) {
        digitalWrite(dirPin,1);
      }
      tone(stepPin,(int)frequency);
      dir = -1;
   } else {
     if (dir != 0) {
       noTone(stepPin);
     }
     dir = 0;
   }
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











