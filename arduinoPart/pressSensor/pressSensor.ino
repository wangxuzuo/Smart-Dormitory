int s_pin = A0;

void setup()

{

Serial.begin(9600);

pinMode(s_pin,INPUT);

}

void loop()

{

Serial.println(analogRead(s_pin));

delay(1000);

}