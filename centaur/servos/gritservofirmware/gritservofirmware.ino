#include <EEPROM.h>
#include <Wire.h>

const int ENB=3;
const int INC=4;
const int IND=7;

byte goal=0;
byte current=0;

const int STARTADDRESS=0;
class Settings {
  public:
  short low;
  short high;
  byte address;
  byte speed;
  byte gateway;
  void writeMemory() {
    byte *ptr=(byte *)this;
    for (int i=0;i<sizeof(Settings);i++){
      EEPROM.write(STARTADDRESS+i,ptr[i]);
    }
  }        
  void readMemory() {
    byte *ptr=(byte *)this;
    for (int i=0;i<sizeof(Settings);i++){
      ptr[i]=EEPROM.read(STARTADDRESS+i);
    }
  }
  void show() {
    Serial.println("Settings");
    Serial.print("low: ");
    Serial.println(low);
    Serial.print("high: ");
    Serial.println(high);
    Serial.print("address: ");
    Serial.println(address);
    Serial.print("speed: ");
    Serial.println(speed);
    Serial.print("gateway: ");
    Serial.println(gateway);
  }
};
Settings settings;

void showMenu() {
  Serial.println("r, s,l,h,a,w,g,m, and p");
  Serial.println("r gateway address [0 to 127]");
  Serial.println("s show sensor values");
  Serial.println("l low position 0 degrees [0 to 1023]");
  Serial.println("h high position 180 degrees [0 to 1023]");
  Serial.println("a I^C address [0 to 127]");
  Serial.println("w write settings to EEProm");
  Serial.println("g goto to position [0 to 180]");
  Serial.println("m speed [0 to 255]"); 
  Serial.println("p print current values");
}

void handleUI(char c) {
  if (c=='s' || c=='S') {
    Serial.print("Sensor 0:");
    Serial.println(analogRead(0));
  } else if (c=='l' || c=='L') {
    int value=Serial.parseInt();
    if (value>=0 && value<=1023) {
      Serial.println("Ok");
      settings.low=value; 
    } else {
      Serial.println("Invalid Low Value");
    }
  } else if (c=='h' || c=='H') {
    int value=Serial.parseInt();
    if (value>=0 && value<=1023) {
      Serial.println("Ok");
      settings.high=value; 
    } else {
      Serial.println("Invalid High Value");
    }
  } else if (c=='m' || c=='M') {
    int value=Serial.parseInt();
    if (value>=0 && value<=255) {
      Serial.println("Ok");
      settings.speed=value; 
    } else {
      Serial.println("Invalid High Value");
    }
  } else if (c=='a' || c=='A') {
    int newAddress=Serial.parseInt();
    if (newAddress>0 && newAddress<=127) {
      Serial.println("Ok");
      settings.address=newAddress;
    } else {
      Serial.println("Invalid Address");
    }
  } else if (c=='w' || c=='W') {
    Serial.println("Ok");
    settings.writeMemory();
  } else if (c=='g' || c=='G') {
    int newGoal=Serial.parseInt();
    if (newGoal>=0 && newGoal<=180) {
      Serial.println("Ok");
      goal=newGoal;
    } else {
      Serial.println("Invalid Goal Position");
    }
  } else if (c=='r' || c=='R') {
    int newGateway=Serial.parseInt();
    if (newGateway>=0 && newGateway<=127) {
      Serial.println("Ok");
      settings.gateway=newGateway;
    } else {
      Serial.println("Invalid Goal Position");
    }
  } else if (c=='p' || c=='P') {
    settings.show();
    Serial.print("Goal: ");
    Serial.println(goal);
  }
}

void forward() {
  digitalWrite(INC,HIGH);
  digitalWrite(IND,LOW);
  analogWrite(ENB,settings.speed);
}

void reverse() {
  digitalWrite(INC,LOW);
  digitalWrite(IND,HIGH);
  analogWrite(ENB,settings.speed);
}

void softStop() { // No Braking
  digitalWrite(INC,LOW);
  digitalWrite(IND,LOW);
  analogWrite(ENB,0);
}

void hardStop() { // Braking
  digitalWrite(INC,LOW);
  digitalWrite(IND,LOW);
  analogWrite(ENB,255);
}

void receiveEvent(int howMany) {
  int newGoal=goal;
  int newSpeed=settings.speed;
  if (Wire.available()>0) {
    newGoal=Wire.read();
  }
  if (newGoal>=0 && newGoal<=180) {
    goal=newGoal;
  }
  if (Wire.available()>0) {
    newSpeed=Wire.read();
  }
  if (newSpeed>=0 && newSpeed<=255) {
    settings.speed=newSpeed;
  }
}

void requestEvent() {
  Wire.write(current);
}

void getCurrent() {
  current=map(analogRead(0),settings.low,settings.high,0,180);
}

void setup() {
  pinMode(ENB,OUTPUT);
  digitalWrite(ENB,LOW);
  pinMode(INC,OUTPUT);
  digitalWrite(INC,LOW);
  pinMode(IND,OUTPUT);
  digitalWrite(IND,LOW);
  analogWrite(ENB,255);
  settings.readMemory();
  Wire.begin(settings.address);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  Serial.begin(9600);
  getCurrent();
  int value=analogRead(0);
//  Serial.println(current);
  if (current>180 || current<0) {
    if (abs(value-settings.low)>abs(value-settings.high)) current=180;
    else current=0;
  }
  goal=current;
  showMenu();
}

void loop() {
  int dd=(abs(settings.low-settings.high)*7)/100;
  if (dd==0) dd=1;
  int joint=analogRead(0);
  getCurrent();
  int goalPosition=map(goal,0,180,settings.low,settings.high);
//  Serial.println(goalPosition);
  if (abs(joint-goalPosition)<dd) {
    hardStop();
  }
  else if(joint > goalPosition) {
    reverse();
  } else {
    forward();
  }
  if (Serial.available()) {
    handleUI(Serial.read());
  }
}
