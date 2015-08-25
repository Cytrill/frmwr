#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const char* ssid     = "HaSi-Kein-Internet-Legacy";
const char* password = "bugsbunny";

const char* host = "10.23.42.176";
const int port = 1337;

WiFiClient client;
WiFiUDP udp;

char packetBuffer[255];
IPAddress serverIP(10, 23, 42, 176);

void setup() {
  Serial.begin(115200);
  delay(10);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  udp.begin(port);
}

void sendPacket(char* packet)
{
  udp.beginPacket(serverIP, port);
  udp.write(packet);
  udp.endPacket();
}

void loop() {
  delay(5000);

  char cmd[] = { 0xFF, 0x80 };

  for (int i = 0; i < 8; i++)
  {
    sendPacket(cmd);
    cmd[1] >>= 1;
    delay(1000);
  }
}
