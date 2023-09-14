#define ButtonPin 8
#define LED_Pin 7
int buttonState = 0;

void setup() {
    pinMode(LED_Pin, OUTPUT);
    pinMode(ButtonPin, INPUT);
    Serial.begin(115200);
}

void loop() {
  buttonState = digitalRead(ButtonPin);
  if(buttonState == HIGH)
  {
    digitalWrite(LED_Pin,HIGH);
  }
  else
  {
    digitalWrite(LED_Pin,LOW);
  }
  delay(100);
}
