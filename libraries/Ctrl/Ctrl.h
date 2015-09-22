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
#define BOUNCING_TIME 10

/********************************************
 ***     ____________________________     ***
 ***    /    _                  _    \    ***
 ***   /  _ |U| _   L0  L1   _ |X| _  \   ***
 ***   | |L| _ |R|          |Y| _ |A| |   ***
 ***   \    |D|                |B|    /   ***
 ***    \____________________________/    ***
 ***                                      ***
 ********************************************/

#define BTN_UP         0x01
#define BTN_RIGHT      0x02
#define BTN_DOWN       0x04
#define BTN_LEFT       0x08

#define BTN_X          0x10
#define BTN_A          0x20
#define BTN_B          0x40
#define BTN_Y          0x80

#define BTN_UP_MASK    0x01
#define BTN_RIGHT_MASK 0x02
#define BTN_DOWN_MASK  0x04
#define BTN_LEFT_MASK  0x08

#define BTN_X_MASK     0x10
#define BTN_A_MASK     0x20
#define BTN_B_MASK     0x40
#define BTN_Y_MASK     0x80

const int BTN_MASKS[8] = {
    BTN_UP_MASK,
    BTN_RIGHT_MASK,
    BTN_DOWN_MASK,
    BTN_LEFT_MASK,
    BTN_X_MASK,
    BTN_A_MASK,
    BTN_B_MASK,
    BTN_Y_MASK
};

#define LED_0          0
#define LED_1          1
#define LED_2          2
#define LED_3          3

#define ADC_PIN        A0

#define BTN_X_PIN      2
#define BTN_A_PIN      4
#define BTN_B_PIN      0
#define BTN_Y_PIN      5

class Ctrl
{
public:
    Ctrl();
    ~Ctrl();

    void begin();
    void begin(int);
    void end();

    void registerButtonCallback(void (*)(int, bool));

    bool getButton(int);
    int getButtons();

    void setLed(int, byte, byte, byte);
    void setLeds(byte[]);

    void loop();

private:
    void debounceButton(int, int);

private:
    void (*_buttonCallback)(int, bool);

    int _bounceCounter[8];
    bool _buttons[8];
    byte _leds[NUM_LEDS * 3];
};

extern Ctrl Cytrill;

#endif
