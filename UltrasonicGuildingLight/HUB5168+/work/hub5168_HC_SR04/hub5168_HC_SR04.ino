const int trigPin = 7;                  //Trig Pin
const int echoPin = 8;                  //Echo Pin
const int LED1_Pin = 10;
const int LED2_Pin = 11;
const int LED3_Pin = 12;
int distance = 0;

long readUltrasonicDistance(int triggerPin, int echoPin)
{
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(5);
  // Sets the trigger pin to HIGH state for 10 microseconds
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  // Reads the echo pin, and returns the sound wave travel time in microseconds
  return pulseIn(echoPin, HIGH);// 收到高電位時的時間
}

void setup() {
  Serial.begin (115200);
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT);
  pinMode(LED1_Pin, OUTPUT);
  pinMode(LED2_Pin, OUTPUT);
  pinMode(LED3_Pin, OUTPUT);
}
 
void loop()
{
  distance = 0.01723 * readUltrasonicDistance(trigPin, echoPin);
  if(distance < 15){
    digitalWrite(LED1_Pin,LOW);
    digitalWrite(LED2_Pin,LOW);
    digitalWrite(LED3_Pin,LOW);
  }
  else if(distance < 30){
    digitalWrite(LED1_Pin,HIGH);
    digitalWrite(LED2_Pin,LOW);
    digitalWrite(LED3_Pin,LOW);
  }
  else if(distance < 45){
    digitalWrite(LED1_Pin,HIGH);
    digitalWrite(LED2_Pin,HIGH);
    digitalWrite(LED3_Pin,LOW);
  }
  else if(distance < 300){
    digitalWrite(LED1_Pin,HIGH);
    digitalWrite(LED2_Pin,HIGH);
    digitalWrite(LED3_Pin,HIGH);
  }
  Serial.print("Distance : "); 
  Serial.print(distance);
  Serial.println("cm");
  delay(50);
}
