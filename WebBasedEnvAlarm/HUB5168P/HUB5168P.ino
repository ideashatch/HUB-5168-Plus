#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <PubSubClient.h>
#include <rtc.h>
#include <ArduinoJson.h>
#include <FlashMemory.h>
#include <Adafruit_PCF8574.h>
#include <U8g2lib.h>
#include "GetHtml.h"
#include "DHT11.h"
#include "LineNotify.h"

#include "WDT.h"
WDT wdt;

Adafruit_PCF8574 pcf;

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

char AP_ssid[] = "AP-HUB5168P"; // your network SSID (name)
char AP_pass[] = "password";    // your network password (use for WPA, or use as key for WEP)
char channel[] = "1";           //Set the AP's channel
int status = WL_IDLE_STATUS;    //Set the Wifi radio's status
int ssid_status = 0;            //Set SSID status, 1 hidden, 0 not hidden
//State the analog pin that you want to read
//int analogChannel = 0;

char nssid[30];               //max 30 char (不支援輸入特殊字元及空白符號 !!) 
char npass[20];               //max 20 char
char ntoken[50];              //max 50 char

String Line_Param_data = "Temperature is :";  // 發送給 LineNotify 的訊息文字 
int Max_Temp_Warning=30;                      // 設定溫度到達該值時，發送 LINE 訊息
bool Chk_Temp_Flag=false;

bool Set_switch_on=false;     // true : 執行 AP 模式(將連線資料透過網頁寫入 FlashMemory 內)
                              // false : 讀取FlashMemory的資料開始對 WiFi 基地台做連線及傳輸資料

String LocalIP;
String Show_info;

#define PCF8574_I2C 0x27
#define LEDx_B 5
#define LEDx_G 6
#define LEDx_R 7

int LedxB_Mode=0;
int LedxG_Mode=0;
int LedxR_Mode=0;

long DelayTime;  

// ------ 以下修改成你MQTT設定 ------
char mqtt_Server[] = "xxxxxxxx.xxx.xxx"; // 私人伺服器  // "mqtt.eclipseprojects.io";  免註冊MQTT伺服器
int mqtt_Port = 1885;                      //1883;                         MQTT 的埠
char subscribeTopic[] = "xxxxxxxx";          // 訂閱 開關發送的訊息
char publishTopic[]   = "xxxxxxxx";        // 發布 DHT11 目前溫溼度內容
char publish_LED_Topic[] = "xxxxxxxx";       // 發布 LED　的狀態
#define MQTT_USER       "xxxxxxxx"             // 視連接的MQTT伺服器的需要 
#define MQTT_PASSWORD   "xxxxxxxx" 
byte mac[6];
String clientId;                                // 使用網卡 MAC 的資料

// -----  NTP　------
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// -----  RTC ---
RTC rtc;
int32_t seconds;
struct tm *timeinfo;

//------------------------------------MQTT Setup---------------------------------
void mqtt_callback(char* topic, byte* payload, unsigned int msgLength);  // MQTT Callback function header
WiFiClient wifiClient;
PubSubClient mqttClient(mqtt_Server, mqtt_Port, mqtt_callback, wifiClient);

//重新連線MQTT Server
bool mqtt_nonblock_reconnect()
{
 bool doConn = false;
 
 
 if (! mqttClient.connected())
 {
  bool isConn = mqttClient.connect(clientId.c_str(), MQTT_USER, MQTT_PASSWORD);
  char logConnected[100];
  sprintf(logConnected, "MQTT Client [%s] Connect %s !", clientId.c_str(), (isConn ? "Successful" : "Failed"));
  Serial.println(logConnected);
  if (isConn)
  {
   mqttClient.subscribe(subscribeTopic);
   doConn=true;
  }
 }
 return doConn;
}

void mqtt_callback(char* topic, byte* payload, unsigned int msgLength)
{
  char temp[80];
  DynamicJsonDocument mqttTemp(100);
  sprintf(temp , "Message arrived with Topic [%s]\n  Data Length: [%d], Payload: [", topic, msgLength);
  Serial.print(temp);
  Serial.write(payload, msgLength);
  Serial.println("]");
  
  deserializeJson(mqttTemp, payload);
  LedxB_Mode = (int)mqttTemp["LED1"];
  LedxG_Mode = (int)mqttTemp["LED2"];
  LedxR_Mode = (int)mqttTemp["LED3"];
  /*Serial.print("LED1 = ");
  Serial.println(Led1_Mode);
  Serial.print("LED2 = ");
  Serial.println(Led2_Mode);
  Serial.print("LED3 = ");
  Serial.println(Led3_Mode);*/
  
  mqttTemp.clear();
  pcf.digitalWrite(LEDx_B,LedxB_Mode);
  pcf.digitalWrite(LEDx_G,LedxG_Mode);
  pcf.digitalWrite(LEDx_R,LedxR_Mode); 
  mqttTemp["LED1"] = LedxB_Mode;
  mqttTemp["LED2"] = LedxG_Mode;
  mqttTemp["LED3"] = LedxR_Mode;
  Show_info="";
  serializeJson(mqttTemp, Show_info);
  Serial.print("LED JSON Data: ");
  Serial.println(Show_info);   
  mqttClient.publish(publish_LED_Topic, Show_info.c_str());

}

void setup()
{
 byte i; 
 Webdata tmp;         //網頁設定資料的結構 GetHtml.h
 Serial.begin(115200);
 wdt.InitWatchdog(180000);  // setup 180s watchdog
 u8g2.begin();        // OLED 的初始化及字型設定
 u8g2.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
 u8g2.clearBuffer(); // display OLED, clear the internal memory
 FlashMemory.read();

 /* Setup some PCF8574 pins for LED demo */
 if (pcf.begin(PCF8574_I2C, &Wire))
 {
  pcf.pinMode(LED_B, OUTPUT);
  pcf.pinMode(LED_G, OUTPUT);
  pcf.pinMode(LED_R, OUTPUT);
 }
 else
  Serial.println("Couldn't find PCF8574");

 rtc.Init();   // RTC 的初始化

//-----------------------------------------------------------
 if (Set_switch_on)   //利用True/False (開關方式)來確認是否要執行網路連線資料的設定 
 {
  // 當需要更新連線設定時，使用下列方式 .....
  if (WiFi.status() == WL_NO_SHIELD)
  {
   Serial.println("WiFi shield not present");
   while (true);
  }
  while (status != WL_CONNECTED)
  {
   Show_info=String("AP_SSID: ") + AP_ssid; 
   Serial.println(Show_info);
   u8g2.drawStr(0,10,Show_info.c_str());
   Show_info=String("AP_PW: ") + AP_pass; 
   Serial.println(Show_info);
   u8g2.drawStr(0,20,Show_info.c_str());
   u8g2.sendBuffer(); 
   status = WiFi.apbegin(AP_ssid, AP_pass, channel, ssid_status);
   // wait 5 seconds for connection:
   delay(5000);
  }
  //AP MODE already started:
  Serial.println("AP mode already started");
  Serial.println();
  printWifiData();
  printCurrentNet();
  LocalIP = String() + WiFi.localIP()[0] + "." + WiFi.localIP()[1] + "." + WiFi.localIP()[2] + "." + WiFi.localIP()[3];
  Show_info=String ("IP:") + LocalIP;
  Serial.println(Show_info);
  u8g2.drawStr(0,30,Show_info.c_str()); 
  u8g2.sendBuffer(); 
  server.begin();
  printWifiStatus();
  while (!SendHTMLData(status,&tmp));
  server.close();
  WiFi.disconnect();
  Show_info=String("SSID: ") + tmp.ssid; 
  Serial.println(Show_info);
  u8g2.drawStr(0,40,Show_info.c_str());
  Show_info=String("PW: ") + tmp.pass; 
  Serial.println(Show_info);
  u8g2.drawStr(0,50,Show_info.c_str());
  u8g2.sendBuffer();   
  Serial.println("Setup ok");
  for (i=0;i<29;i++)
  {
   FlashMemory.buf[i] = tmp.ssid[i];
   FlashMemory.update();
   if (tmp.ssid[i]=='\0') break;
  }
  for (i=30;i<49;i++)
  {
   FlashMemory.buf[i] = tmp.pass[i-30];
   FlashMemory.update();
   if (tmp.pass[i-30]=='\0') break;
  }
  for (i=50;i<99;i++)
  {
   FlashMemory.buf[i] = tmp.token[i-50];
   FlashMemory.update();
   if (tmp.token[i-50]=='\0') break;
  }
  Show_info=String("FlashMemory is OK"); 
  Serial.println(Show_info);
  u8g2.drawStr(0,60,Show_info.c_str());   
  u8g2.sendBuffer(); 
  exit(0);
  // ========================================================
 }
 else // 若網路設置已經完成，則讀出 FlashMemory 中的資料 WiFi AP 連線
 {  
  delay (3000);
  for (i=0;i<29;i++)
  {
   nssid[i]=FlashMemory.buf[i];
   if (nssid[i]=='\0') break;
  }  
  for (i=30;i<49;i++)
  {
   npass[i-30]=FlashMemory.buf[i];
   if (npass[i-30]=='\0') break;
  }
  for (i=50;i<99;i++)
  {
   ntoken[i-50]=FlashMemory.buf[i];
   if (ntoken[i-50]=='\0') break;
  }        
  //Serial.println(nssid);
  //Serial.println(npass);
  //Serial.println(ntoken);  
  Serial.println(Show_info);
  while (status != WL_CONNECTED)
  {
   Show_info="Connection to WiFi..";  
   u8g2.drawStr(0,10,Show_info.c_str());
   Show_info=String("SSID: ") + nssid; 
   u8g2.drawStr(0,20,Show_info.c_str());
   u8g2.sendBuffer(); // transfer internal memory to the display 

   status = WiFi.begin(nssid, npass);

   // wait 0.5 seconds for connection:
   delay(500);
   u8g2.clearBuffer(); // display OLED, clear the internal memory
  }
  u8g2.clearBuffer(); 
  LocalIP = String() + WiFi.localIP()[0] + "." + WiFi.localIP()[1] +
            "." + WiFi.localIP()[2] + "." + WiFi.localIP()[3];
            
  printWifiStatus();
  timeClient.begin(); // NTP 連接
  timeClient.setTimeOffset(28800); // GMT +8 = 28800 台灣時區
  timeClient.update();
  rtc.Write(timeClient.getEpochTime()); // 
  timeClient.end();
  WiFi.macAddress(mac);
  clientId="Mac-" + String(mac[0])+ String(mac[1])+ String(mac[2])
           + String(mac[3])+ String(mac[4])+ String(mac[5]);
  Serial.println(clientId);
  delay(100);

  wdt.StartWatchdog();  // enable watchdog timer
  // ================================================================
  DelayTime=millis();
 }
}

void loop()
{
 char NowTime[100];
 String Strtemp;
 DynamicJsonDocument json_doc(200) ; 
 mqtt_nonblock_reconnect();
 mqttClient.loop();
 Show_info=String("SSID: ") + nssid;
 u8g2.drawStr(0,10,Show_info.c_str());
 Show_info=String ("IP:") + LocalIP;
 u8g2.drawStr(0,20,Show_info.c_str()); 
 u8g2.sendBuffer();  
 while ((millis()-DelayTime) > 10000)
 { 
  if (WiFi.status()==WL_CONNECTED) wdt.RefreshWatchdog(); 
  u8g2.clearBuffer();
  seconds = rtc.Read();
  timeinfo = localtime(&seconds);
  sprintf (NowTime,"%04d-%02d-%02d %02d:%02d:%02d",
          timeinfo->tm_year + 1900,
          timeinfo->tm_mon + 1,
          timeinfo->tm_mday,
          timeinfo->tm_hour,
          timeinfo->tm_min,
          timeinfo->tm_sec);   
  Show_info=String(NowTime);
  u8g2.drawStr(0,40,Show_info.c_str());
  DelayTime=millis();
  if (Get_DHT11())
  {
   Show_info="T: "+DHT_Temperature+"'c"+"    "+"H: "+DHT_Humidity+"%";
   //Serial.println(Show_info);
   u8g2.drawStr(0,30,Show_info.c_str());  // 取得溫溼度並顯示資訊
   u8g2.sendBuffer();
   
   json_doc["Temp"] = DHT_Temperature;
   json_doc["Humi"] = DHT_Humidity;
   json_doc["Time"] = (String) NowTime;
   serializeJson(json_doc, Strtemp);     // 將溫濕度及現在時間包裝成 JSON 格式
   Serial.print("DHT11 JSON Data: ");
   Serial.println(Strtemp);
   mqttClient.publish(publishTopic, Strtemp.c_str()); // 發布目前DHT11及時間狀態  

   Strtemp="";
   json_doc.clear();
   json_doc["LED1"] = LedxB_Mode;
   json_doc["LED2"] = LedxG_Mode;
   json_doc["LED3"] = LedxR_Mode;
   serializeJson(json_doc, Strtemp);    //將三色LED 的狀態包裝成 JSON 格式 
   Serial.print("LED JSON Data: ");
   Serial.println(Strtemp);   
   mqttClient.publish(publish_LED_Topic, Strtemp.c_str());  // 發布目前三色LED 的發光狀態
   
   if (DHT_Temperature.toInt()>=Max_Temp_Warning && Chk_Temp_Flag == false) // 判斷目前溫度是否大於設定的最大值
   {
    Chk_Temp_Flag=true;
    Line_Param_data+=DHT_Temperature;
    if (sendLineMsg(status,String(ntoken),Line_Param_data)) // 送出 LineNotify 的資料
     Show_info=String ("Line Send Success.");
    else
     Show_info=String ("Line Send Fail!!");
    Serial.println(Line_Param_data);
    u8g2.drawStr(0,50,Line_Param_data.c_str());    
    Serial.println(Show_info);
    u8g2.drawStr(0,60,Show_info.c_str()); 
    u8g2.sendBuffer(); 
   }
   if (DHT_Temperature.toInt()<=Max_Temp_Warning-2) Chk_Temp_Flag=false; 
  }
  else
   Serial.println("DHT11 .......");
 }
}

void printWifiData() {
    // print your WiFi shield's IP address:
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);

    // print your subnet mask:
    IPAddress subnet = WiFi.subnetMask();
    Serial.print("NetMask: ");
    Serial.println(subnet);

    // print your gateway address:
    IPAddress gateway = WiFi.gatewayIP();
    Serial.print("Gateway: ");
    Serial.println(gateway);
    Serial.println();
}

void printCurrentNet() {
    // print the SSID of the AP:
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());

    // print the MAC address of AP:
    byte bssid[6];
    WiFi.BSSID(bssid);
    Serial.print("BSSID: ");
    Serial.print(bssid[0], HEX);
    Serial.print(":");
    Serial.print(bssid[1], HEX);
    Serial.print(":");
    Serial.print(bssid[2], HEX);
    Serial.print(":");
    Serial.print(bssid[3], HEX);
    Serial.print(":");
    Serial.print(bssid[4], HEX);
    Serial.print(":");
    Serial.println(bssid[5], HEX);

    // print the encryption type:
    byte encryption = WiFi.encryptionType();
    Serial.print("Encryption Type:");
    Serial.println(encryption, HEX);
    Serial.println();
}
void printWifiStatus() {
    // print the SSID of the network you're attached to:
    Serial.println();
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());

    // print your WiFi shield's IP address:
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);

    // print the received signal strength:
    long rssi = WiFi.RSSI();
    Serial.print("signal strength (RSSI):");
    Serial.print(rssi);
    Serial.println(" dBm");
}
