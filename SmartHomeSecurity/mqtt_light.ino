#include <WiFi.h>
#include <WiFiUdp.h>
#include <PubSubClient.h>

char *ssid = "以遮蔽";
char *password = "以遮蔽";

const char *broadcast_ip = "255.255.255.255"; 
const int broadcast_port = 12345;             

char mqtt_server[16];
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

const unsigned int light1 = 2;
const unsigned int light2 = 3;

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

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("收到指令 [");
  Serial.print(topic);
  Serial.print("] ");
  String payloadStr = "";
  for (unsigned int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
    payloadStr += (char)payload[i];
  }
  payloadStr.trim();
  Serial.println();
  Serial.println(payloadStr);
  payloadStr.toLowerCase();
  if (strcmp(topic, "home/light1") == 0)
  {
    Serial.println("收到指令 home/light1");
    // if paylod=="ON" or "OFF" then do something
    if (payloadStr == "on")
    {
      digitalWrite(light1, HIGH);
      Serial.println("開燈");
    }
    else if (payloadStr == "off")
    {
      digitalWrite(light1, LOW);
      Serial.println("關燈");
    }
  }
  else if (strcmp(topic, "home/light2") == 0)
  {
    Serial.println("收到指令 home/light2");
    // if paylod=="ON" or "OFF" then do something
    if (payloadStr == "on")
    {
      digitalWrite(light2, LOW);
      Serial.println("開燈");
    }
    else if (payloadStr == "off")
    {
      digitalWrite(light2, HIGH);
      Serial.println("關燈");
    }
  }
}

void setup_mqtt()
{
  client.setServer(mqtt_server, mqtt_port);

  while (!client.connected())
  {
    if (client.connect("5168LS"))
    {
      Serial.println("MQTT連線成功");
      client.subscribe("home/light1");
      client.subscribe("home/light2");
      client.setCallback(callback);
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
    if (client.connect("5168LS"))
    {
      Serial.println("MQTT連線成功");
      client.subscribe("home/light1");
      client.subscribe("home/light2");
      client.setCallback(callback);
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
    while (udp.parsePacket() == 0){}
    
    String response = "";
    int len = udp.read(packetBuffer, 255);
    if (len > 0) packetBuffer[len] = 0;
    Serial.println("Contents:");
    Serial.println(packetBuffer);
    response = packetBuffer;
    if (response.startsWith("RASPBERRY_PI_IP:")){
      String ip = response.substring(16);
      Serial.println("找到Raspberry Pi的IP地址：" + ip);
      ip.toCharArray(mqtt_server, 16);

      setup_mqtt();
      break;
    }else{
      Serial.println("未收到正確回復 recive:");
      // print the received response
      Serial.println(response);
      const char *responseStr = response.c_str();
      Serial.println(responseStr);
      delay(1000);
      continue;
    }
  }
  pinMode(light1, OUTPUT);
  pinMode(light2, OUTPUT);
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
}