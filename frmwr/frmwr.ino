#include "SPI.h"
#include "Ctrl.h"
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#define CMD_KEEP_ALIVE      0x10
#define CMD_BUTTONS_CHANGED 0x11

#define CMD_SET_LED_0       0x20
#define CMD_SET_LED_1       0x21

#define MSG_SIZE            6

#define KEEP_ALIVE_INTERVAL 2000

#define DEBUG
#define BTN_DEBUG

const char *ssid     = "HaSi-Kein-Internet-Legacy";
const char *password = "bugsbunny";

const char *gameServer = "10.23.42.168";
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
}

void sendMessage(char message[])
{
#ifdef DEBUG
    Serial.print("Sending data: ");
    Serial.write(message, MSG_SIZE);
    Serial.println();
#endif

    udp.beginPacket(gameServer, gamePort);
    udp.write(message, MSG_SIZE);
    udp.endPacket();
}

void sendButtonsChanged(int newButtonStates)
{
    char message[] = {
        CMD_BUTTONS_CHANGED,
        char(newButtonStates & 0xFF),
        char((newButtonStates ^ buttonStates) & 0xFF),
        0x00,
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
        0x00,
        CMD_KEEP_ALIVE
    };

    sendMessage(message);
}

bool receiveMessage(char *buffer)
{
    int size = udp.parsePacket();

    if (size == MSG_SIZE)
    {
        udp.read(buffer, MSG_SIZE);

       return true;
    }
    
    return false;
}

void setup()
{
#ifdef DEBUG
    Serial.begin(115200);
#endif

    Cytrill.begin();
    
    setupWifi();
    udp.begin(gamePort);

    Cytrill.setLed(LED_0, 0x00, 0x00, 0x00, 31);
    Cytrill.setLed(LED_1, 0x00, 0x00, 0x00, 31);
}

void loop()
{
    static int keepAliveCounter = 0;

    char msgBuffer[MSG_SIZE] = { 0x00, };
    int newButtonStates = Cytrill.getButtons();

    if (receiveMessage(msgBuffer))
    {
        // Some sanity check
        if (msgBuffer[0] == msgBuffer[MSG_SIZE - 1])
        {
            switch (msgBuffer[0])
            {
                case CMD_SET_LED_0:
                    Cytrill.setLed(LED_0, msgBuffer[1], msgBuffer[2], msgBuffer[3], msgBuffer[4]);
                    break;

                case CMD_SET_LED_1:
                    Cytrill.setLed(LED_1, msgBuffer[1], msgBuffer[2], msgBuffer[3], msgBuffer[4]);
                    break;
            }
        }
    }

    if (newButtonStates != buttonStates)
    {
        sendButtonsChanged(newButtonStates);

        buttonStates = newButtonStates;
    }

    if (keepAliveCounter == KEEP_ALIVE_INTERVAL)
    {
        sendKeepAlive();
        keepAliveCounter = 0;
    }
    else
    {
        keepAliveCounter++;
    }

    if (WiFi.status() != WL_CONNECTED)
    {
        setupWifi();
    }

    // Necessary for some reason
    delay(3);

    Cytrill.loop();
}

