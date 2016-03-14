#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <Arduino.h>
#include <EEPROM.h>

#define ESSID_LENGTH    32
#define PASSWORD_LENGTH 96

class Configuration
{
public:
    Configuration();
    ~Configuration();

    void setup();
    void loop();

    char *getEssid();
    char *getPassword();

private:
    void readWirelessConfig();
    void writeWirelessConfig();

private:
    char _essid[ESSID_LENGTH];
    char _password[PASSWORD_LENGTH];
    char _tmpEssid[ESSID_LENGTH];
    char _tmpPassword[PASSWORD_LENGTH];
    int _mode;
};

#endif
