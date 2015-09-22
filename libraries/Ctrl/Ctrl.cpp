#include "Ctrl.h"

Ctrl::Ctrl()
    : _buttonCallback(NULL),
      _buttons(0),
      _leds{ 0, }
{
}

Ctrl::~Ctrl()
{
}

void Ctrl::begin()
{
    // SPI
    SPI.begin ();  

    // reset the pin mode of MISO, we use it as input
    SPI.setBitOrder (MSBFIRST);
    SPI.setDataMode (0);
    SPI.setFrequency (10000000);

    SPI.writePattern (_leds, sizeof (_leds), 0x00);

    // relevant for boot mode, needs pullup on startup for flash boot
    pinMode (0, INPUT_PULLUP);

    // UART0 TX. Use for serial tx. Also for the builtin LED.
    // pinMode (1, SPECIAL);
    //
    // UART1 TX during flash programming, no external pullup!
    pinMode (2, INPUT_PULLUP);

    // UART0 RX. Use for serial rx
    // pinMode (3, SPECIAL);

    pinMode (4, INPUT_PULLUP);
    pinMode (5, INPUT_PULLUP);

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
    return bool(_buttons & button);
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
}

Ctrl Cytrill;
