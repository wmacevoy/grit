// Wire Master Reader
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Reads data from an I2C/TWI slave device
// Refer to the "Wire Slave Sender" example for use with this

// Created 29 March 2006

// This example code is in the public domain.

#include <Wire.h>
#include <avr/pgmspace.h>

static const unsigned short crc_table[256] PROGMEM = {
  0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
  0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
  0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
  0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
  0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
  0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
  0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
  0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
  0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
  0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
  0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
  0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
  0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
  0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
  0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
  0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
  0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
  0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
  0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
  0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
  0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
  0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
  0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
  0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
  0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
  0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
  0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
  0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
  0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
  0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
  0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
  0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};

/* CRC calculation macros */
#define CRC_INIT 0xFFFF
#define CRC(crcval,newchar) (crcval) = ((crcval) >> 8) ^ pgm_read_word_near(crc_table+(((crcval) ^ (newchar)) & 0x00ff))

unsigned short crcsum(const unsigned char* message, unsigned long length,
       unsigned short crc)
{
  unsigned long i;

  for(i = 0; i < length; i++)
    {
      CRC(crc, message[i]);
    }
  return crc;
}

#define GYROADDR 0x68
#define COMPASSADDR 0x1e
#define ACCELADDR 0x53

union XYZBuffer {
  struct {
    short x,y,z;
  } value;
  byte buff[6];
};

void changeEndian(union XYZBuffer *xyz) {
  for (int i=0;i<6;i+=2) {
    byte t=xyz->buff[i];
    xyz->buff[i]=xyz->buff[i+1];
    xyz->buff[i+1]=t;
  }
}

// Generically useful reading into a union type
void readXYZ(int device,union XYZBuffer *xyz) {     
  Wire.requestFrom(device, 6);      
  long start=millis();
  while (!Wire.available() && (millis()-start)<100);
  if (millis()-start<100) {
    for (int i=0;i<6;i++)
      xyz->buff[i]=Wire.read();
  }
}

void setupAccel(int device) {
  // Check ID to see if we are communicating
  Wire.beginTransmission(device);
  Wire.write((uint8_t)0x00); // One Reading
  Wire.endTransmission(); 
  Wire.requestFrom(device,1);
  while (!Wire.available());  
  byte ch=Wire.read();
  Serial.print("Accel id is 0x");
  Serial.println(ch,HEX);
  // Should output E5
  
  // https://www.sparkfun.com/datasheets/Sensors/Accelerometer/ADXL345.pdf
  // Page 16
  Wire.beginTransmission(device);
  Wire.write((uint8_t)0x2d);
  Wire.write((uint8_t)0x08);
  Wire.endTransmission();
  Wire.beginTransmission(device);
  Wire.write((uint8_t)0x38);
  Wire.write((uint8_t)0x84);
  Wire.endTransmission();

}
void readAccel(int device,union XYZBuffer *xyz) {
  Wire.beginTransmission(device);
  Wire.write(0x32); // One Reading
  Wire.endTransmission(); 
  readXYZ(device,xyz);
}

void setupCompass(int device) {
  // Check ID to see if we are communicating
  Serial.print("Compass id is ");
  Wire.beginTransmission(device);
  Wire.write((uint8_t)10); // One Reading
  Wire.endTransmission(); 
  Wire.requestFrom(device,2); 
  while (!Wire.available());
  char ch=Wire.read();
  Serial.print(ch);   
  ch=Wire.read();
  Serial.println(ch);
  // Should output H4  
  
// Page 18
// at http://dlnmh9ip6v2uc.cloudfront.net/datasheets/Sensors/Magneto/HMC5883L-FDS.pdf
  Wire.beginTransmission(device);
  Wire.write((uint8_t)0x00); Wire.write((uint8_t)0x70);
  Wire.endTransmission();
  Wire.beginTransmission(device);
  Wire.write((uint8_t)0x01); Wire.write((uint8_t)0xA0);
  Wire.endTransmission();
  Wire.beginTransmission(device);
  Wire.write((uint8_t)0x02); Wire.write((uint8_t)0x00); //  Reading
  Wire.endTransmission(); 
  delay(6);
}
void readCompass(int device,union XYZBuffer *xyz) {
  readXYZ(device,xyz);
  changeEndian(xyz);
  Wire.beginTransmission(device);
  Wire.write((uint8_t)0x03);
  Wire.endTransmission(); 
}

void setupGyro(int device) {
  // Check ID to see if we are communicating
  Wire.beginTransmission(device);
  Wire.write((uint8_t)0x00); // One Reading
  Wire.endTransmission(); 
  Wire.requestFrom(device,1);
  while (!Wire.available());  
  byte ch=Wire.read();
  Serial.print("Gyro id is 0x");
  Serial.println(ch,HEX);  
  // Should output 69
}
void readGyro(int device,union XYZBuffer *xyz) {
  // https://www.sparkfun.com/datasheets/Sensors/Gyro/PS-ITG-3200-00-01.4.pdf
  // page 20
  Wire.beginTransmission(device);
  Wire.write((uint8_t)0x1d);
  Wire.endTransmission(); 
  readXYZ(device,xyz);
  changeEndian(xyz);  
}

void pad(int width,int number) {
  int n=abs(number);
  int w=width;
  if (number<0) w--;
  while (n>0) {
    w--;
    n/=10;
  }
  if (number==0) w--;
  for (int i=0;i<w;i++) Serial.print(' ');
}

void output(union XYZBuffer xyz) {
//  pad(6,xyz.value.x);
  Serial.print(xyz.value.x);
  Serial.print(',');
//  pad(6,xyz.value.y);
  Serial.print(xyz.value.y);
  Serial.print(',');
//  pad(6,xyz.value.z);
  Serial.print(xyz.value.z);
}

typedef struct {
  int pin;
  bool output; // input/output
  bool analog; // analog/digital
  bool pullup; // pullup/no pullup
  int value;
} io_t;

int nios=0;
io_t ios[8]; // at least as many as are setup()

unsigned long nextWrite;

void setup()
{
  Serial.begin(115200);  // start serial for output
  Wire.begin();        // join i2c bus (address optional for master)
  setupCompass(COMPASSADDR);
  setupAccel(ACCELADDR);
  setupGyro(GYROADDR);

  nextWrite = millis();

  nios=0;
  
  // safe light
  ios[nios].pin=9;
  ios[nios].output=true;
  ios[nios].analog=true;
  ios[nios].pullup=false;
  ios[nios].value=0;
  ++nios;

  ios[nios].pin = 10;
  ios[nios].output = true;
  ios[nios].analog = true;
  ios[nios].pullup = false;
  ios[nios].value = 0;
  ++nios;

  ios[nios].pin = 11;
  ios[nios].output = true;
  ios[nios].analog = true;
  ios[nios].pullup = false;
  ios[nios].value = 0;
  ++nios;

  for (int i=0; i<nios; ++i) {
    if (ios[i].output) {
      if (ios[i].analog) {
        analogWrite(ios[i].pin,ios[i].value);
      } else {
        digitalWrite(ios[i].pin,ios[i].value);
        pinMode(ios[i].pin,OUTPUT);
      }
    } else {
      if (ios[i].analog) {
        ios[i].value = analogRead(ios[i].pin);
      } else {
        if (ios[i].pullup) {
          pinMode(ios[i].pin,INPUT);
          digitalWrite(ios[i].pin,HIGH);
        } else {
          pinMode(ios[i].pin,INPUT);
        }
	ios[i].value = digitalRead(ios[i].pin);
      }
    }
  }
}

int len=0;
char data[80];

int state = 0;
int index = 0;
int value = 0;

void process(char ch)
{
  switch(state) {
  case 0: 
    if (ch == 'S') { 
      index = 0; 
      state = 1; 
    }
    break;
  case 1:
    if (ch >= '0' && ch <= '9') {
      index = index*10 + (ch - '0');
    } else if (ch == '=') {
      value = 0;
      state = 2;
    } else {
      state = 0;
    }
    break;
  case 2:
    if (ch >= '0' && ch <= '9') {
      value = value*10 + (ch-'0');
    } else if (ch == '$' || ch == ',') {
      if (index < nios && ios[index].output) {
        ios[index].value = value;
        if (ios[index].analog) {
          analogWrite(ios[index].pin,value);
        } else {
          digitalWrite(ios[index].pin,value);
        }
      }
      state = 0;
    } else {
      state = 0;
    }
    break;
  default:
    state=0;
  }
}

bool check(char *mark)
{
  int m=mark-data;
  unsigned short crc0 = CRC_INIT;
  for (char *p=data; p != mark; ++p) CRC(crc0,*p);

  unsigned short crc1 = 0;
  for (char *p=mark+1; *p != 0; ++p) {
    char x=*p;
    if ('0' <= x && x <= '9') {
      crc1 = crc1*16 + (x-'0');
    } else if ('a' <= x && x <= 'f') {
      crc1 = crc1*16 + (x-'a'+10);
    } else if ('A' <= x && x <= 'F') {
      crc1 = crc1*16 + (x-'A'+10);
    } else {
      return false;
    }
  }
  return (crc0 == crc1);
}


void read()
{
  while (Serial.available()) {
    char ch = Serial.read();
    if (ch == '\n' || ch == '\r') {
      if (len < sizeof(data)) {
	data[len]=0;
	char *mark = strrchr(data,'$');
	if (mark != 0 && (strcmp(mark,"$ok")==0 || check(mark))) {
	  state=0;
	  char *p=data;
	  for (;;) {
	    process(*p);
	    if (p == mark) break;
	    ++p;
	  }
	}
      }
      len=0;
    } else if (len < sizeof(data)) {
      data[len]=ch;
      ++len;
    }
  }
}

void write()
{
  union XYZBuffer compass,gyro,accel;
  int l1,l2,l3,l4;

  for (int i=0; i<nios; ++i) {
    if (!ios[i].output) {
      if (ios[i].analog) {
        ios[i].value = analogRead(ios[i].pin);
      } else {
        ios[i].value = digitalRead(ios[i].pin);
      }
    }
  }

  l1=analogRead(0);
  l2=analogRead(1);
  l3=analogRead(2);
  l4=analogRead(3);

  readAccel(ACCELADDR,&accel);
  readCompass(COMPASSADDR,&compass);
  readGyro(GYROADDR,&gyro);

  Serial.print("A,");
  output(accel);
  Serial.print(",C,");
  output(compass);
  Serial.print(",G,");  
  output(gyro);
  Serial.print(",L,");
  Serial.print(l1);
  Serial.print(",");
  Serial.print(l2);
  Serial.print(",");
  Serial.print(l3);
  Serial.print(",");
  Serial.print(l4);
  if (nios > 0) {
    Serial.print(",S");
    for (int i=0; i<nios; ++i) {
      Serial.print(",");
      Serial.print(ios[i].value);
    }
  }
  Serial.println();
}

void loop()
{
  read();
  if (millis() > nextWrite) {
    write();
    nextWrite=millis() + 50;
  }
}

