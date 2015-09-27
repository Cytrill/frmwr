#include "SPI.h"
#include "Ctrl.h"
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Ticker.h>

#define CMD_KEEP_ALIVE      0x10
#define CMD_BUTTONS_CHANGED 0x11

#define CMD_SET_LED_0       0x20
#define CMD_SET_LED_1       0x21
#define CMD_SET_LED_2       0x22
#define CMD_SET_LED_3       0x23

#define KEEP_ALIVE_INTERVAL 5.0

#define DEBUG

const char *ssid     = "HaSi-Kein-Internet-Legacy";
const char *password = "bugsbunny";

const char *gameServer = "10.23.42.176";
const int gamePort = 1337;

WiFiClient client;
WiFiUDP udp;

int buttonStates = 0x00;

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

void sendMessage(char message[])
{
#ifdef DEBUG
    Serial.print("Sending data: ");
    Serial.write(message, 5);
    Serial.println();
#endif

    udp.beginPacket(gameServer, gamePort);
    udp.write(message, 5);
    udp.endPacket();
}

void sendButtonsChanged(int newButtonStates)
{
    char message[] = {
        CMD_BUTTONS_CHANGED,
        char(newButtonStates & 0xFF),
        char((newButtonStates ^ buttonStates) & 0xFF),
        0x00,
        CMD_BUTTONS_CHANGED
    };

    sendMessage(message);
}

void sendKeepAlive()
{
    char message[] = {
        CMD_KEEP_ALIVE,
        char(buttonStates & 0xFF),
        0x00,
        0x00,
        CMD_KEEP_ALIVE
    };

    sendMessage(message);
}

void setup()
{
#ifdef DEBUG
    Serial.begin(115200);
#endif

    Cytrill.begin();
    
    setupWifi();
}

void loop()
{
    static int keepAliveCounter = 0;
    
    int newButtonStates = Cytrill.getButtons();

    if (newButtonStates != buttonStates)
    {
        sendButtonsChanged(newButtonStates);

        buttonStates = newButtonStates;
    }

    if (keepAliveCounter == 2000)
    {
        sendKeepAlive();
        keepAliveCounter = 0;

        Serial.print(Cytrill.getButtons(), BIN);
        Serial.println();

        Serial.println("Sending Keep alive");
    }
    else
    {
        keepAliveCounter++;
    }

    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("Lost wifi connection");
    }

    delay(3);

    Cytrill.loop();
}

