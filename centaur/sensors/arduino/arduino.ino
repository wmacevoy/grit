// Wire Master Reader
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Reads data from an I2C/TWI slave device
// Refer to the "Wire Slave Sender" example for use with this

// Created 29 March 2006

// This example code is in the public domain.


#include <Wire.h>

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

void setup()
{
  Serial.begin(9600);  // start serial for output
  Wire.begin();        // join i2c bus (address optional for master)
  setupCompass(COMPASSADDR);
  setupAccel(ACCELADDR);
  setupGyro(GYROADDR);

  nios=0;
  
  // safe light
  ios[nios].pin=13;
  ios[nios].output=true;
  ios[nios].analog=false;
  ios[nios].pullup=false;
  ios[nios].value=0;
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

int state = 0;
int index = 0;
int number = 0;

void process(char ch)
{
  switch(state) {
  case 0: 
    if (ch == 'S') { state = 1; }
    break;
  case 1:
    if (ch >= '0' && ch <= '9') {
      index = (ch - '0');
      state = 2;
    } else {
      state = 0;
    }
    break;
  case 2:
    if (ch == '=') {
      state = 3;
      number = 0;
    } else {
      state = 0;
    }
    break;
  case 3:
    if (ch == '\n' || ch == '\r') {
      if (index < nios && ios[index].output) {
        ios[index].value = number;
        if (ios[index].analog) {
          analogWrite(ios[index].pin,number);
        } else {
          digitalWrite(ios[index].pin,number);
        }
      }
      state = 0;
    } else if (ch >= '0' && ch <= '9') {
      number = number*10 + (ch-'0');
    } else {
      state = 0;
    }
    break;
  default:
    state=0;
  }
}

void read()
{
  while (Serial.available()) {
    process(Serial.read());
  }
}

void loop()
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

  while (Serial.available()) {
    process(Serial.read());
  }


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
