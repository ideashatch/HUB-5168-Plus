#include <WiFi.h>
#include <WiFiUdp.h>
#include <PubSubClient.h>
#include "DHT.h"

char *ssid = "以遮蔽";
char *password = "以遮蔽";

const char *broadcast_ip = "255.255.255.255"; 
const int broadcast_port = 12345;             

char mqtt_server[16];
const int mqtt_port = 1883; 

float h = 0;
float t = 0;
float f = 0;

#define DHTPIN 8
#define DHTTYPE DHT11 // DHT 11
DHT dht(DHTPIN, DHTTYPE);

WiFiClient espClient;
PubSubClient client(espClient);

unsigned int count = 0;
unsigned int WARNING_count = 0;
const unsigned int gas_gate=180;

void setup_wifi()
{
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi連線成功");
}

void setup_mqtt()
{
  client.setServer(mqtt_server, mqtt_port);

  while (!client.connected())
  {
    if (client.connect("5168TG"))
    {
      Serial.println("MQTT連線成功");
    }
    else
    {
      Serial.print("MQTT連線失敗, rc=");
      Serial.print(client.state());
      Serial.println(" reconnect...");
      delay(2000);
    }
  }
}

void reconnect()
{
  while (!client.connected())
  {
    Serial.print("嘗試重新連線至MQTT伺服器...");
    if (client.connect("5168TG"))
    {
      Serial.println("MQTT連線成功");
    }
    else
    {
      Serial.print("MQTT連線失敗, rc=");
      Serial.print(client.state());
      Serial.println(" reconnect...");
      delay(2000);
    }
  }
}

int readSensor()
{
  unsigned int sensorValue = analogRead(6);                     // Read the analog value from sensor
  unsigned int outputValue = map(sensorValue, 0, 1023, 0, 255); // map the 10-bit data to 8-bit data
  // Serial.print("gas =");
  // Serial.print(sensorValue);
  // Serial.print(" ");
  // Serial.println(outputValue);

  return outputValue; // Return analog moisture value
}

char packetBuffer[255]; // buffer to hold incoming packet
void setup()
{
  Serial.begin(115200);
  setup_wifi();
  WiFiUDP udp;
  while (true)
  {
    udp.begin(broadcast_port);
    udp.beginPacket(broadcast_ip, broadcast_port);
    udp.print("RASPBERRY_PI_IP_REQUEST");
    udp.endPacket();
    Serial.println("等待回復...");
    while (udp.parsePacket() == 0)
    {
    }

    String response = "";
    int len = udp.read(packetBuffer, 255);
    if (len > 0)
    {
      packetBuffer[len] = 0;
    }
    Serial.println("Contents:");
    Serial.println(packetBuffer);
    response = packetBuffer;

    if (response.startsWith("RASPBERRY_PI_IP:"))
    {
      String ip = response.substring(16);
      Serial.println("找到Raspberry Pi的IP地址：" + ip);
      ip.toCharArray(mqtt_server, 16);

      setup_mqtt();
      break;
    }
    else
    {
      Serial.println("未收到正確回復 recive:");
      // print the received response
      Serial.println(response);
      const char *responseStr = response.c_str();
      Serial.println(responseStr);
      delay(1000);
      continue;
    }
  }
  pinMode(5, INPUT);
  dht.begin();
  count = 0;
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  if (count > 60)
    count = 0;
  else
    count += 1;
  if (count % 5 == 0)
  {
    int sensor3 = readSensor();
    if (sensor3 > gas_gate)
    {
      if (WARNING_count >= 5)
      {
        // send sensor3 data to mqtt
        char sensor3_str[10];
        sprintf(sensor3_str, "%d", sensor3);
        client.publish("home/sensor3", sensor3_str);
        Serial.print("gas warning");
        Serial.println(sensor3);
        WARNING_count = 0;
      }
      else
        WARNING_count += 1;
    }
    else
      WARNING_count = 6;
  }
  if (count % 10 == 0)
  {
    h = dht.readHumidity();t = dht.readTemperature();f = dht.readTemperature(true);
    if (isnan(h) || isnan(t) || isnan(f))
    {
      Serial.println("Failed to read from DHT sensor!");
    }
    else
    {
      Serial.print("Humidity: ");
      Serial.print(h);
      Serial.print("% ,Temperature: ");
      Serial.print(t);
      Serial.print("°C ");
      Serial.print(f);
      Serial.println("°F");
      int a = static_cast<int>((t - static_cast<int>(t)) * 10000);
      int b = static_cast<int>((h - static_cast<int>(h)) * 10000);
      char temp[10];
      char humi[10];
      sprintf(temp, "%d.%d", static_cast<int>(t), a);
      sprintf(humi, "%d.%d", static_cast<int>(h), b);
      client.publish("home/sensor1", temp);
      client.publish("home/sensor2", humi);
    }
  }
  delay(100);
}