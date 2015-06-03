#include <EEPROM.h>
#include <Wire.h>

const int ENABLE=7;
const int STEP1=3;
const int DIR1=4;
//const int STEP2=5;
const int DIR2=6;
const int MULTIPLIER=64;

byte goal=0;
byte current=0;

const int STARTADDRESS=0;
class Settings {
  public:
  short low;
  short high;
  byte address;
  byte speed;
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
  }
};
Settings settings;

void showMenu() {
  Serial.println("Swerve s,l,h,a,w,g,m, and p");
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
  } else if (c=='p' || c=='P') {
    settings.show();
    Serial.print("Goal: ");
    Serial.println(goal);
  }
}

void receiveEvent(int howMany) {
  int newGoal=goal;
  int newSpeed=settings.speed;
  while (!Wire.available());
  if (Wire.available()>0) {
    newGoal=Wire.read();
  }
  if (newGoal>=0 && newGoal<=180) {
    goal=newGoal;
  }
  while (!Wire.available());
  if (Wire.available()>0) {
    newSpeed=Wire.read();
  }
  if (newSpeed>=0 && newSpeed<=255) {
    settings.speed=newSpeed;
  }
}

void requestEvent() {
  Wire.write(goal);
  Wire.write(current);
  Wire.write(settings.speed);
}

void getCurrent() {
  current=map(analogRead(0),settings.low,settings.high,0,180);
}

void setup() {
  pinMode(ENABLE,OUTPUT);
  digitalWrite(ENABLE,LOW);
  pinMode(STEP1,OUTPUT);
  digitalWrite(STEP1,LOW);
  pinMode(DIR1,OUTPUT);
  digitalWrite(DIR1,LOW);
//  pinMode(STEP2,OUTPUT);
//  digitalWrite(STEP2,LOW);
  pinMode(DIR2,OUTPUT);
  digitalWrite(DIR1,LOW);
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

void swerve(int current,int goal,byte speed) {
  if (speed>0) digitalWrite(ENABLE,HIGH);
  else digitalWrite(ENABLE,LOW);
  int frequency=abs(speed-128);
  if (abs(current-goal)<50) { // On target
    if (speed>128) { // Reverse
      digitalWrite(DIR1,LOW); 
      digitalWrite(DIR2,HIGH);
    } else {
      digitalWrite(DIR1,HIGH);
      digitalWrite(DIR2,LOW);
    }
    tone(STEP1,frequency*MULTIPLIER);
 //   tone(STEP2,frequency*MULTIPLIER);
  }
  else if (current<goal) {
    if (speed>128) { // Reverse
      digitalWrite(DIR1,HIGH); 
      digitalWrite(DIR2,HIGH);
    } else {
      digitalWrite(DIR1,LOW);
      digitalWrite(DIR2,LOW);
    }
    tone(STEP1,frequency*MULTIPLIER);
//    tone(STEP2,frequency*MULTIPLIER);
  } else {
    if (speed>128) { // Reverse
      digitalWrite(DIR1,LOW); 
      digitalWrite(DIR2,LOW);
    } else {
      digitalWrite(DIR1,HIGH);
      digitalWrite(DIR2,HIGH);
    }
    tone(STEP1,frequency*MULTIPLIER);
//    tone(STEP2,frequency*MULTIPLIER);    
  }
}

void loop() {
  int dd=(abs(settings.low-settings.high)*7)/100;
  if (dd==0) dd=1;
  int joint=analogRead(0);
  getCurrent();
  int goalPosition=map(goal,0,180,settings.low,settings.high);
  swerve(joint,goalPosition,settings.speed);
  if (Serial.available()) {
    handleUI(Serial.read());
  }
}
