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

const int trigger_pin = 12;
const int echo_pin = 11;
const int PIRSensor = 2;

int count = 0;
int warning_count=0;
int warning_temp=0;


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

int distance()
{
  float duration, distance;

  // trigger a 10us HIGH pulse at trigger pin
  digitalWrite(trigger_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger_pin, LOW);

  // measure time cost of pulse HIGH at echo pin
  duration = pulseIn(echo_pin, HIGH);

  // calculate the distance from duration
  distance = duration / 58;

  return distance;
}

void setup_mqtt()
{
  client.setServer(mqtt_server, mqtt_port);

  while (!client.connected())
  {
    if (client.connect("5168PIR"))
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
    if (client.connect("5168PIR"))
    {
      Serial.println("WiFi連線成功");
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
  pinMode(trigger_pin, OUTPUT);
  pinMode(echo_pin, INPUT);
  pinMode(PIRSensor, INPUT); // 從感應器讀入訊息
  count = 0;
}

void send_dis(int dis){
  char dis_C[16]; // Make sure there is enough space for the converted string
  itoa(dis, dis_C, 10);
  client.publish("home/sensor4", dis_C);
  Serial.println("send");
  Serial.println(dis);
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  if (count >= 20)
    count = 0;
  else
    count += 1;

  if (count % 5==0)
  {
    bool close = digitalRead(PIRSensor);
    int dis = distance();
    int now_temp=0;
    if (close)
    {
      if (warning_count>2){
        Serial.println("Detect");
        if (dis <30){
          now_temp=3;
        }else if (dis <150){
          now_temp=2;
        }else{
          now_temp=1;
        }
        if (now_temp!=warning_temp){
          warning_temp=now_temp;
          send_dis(dis);
        }
      }else{
        warning_count++;
      }
      
    }else{
      warning_count=0;
      warning_temp=0;
    }
  }
  delay(100);
}