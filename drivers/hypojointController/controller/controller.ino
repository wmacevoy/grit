
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

int  addr      = 0;          //eeprom starting memory address

unsigned long t;

//PID stuffs
long  error, previous_error;
long  integral, derivative;


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
#define KP (*(float*)(&registers[12]))
#define KI (*(float*)(&registers[16]))
#define KD (*(float*)(&registers[20]))

#define TEMP registers[24]
#define FREQ registers[25]
#define DIR registers[26]
#define POSITION registers[27]
#define GOAL registers[28]
#define REBOOT registers[29]
#define SAVE registers[30]
#define _DEFAULT registers[31]

float f=0;
float df=0;
int dt=0;

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
void defaultConfigure();
bool writeConfig();
void crcInit();
crc crcFast(const crc message[], int nBytes);

void printRegisters()
{
Serial.print("Registers:\nID: ");
uint16_t p=ID;
Serial.print(p);
Serial.print("\nPotpin: \n");
p=POTPIN;
Serial.print(p);
Serial.print("\nDirpin: \n");
p=DIRPIN;
Serial.print(p);
Serial.print("\nSteppin: \n");
p=STEPPIN;
Serial.print(p);
Serial.print("\nEnablepin: \n");
p=ENABLEPIN;
Serial.print(p);
Serial.print("\nLedpin: \n");
p=LEDPIN;
Serial.print(p);
Serial.print("\nMinfreq: \n");
p=MINFREQ;
Serial.print(p);
Serial.print("\nMaxfreq: \n");
p=MAXFREQ;
Serial.print(p);
Serial.print("\nMinpos: \n");
p=MINPOS;
Serial.print(p);
Serial.print("\nMaxpos: \n");
p=MAXPOS;
Serial.print(p);
Serial.print("\nMaxtemp: \n");
p=MAXTEMP;
Serial.print(p);
Serial.print("\nKp: \n");
float f=KP;
Serial.print(f);
Serial.print("\nKi: \n");
f=KI;
Serial.print(f);
Serial.print("\nKd: \n");
f=KD;
Serial.print(f);

Serial.print("\nTemp: \n");
p=TEMP;
Serial.print(p);
Serial.print("\nFreq: \n");
p=FREQ;
Serial.print(p);
Serial.print("\nDir: \n");
p=DIR;
Serial.print(p);
Serial.print("\nPosition: \n");
p=POSITION;
Serial.print(p);
Serial.print("\nGOAL: \n");
p=GOAL;
Serial.print(p);
Serial.print("\nReboot: \n");
p=REBOOT;
Serial.print(p);
Serial.print("\nSave: \n");
p=SAVE;
Serial.print(p);
Serial.print("\n_Default: \n");
p=_DEFAULT;
Serial.print(p);
Serial.print("\n \n \n \n \n ");
}

void setup()
{
   crcInit();
   Serial.begin(9600);
   
   if(!checksum()) 
   {
    Serial.print("Checksum does not match...\nDefaulting...\n");
    defaultConfigure();    
   }
   
   addr+=EEPROM_readAnything(addr, ID);   //Read ID
   addr+=EEPROM_readAnything(addr, POTPIN);   //Read potPin
   addr+=EEPROM_readAnything(addr, DIRPIN);   //Read dirPin
   addr+=EEPROM_readAnything(addr, STEPPIN);   //Read stepPin
   addr+=EEPROM_readAnything(addr, ENABLEPIN);   //Read enablePin
   addr+=EEPROM_readAnything(addr, LEDPIN);   //Read stepPin
   addr+=EEPROM_readAnything(addr, MINFREQ);   //Read minFrequency
   addr+=EEPROM_readAnything(addr, MAXFREQ);   //Read maxFrequency
   addr+=EEPROM_readAnything(addr, MINPOS);   //Read minPosition
   addr+=EEPROM_readAnything(addr, MAXPOS);   //Read maxPosition
   addr+=EEPROM_readAnything(addr, KP);
   addr+=EEPROM_readAnything(addr, KI);
   addr+=EEPROM_readAnything(addr, KD);

   Serial.println(ID);
   Serial.println(POTPIN);
   Serial.println(DIRPIN);
   Serial.println(STEPPIN);
   Serial.println(ENABLEPIN);
   Serial.println(LEDPIN);
   Serial.println(MINFREQ);
   Serial.println(MAXFREQ);
   Serial.println(MINPOS);
   Serial.println(MAXPOS);
   Serial.println((float)KP);
   Serial.println((float)KI);
   Serial.println((float)KD);
   Wire.begin(4);            // join i2c bus with address read from above
   Wire.onRequest(requestEvent);
   Wire.onReceive(wireReceiver);
   pinMode(DIRPIN,OUTPUT);
   pinMode(STEPPIN,OUTPUT);
   pinMode(ENABLEPIN, OUTPUT);
   pinMode(LEDPIN, OUTPUT);
   digitalWrite(STEPPIN,LOW);
   digitalWrite(ENABLEPIN, LOW);
   
   GOAL=512;

   t=millis();
}

void requestEvent()
{
  //Wire.write("Address");
}

void wireReceiver(int count)
{
  Serial.print("\nGot a message of ");
  Serial.print(count);
  Serial.print(" bytes.\n");
  
  byte * b = (byte*) calloc(count, sizeof(byte));
  for(int i=0; i < count; ++i)
  {
    b[i] = Wire.read();
    //Serial.print((int)b[i]);
  }
  
  crc checksum = crcFast(b, count-1);
  //Serial.print("Checksum: ");
  //Serial.print(checksum);
  if(checksum == b[count-1])
  {
   //Serial.print("YAAAAY");
   for(int j=0; j < count-1; j += 3)
   {
    Serial.print("address: ");
    Serial.print(b[j]);
    
    uint8_t address = b[j];
    uint16_t value = (uint16_t) b[j+1];
    
    Serial.print("value: ");
    Serial.print((uint16_t) b[j+1]);
    Serial.print("\n ");
    registers[address] = value;
    
   } 
  }
}


void loop()
{
  dt=millis()-t;
  t += dt;
  //Check registers
  if(_DEFAULT)
  {
    Serial.print("If check got DEFAULT flag");
    defaultConfigure();
    _DEFAULT = 0;
  }
  if(SAVE)
  {
    Serial.print("If check got SAVE flag"); 
    writeConfig();
    SAVE = 0;
  }
  if(REBOOT)
  {
    Serial.print("If check got REBOOT flag");
    REBOOT = 0;
   //handle reboot somehow. 
  }
  
  POSITION = (uint16_t) analogRead(POTPIN);
 // f positive increases POSITION
   df=0;
   
   int error = POSITION-GOAL;
   
   if (abs(error)>100) {
      if (error < 0) {
       f += dt*0.001*float(MAXFREQ);   
      } else {
       f -= dt*0.001*float(MAXFREQ);
      }
   } else if (abs(error) > 10) {
      f = -KP*float(MAXFREQ)/200.0*error; 
   } else {
     f=0;
   }

   if (f > float(MAXFREQ)) {
     f=float(MAXFREQ);
   }
   if (f < -float(MAXFREQ)) {
     f=-float(MAXFREQ);
   }
  
   // f=3000;

   if (f < 0) {
     FREQ=-f;
   } else {
     FREQ=f;
   }

  // if (FREQ != 0 && FREQ < MINFREQ) {
  //   FREQ=MINFREQ;
  // }
  
   if (f > 0.0) {
     digitalWrite(DIRPIN,0);
     Serial.println(" dir=0");
   } else {
     digitalWrite(DIRPIN,1);
     Serial.println(" dir=1");
   }
   digitalWrite(ENABLEPIN,FREQ != 0);
   //NewTone((uint8_t)STEPPIN, (long)FREQ);
   tone(STEPPIN,FREQ);
   Serial.print(" P="); Serial.print(POSITION);
   Serial.print(" G="); Serial.print(GOAL);
   Serial.print(" dt="); Serial.print(dt);
   Serial.print(" f="); Serial.print(f);
   Serial.print(" df="); Serial.print(df);
   Serial.print(" MAXFREQ="); Serial.print(MAXFREQ); Serial.println();
  
#if 0 
  error = GOAL-POSITION;
  integral = integral + error*dt;
  derivative = (error - previous_error)/dt;
  FREQ = KP*error + KI*integral + KD*derivative;
  FREQ = ((MAXFREQ - (FREQ*FREQ) / 860.2));
//  if(FREQ < MINFREQ) FREQ=MINFREQ;
  if(FREQ > MAXFREQ) FREQ=MAXFREQ;
  previous_error = error;
   
  if (DIR == 0 && abs(GOAL-POSITION) <= cutoff) {
    return;
  }
  if (POSITION < GOAL) {
      if (DIR != 1) {
        digitalWrite(DIRPIN,1); 
      }
      DIR = 1;
      tone(STEPPIN,(int)FREQ);
   } else if (POSITION > GOAL) {
      if (DIR != -1) {
        digitalWrite(DIRPIN,00);
      }
      tone(STEPPIN,(int)FREQ);
      DIR = -1;
   } else {
     if (DIR != 0) {
       noTone(STEPPIN);
     }
     DIR = 0;
   }
   
   Serial.print(" P="); Serial.print(POSITION);
   Serial.print(" G="); Serial.print(GOAL);
   Serial.print(" dt="); Serial.print(dt);
   Serial.print(" f="); Serial.print(FREQ);
   Serial.println();
#endif
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

  addr += EEPROM_writeAnything(_id.address, (uint16_t)ID);
  addr += EEPROM_writeAnything(_potPin.address, (uint16_t)POTPIN);
  addr += EEPROM_writeAnything(_dirPin.address, (uint16_t)DIRPIN);
  addr += EEPROM_writeAnything(_stepPin.address, (uint16_t)STEPPIN);
  addr += EEPROM_writeAnything(_enablePin.address, (uint16_t)ENABLEPIN);
  addr += EEPROM_writeAnything(_ledPin.address, (uint16_t)LEDPIN);
  addr += EEPROM_writeAnything(_minFreq.address, (uint16_t)MINFREQ);
  addr += EEPROM_writeAnything(_maxFreq.address, (uint16_t)MAXFREQ);
  addr += EEPROM_writeAnything(_minPos.address, (uint16_t)MINPOS);
  addr += EEPROM_writeAnything(_maxPos.address, (uint16_t)MAXPOS);
  addr += EEPROM_writeAnything(_maxTemp.address, (uint16_t)MAXTEMP);
  
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
  
  //Write minFREQ
  Serial.print("Address is: ");
  Serial.print(addr);
  addr += EEPROM_writeAnything(_minFreq.address, _minFreq.val);
  Serial.print("   Value is: ");
  Serial.print(_minFreq.val);
  Serial.print('\n');
  
  //Write maxFREQ
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
  
  Serial.print("Done writing values to memory\n");
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
