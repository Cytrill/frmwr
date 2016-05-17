#include "Ctrl.h"

Ctrl::Ctrl()
    : _buttonsCallback(NULL),
      _buttonUpCallback(NULL),
      _buttonRightCallback(NULL),
      _buttonDownCallback(NULL),
      _buttonLeftCallback(NULL),
      _buttonXCallback(NULL),
      _buttonACallback(NULL),
      _buttonBCallback(NULL),
      _buttonYCallback(NULL),
      _bounceCounter{ 0, },
      _buttons{ false, },
      _leds{ 0x00, }
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
    SPI.begin();

    // reset the pin mode of MISO, we use it as input
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(0);
    SPI.setFrequency(frequency);

    delay(5);

    for (int i = 0; i < NUM_LEDS; i++)
    {
        _leds[i * 4] = 0xE0;
    }

    SPI.writeBytes(APA102_START_FRAME, sizeof (APA102_START_FRAME));
    SPI.writeBytes(_leds, sizeof (_leds));
    SPI.writeBytes(APA102_END_FRAME, sizeof (APA102_END_FRAME));

    // relevant for boot mode, needs pullup on startup for flash boot
    pinMode(BTN_B_PIN, INPUT_PULLUP);

    // UART0 TX. Use for serial tx. Also for the builtin LED.
    // pinMode (1, SPECIAL);
    //
    // UART1 TX during flash programming, no external pullup!
    pinMode(BTN_X_PIN, INPUT_PULLUP);

    // UART0 RX. Use for serial rx
    // pinMode (3, SPECIAL);

    pinMode(BTN_A_PIN, INPUT_PULLUP);
    pinMode(BTN_Y_PIN, INPUT_PULLUP);

    // 6:

    // this overrides SPI MISO which we don't use
    pinMode(12, INPUT_PULLUP);

    // 13: SPI MOSI, we use it for the LEDs, might possibly be used for a button with series resistor as well.
    // 14: SPI CLK, we use it for the LEDs

    // relevant for boot mode, needs Pulldown on startup
    pinMode(15, INPUT_PULLUP);
}

void Ctrl::end()
{
    SPI.end();
}

void Ctrl::registerButtonsCallback(const std::function<void (int, bool)> &callback)
{
    _buttonsCallback = callback;
}

void Ctrl::registerButtonUpCallback(const std::function<void (bool)> &callback)
{
    _buttonUpCallback = callback;
}

void Ctrl::registerButtonRightCallback(const std::function<void (bool)> &callback)
{
    _buttonRightCallback = callback;
}

void Ctrl::registerButtonDownCallback(const std::function<void (bool)> &callback)
{
    _buttonDownCallback = callback;
}

void Ctrl::registerButtonLeftCallback(const std::function<void (bool)> &callback)
{
    _buttonLeftCallback = callback;
}

void Ctrl::registerButtonXCallback(const std::function<void (bool)> &callback)
{
    _buttonXCallback = callback;
}

void Ctrl::registerButtonACallback(const std::function<void (bool)> &callback)
{
    _buttonACallback = callback;
}

void Ctrl::registerButtonBCallback(const std::function<void (bool)> &callback)
{
    _buttonBCallback = callback;
}

void Ctrl::registerButtonYCallback(const std::function<void (bool)> &callback)
{
    _buttonYCallback = callback;
}

bool Ctrl::getButton(int button)
{
    return _buttons[button];
}

bool Ctrl::getButtonUp()
{
    return _buttons[BTN_UP];
}

bool Ctrl::getButtonRight()
{
    return _buttons[BTN_RIGHT];
}

bool Ctrl::getButtonDown()
{
    return _buttons[BTN_DOWN];
}

bool Ctrl::getButtonLeft()
{
    return _buttons[BTN_LEFT];
}

bool Ctrl::getButtonX()
{
    return _buttons[BTN_X];
}

bool Ctrl::getButtonA()
{
    return _buttons[BTN_A];
}

bool Ctrl::getButtonB()
{
    return _buttons[BTN_B];
}

bool Ctrl::getButtonY()
{
    return _buttons[BTN_Y];
}

int Ctrl::getButtons()
{
    return (_buttons[BTN_UP] ? BTN_UP_MASK : 0x00) |
           (_buttons[BTN_RIGHT] ? BTN_RIGHT_MASK : 0x00) |
           (_buttons[BTN_DOWN] ? BTN_DOWN_MASK : 0x00) |
           (_buttons[BTN_LEFT] ? BTN_LEFT_MASK : 0x00) |
           (_buttons[BTN_X] ? BTN_X_MASK : 0x00) |
           (_buttons[BTN_A] ? BTN_A_MASK : 0x00) |
           (_buttons[BTN_B] ? BTN_B_MASK : 0x00) |
           (_buttons[BTN_Y] ? BTN_Y_MASK : 0x00);
}

void Ctrl::setLed(int led, byte red, byte green, byte blue, byte brightness)
{
    if (led < NUM_LEDS)
    {
        _leds[led * 4] = brightness | 0xE0;
        _leds[led * 4 + 1] = blue;
        _leds[led * 4 + 2] = green;
        _leds[led * 4 + 3] = red;
    }

    SPI.writeBytes(APA102_START_FRAME, sizeof (APA102_START_FRAME));
    SPI.writeBytes(_leds, sizeof(_leds));
    SPI.writeBytes(APA102_END_FRAME, sizeof (APA102_END_FRAME));
}

void Ctrl::setLedLeft(byte red, byte green, byte blue, byte brightness)
{
    setLed(LED_LEFT, red, green, blue, brightness);
}

void Ctrl::setLedRight(byte red, byte green, byte blue, byte brightness)
{
    setLed(LED_RIGHT, red, green, blue, brightness);
}

void Ctrl::setLeds(byte leds[])
{
    memcpy(_leds, leds, sizeof(_leds));

    SPI.writeBytes(APA102_START_FRAME, sizeof (APA102_START_FRAME));
    SPI.writeBytes(_leds, sizeof(_leds));
    SPI.writeBytes(APA102_END_FRAME, sizeof (APA102_END_FRAME));
}

void Ctrl::loop()
{
    int btnUp, btnRight, btnDown, btnLeft;
    int btnX, btnA, btnB, btnY;

    int adcValue = analogRead(A0);

    const float vADCMax = 1.0f;
    const float adcValueMax = 1024.f;
    const float vCC = 3.3f;
    const float r0 = 2500.0f;
    const float r1 = 87000.0f;

    float vADC = float(adcValue) * (vADCMax / adcValueMax);

    int i = int((r1 * vADC) / (r0 * (vCC - vADC)) + 0.25f);

    btnUp = bool(i & 0x01) ? LOW : HIGH;
    btnRight = bool(i & 0x04) ? LOW : HIGH;
    btnDown = bool(i & 0x08) ? LOW : HIGH;
    btnLeft = bool(i & 0x02) ? LOW : HIGH;

#ifdef BTN_DEBUG
    static int debugCounter = 0;
    debugCounter++;

    if (debugCounter > 500)
    {
        Serial.print("BTN_UP: ");
        Serial.println(btnUp);
        Serial.print("BTN_RIGHT: ");
        Serial.println(btnRight);
        Serial.print("BTN_DOWN: ");
        Serial.println(btnDown);
        Serial.print("BTN_LEFT: ");
        Serial.println(btnLeft);
        Serial.println();

        debugCounter = 0;
    }
#endif

    btnX = digitalRead(BTN_X_PIN) ? LOW : HIGH;
    btnA = digitalRead(BTN_A_PIN) ? LOW : HIGH;
    btnB = digitalRead(BTN_B_PIN) ? LOW : HIGH;
    btnY = digitalRead(BTN_Y_PIN) ? LOW : HIGH;

    delay(0);

#ifdef BTN_DEBUG
    if (debugCounter > 500)
    {
        Serial.print("BTN_X: ");
        Serial.println(btnX);
        Serial.print("BTN_A: ");
        Serial.println(btnA);
        Serial.print("BTN_B: ");
        Serial.println(btnB);
        Serial.print("BTN_Y: ");
        Serial.println(btnY);
        Serial.println();

        debugCounter = 0;
    }
#endif

    debounceButton(BTN_UP, btnUp);
    debounceButton(BTN_RIGHT, btnRight);
    debounceButton(BTN_DOWN, btnDown);
    debounceButton(BTN_LEFT, btnLeft);
    debounceButton(BTN_X, btnX);
    debounceButton(BTN_A, btnA);
    debounceButton(BTN_B, btnB);
    debounceButton(BTN_Y, btnY);
}

void Ctrl::loop(int count)
{
    for (int i = 0; i < count; i++)
    {
        Cytrill.loop();
    }
}

void Ctrl::debounceButton(int button, int newValue)
{
    bool newState = (newValue == HIGH);

    if (getButton(button) != newState &&
        _bounceCounter[button] == 0)
    {
        _bounceCounter[button] = BOUNCING_TIME;
    }
    else
    {
        _bounceCounter[button] = std::max(1, _bounceCounter[button]) - 1;
    }

    if (_bounceCounter[button] == 1)
    {
        _buttons[button] = newState;

        if (_buttonsCallback != NULL)
        {
            _buttonsCallback(button, newState);
        }

        if (button == BTN_UP && _buttonUpCallback != NULL)
        {
            _buttonUpCallback(newState);
        }
        else if (button == BTN_RIGHT && _buttonRightCallback != NULL)
        {
            _buttonRightCallback(newState);
        }
        else if (button == BTN_DOWN && _buttonDownCallback != NULL)
        {
            _buttonDownCallback(newState);
        }
        else if (button == BTN_LEFT && _buttonLeftCallback != NULL)
        {
            _buttonLeftCallback(newState);
        }
        else if (button == BTN_X && _buttonXCallback != NULL)
        {
            _buttonXCallback(newState);
        }
        else if (button == BTN_A && _buttonACallback != NULL)
        {
            _buttonACallback(newState);
        }
        else if (button == BTN_B && _buttonBCallback != NULL)
        {
            _buttonBCallback(newState);
        }
        else if (button == BTN_Y && _buttonYCallback != NULL)
        {
            _buttonYCallback(newState);
        }
    }
}

Ctrl Cytrill;
