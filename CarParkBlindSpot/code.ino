#include <HttpClient.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include "DHT.h"

//#define DHTPIN 8
//#define DHTTYPE DHT11   // DHT 11
//DHT dht(DHTPIN, DHTTYPE);
// tone 12 3
const int trigPin = 7;                  //Trig Pin
const int echoPin = 8;                  //Echo Pin
const int LED1_Pin = 10;
const int LED2_Pin = 11;
const int LED3_Pin = 9;
const int LED4_Pin = 6;
int distance = 0;
int cnt=0;
const int people_out=2;

char ssid[] = "D-Link_DIR-612"; // your network SSID (name)
char pass[] = "0986067099";     // your network password (use for WPA, or use as key for WEP)

// Name of the server we want to connect to
const char kHostname[] = "maker.ifttt.com";

String kBuffer = "/trigger/email_test/with/key/bpWMUhUdaVFz3Yk-0NGUCA?";
String kBuffer1 = "/trigger/people/with/key/bpWMUhUdaVFz3Yk-0NGUCA?";
const int kNetworkTimeout = 2 * 1000;
const int kNetworkDelay = 1000;
int status = WL_IDLE_STATUS;

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
    Serial.begin(115200);
    //dht.begin();
    pinMode(trigPin, OUTPUT); 
    pinMode(echoPin, INPUT);
    pinMode(LED1_Pin, OUTPUT);
    pinMode(LED2_Pin, OUTPUT);
    pinMode(LED3_Pin, OUTPUT);
    pinMode(LED4_Pin, OUTPUT);
    pinMode(people_out, INPUT);
    
    while (status != WL_CONNECTED) {
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(ssid);
        status = WiFi.begin(ssid, pass);
        // wait 10 seconds for connection:
        delay(10000);
    }
    Serial.println("Connected to wifi");
    printWifiStatus();
    
}

void loop() {
    /*float h = 2.2;
    float t = 2.3;
    */
    /*float h = dht.readHumidity();
    float t = dht.readTemperature();
    // Check if any reads failed and exit early (to try again).
    if (isnan(h)|| isnan(t) ) {
        Serial.println(F("Failed to read from DHT sensor!"));
        return;
    }*/
    if(digitalRead(people_out)){
      digitalWrite(LED4_Pin,HIGH);
       distance = 0.01723 * readUltrasonicDistance(trigPin, echoPin);
     if(distance < 10){
      digitalWrite(LED1_Pin,HIGH);
      digitalWrite(LED2_Pin,HIGH);
      digitalWrite(LED3_Pin,HIGH);
        tone(12,900,1000);         
        tone(3,900,1000);
        delay(1000);
        noTone(12);
        noTone(3);
      }
      else if(distance < 20){
        digitalWrite(LED1_Pin,HIGH);
        digitalWrite(LED2_Pin,HIGH);
        digitalWrite(LED3_Pin,LOW);
        tone(12,400,250);         
        tone(3,400,250);
        delay(250);
        noTone(12);
        noTone(3);
      }
      else if(distance < 30){
        digitalWrite(LED1_Pin,HIGH);
        digitalWrite(LED2_Pin,LOW);
        digitalWrite(LED3_Pin,LOW);
      }
      else if(distance < 300){
        digitalWrite(LED1_Pin,LOW);
        digitalWrite(LED2_Pin,LOW);
        digitalWrite(LED3_Pin,LOW);      
  
        }
       // tone(12,560,2500);
      
      Serial.print("Distance : "); 
      Serial.print(distance);
      Serial.println("cm");
      delay(50);
      Transmit_data(distance,cnt,kBuffer);
    }
    else {
      digitalWrite(LED1_Pin,LOW);
      digitalWrite(LED2_Pin,LOW);
      digitalWrite(LED3_Pin,LOW);  
      digitalWrite(LED4_Pin,LOW);
      Transmit_data1(kBuffer1);
    }
    delay(1000);
    cnt++;
}

void Transmit_data(float h ,float t ,String API_Path){
    int err = 0;
    char kPath[128];
    String url_s =API_Path + "value1=" + (int)t + "number&value2=" + (int)h + "cm";
    url_s.toCharArray(kPath,url_s.length() + 1);
    WiFiClient c;
    HttpClient http(c);
    err = http.get(kHostname, kPath);
    if (err == 0) {
        Serial.println("startedRequest ok");
        err = http.responseStatusCode();
        if (err >= 0) {
            Serial.print("Got status code: ");
            Serial.println(err);
            err = http.skipResponseHeaders();
            if (err >= 0) {
                int bodyLen = http.contentLength();
                Serial.print("Content length is: ");
                Serial.println(bodyLen);
                Serial.println();
                Serial.println("Body returned follows:");
                unsigned long timeoutStart = millis();
                char c;
                while ((http.connected() || http.available()) && ((millis() - timeoutStart) < kNetworkTimeout)) {
                    if (http.available()) {
                        c = http.read();
                        Serial.print(c);
                        bodyLen--;
                        timeoutStart = millis();
                    } else {
                        delay(kNetworkDelay);
                    }
                }
            }
        }
    }
    http.stop();
}
void Transmit_data1(String API_Path){
    int err = 0;
    char kPath[128];
    String url_s =API_Path ;
    url_s.toCharArray(kPath,url_s.length() + 1);
    WiFiClient c;
    HttpClient http(c);
    err = http.get(kHostname, kPath);
    if (err == 0) {
        Serial.println("startedRequest ok");
        err = http.responseStatusCode();
        if (err >= 0) {
            Serial.print("Got status code: ");
            Serial.println(err);
            err = http.skipResponseHeaders();
            if (err >= 0) {
                int bodyLen = http.contentLength();
                Serial.print("Content length is: ");
                Serial.println(bodyLen);
                Serial.println();
                Serial.println("Body returned follows:");
                unsigned long timeoutStart = millis();
                char c;
                while ((http.connected() || http.available()) && ((millis() - timeoutStart) < kNetworkTimeout)) {
                    if (http.available()) {
                        c = http.read();
                        Serial.print(c);
                        bodyLen--;
                        timeoutStart = millis();
                    } else {
                        delay(kNetworkDelay);
                    }
                }
            }
        }
    }
    http.stop();
}

void printWifiStatus() {
    // print the SSID of the network you're attached to:
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
