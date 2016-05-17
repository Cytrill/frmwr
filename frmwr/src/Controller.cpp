#include "Controller.hpp"

Controller::Controller()
    : _currentGameHost(0),
      _gamePort(1337),
      _nsPort(1338),
      _gameHostCounter(0),
      _gameHosts{ 0, },
      _gameHostColors{ { 0, 0, 0 }, },
      _gameHostSelection(0),
      _buttonStates(0x00),
      _client(),
      _udp()
{
}

Controller::~Controller()
{
}

void Controller::setup()
{
    _udp.begin(_gamePort);

    Cytrill.registerButtonUpCallback([this](bool pressed)
    {
        if (_currentGameHost == 0 && pressed)
        {
            _gameHostSelection++;

            if (_gameHostSelection >= _gameHostCounter)
            {
                _gameHostSelection = 0;
            }

            updateHostSelectionColor();
        }
    });

    Cytrill.registerButtonDownCallback([this](bool pressed)
    {
        if (_currentGameHost == 0 && pressed)
        {
            _gameHostSelection--;

            if (_gameHostSelection < 0)
            {
                _gameHostSelection = _gameHostCounter == 0 ? 0 : _gameHostCounter - 1;
            }

            updateHostSelectionColor();
        }
    });

    Cytrill.registerButtonACallback([this](bool pressed)
    {
        if (_currentGameHost == 0 && pressed)
        {
          _currentGameHost = _gameHosts[_gameHostSelection];

          Cytrill.setLedLeft(0x00, 0x00, 0x00, 31);
          Cytrill.setLedRight(0x00, 0x00, 0x00, 31);
        }
    });

    delay(100);
}

void Controller::loop()
{
    static int lastKeepAlive = 0;

    if (WiFi.status() == WL_DISCONNECTED)
    {
        sendAskHost();
    }

    char msgBuffer[MSG_SIZE] = { 0x00, };
    uint32_t remoteIP = 0;

    int newButtonStates = Cytrill.getButtons();

    if (receiveMessage(msgBuffer, &remoteIP))
    {
        // Some sanity check
        if (msgBuffer[0] == msgBuffer[MSG_SIZE - 1])
        {
            switch (msgBuffer[0])
            {
                case CMD_SET_LED_LEFT:
                    Cytrill.setLedLeft(msgBuffer[1], msgBuffer[2], msgBuffer[3], msgBuffer[4]);
                    break;

                case CMD_SET_LED_RIGHT:
                    Cytrill.setLedRight(msgBuffer[1], msgBuffer[2], msgBuffer[3], msgBuffer[4]);
                    break;

                case CMD_PROPAGATE_HOST:
#ifdef DEBUG
                    Serial.print("Game host propagated itself: ");
                    Serial.print(remoteIP & 0xFF); Serial.print(".");
                    Serial.print((remoteIP >> 8) & 0xFF); Serial.print(".");
                    Serial.print((remoteIP >> 16) & 0xFF); Serial.print(".");
                    Serial.print((remoteIP >> 24) & 0xFF);
                    Serial.println();
#endif
                    if (_currentGameHost == 0)
                    {
                        addHostToList(remoteIP, msgBuffer[1], msgBuffer[2], msgBuffer[3], msgBuffer[4]);
                        updateHostSelectionColor();
                    }
                    break;
            }
        }
    }

    if (_currentGameHost != 0 && newButtonStates != _buttonStates)
    {
        sendButtonsChanged(newButtonStates);
        _buttonStates = newButtonStates;
    }

    if (millis() - lastKeepAlive >= KEEP_ALIVE_INTERVAL)
    {
        if (_currentGameHost == 0)
        {
            sendAskHost();
        }
        else
        {
            sendKeepAlive();
        }

        lastKeepAlive = millis();
    }

    Cytrill.loop();

    // Necessary for some reason, value found by empiric studies
    delayMicroseconds(2000);
}

void Controller::sendAskHost()
{
#ifdef DEBUG
    Serial.println("Asking for a game host in the network.");
#endif

    IPAddress broadcast = WiFi.localIP();
    broadcast[3] = 255;

    char askHost[] = { CMD_ASK_HOST, 0x00, 0x00, 0x00, 0x00, CMD_ASK_HOST };

    _udp.beginPacket(broadcast, _gamePort);
    _udp.write(askHost, MSG_SIZE);
    _udp.endPacket();
}

void Controller::sendSetName()
{
#ifdef DEBUG
    Serial.println("Requesting the nameserver to set a name.");
#endif

    IPAddress broadcast = WiFi.localIP();
    broadcast[3] = 255;

    char setName[] = { CMD_SET_NAME, 0x00, 0x00, 0x00, 0x00, CMD_SET_NAME };

    _udp.beginPacket(broadcast, _nsPort);
    _udp.write(setName, MSG_SIZE);
    _udp.endPacket();
}

void Controller::sendButtonsChanged(int newButtonStates)
{
    char message[] = {
        CMD_BUTTONS_CHANGED,
        char(newButtonStates & 0xFF),
        char((newButtonStates ^ _buttonStates) & 0xFF),
        0x00,
        0x00,
        CMD_BUTTONS_CHANGED
    };

    sendMessage(message);
}

void Controller::sendKeepAlive()
{
    char message[] = {
        CMD_KEEP_ALIVE,
        char(_buttonStates & 0xFF),
        0x00,
        0x00,
        0x00,
        CMD_KEEP_ALIVE
    };

    sendMessage(message);
}

void Controller::addHostToList(uint32_t gameHost, byte r, byte g, byte b, byte brightness)
{
    for (int i = 0; i < MAX_GAME_HOSTS; i++)
    {
        if (_gameHosts[i] == gameHost)
        {
            return;
        }
    }

    _gameHosts[_gameHostCounter] = gameHost;

    _gameHostColors[_gameHostCounter][0] = r;
    _gameHostColors[_gameHostCounter][1] = g;
    _gameHostColors[_gameHostCounter][2] = b;
    _gameHostColors[_gameHostCounter][3] = brightness;

    _gameHostCounter++;
}

void Controller::updateHostSelectionColor()
{
    byte r = _gameHostColors[_gameHostSelection][0];
    byte g = _gameHostColors[_gameHostSelection][1];
    byte b = _gameHostColors[_gameHostSelection][2];
    byte brightness = _gameHostColors[_gameHostSelection][3];

    Cytrill.setLedLeft(r, g, b, brightness);
    Cytrill.setLedRight(r, g, b, brightness);
}

void Controller::sendMessage(char message[])
{
#ifdef DEBUG
    Serial.print("Sending data: ");

    for (int i = 0; i < MSG_SIZE; i++)
    {
        Serial.print(message[i], HEX); Serial.print(", ");
    }

    Serial.println();
#endif

    _udp.beginPacket(_currentGameHost, _gamePort);
    _udp.write(message, MSG_SIZE);
    _udp.endPacket();
}

bool Controller::receiveMessage(char *buffer, uint32_t *remoteIP)
{
    int size = _udp.parsePacket();
    *remoteIP = _udp.remoteIP();

    if (size == MSG_SIZE)
    {
        _udp.read(buffer, MSG_SIZE);

        return true;
    }

    return false;
}
