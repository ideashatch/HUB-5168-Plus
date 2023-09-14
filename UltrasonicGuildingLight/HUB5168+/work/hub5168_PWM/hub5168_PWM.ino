const int LED_Pin = 3;
int delay_time=25;

void setup() {
    pinMode(LED_Pin, OUTPUT);
}

void loop() {
  
  for(int i=0;i<20;i++)
  {
    analogWrite(LED_Pin,i*10);
    delay(delay_time);
  }
  for(int i=20;i>0;i--)
  {
    analogWrite(LED_Pin,i*10);
    delay(delay_time);
  }
}
