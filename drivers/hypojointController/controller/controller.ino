
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

//Write a '1' or other non-zero number to address 34 to turn on debug serial messages.


#include <Wire.h>
#include <EEPROM.h>
#include "typedef.h"
#include "EEPROMAnything.h"
#include "default.h"


const int totalBytes = 34;
const int cutoff=12;

int  addr = 0;          //eeprom starting memory address
unsigned long t;
unsigned int lastpos;
unsigned int stalltime;
unsigned int dp;

int myabs(int x) { return (x<0) ? -x : x; }
float myfabs(float x) { return (x<0) ? -x : x; }

//PID stuffs
int  error, previous_error;
long  integral, derivative;

//CRC stuffs
crc crcTable[256];
#define WIDTH  (8 * sizeof(crc))
#define TOPBIT (1 << (WIDTH - 1))
#define POLYNOMIAL 0xD8

//Register set
//first 17 registers are data from the EEPROM
const int maxRegisters = 35;
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
#define PRINTREG registers[32]
#define READREG registers[33]
#define DEBUG registers[34]

float f=0;
float df=0;
int   dt=0;

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
Serial.print(ID);
Serial.print("\nPotpin: ");
Serial.print(POTPIN);
Serial.print("\nDirpin: ");
Serial.print(DIRPIN);
Serial.print("\nSteppin: ");
Serial.print(STEPPIN);
Serial.print("\nEnablepin: ");
Serial.print(ENABLEPIN);
Serial.print("\nLedpin: ");
Serial.print(LEDPIN);
Serial.print("\nMinfreq: ");
Serial.print(MINFREQ);
Serial.print("\nMaxfreq: ");
Serial.print(MAXFREQ);
Serial.print("\nMinpos: ");
Serial.print(MINPOS);
Serial.print("\nMaxpos: ");
Serial.print(MAXPOS);
Serial.print("\nMaxtemp: ");
Serial.print(MAXTEMP);
Serial.print("\nKp: ");
Serial.print(KP);
Serial.print("\nKi: ");
Serial.print(KI);
Serial.print("\nKd: ");
Serial.print(KD);
Serial.print("\nTemp: ");
Serial.print(TEMP);
Serial.print("\nFreq: ");
Serial.print(FREQ);
Serial.print("\nDir: ");
Serial.print(DIR);
Serial.print("\nPosition: ");
Serial.print(POSITION);
Serial.print("\nGOAL: ");
Serial.print(GOAL);
Serial.print("\nReboot: ");
Serial.print(REBOOT);
Serial.print("\nSave: ");
Serial.print(SAVE);
Serial.print("\n_Default: ");
Serial.print(_DEFAULT);
Serial.print("\n_Debug: ");
Serial.print(DEBUG);
Serial.print("\n \n \n \n \n ");
}

void readEEPROM()
{
   addr = 0;
   addr+=EEPROM_readAnything(addr, ID);       //Read ID
   addr+=EEPROM_readAnything(addr, POTPIN);   //Read potPin
   addr+=EEPROM_readAnything(addr, DIRPIN);   //Read dirPin
   addr+=EEPROM_readAnything(addr, STEPPIN);  //Read stepPin
   addr+=EEPROM_readAnything(addr, ENABLEPIN);//Read enablePin
   addr+=EEPROM_readAnything(addr, LEDPIN);   //Read stepPin
   addr+=EEPROM_readAnything(addr, MINFREQ);  //Read minFrequency
   addr+=EEPROM_readAnything(addr, MAXFREQ);  //Read maxFrequency
   addr+=EEPROM_readAnything(addr, MINPOS);   //Read minPosition
   addr+=EEPROM_readAnything(addr, MAXPOS);   //Read maxPosition
   addr+=EEPROM_readAnything(addr, MAXTEMP);  //Read maxTemp
   addr+=EEPROM_readAnything(addr, KP);
   addr+=EEPROM_readAnything(addr, KI);
   addr+=EEPROM_readAnything(addr, KD);
}

void setup()
{
   crcInit();
   Serial.begin(9600);
   
   if(!checksum()) 
   {
    if(DEBUG) {
     Serial.println("Checksum does not match...\nDefaulting...");
    }
    defaultConfigure();    
   }
   
   memset(registers, 0, sizeof(registers));
   
   readEEPROM();

   if(DEBUG) {
   printRegisters();
   }
   Wire.begin((int)ID);            // join i2c bus with address read from above
   Wire.onRequest(requestEvent);
   Wire.onReceive(wireReceiver);
   pinMode(DIRPIN,OUTPUT);
   pinMode(STEPPIN,OUTPUT);
   pinMode(ENABLEPIN, OUTPUT);
   pinMode(LEDPIN, OUTPUT);
   digitalWrite(STEPPIN,LOW);
   digitalWrite(ENABLEPIN, LOW);

   POSITION = (uint16_t) analogRead(POTPIN);
   lastpos = POSITION; 
   
   t=millis();
}

void requestEvent()
{
  //Wire.write("Address");
}

void wireReceiver(int count)
{
  if(DEBUG) {
  Serial.print("\nGot a message of ");
  Serial.print(count);
  Serial.println(" bytes.");
  }
  
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
    if(DEBUG) {
    Serial.print("address: ");
    Serial.print(b[j]);
    }
    uint8_t address = b[j];
    uint16_t value = (*(uint16_t*)(&b[j+1]));
    if(DEBUG) {  
    Serial.print(" value: ");
    Serial.println(value);
    }
    registers[address] = value;
   } 
  }
  free(b);
}
void loop()
{
  dt=millis()-t;
  t += dt;
  POSITION = (uint16_t) analogRead(POTPIN);
  
  //If motot is stalled, reset f.
  dp = myabs(POSITION - lastpos);
  lastpos = POSITION;
  if(myabs(error) > cutoff)
     {
     if(dp < 4)
       {
       stalltime += dt;
       if(stalltime >= 500)
         {
          f = MINFREQ;
          stalltime = 0;
          dp = 0;
          if(DEBUG) {
          Serial.println("motor stalled");
          }
         }
       }
     else
       {
       dp = 0;
       stalltime = 0;
       }
     }
     
  //Check registers
  if(READREG)
  {
    if(DEBUG) {
    Serial.println("If check got READREG flag");
    }
    readEEPROM();
    READREG = 0;
  }
  if(PRINTREG)
  {
    printRegisters();
    PRINTREG = 0;
  }
  if(_DEFAULT)
  {
    if(DEBUG) {
    Serial.println("If check got DEFAULT flag");
    }
    defaultConfigure();
    _DEFAULT = 0;
  }
  if(SAVE)
  {
    if(DEBUG) {
    Serial.println("If check got SAVE flag"); 
    }
    writeConfig();
    SAVE = 0;
  }
  if(REBOOT)
  {
    if(DEBUG) {
    Serial.println("If check got REBOOT flag");
    }
    REBOOT = 0;
   //handle reboot somehow. 
  }
  
  //Motor movement 
   error = POSITION-GOAL;
   if (myabs(error) > cutoff) {
      float g=-KP*float(MAXFREQ)/200.0*error;
      df=g-f;
      if (myfabs(df) > dt*0.0008*float(MAXFREQ)) {
        if (df < 0) {
          df=-dt*0.0008*float(MAXFREQ);
        } else {
          df=dt*0.0008*float(MAXFREQ);
        }
     }
     f=f+df;  
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
//     Serial.println(" dir=0");
   } else {
     digitalWrite(DIRPIN,1);
//     Serial.println(" dir=1");
   }
   digitalWrite(ENABLEPIN,FREQ != 0);
   tone(STEPPIN,FREQ);
}

bool checksum(){
  byte b[totalBytes];
  
  for(int i = 0; i < totalBytes; ++i){
   b[i] = EEPROM.read(i);
  }
  
  crc checksum = crcFast(b, totalBytes);
  
  if(DEBUG) {
  Serial.print("Checksum is: ");
  Serial.println(checksum);
  }
  
  crc c = EEPROM.read(totalBytes);
  if(checksum == c){
    return true;
  } else {
    return false;
  }
}

bool writeConfig(){
  if(DEBUG) {
  Serial.println("Writing configuration to memory...");
  }
  digitalWrite(_ledPin.val, HIGH);

  addr = 0;
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
  //addr += EEPROM_writeAnything(_Kp.address + 2, ++KP);
  
  addr += EEPROM_writeAnything(_Ki.address, KI);
  //addr += EEPROM_writeAnything(_Ki.address + 2, KI);
  
  addr += EEPROM_writeAnything(_Kd.address, KD);
  //addr += EEPROM_writeAnything(_Kd.address + 2, KD);
  
  //Return address to 0
  addr = 0;
  
  //Do CRC
  byte b[totalBytes];
  for(int i = 0; i < totalBytes; ++i){
   b[i] = EEPROM.read(i);
  }
  crc checksum = crcFast(b, totalBytes);
 
  if(DEBUG) {
  Serial.print("Checksum is: ");
  Serial.println(checksum);
  }
  EEPROM.write(totalBytes, checksum);

  if(DEBUG) {
  Serial.print("Done writing values to memory");
  }
  delay(1000);
  digitalWrite(_ledPin.val, LOW);
}

void defaultConfigure(){
  if(DEBUG) {
  Serial.print("Writing default values to memory...\n");
  }
  digitalWrite(_ledPin.val, HIGH);
  addr = 0;
  //Write device id
  addr += EEPROM_writeAnything(_id.address, _id.val);
  if(DEBUG) {
  Serial.print("Address is: ");
  Serial.print(addr);
  Serial.print("   Value is: ");
  Serial.println(_id.val);
  }
  
  //Write potentiometer pin
  addr += EEPROM_writeAnything(_potPin.address, _potPin.val);
  if(DEBUG) {
  Serial.print("Address is: ");
  Serial.print(addr);
  Serial.print("   Value is: ");
  Serial.println(_potPin.val);
  }

  //Write direction pin
  addr += EEPROM_writeAnything(_dirPin.address, _dirPin.val);
  if(DEBUG) {
  Serial.print("Address is: ");
  Serial.print(addr);
  Serial.print("   Value is: ");
  Serial.println(_dirPin.val);
  }
  
  //Write stepper motor pin
  addr += EEPROM_writeAnything(_stepPin.address, _stepPin.val);
  if(DEBUG) {
  Serial.print("Address is: ");
  Serial.print(addr);
  Serial.print("   Value is: ");
  Serial.println(_stepPin.val);
  }
  
  //Write enable pin
  addr += EEPROM_writeAnything(_enablePin.address, _enablePin.val);
  if(DEBUG) {
  Serial.print("Address is: ");
  Serial.print(addr);
  Serial.print("   Value is: ");
  Serial.println(_enablePin.val);
  }
  
  //Write led pin
  addr += EEPROM_writeAnything(_ledPin.address, _ledPin.val);
  if(DEBUG) {
  Serial.print("Address is: ");
  Serial.print(addr);
  Serial.print("   Value is: ");
  Serial.println(_ledPin.val);
  }
  
  //Write minFREQ
  addr += EEPROM_writeAnything(_minFreq.address, _minFreq.val);
  if(DEBUG) {
  Serial.print("Address is: ");
  Serial.print(addr);
  Serial.print("   Value is: ");
  Serial.println(_minFreq.val);
  }
  
  //Write maxFREQ
  addr += EEPROM_writeAnything(_maxFreq.address, _maxFreq.val);
  if(DEBUG) {
  Serial.print("Address is: ");
  Serial.print(addr);
  Serial.print("   Value is: ");
  Serial.println(_maxFreq.val);
  }
  
  //Write minPosition
  addr += EEPROM_writeAnything(_minPos.address, _minPos.val);
  if(DEBUG) {
  Serial.print("Address is: ");
  Serial.print(addr);
  Serial.print("   Value is: ");
  Serial.println(_minPos.val);
  }
  //Write maxPosition
  addr += EEPROM_writeAnything(_maxPos.address, _maxPos.val);
  if(DEBUG) {
  Serial.print("Address is: ");
  Serial.print(addr);
  Serial.print("   Value is: ");
  Serial.println(_maxPos.val);
  }
  
  //Write maxTemp
  addr += EEPROM_writeAnything(_maxTemp.address, _maxTemp.val);
  if(DEBUG) {
  Serial.print("Address is: ");
  Serial.print(addr);
  Serial.print("   Value is: ");
  Serial.println(_maxTemp.val);
  }
  
  //Write Kp
  addr += EEPROM_writeAnything(_Kp.address, _Kp.val);
  if(DEBUG) {
  Serial.print("Address is: ");
  Serial.print(addr);
  Serial.print("   Value is: ");
  Serial.println(_Kp.val);
  }
  
  //Write Ki
  addr += EEPROM_writeAnything(_Ki.address, _Ki.val);
  if(DEBUG) {
  Serial.print("Address is: ");
  Serial.print(addr);
  Serial.print("   Value is: ");
  Serial.println(_Ki.val);
  }
  
  //Write Kd
  addr += EEPROM_writeAnything(_Kd.address, _Kd.val);
  if(DEBUG) {
  Serial.print("Address is: ");
  Serial.print(addr);
  Serial.print("   Value is: ");
  Serial.println(_Kd.val);
  }

  //Return address to 0
  addr = 0;
  
  //Do CRC
  byte b[totalBytes];
  for(int i = 0; i < totalBytes; ++i){
   b[i] = EEPROM.read(i);
  }
  crc checksum = crcFast(b, totalBytes);
  EEPROM.write(totalBytes, checksum);
  
  if(DEBUG) {
  Serial.print("Checksum is: ");
  Serial.println(checksum);
  Serial.println("Done writing values to memory");
  }
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
