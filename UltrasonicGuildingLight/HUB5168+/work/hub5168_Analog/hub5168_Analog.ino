 int sensorReading = 0;
#define analogPin A2
#define LED_Pin 7

void setup() {
    pinMode(LED_Pin, OUTPUT);
    Serial.begin(115200);
}

void loop() {
  sensorReading = analogRead(analogPin);
  Serial.println(sensorReading);
  analogWrite(LED_Pin,sensorReading/4);
  delay(100);
}
