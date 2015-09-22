#include "Ctrl.h"

Ctrl::Ctrl()
    : _buttonCallback(NULL),
      _buttonsBouncing{ false, },
      _bouncingTime{ 0, },
      _buttons{ false, },
      _leds{ 0, }
{
}

Ctrl::~Ctrl()
{
}

void Ctrl::begin()
{
    begin(10000000);
}

void Ctrl::begin(int frequency)
{
    // SPI
    SPI.begin ();  

    // reset the pin mode of MISO, we use it as input
    SPI.setBitOrder (MSBFIRST);
    SPI.setDataMode (0);
    SPI.setFrequency (frequency);

    SPI.writePattern (_leds, sizeof (_leds), 0x00);

    // relevant for boot mode, needs pullup on startup for flash boot
    pinMode (BTN_B_PIN, INPUT_PULLUP);

    // UART0 TX. Use for serial tx. Also for the builtin LED.
    // pinMode (1, SPECIAL);
    //
    // UART1 TX during flash programming, no external pullup!
    pinMode (BTN_X_PIN, INPUT_PULLUP);

    // UART0 RX. Use for serial rx
    // pinMode (3, SPECIAL);

    pinMode (BTN_A_PIN, INPUT_PULLUP);
    pinMode (BTN_Y_PIN, INPUT_PULLUP);

    // 6:

    // this overrides SPI MISO which we don't use
    pinMode (12, INPUT_PULLUP);

    // 13: SPI MOSI, we use it for the LEDs, might possibly be used for a button with series resistor as well.
    // 14: SPI CLK, we use it for the LEDs

    // relevant for boot mode, needs Pulldown on startup
    pinMode (15, INPUT_PULLUP);
}

void Ctrl::end()
{
    SPI.end();
}

void Ctrl::registerButtonCallback(void (*callback)(int, bool))
{
    _buttonCallback = callback;
}

bool Ctrl::getButton(int button)
{
    return _buttons[button];
}

int Ctrl::getButtons()
{
    return _buttons[BTN_UP] ? BTN_UP_MASK : 0x00  ||
           _buttons[BTN_RIGHT] ? BTN_RIGHT_MASK : 0x00 ||
           _buttons[BTN_DOWN] ? BTN_DOWN_MASK : 0x00 ||
           _buttons[BTN_LEFT] ? BTN_LEFT_MASK : 0x00 ||
           _buttons[BTN_X] ? BTN_X_MASK : 0x00 ||
           _buttons[BTN_A] ? BTN_A_MASK : 0x00 ||
           _buttons[BTN_B] ? BTN_B_MASK : 0x00 ||
           _buttons[BTN_Y] ? BTN_Y_MASK : 0x00;
}

void Ctrl::setLed(int led, byte r, byte g, byte b)
{
    if (led < NUM_LEDS)
    {
        _leds[led * 3] = r;
        _leds[led * 3 + 1] = g;
        _leds[led * 3 + 2] = b;
    }

    SPI.writeBytes(_leds, sizeof(_leds));
}

void Ctrl::setLeds(byte leds[])
{
    memcpy(_leds, leds, NUM_LEDS);

    SPI.writeBytes(_leds, sizeof(_leds));
}

void Ctrl::loop()
{
    int btnUp, btnRight, btnDown, btnLeft;
    int btnX, btnA, btnB, btnY;

    btnX = digitalRead(BTN_X_PIN);
    btnA = digitalRead(BTN_A_PIN);
    btnB = digitalRead(BTN_B_PIN);
    btnY = digitalRead(BTN_Y_PIN);

    debounceButton(BTN_UP, btnX);
    debounceButton(BTN_RIGHT, btnA);
    debounceButton(BTN_DOWN, btnB);
    debounceButton(BTN_LEFT, btnY);
    debounceButton(BTN_X, btnX);
    debounceButton(BTN_A, btnA);
    debounceButton(BTN_B, btnB);
    debounceButton(BTN_Y, btnY);
}

void Ctrl::debounceButton(int button, int newValue)
{
    if (getButton(button) != (newValue == HIGH) &&
        !_buttonsBouncing[button])
    {
        _buttonsBouncing[button] = true;
        _bouncingTime[button] = 0;
    }
    else if (_buttonsBouncing[button])
    {
        if (_bouncingTime[button] < 100)
        {
            _bouncingTime[button]++;
        }
        else
        {
            _buttonsBouncing[button] = false;

            if (getButton(button) != (newValue == HIGH))
            {
                _buttons[button] = (newValue == HIGH);

                if (_buttonCallback != NULL)
                {
                    _buttonCallback(button, newValue);
                }
            }
        }
    }
}

Ctrl Cytrill;
