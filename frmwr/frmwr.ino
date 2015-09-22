#include "SPI.h"
#include "Ctrl.h"
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#define DEBUG

const char *ssid     = "HaSi-Kein-Internet-Legacy";
const char *password = "bugsbunny";

const char *gameServer = "10.23.42.176";
const int gamePort = 1337;

WiFiClient client;
WiFiUDP udp;

void setupWifi()
{
#ifdef DEBUG
    Serial.print("Connecting to ");
    Serial.println(ssid);
#endif
    
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
#ifdef DEBUG
        Serial.print(".");
#endif
    }

#ifdef DEBUG
    Serial.println("");
    Serial.println("WiFi connected");  
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
#endif
  
    udp.begin(gamePort);
}

void sendPacket(char *packet)
{
    udp.beginPacket(gameServer, gamePort);
    udp.write(packet);
    udp.endPacket();
}

void setup()
{
    Serial.begin(115200);
    Cytrill.begin();
    
    setupWifi();
}

void loop()
{
    Cytrill.loop();
}
