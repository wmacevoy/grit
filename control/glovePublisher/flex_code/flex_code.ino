//To caibrate push and hold the button.  Black button is left hand, red button is right hand.

//Pin Setup
int flexSensorPin[8] = { A0, A1, A2, A3, A4, A5, A6, A7 };
int flexSetButtonPin1 = 10; //digital read button for setting flex constraints - left
int flexSetButtonPin2 = 11; //digital read button for setting flex constraints - right
int led1 = 7, led2 = 6; //led's turn on when calibrating
// A0 = pin 0  Hand 1.1
// A1 = pin 1  Hand 1.2
// A2 = pin 2  Hand 1.3
// A3 = pin 3  Hand 1.4
// A4 = pin 4  Hand 2.1
// A5 = pin 5  Hand 2.2
// A6 = pin 6  Hand 2.3
// A7 = pin 7  Hand 2.4

//Flex variables
int i, head, tail;
long minFlexMap = 360, maxFlexMap = 0;  //map value range 0 - 100
int flexButtonRead1, flexButtonRead2;
long flexReading[8];
long flexMap[8];
long minFlexReading[8];
long maxFlexReading[8];

void setup() {
  pinMode (flexSetButtonPin1, INPUT);
  pinMode (flexSetButtonPin2, INPUT);
  digitalWrite (flexSetButtonPin1, HIGH);  //set pull-up resistor
  digitalWrite (flexSetButtonPin2, HIGH);  //set pull-up resistor
  pinMode (led1, OUTPUT);
  pinMode (led2, OUTPUT);
  Serial.begin (9600);
}

void getFlexReadings() { for (i=0; i<8; i++) flexReading[i] = analogRead(flexSensorPin[i]); }
void mapFlexReadings() { for (i=0; i<8; i++) flexMap[i] = map(flexReading[i], minFlexReading[i], maxFlexReading[i], minFlexMap, maxFlexMap); }

void calibrate() {
  if (flexButtonRead1 == LOW) {
    head = 0;
    tail = 4;
    digitalWrite(led1,HIGH);
  } else {
    head = 4;
    tail = 8;
    digitalWrite(led2,HIGH);
  }
  for (i=head; i<tail; i++) { minFlexReading[i] = 1024; maxFlexReading[i] = 0; }
  while (flexButtonRead1 == LOW || flexButtonRead2 == LOW) {
    getFlexReadings();
    for (i=head; i<tail; i++) {
      if (flexReading[i] < minFlexReading[i]) minFlexReading[i] = flexReading[i];
      if (flexReading[i] > maxFlexReading[i]) maxFlexReading[i] = flexReading[i];
    }
    flexButtonRead1 = digitalRead(flexSetButtonPin1);
    flexButtonRead2 = digitalRead(flexSetButtonPin2);
  }
  digitalWrite(led1,LOW);
  digitalWrite(led2,LOW);
}

void printData() {
  //Serial.print("L,");
  for (i=0; i<4; i++) {
    Serial.print(flexMap[i]);
    Serial.print(" ");
  }
  //Serial.print("R,");
  for (i=4; i<8; i++) {
    Serial.print(flexMap[i]);
    Serial.print(" ");
  }
  Serial.println();
}

void loop () {
  flexButtonRead1 = digitalRead(flexSetButtonPin1);
  flexButtonRead2 = digitalRead(flexSetButtonPin2);
  
  getFlexReadings();
  if (flexButtonRead1 == LOW || flexButtonRead2 == LOW) calibrate();
  mapFlexReadings();
  printData();

  delay (300);
}
