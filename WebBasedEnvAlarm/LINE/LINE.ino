#include <WiFi.h>
#include "LineNotify.h"
char ssid[] = "***********"; // your network SSID (name)
char pass[] = "**********";

int status=WL_IDLE_STATUS;
char LocalIP[20];
char clientId[15];
byte mac[6];
String ntoken="****************************************";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (status != WL_CONNECTED)
  {
   status = WiFi.begin(ssid, pass);
   // wait 0.5 seconds for connection:
   delay(500);
  }
  Serial.println();
  sprintf(LocalIP,"%d.%d.%d.%d",WiFi.localIP()[0],WiFi.localIP()[1],WiFi.localIP()[2],WiFi.localIP()[3]);
  WiFi.macAddress(mac);
  sprintf(clientId,"Mac-%02X%02X%02X%02X%02X%02X",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);          
  Serial.println(LocalIP);         
  Serial.println(clientId);
  delay(100);
}

void loop() {
  // put your main code here, to run repeatedly:
 String data= "";	
 while (Serial.available())
 {
  data=Serial.readStringUntil('\n');
  Serial.println(data);
  if (sendLineMsg(status,String(ntoken),data)) // 送出 LineNotify 的資料
   Serial.println("Line Send Success.");
  else
   Serial.println("Line Send Fail!!");
  break;
 }
}
