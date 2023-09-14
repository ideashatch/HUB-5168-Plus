#include <WiFi.h>

char ssid[] = "ssid";    // your network SSID (name)
char pass[] = "pass";       // your network password
int keyIndex = 0;               // your network key Index number (needed only for WEP)

//State the analog pin that you want to read
int analogChannel = A2;

int status = WL_IDLE_STATUS;

WiFiServer server(80);
void setup() {
    Serial.begin(115200);
    if (WiFi.status() == WL_NO_SHIELD) {
        Serial.println("WiFi shield not present");
        while (true);
    }
    while (status != WL_CONNECTED) {
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(ssid);
        status = WiFi.begin(ssid, pass);
        delay(10000);
    }
    server.begin();
    printWifiStatus();
}

void check_client(){
    WiFiClient client = server.available();
    if (client) {
        Serial.println("new client");
        boolean currentLineIsBlank = true;
        while (client.connected()) {
            if (client.available()) {
                char c = client.read();
                Serial.write(c);
                if (c == '\n' && currentLineIsBlank) {
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type: text/html");
                    client.println("Connection: close");  // the connection will be closed after completion of the response
                    client.println("Refresh: 5");  // refresh the page automatically every 5 sec
                    client.println();
                    client.println("<!DOCTYPE HTML>");
                    client.println("<html>");
                    client.print("analog input ");
                    client.print(analogChannel);
                    client.print(" is ");
                    client.print(analogRead(analogChannel));
                    client.println("<br />");
                    client.println("</html>");
                    break;
                }
                if (c == '\n') {
                  currentLineIsBlank = true;
                } else if (c != '\r') {
                  currentLineIsBlank = false;
                }
            }
        }
        delay(1);
        client.stop();
        Serial.println("client disonnected");
    }
}

void loop() {
    check_client();
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
