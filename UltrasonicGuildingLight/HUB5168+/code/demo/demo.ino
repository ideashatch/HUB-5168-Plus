#include "WS2812B.h"

#define EspPIN 10                 // 接收esp32訊號腳位
#define BtnPIN 6                  // 按鈕腳位，用來控制燈條顯示模式
#define TRIG_PIN 7                // 超音波TRIG
#define ECHO_PIN 8                // 超音波ECHO

#define LED_PIN 12                // LED燈條腳位
#define NUM_LEDS 60               // 燈珠個數

const int numReadings = 8;        // 時間窗口大小
int readings[numReadings];        // stac，用來存储一個距離的數組
int index = 0;                    // 上述數组之索引
int distance = 0;                 // 距離資料
int openState = 0;                // esp輸入狀態
int ledState = 0;                 // led燈顯示模式
int btnState = HIGH;              // 按鈕狀態
int lastbtnState = HIGH;          // 防彈跳用狀態
int debounceDelay = 10;           // 防彈跳delay
unsigned long lastDebounceTime;   // 上次防彈跳時間
WS2812B led(LED_PIN,NUM_LEDS);    // 實例化LED燈條

// SC-HR04 function
long readUltrasonicDistance(int triggerPin, int echoPin){
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(5);
  // Sets the trigger pin to HIGH state for 10 microseconds
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  // Reads the echo pin, and returns the sound wave travel time in microseconds
  return pulseIn(echoPin, HIGH); // 收到高電位時的時間
}

// 燈帶遞進
void colorWipe(uint8_t rColor, uint8_t gColor, uint8_t bColor, uint8_t wait) {
      //Serial.println(btnState); 
      if(btnState == LOW){
        return;
      }else{
        for (uint16_t i = 0; i < NUM_LEDS; i++) {
          led.setPixelColor(i, rColor, gColor, bColor);
          led.show();
          delay(wait);
        }
      }   

}
// 彩虹
void rainbow(uint8_t wait) {
  for(long firstPixelHue = 0; firstPixelHue < 65536; firstPixelHue += 256) {
    led.rainbow(firstPixelHue);
    led.show();
    delay(wait);
  }
}
// 距離燈，最遠大約 1M
void distancewipe(uint8_t rColor, uint8_t gColor, uint8_t bColor,uint16_t distance){
  led.clear();
  distance = distance>100 ? 100 : distance;
  int lightleds = (distance*NUM_LEDS/100)+8;
  lightleds = lightleds>NUM_LEDS ? NUM_LEDS : lightleds;
  // Serial.println(String(distance)+"  "+String(lightleds));
  led.fill(rColor,gColor,bColor,0,lightleds);
  led.show();
}

//資料平滑處，必免不必要的突波訊號
int smooth(int inputValue) {
  // 移除舊值，添加新值
  readings[index] = inputValue;

  // 更新索引
  index++;
  if (index >= numReadings) {
    index = 0;
  }
  // 計算平均值
  int total = 0;
  for (int i = 0; i < numReadings; i++) {
    total += readings[i];
  }
  int average = total / numReadings;
  //回傳平均值
  return average;
}

// led模式選擇
void modeSelect(int mode){
  switch (mode) {
      case 0:
        distance = smooth(0.01723 * readUltrasonicDistance(TRIG_PIN, ECHO_PIN));
        distancewipe(255/5,255/5,255/5,int(distance));
        break;
      case 1:
        distance = smooth(0.01723 * readUltrasonicDistance(TRIG_PIN, ECHO_PIN));
        distancewipe(255/5,125/5,30/5,int(distance));
        break;
      case 2:
        led.clear();
        colorWipe(240/5, 235/5, 213/5, 30);
        break;
      case 3:
        led.fill(255/5,125/5,30/5,0,NUM_LEDS);
        led.show();
        break;
      case 4:
        led.fill(255/5,255/5,255/5,0,NUM_LEDS);
        led.show();
        break;
      case 5:
        led.clear();
        led.show();
        break;
    }

}

void setup(){
  pinMode(EspPIN, INPUT);
  pinMode(BtnPIN, INPUT_PULLUP);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  Serial.begin(115200);
  led.begin();
  Serial.println("ok");
}

void loop(){
  openState = digitalRead(EspPIN);
  if (openState == HIGH)
  {
    // 防彈跳
    int reading = digitalRead(BtnPIN);
    if (reading != lastbtnState) {
        lastDebounceTime = millis();  // 紀錄按鈕改變狀態的時間
    }
    if ((millis() - lastDebounceTime) > debounceDelay) {
      if (reading != btnState) {
        btnState = reading;
        if (btnState == LOW) {
          //Serial.println("按钮按下");
        } else {
          ledState++;
          ledState = ledState % 6;
          Serial.println(String(ledState));
        }
      }
    }
    lastbtnState = reading;  // 更新上一次按钮狀態
    modeSelect(ledState);

  }
  else
  {
    //Serial.println("low");
    led.clear();
    led.show();
  }
  delay(100);
}
