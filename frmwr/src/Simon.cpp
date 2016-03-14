#include "Simon.hpp"

Simon::Simon()
    : _pattern{ 0, },
      _correctPresses(0),
      _level(1),
      _mode(0)
{
}

Simon::~Simon()
{
}

void Simon::setup()
{
    reset();

    Cytrill.registerButtonRightCallback([this](bool pressed)
    {
        if (_mode == 0)
        {
            _mode = 1;
        }
    });

    Cytrill.registerButtonsCallback([this](int button, bool pressed)
    {
        if (pressed && _mode == 3)
        {
            if ((button == BTN_X && _pattern[_correctPresses] == 0) ||
                (button == BTN_A && _pattern[_correctPresses] == 1) ||
                (button == BTN_B && _pattern[_correctPresses] == 2) ||
                (button == BTN_Y && _pattern[_correctPresses] == 3))
            {
                _correctPresses++;

                if (_correctPresses == _level)
                {
                    Cytrill.setLedLeft(0xFF, 0xFF, 0xFF, _level);

                    _level++;

                    _correctPresses = 0;
                    _mode = 0;

                    if (_level == 20)
                    {
                        Cytrill.setLedLeft(0x00, 0x00, 0xFF, 31);

                        reset();

                        _level = 1;
                    }
                }
            }
            else
            {
                Cytrill.setLedLeft(0xFF, 0xFF, 0xFF, 0);

                reset();

                _level = 1;

                _correctPresses = 0;
                _mode = 0;
            }
        }
    });
}

void Simon::loop()
{
    static int playedColor = 0;
    static long lastTime = 0;

    if (_mode == 1)
    {
      if (playedColor == _level)
      {
          Cytrill.setLedRight(0x00, 0x00, 0x00, 6);

          playedColor = 0;
          _mode = 3;
      }
      else if (millis() - lastTime > 100)
      {
          lastTime = millis();

          if (_pattern[playedColor] == 0)
          {
              Cytrill.setLedRight(0x00, 0x00, 0xFF, 8);
          }
          else if (_pattern[playedColor] == 1)
          {
              Cytrill.setLedRight(0xFF, 0x00, 0x00, 8);
          }
          else if (_pattern[playedColor] == 2)
          {
              Cytrill.setLedRight(0xFF, 0xFF, 0x00, 8);
          }
          else if (_pattern[playedColor] == 3)
          {
              Cytrill.setLedRight(0x00, 0xFF, 0x00, 8);
          }

          playedColor++;
          _mode = 2;
        }
    }
    else if (_mode == 2)
    {
        if (millis() - lastTime > 600)
        {
            lastTime = millis();

            Cytrill.setLedRight(0x00, 0x00, 0x00, 6);

            _mode = 1;
        }
    }

    Cytrill.loop();
}

void Simon::reset()
{
    for (int i = 0; i < 32; i++)
    {
        _pattern[i] = random(0, 4);
    }
}
