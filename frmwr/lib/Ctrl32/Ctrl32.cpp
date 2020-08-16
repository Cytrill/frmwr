#include "Ctrl32.h"
//#define BTN_DEBUG
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
    pinMode(CLK,OUTPUT);
    pinMode(DATA, OUTPUT); 

    delay(5);


    for (int i = 0; i < NUM_LEDS; i+=4)
    {
        _leds[i * 4] = 0xE0;
    }

/*
    bitBangSPI(APA102_START_FRAME, sizeof (APA102_START_FRAME));
    bitBangSPI(_leds, sizeof (_leds));
    bitBangSPI(APA102_END_FRAME, sizeof (APA102_END_FRAME));
*/
    leds.startFrame();
    for(int i = 0; i<sizeof(_leds); i+=4){
        leds.sendColor(_leds[i+3],_leds[i+2],_leds[i+1],_leds[i]);
    }
    leds.endFrame(NUM_LEDS);
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

    pinMode(BTN_UP_PIN, INPUT_PULLUP);
    pinMode(BTN_DOWN_PIN, INPUT_PULLUP);
    pinMode(BTN_LEFT_PIN, INPUT_PULLUP);
    pinMode(BTN_RIGHT_PIN, INPUT_PULLUP);

}

void Ctrl::end()
{

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
/*
    bitBangSPI(APA102_START_FRAME, sizeof (APA102_START_FRAME));
    bitBangSPI(_leds, sizeof(_leds));
    bitBangSPI(APA102_END_FRAME, sizeof (APA102_END_FRAME));
 */
    leds.startFrame();
    for(int i = 0; i<sizeof(_leds); i+=4){
        leds.sendColor(_leds[i+3],_leds[i+2],_leds[i+1],_leds[i]);
    }
    leds.endFrame(NUM_LEDS);
}


void Ctrl::setLedLeft(byte red, byte green, byte blue, byte brightness)
{
    setLed(LED_LEFT, red, green, blue, brightness);
}

void Ctrl::setLedRight(byte red, byte green, byte blue, byte brightness)
{
    setLed(LED_RIGHT, red, green, blue, brightness);
}

void Ctrl::setLeds(byte ledsParam[])
{
    memcpy(_leds, ledsParam, sizeof(_leds));
/*
    bitBangSPI(APA102_START_FRAME, sizeof (APA102_START_FRAME));
    bitBangSPI(_leds, sizeof(_leds));
    bitBangSPI(APA102_END_FRAME, sizeof (APA102_END_FRAME));
*/
    leds.startFrame();
    for(int i = 0; i<sizeof(_leds); i+=4){
        leds.sendColor(_leds[i+3],_leds[i+2],_leds[i+1],_leds[i]);
    }
    leds.endFrame(NUM_LEDS);
}

void Ctrl::loop()
{
    int btnX, btnA, btnB, btnY, btnUP, btnRIGHT, btnDOWN, btnLEFT;



    btnX = digitalRead(BTN_X_PIN) ? LOW : HIGH;
    btnA = digitalRead(BTN_A_PIN) ? LOW : HIGH;
    btnB = digitalRead(BTN_B_PIN) ? LOW : HIGH;
    btnY = digitalRead(BTN_Y_PIN) ? LOW : HIGH;
    btnUP = digitalRead(BTN_UP_PIN) ? LOW : HIGH;
    btnDOWN = digitalRead(BTN_DOWN_PIN) ? LOW : HIGH;
    btnLEFT = digitalRead(BTN_LEFT_PIN) ? LOW : HIGH;
    btnRIGHT = digitalRead(BTN_RIGHT_PIN) ? LOW : HIGH;
    
    delay(0);

#ifdef BTN_DEBUG
    if(btnX){
        Serial.print('X');
    } else {
        Serial.print('x');
    }
    if(btnY){
        Serial.print('Y');
    } else {
        Serial.print('y');
    }
    if(btnA){
        Serial.print('A');
    } else {
        Serial.print('a');
    }
    if(btnB){
        Serial.print('B');
    } else {
        Serial.print('b');
    }
    if(btnUP){
        Serial.print('U');
    } else {
        Serial.print('u');
    }
    if(btnDOWN){
        Serial.print('D');
    } else {
        Serial.print('d');
    }
    if(btnLEFT){
        Serial.print('L');
    } else {
        Serial.print('l');
    }
    if(btnRIGHT){
        Serial.println('R');
    } else {
        Serial.println('r');
    }
#endif

    debounceButton(BTN_UP, btnUP);
    debounceButton(BTN_RIGHT, btnRIGHT);
    debounceButton(BTN_DOWN, btnDOWN);
    debounceButton(BTN_LEFT, btnLEFT);
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

/*
void Ctrl::bitBangSPI(uint8_t * data, uint32_t size){

    for(uint32_t k=size; k>=0; k--){

        for(uint8_t i=7; i>=0 ;i--) {

            if((data[k] >> i)& 1){
                digitalWrite(DATA, HIGH);
            }else{
                digitalWrite(DATA, LOW);
            }
            digitalWrite(CLK, HIGH);
            digitalWrite(CLK, LOW);

        }
    }
}
*/
Ctrl Cytrill;
