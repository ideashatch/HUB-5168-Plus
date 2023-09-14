#define SensorPin 8
#define LEDPin 3
int SensorReading = 0;

void setup() {
    pinMode(SensorPin, INPUT);
    pinMode(LEDPin, OUTPUT);
    Serial.begin(115200);
}

void loop() {
  SensorReading = digitalRead(SensorPin);
  Serial.println(SensorReading);
  if(SensorReading)
  {
    digitalWrite(LEDPin,HIGH);
  }
  else
  {
    digitalWrite(LEDPin,LOW);
  }
  delay(100);
}
