/*
 Ctrl.h - Development library for the Cytrill game controller.
  danb
  http://hasi.it
*/

#ifndef CTRL_H
#define CTRL_H

#include <Arduino.h>
#include <stdint.h>
#include <functional>
#include <APA102.h>

#define NUM_LEDS 10

#define DATA 4
#define CLK 0

#define BOUNCING_TIME 5


/********************************************
 ***     ____________________________     ***
 ***    /    _                  _    \    ***
 ***   /  _ |U| _   L0  L1   _ |X| _  \   ***
 ***   | |L| _ |R|          |Y| _ |A| |   ***
 ***   \    |D|                |B|    /   ***
 ***    \____________________________/    ***
 ***                                      ***
 ********************************************/

#define BTN_UP         0
#define BTN_RIGHT      1
#define BTN_DOWN       2
#define BTN_LEFT       3

#define BTN_X          4
#define BTN_A          5
#define BTN_B          6
#define BTN_Y          7

#define BTN_UP_MASK    0x01
#define BTN_RIGHT_MASK 0x02
#define BTN_DOWN_MASK  0x04
#define BTN_LEFT_MASK  0x08

#define BTN_X_MASK     0x10
#define BTN_A_MASK     0x20
#define BTN_B_MASK     0x40
#define BTN_Y_MASK     0x80

#define LED_LEFT       0
#define LED_RIGHT      1

#define BTN_X_PIN      2
#define BTN_A_PIN      11
#define BTN_B_PIN      13
#define BTN_Y_PIN      14

#define BTN_UP_PIN     12
#define BTN_DOWN_PIN   27
#define BTN_LEFT_PIN   32
#define BTN_RIGHT_PIN  25

class Ctrl
{
public:
    Ctrl();
    ~Ctrl();

    void begin();
    void begin(int);
    void end();

    void registerButtonsCallback(const std::function<void (int, bool)> &);
    void registerButtonUpCallback(const std::function<void (bool)> &);
    void registerButtonRightCallback(const std::function<void (bool)> &);
    void registerButtonDownCallback(const std::function<void (bool)> &);
    void registerButtonLeftCallback(const std::function<void (bool)> &);
    void registerButtonXCallback(const std::function<void (bool)> &);
    void registerButtonACallback(const std::function<void (bool)> &);
    void registerButtonBCallback(const std::function<void (bool)> &);
    void registerButtonYCallback(const std::function<void (bool)> &);

    bool getButton(int);
    bool getButtonUp();
    bool getButtonRight();
    bool getButtonDown();
    bool getButtonLeft();
    bool getButtonX();
    bool getButtonA();
    bool getButtonB();
    bool getButtonY();
    int getButtons();

    void setLed(int, byte, byte, byte, byte);
    void setLedLeft(byte, byte, byte, byte);
    void setLedRight(byte, byte, byte, byte);
    void setLeds(byte[]);

    void loop();
    void loop(int);

private:
    void debounceButton(int, int);
//    void bitBangSPI(uint8_t *, uint32_t);

private:
    byte APA102_START_FRAME[4] = { 0x00, };
    byte APA102_END_FRAME[4] = { 0xFF, };

    std::function<void (int, bool)> _buttonsCallback;

    std::function<void (bool)> _buttonUpCallback;
    std::function<void (bool)> _buttonRightCallback;
    std::function<void (bool)> _buttonDownCallback;
    std::function<void (bool)> _buttonLeftCallback;
    std::function<void (bool)> _buttonXCallback;
    std::function<void (bool)> _buttonACallback;
    std::function<void (bool)> _buttonBCallback;
    std::function<void (bool)> _buttonYCallback;

    int _bounceCounter[8];
    bool _buttons[8];
    byte _leds[NUM_LEDS * 4];
    APA102<DATA, CLK> leds;
};

extern Ctrl Cytrill;

#endif
