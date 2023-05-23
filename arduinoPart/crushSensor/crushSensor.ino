// #
// # Editor     : Lauren from DFRobot
// # Date       : 26.09.2012
// # E-Mail : Lauren.pan@dfrobot.com

// # Product name: Crash sensor
// # Product SKU : SEN0138
// # Version     : 0.1

int ledPin = 7;                // choose the pin for the LED
int inputPin = 2;               // Connect sensor to input pin 3
int status=0; //0 ->off 1->on 

void setup() {
  Serial.begin(9600);           // Init the serial port

  pinMode(ledPin, OUTPUT);      // declare LED as output
  pinMode(inputPin, INPUT);     // declare Micro switch as input
}

void loop(){
  int val = digitalRead(inputPin);  // read input value
  if (val == HIGH) {  // check if the input is HIGH
    status;
    digitalWrite(ledPin, LOW);      // turn LED OFF
    Serial.println("not Pressed!");

  } 
  else {
    digitalWrite(ledPin, HIGH);     // turn LED ON
    Serial.println("Switch Pressed!");
  }
  delay(5);
}