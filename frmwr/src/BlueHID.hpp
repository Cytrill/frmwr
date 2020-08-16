#ifndef BLUEHID_HPP
#define BLUEHID_HPP

#include <Arduino.h>

#include "Ctrl32.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "BLE2902.h"
#include "BLEHIDDevice.h"
#include "HIDTypes.h"
#include "BlueConnection.hpp"


class BlueHID
{
public:
    BlueHID();
    ~BlueHID();

    void setup();
    void loop();

    void reset();


private:

    static void taskServer(void*);
    static bool _connected;
    static BLEHIDDevice* _hid;
    static BLECharacteristic* _input;
    static BLECharacteristic* _output;

    uint8_t lastMsg;




};

#endif
