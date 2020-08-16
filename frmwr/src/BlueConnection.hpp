#ifndef BLUECONNECTION_HPP
#define BLUECONNECTION_HPP

#include <Arduino.h>

#include "BLE2902.h"
#include <BLEServer.h>



class BlueConnection : public BLEServerCallbacks
{
public:
    BlueConnection(BLECharacteristic*);
    ~BlueConnection();

    void onConnect(BLEServer*);
    void onDisconnect(BLEServer*);

    bool getConnectionStatus();


private:
    BLECharacteristic* _input;
    bool _connected;

};

#endif
