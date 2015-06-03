#include <Wire.h>

int32_t timeout;
const uint32_t BAUD=9600L;
const uint32_t TIMEOUT = 15; // milliseconds
const int LED_PIN = 13;
uint8_t state;

// 'g' goto , position is the only command supported.
uint8_t command; 

// 0-127 i2c address to send to 
uint8_t address; 

// 0-255 value to send (1b)
uint8_t value0;
uint8_t value1;

void receiveEvent(int howMany) 
{
  if (state == 4) {
    while (howMany > 0) {
      while (Wire.available()==0);
      Serial.write(Wire.read());
      --howMany;
    }
    state=0;
  }
}

bool ch(uint8_t &c)
{
  if (Serial.available()>0) {
    c=Serial.read();
    return true;
  } else {
    return false;
  }
}

void setup()
{
  Serial.begin(BAUD);
  state = 0;
  Wire.begin();
  Wire.onReceive(receiveEvent);
  pinMode(LED_PIN,OUTPUT);
  for (int i=0; i<10; ++i) {
    digitalWrite(LED_PIN,i % 2 == 0);
    delay(25);
  }
}

static uint8_t angles[4] = {45,135,45,135};
static int8_t  dirs[4] = {1,1,-1,-1};

void loop() 
{
  if (state != 0 && (long(millis())-timeout) > 0) {
    state = 0;
  }
  digitalWrite(LED_PIN, state != 0);
  switch(state) {
  case 0:
    if (ch(command)) {
      switch(command) {
      case 'f':
	for (int i=0; i<4; ++i) {
	  Wire.beginTransmission(10*(i+1));
	  Wire.write(angles[i]);
	  Wire.write(128+4*dirs[i]);
	  Wire.endTransmission();
	  delay(10);
	}
	break;
      case 's':
	for (int i=0; i<4; ++i) {
	  Wire.beginTransmission(10*(i+1));
	  Wire.write(90);
	  Wire.write(0);
	  Wire.endTransmission();
	  delay(10);
	}
	digitalWrite(LED_PIN,1-digitalRead(LED_PIN));
	break;
      case 'g':
	timeout=millis()+TIMEOUT;
	state = 1;
	break;
      }
    }
    break;
  case 1:
    if (ch(address) && (address >= 0 && address <= 127)) {
	timeout=millis()+TIMEOUT;
	state = 2;
    }
    break;
  case 2:
    if (ch(value0)) {
	timeout=millis()+TIMEOUT;
	state = 3;
    }
  case 3:
    if (ch(value1)) {
        Wire.beginTransmission(address);
        Wire.write(value0);
        Wire.write(value1);
        Wire.endTransmission();
	Wire.requestFrom((int)address, 1);
	timeout=millis()+TIMEOUT;
	state = 4;
    }
    break;
  case 4:
    /* rx on i2c message should reset this */
    break;
  }
}
