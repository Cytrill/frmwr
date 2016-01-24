#include "SPI.h"
#include "Ctrl.h"
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <EEPROM.h>

#define CMD_KEEP_ALIVE      0x10
#define CMD_BUTTONS_CHANGED 0x11

#define CMD_SET_LED_0       0x20
#define CMD_SET_LED_1       0x21

#define CMD_PROPAGATE_HOST  0x30
#define CMD_ASK_HOST        0x31

#define ESSID_LENGTH        32
#define PASSWORD_LENGTH     96

#define MSG_SIZE            6

#define MAX_GAME_HOSTS    10

#define KEEP_ALIVE_INTERVAL 4000

#define DEBUG

bool configMode = false;

char essid[ESSID_LENGTH] = "";
char password[PASSWORD_LENGTH] = "";

String tmpESSID = "";
String tmpPassword = "";

uint32_t currentGameHost = 0;
const int gamePort = 1337;

int gameHostCounter = 0;
uint32_t gameHosts[MAX_GAME_HOSTS] = { 0, };
byte gameHostColors[MAX_GAME_HOSTS][4] = { { 0, 0, 0 }, };

WiFiClient client;
WiFiUDP udp;

int buttonStates = 0x00;

void readWirelessConfig(char *destESSID, char *destPassword)
{
  for (int i = 0; i < ESSID_LENGTH; i++)
  {
    destESSID[i] = char(EEPROM.read(i));
  }

  for (int i = 0; i < PASSWORD_LENGTH; i++)
  {
    destPassword[i] = char(EEPROM.read(ESSID_LENGTH + i));
  }
}

void writeWirelessConfig(const char *newESSID, const char *newPassword)
{
  for (int i = 0; i < ESSID_LENGTH - 1; i++)
  {
    EEPROM.write(i, newESSID[i]);

    if (newESSID[i] == 0)
    {
      break;
    }
  }

  EEPROM.write(ESSID_LENGTH - 1, 0);

  for (int i = 0; i < PASSWORD_LENGTH - 1; i++)
  {
    EEPROM.write(ESSID_LENGTH + i, newPassword[i]);

    if (newPassword[i] == 0)
    {
      break;
    }
  }

  EEPROM.write(ESSID_LENGTH + PASSWORD_LENGTH - 1, 0);

  EEPROM.commit();
}

void setupWifi()
{
#ifdef DEBUG
  Serial.println("Reading wireless config from EEPROM...");
#endif

  readWirelessConfig(essid, password);

#ifdef DEBUG
  Serial.print("Connecting to ");
  Serial.println(essid);
#endif

  WiFi.begin(essid, password);

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

void sendAskHost()
{
#ifdef DEBUG
  Serial.println("Asking for a game host in the network.");
#endif

  IPAddress broadcast = WiFi.localIP();
  broadcast[3] = 255;

  char askHost[] = { CMD_ASK_HOST, 0x00, 0x00, 0x00, 0x00, CMD_ASK_HOST };

  udp.beginPacket(broadcast, gamePort);
  udp.write(askHost, MSG_SIZE);
  udp.endPacket();
}

void addHostToList(uint32_t gameHost, byte r, byte g, byte b, byte brightness)
{
  for (int i = 0; i < MAX_GAME_HOSTS; i++)
  {
    if (gameHosts[i] == gameHost)
    {
      return;
    }
  }

  gameHosts[gameHostCounter] = gameHost;
  gameHostColors[gameHostCounter][0] = r;
  gameHostColors[gameHostCounter][1] = g;
  gameHostColors[gameHostCounter][2] = b;
  gameHostColors[gameHostCounter][3] = brightness;

  gameHostCounter++;
}

void updateHostSelectionColor(int gameHostSelection)
{
  byte r = gameHostColors[gameHostSelection][0];
  byte g = gameHostColors[gameHostSelection][1];
  byte b = gameHostColors[gameHostSelection][2];
  byte brightness = gameHostColors[gameHostSelection][3];

  Cytrill.setLed(LED_0, r, g, b, brightness);
  Cytrill.setLed(LED_1, r, g, b, brightness);
}

void sendMessage(char message[])
{
#ifdef DEBUG
  Serial.print("Sending data: ");
  for (int i = 0; i < MSG_SIZE; i++)
  {
    Serial.print(message[i], HEX); Serial.print(", ");
  }
  Serial.println();
#endif

  udp.beginPacket(currentGameHost, gamePort);
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

bool receiveMessage(char *buffer, uint32_t *remoteIP)
{
  int size = udp.parsePacket();
  *remoteIP = udp.remoteIP();

  if (size == MSG_SIZE)
  {
    udp.read(buffer, MSG_SIZE);

    return true;
  }

  return false;
}

void configureSetup()
{
  readWirelessConfig(essid, password);
}

void mainSetup()
{
  setupWifi();
  udp.begin(gamePort);

  delay(100);
}

void setup()
{
  Serial.begin(115200);
  EEPROM.begin(512);
  Cytrill.begin();

  Cytrill.setLed(LED_0, 0x00, 0x00, 0x00, 31);
  Cytrill.setLed(LED_1, 0x00, 0x00, 0x00, 31);

  delay(100);

  Cytrill.loop(10);

  if (Cytrill.getButton(BTN_DOWN))
  {
    configMode = true;
  }

  if (configMode)
  {
    configureSetup();
  }
  else
  {
    mainSetup();
  }
}

void configureLoop()
{
  static int mode = 0;

  if (mode == 0)
  {
    Serial.println("CONFIGURING MODE");
    Serial.println("================");
    Serial.println("Current configuration is the following:");
    Serial.print("ESSID=");
    Serial.println(essid);
    Serial.print("Password=");
    Serial.println(password);
    Serial.println();

    Serial.println("Please enter an ESSID...");

    mode = 1;
  }
  else if (mode == 1)
  {
    tmpESSID = Serial.readString();

    if (strcmp(tmpESSID.c_str(), "") != 0)
    {
      mode = 2;
    }
  }
  else if (mode == 2)
  {
    Serial.println("Please enter a password...");

    mode = 3;
  }
  else if (mode == 3)
  {
    tmpPassword = Serial.readString();

    if (strcmp(tmpPassword.c_str(), "") != 0)
    {
      mode = 4;
    }
  }
  else if (mode == 4)
  {
    writeWirelessConfig(tmpESSID.c_str(), tmpPassword.c_str());

    Serial.println("Writing config to EEPROM!");

    mode = 5;
  }
}

void mainLoop()
{
  static int lastKeepAlive = 0;

  static bool lastUpState = false;
  static bool lastDownState = false;
  static bool lastAState = false;

  static int gameHostSelection = 0;

  if (WiFi.status() == WL_DISCONNECTED)
  {
    setupWifi();
    sendAskHost();
  }

  char msgBuffer[MSG_SIZE] = { 0x00, };
  uint32_t remoteIP = 0;

  int newButtonStates = Cytrill.getButtons();

  if (currentGameHost == 0)
  {
    bool newUpState = Cytrill.getButton(BTN_UP);
    bool newDownState = Cytrill.getButton(BTN_DOWN);
    bool newAState = Cytrill.getButton(BTN_A);
    
    if (!lastUpState && newUpState)
    {
      gameHostSelection++;

      if (gameHostSelection >= gameHostCounter)
      {
        gameHostSelection = 0;
      }

      updateHostSelectionColor(gameHostSelection);
    }

    if (!lastDownState && newDownState)
    {
      gameHostSelection--;

      if (gameHostSelection < 0)
      {
        gameHostSelection = gameHostCounter == 0 ? 0 : gameHostCounter - 1;
      }

      updateHostSelectionColor(gameHostSelection);
    }

    if (!lastAState && newAState)
    {
      currentGameHost = gameHosts[gameHostSelection];

      Cytrill.setLed(LED_0, 0x00, 0x00, 0x00, 31);
      Cytrill.setLed(LED_1, 0x00, 0x00, 0x00, 31);
    }

    lastUpState = newUpState;
    lastDownState = newDownState;
    lastAState = newAState;
  }

  if (receiveMessage(msgBuffer, &remoteIP))
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

        case CMD_PROPAGATE_HOST:
#ifdef DEBUG
          Serial.print("Game host propagated itself: ");
          Serial.print(remoteIP & 0xFF); Serial.print(".");
          Serial.print((remoteIP >> 8) & 0xFF); Serial.print(".");
          Serial.print((remoteIP >> 16) & 0xFF); Serial.print(".");
          Serial.print((remoteIP >> 24) & 0xFF);
          Serial.println();
#endif
          if (currentGameHost == 0)
          {
            addHostToList(remoteIP, msgBuffer[1], msgBuffer[2], msgBuffer[3], msgBuffer[4]);
            updateHostSelectionColor(gameHostSelection);
          }
          break;
      }
    }
  }

  if (currentGameHost != 0 && newButtonStates != buttonStates)
  {
    sendButtonsChanged(newButtonStates);
    buttonStates = newButtonStates;
  }

  if (millis() - lastKeepAlive >= KEEP_ALIVE_INTERVAL)
  {
    if (currentGameHost == 0)
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
  delayMicroseconds(3000);
}

void loop()
{
  if (configMode)
  {
    configureLoop();
  }
  else
  {
    mainLoop();
  }
}

