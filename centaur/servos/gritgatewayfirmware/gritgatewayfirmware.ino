#include <Wire.h>

int32_t timeout;
const uint32_t BAUD=9600L;
const uint32_t TIMEOUT = 15; // milliseconds
const int I2C_ADDRESS = 126;
const int LED_PIN = 13;
uint8_t state;

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
  Wire.begin(I2C_ADDRESS);
  Wire.onReceive(receiveEvent);
  pinMode(LED_PIN,OUTPUT);
  for (int i=0; i<10; ++i) {
    digitalWrite(LED_PIN,i % 2 == 0);
    delay(25);
  }
}

int lastState = -1;

void loop() 
{
  if (state != lastState) {
    lastState = state;
  }
  if (state != 0 && (long(millis())-timeout) > 0) {
    state = 0;
  }
  digitalWrite(LED_PIN, state != 0);
  switch(state) {
  case 0:
    if (ch(command) && command == 'g') {
      timeout=millis()+TIMEOUT;
      state = 1;
    }
    break;
  case 1:
    if (ch(address) && (address >= 0 && address <= 127)) {
	timeout=millis()+TIMEOUT;
	state = 2;
    }
    break;
  case 2:
    if (ch(value)) {
        uint8_t speed=100;
        Serial.print(address); Serial.print(":"); Serial.print(value); Serial.println();
        Wire.beginTransmission(address);
        Wire.write(value);
        Wire.write(speed);
        Wire.endTransmission();
 //       Serial.println("sent.");
	if (command == 'g') {
          Wire.requestFrom((int)address, 1);
  	  timeout=millis()+TIMEOUT;
//          Serial.print(value); Serial.print("@"); Serial.println(address);
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
