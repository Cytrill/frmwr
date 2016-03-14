#include "Configuration.hpp"

Configuration::Configuration()
    : _essid{ 0, },
      _password{ 0, },
      _tmpEssid{ 0, },
      _tmpPassword{ 0, },
      _mode(0)
{
}

Configuration::~Configuration()
{
    readWirelessConfig();
}

void Configuration::setup()
{
    EEPROM.begin(512);

    readWirelessConfig();
}

void Configuration::loop()
{
    static int i = 0;

    if (_mode == 0)
    {
        Serial.println("CONFIGURING MODE");
        Serial.println("================");
        Serial.println("Current configuration is the following:");
        Serial.print("ESSID=");
        Serial.println(_essid);
        Serial.print("Password=");
        Serial.println(_password);
        Serial.println();

        Serial.println("Please enter an ESSID...");

        _mode = 1;
    }
    else if (_mode == 1)
    {
        if (Serial.available())
        {
            char next = 255;
            bool readFurther = true;

            do
            {
                next = Serial.read();
                readFurther = next != 0 && next != '\r' && next != '\n';
                _tmpEssid[i] = next;
                i++;
            } while (Serial.available() && readFurther);

            if (!readFurther)
            {
                _tmpEssid[i - 1] = 0;
                i = 0;
                _mode = 2;
            }
        }
    }
    else if (_mode == 2)
    {
        Serial.println("Please enter a password...");

        _mode = 3;
    }
    else if (_mode == 3)
    {
        if (Serial.available())
        {
            char next = 255;
            bool readFurther = true;

            do
            {
                next = Serial.read();
                readFurther = next != 0 && next != '\r' && next != '\n';
                _tmpPassword[i] = next;
                i++;
            } while (Serial.available() && readFurther);

            if (!readFurther)
            {
                _tmpPassword[i - 1] = 0;
                i = 0;
                _mode = 4;
            }
        }
    }
    else if (_mode == 4)
    {
        writeWirelessConfig();

        Serial.println("Writing config to EEPROM!");

        _mode = 5;
    }
}

char *Configuration::getEssid()
{
    return _essid;
}

char *Configuration::getPassword()
{
    return _password;
}

void Configuration::readWirelessConfig()
{
    for (int i = 0; i < ESSID_LENGTH; i++)
    {
        _essid[i] = char(EEPROM.read(i));
    }

    for (int i = 0; i < PASSWORD_LENGTH; i++)
    {
        _password[i] = char(EEPROM.read(ESSID_LENGTH + i));
    }
}

void Configuration::writeWirelessConfig()
{
    for (int i = 0; i < ESSID_LENGTH - 1; i++)
    {
        EEPROM.write(i, _tmpEssid[i]);

        if (_tmpEssid[i] == 0)
        {
            break;
        }
    }

    EEPROM.write(ESSID_LENGTH - 1, 0);

    for (int i = 0; i < PASSWORD_LENGTH - 1; i++)
    {
        EEPROM.write(ESSID_LENGTH + i, _tmpPassword[i]);

        if (_tmpPassword[i] == 0)
        {
            break;
        }
    }

    EEPROM.write(ESSID_LENGTH + PASSWORD_LENGTH - 1, 0);

    EEPROM.commit();
}
