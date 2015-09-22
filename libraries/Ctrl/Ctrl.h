/*
 Ctrl.h - Development library for the Cytrill game controller.
  danb
  http://hasi.it
*/

#ifndef CTRL_H
#define CTRL_H

#include <Arduino.h>
#include <stdint.h>
#include "SPI.h"
#include "esp8266_peri.h"

#define NUM_LEDS 4

/********************************************
 ***     ____________________________     ***
 ***    /    _                  _    \    ***
 ***   /  _ |U| _   L0  L1   _ |X| _  \   ***
 ***   | |L| _ |R|          |Y| _ |A| |   ***
 ***   \    |D|                |B|    /   ***
 ***    \____________________________/    ***
 ***                                      ***
 ********************************************/

#define BTN_UP    0x01
#define BTN_RIGHT 0x02
#define BTN_DOWN  0x04
#define BTN_LEFT  0x08

#define BTN_X     0x10
#define BTN_A     0x20
#define BTN_B     0x40
#define BTN_Y     0x80

#define LED_0     0
#define LED_1     1
#define LED_2     2
#define LED_3     3

class Ctrl
{
public:
    Ctrl();
    ~Ctrl();

    void begin();
    void end();

    void registerButtonCallback(void (*)(int, bool));

    bool getButton(int);

    void setLed(int, byte, byte, byte);
    void setLeds(byte[]);

    void loop();

private:
    void (*_buttonCallback)(int, bool);

    unsigned int _buttons;
    byte _leds[NUM_LEDS * 3];
};

extern Ctrl Cytrill;

#endif
