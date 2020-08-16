//#define DEBUG_CYTRILL
//#define BTN_DEBUG

#ifdef ARDUINO_ESP8266_NODEMCU
  #include <ESP8266WiFi.h>
  #include <ESP8266mDNS.h>
  #include "Ctrl.h"
#elif defined ARDUINO_ESP32_DEV
  #include <WiFi.h>
  #include <ESPmDNS.h>
  #include "Ctrl32.h"
  #include "BlueHID.hpp"
#endif
#include <WiFiUdp.h>

#include <ArduinoOTA.h>
#include "Simon.hpp"
#include "Configuration.hpp"
#include "Controller.hpp"

#define HOSTNAME            "cytrill-5"
#define OTA_PASSWORD        "bugsbunny"

#define CONFIG_MODE         0
#define SIMON_MODE          1
#define CONTROLLER_MODE     2
#define BLUETOOTH_MODE      3

//#define DEBUG_CYTRILL
//#define BTN_DEBUG

int runMode = CONTROLLER_MODE;

// Simon mode
Simon simon;

// Configuration mode
Configuration configuration;

// Controller mode
Controller controller;

#ifdef ARDUINO_ESP32_DEV
    BlueHID bluetoothHID;
#endif

void setupWifi()
{
#ifdef DEBUG_CYTRILL
    Serial.print("Connecting to ");
    Serial.println(configuration.getEssid());
#endif

    #ifdef ARDUINO_ESP8266_NODEMCU
      WiFi.hostname(HOSTNAME);
    #elif defined ARDUINO_ESP32_DEV
      WiFi.setHostname(HOSTNAME);
    #endif
    WiFi.begin(configuration.getEssid(), configuration.getPassword());

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);

#ifdef DEBUG_CYTRILL
        Serial.print(".");
#endif
    }

#ifdef DEBUG_CYTRILL
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
#endif
}

void setup()
{
    Serial.begin(115200);

    Cytrill.begin();

    Cytrill.setLedLeft(0x00, 0x00, 0x00, 31);
    Cytrill.setLedRight(0x00, 0x00, 0x00, 31);

    delay(100);

    Cytrill.loop(100);

    bool setName = false;

    if (Cytrill.getButton(BTN_UP))
    {
        setName = true;
    }

    if (Cytrill.getButton(BTN_RIGHT))
    {
        runMode = SIMON_MODE;
    }
    else if (Cytrill.getButton(BTN_DOWN))
    {
        runMode = CONFIG_MODE;
    }
#ifdef ARDUINO_ESP32_DEV
    else if (Cytrill.getButton(BTN_X))
    {
        runMode = BLUETOOTH_MODE;
    }
#endif
    else
    {
        runMode = CONTROLLER_MODE;
    }

    configuration.setup();


    if (runMode != CONFIG_MODE && runMode != BLUETOOTH_MODE) //Simon mode need also no Wifi, but reset at moment.
    {
        setupWifi();
    }

    if (runMode == SIMON_MODE)
    {
        
        simon.setup();
    }
#ifdef ARDUINO_ESP32_DEV
    if (runMode == BLUETOOTH_MODE)
    {
        bluetoothHID.setup();
    }
#endif
    else if (runMode == CONTROLLER_MODE)
    {
        controller.setup();

        if (setName)
        {
            controller.sendSetName();
        }
    }

    if (runMode != CONFIG_MODE)
    {
#ifdef DEBUG_CYTRILL
        ArduinoOTA.onStart([]()
        {
            Serial.println("Starting OTA updates...");
        });

        ArduinoOTA.onEnd([]()
        {
            Serial.println("Stopping OTA updates...");
        });

        ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
        {
            Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
        });

        ArduinoOTA.onError([](ota_error_t error)
        {
            Serial.printf("Error[%u]: ", error);

            if (error == OTA_AUTH_ERROR)
            {
                Serial.println("Authentication failed!");
            }
            else if (error == OTA_BEGIN_ERROR)
            {
                Serial.println("Begin failed!");
            }
            else if (error == OTA_CONNECT_ERROR)
            {
                Serial.println("Connect failed!");
            }
            else if (error == OTA_RECEIVE_ERROR)
            {
                Serial.println("Receive failed!");
            }
            else if (error == OTA_END_ERROR)
            {
                Serial.println("End failed!");
            }
        });
#endif
/*
        ArduinoOTA.setHostname(HOSTNAME);
        ArduinoOTA.setPassword(OTA_PASSWORD);
        ArduinoOTA.begin();*/
    }
}

void loop()
{
    /*
    ArduinoOTA.handle();
*/
    if (WiFi.status() == WL_DISCONNECTED && runMode != (CONFIG_MODE || BLUETOOTH_MODE))
    {
        setupWifi();
    }

    if (runMode == SIMON_MODE)
    {
        simon.loop();
    }
#ifdef ARDUINO_ESP32_DEV
    else if (runMode == BLUETOOTH_MODE)
    {
        bluetoothHID.loop();
    }
#endif
    else if (runMode == CONFIG_MODE)
    {
        configuration.loop();
    }
    else
    {
        controller.loop();
    }
}
