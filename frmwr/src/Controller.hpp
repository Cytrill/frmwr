#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#define DEBUG

#include <Arduino.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "Ctrl.h"

#define CMD_KEEP_ALIVE      0x10
#define CMD_BUTTONS_CHANGED 0x11

#define CMD_SET_LED_LEFT    0x20
#define CMD_SET_LED_RIGHT   0x21

#define CMD_PROPAGATE_HOST  0x30
#define CMD_ASK_HOST        0x31

#define CMD_SET_NAME        0x40

#define MSG_SIZE            6

#define MAX_GAME_HOSTS      10

#define KEEP_ALIVE_INTERVAL 4000

class Controller
{
public:
    Controller();
    ~Controller();

    void setup();
    void loop();

    void sendAskHost();
    void sendSetName();
    void sendButtonsChanged(int newButtonStates);
    void sendKeepAlive();

private:
    void addHostToList(uint32_t gameHost, byte r, byte g, byte b, byte brightness);
    void updateHostSelectionColor();
    void sendMessage(char message[]);
    bool receiveMessage(char *buffer, uint32_t *remoteIP);

private:
    uint32_t _currentGameHost;
    int _gamePort;
    int _nsPort;

    int _gameHostCounter;
    uint32_t _gameHosts[MAX_GAME_HOSTS];
    byte _gameHostColors[MAX_GAME_HOSTS][4];
    int _gameHostSelection;

    int _buttonStates;

    WiFiClient _client;
    WiFiUDP _udp;
};

#endif
