// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain

#include "DHT.h"

float h=0;
float t=0;
float f=0;

#define DHTPIN 8
#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE);

void setup() {
    Serial.begin(115200);
    Serial.println("DHTxx test!");
    dht.begin();
}

void loop() {
    delay(2000);

    h = dht.readHumidity();
    t = dht.readTemperature();
    f = dht.readTemperature(true);
    
    if (isnan(h) || isnan(t) || isnan(f)) {
        Serial.println("Failed to read from DHT sensor!");
    }
    else{
        Serial.print("Humidity: ");
        Serial.print(h);
        Serial.print("% ,Temperature: ");
        Serial.print(t);
        Serial.print("°C ");
        Serial.print(f);
        Serial.println("°F");
    }
}
