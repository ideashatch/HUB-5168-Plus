/*
  MQTT Authentication example
  - connects to an MQTT server, providing username and password
  - publishes "hello world" to the topic "outTopic"
  - subscribes to the topic "inTopic"

  Example guide:
  https://www.amebaiot.com/en/amebad-arduino-mqtt-upload-listen/
*/

//匯入函式庫
#include <WiFi.h>
#include <PubSubClient.h>
#include "WS2812B.h"

//定義開發板的SPI_MOSI腳位
#if defined(BOARD_RTL8722DM)
#define SPI_MOSI_PIN        11
#elif defined(BOARD_RTL8722DM_MINI)
#define SPI_MOSI_PIN       9
#elif defined(BOARD_RTL8720DN_BW16)
#define SPI_MOSI_PIN       PA12
#endif

#define NUM_OF_LEDS 8    //宣告燈珠數量

//定義音符頻率，目的是給蜂鳴器能產生相應的聲音
#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_B4  494
#define NOTE_C5  523

#define BUZZER_PIN  11   //宣告蜂鳴器腳位
#define led_PIN     3    //宣告wifi狀態燈腳位

//創建NeoPixel對象，目的是控制連接WS2813 or WS2812LED
WS2812B led(SPI_MOSI_PIN, NUM_OF_LEDS);

char ssid[] = "hel";                 // 網路名稱
char pass[] = "1234567890";          // 網路密碼
int status = WL_IDLE_STATUS;         // Wifi 狀態指示器

char mqttServer[]     = "120.102.36.38";  //MQTT伺服器IP
char subscribeTopic[] = "QAQ";            //MQTT訂閱主題

bool isPlaying = false;              //用於追蹤音樂播放狀態

//訂閱主題資料接收函式
void callback(char* topic, byte* payload, unsigned int length) {
  //顯示是哪個主題傳資料
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)(payload[i]));
  }
  Serial.println();
  //將資料轉為整數狀態
  int command = (char)payload[0] - '0';
  //command = 0，就關閉全部燈珠和蜂鳴器
  if (command == 0) {
    digitalWrite(BUZZER_PIN, LOW);
    led.fill(0, 0, 0, 1, 2);         //設定R、G、B、從第幾顆、到第幾顆
    led.show();
    Serial.println(" Turn Off BUZZER and LED! ");
    isPlaying = false; // 停止播放
  } 
  else if (command >= 1 && command <= NUM_OF_LEDS) {
    //宣告音符陣列與播放音符間隔時間 
    int melody[] = { NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_A4, NOTE_B4, NOTE_C5 };
    int noteDuration = 200; // 单位：毫秒
    tone(BUZZER_PIN, melody[command - 1]); // 播放音符
    
    for (int i = 0; i < NUM_OF_LEDS; i++) {   
      if (i == command - 1) {              //針對接收的命令，亮對應燈珠，其餘不亮          
        if (command == 1)
          // 设置粉色
          led.setPixelColor(i, 255, 0, 204);
        else if (command == 2)
           // 设置紫色
          led.setPixelColor(i, 155, 0, 200);
        else if (command == 3)
          // 设置深藍色
          led.setPixelColor(i, 0, 0, 153);
        else if (command == 4)
          // 设置淺藍色
          led.setPixelColor(i, 0, 153, 255);
        else if (command == 5)
          // 设置綠色
          led.setPixelColor(i, 0, 255, 0);
        else if (command == 6)
          // 设置黃色
          led.setPixelColor(i, 255, 204, 0);
        else if (command == 7)
          // 设置橘色
          led.setPixelColor(i, 255, 21, 0);
        else if (command == 8)
          // 设置紅色
          led.setPixelColor(i, 255, 0, 0);
      } else {
         // 其他 LED 关闭
        led.setPixelColor(i, 0, 0, 0);
      }
    }
    led.show();
    delay(noteDuration); // 播放持续时间
    noTone(BUZZER_PIN); // 停止播放音符
    Serial.print(" Playing Note: ");
    Serial.println(command);
  }
}

WiFiClient wifiClient;
PubSubClient client(mqttServer, 5007, callback, wifiClient);

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ;
  }
  //宣告wifi狀態燈輸出，並關閉
  pinMode(led_PIN, OUTPUT);
  digitalWrite(led_PIN, LOW);
  //連線網路
  while (status != WL_CONNECTED) {
    Serial.print("\r\nAttempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  //開啟wifi狀態燈
  digitalWrite(led_PIN, HIGH);
  //宣告蜂鳴器輸出
  pinMode(BUZZER_PIN, OUTPUT);
  //宣告燈條傳輸開啟
  led.begin();
}

void loop() {
  if (!client.connected()) {
    // 如果客戶端未連線
    reconnect();
    // 重新連線
  }
  client.loop();
  // MQTT 客戶端主循環，用於維持連線並處理接收的訊息
}

void reconnect() {
  // 連線函數
  while (!client.connected()) {
    // 當未連線時執行迴圈
    Serial.print("Attempting MQTT connection...");
    // 嘗試連線至 MQTT 伺服器
    String clientId = "ESP8266Client-";
    // 建立客戶端識別碼
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      // 如果成功連線
      Serial.println("connected");
      // 顯示連線成功
      client.subscribe("QAQ");
      // 訂閱特定主題
    } else {
      // 如果連線失敗
      Serial.print("failed, rc=");
      // 顯示失敗及錯誤碼
      Serial.print(client.state());
      // 顯示 MQTT 客戶端狀態
      Serial.println(" try again in 5 seconds");
      // 顯示嘗試重新連線訊息
      delay(6000);
      // 等待 6 秒再重新嘗試
    }
  }
}
