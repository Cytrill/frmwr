#ifndef SIMON_HPP
#define SIMON_HPP

#include <Arduino.h>
#include "Ctrl.h"

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
