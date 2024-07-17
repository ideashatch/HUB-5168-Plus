#include <SimpleDHT.h>

const int pinDHT11 = 2;                           // DHT11 溫濕度傳感器 使用 2 腳位
SimpleDHT11 dht11;
String DHT_Temperature="Temperature";
String DHT_Humidity="Humidity";


bool Get_DHT11()
{
 byte temperature,humidity;
 byte CorrectionTEMP = 1;       // 溫度校正值
 byte CorrectionHUMI = 12;       // 溼度校正值 
 int err = SimpleDHTErrSuccess; 
 //Serial.println("溫溼度模組 DHT11 資料讀取中....");
 byte count_i=0; 
 while ((err = dht11.read(pinDHT11,&temperature, &humidity, NULL)) != SimpleDHTErrSuccess)
 {
   //Serial.println("溫溼度資料讀取錯誤,錯誤碼:" + String (err));
   delay (1000);
   count_i++;
   if (count_i>3) return false;
 }
 DHT_Temperature = String(temperature - CorrectionTEMP);
 DHT_Humidity = String(humidity - CorrectionHUMI);
 //Serial.print("攝氏溫度：");Serial.print(DHTtemp); Serial.println(" °C"); 
 //Serial.print("環境溼度：");Serial.print(DHThumi); Serial.println(" ％");
 return true;    
}
