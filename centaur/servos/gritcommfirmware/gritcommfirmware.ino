#include <Wire.h>

int32_t timeout;
const uint32_t BAUD=9600L;
const uint32_t TIMEOUT = 100000; // milliseconds
const int I2C_ADDRESS = 126;

// 'p', position is the only command supported.
uint8_t command; 

// 0-127 i2c address to send to 
uint8_t address; 

// 0-255 value to send (1b)
uint8_t value;

void receiveEvent(int howMany) 
{
  if (state == 3) {
    while (howMany > 0) {
      Serial.write(Wire.read());
      --howMany;
    }
    state=0;
  }
}

bool ch(char &c)
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
  Wire.begin(I2C_ADDRESS);
  Wire.onReceive(receiveEvent);
}

void loop() 
{
  if (state != 0 && (long(millis())-timeout) > 0) {
    state = 0;
  }

  switch(state) {
  case 0:
    if (ch(command) && command == 'g') {
      timeout=millis()+TIMEOUT;
      state = 1;
    }
    break;
  case 1:
    if (ch(address)) {
      if (address > 0 && address < 127) {
	time=millis()+TIMEOUT;
	state = 2;
      }
    }
    break;
  case 2:
    if (ch(value)) {
        Wire.beginTransmission(address);
        Wire.write(value);
        Wire.endTransmission();
	if (command == 'g') {
	  time=millis()+TIMEOUT;
	  state = 3;
	} else {
	  state = 0;
	}
    }
    break;
  case 3:
    /* rx on i2c message should reset this */
    break;
  }
}
