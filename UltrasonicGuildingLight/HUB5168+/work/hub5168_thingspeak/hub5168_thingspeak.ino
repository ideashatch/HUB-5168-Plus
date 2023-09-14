#include <HttpClient.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include "DHT.h"

char ssid[] = "ssid"; // your network SSID (name)
char pass[] = "pass";     // your network password (use for WPA, or use as key for WEP)

#define DHT_Pin 8
#define DHT_TYPE DHT11
DHT dht(DHT_Pin, DHT_TYPE);

// Name of the server we want to connect to
const char kHostname[] = "api.thingspeak.com";

String kPath = "/update?api_key=SLIE37KLQ57CJAA4";
const int kNetworkTimeout = 30 * 1000;
const int kNetworkDelay = 1000;
int status = WL_IDLE_STATUS;

void setup() {
    Serial.begin(115200);
    Serial.println(F("DHTxx test!"));
    dht.begin();
    
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
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)) {
        Serial.println(F("Failed to read from DHT sensor!"));
        return;
    }
    Transmit_data(h,t,kPath);
    delay(2000);
}

void Transmit_data(float h ,float t ,String API_Path){
    int err = 0;
    WiFiClient c;
    HttpClient http(c);
    String url_s =API_Path + "&field1=" + (int)t + "&field2=" + (int)h;
    char url[64];
    url_s.toCharArray(url,url_s.length() + 1);
    err = http.get(kHostname, url);
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
