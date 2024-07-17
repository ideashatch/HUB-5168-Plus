#include "DHT11.h"
void setup() {
  // put your setup code here, to run once:
 Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Get_DHT11())
  {
   Serial.print("攝氏溫度：");Serial.print(DHT_Temperature); Serial.println(" °C"); 
   Serial.print("環境溼度：");Serial.print(DHT_Humidity); Serial.println(" ％");
  }
  delay (5000);
}
