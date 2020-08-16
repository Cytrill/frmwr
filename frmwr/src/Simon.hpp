#ifndef SIMON_HPP
#define SIMON_HPP

#include <Arduino.h>
#ifdef ARDUINO_ESP8266_NODEMCU
  #include "Ctrl.h"
#elif defined ARDUINO_ESP32_DEV
  #include "Ctrl32.h"
#endif

class Simon
{
public:
    Simon();
    ~Simon();

    void setup();
    void loop();

    void reset();

private:
    int _pattern[32] = { 0, };
    int _correctPresses = 0;
    int _level = 1;
    int _mode = 0;
};

#endif
