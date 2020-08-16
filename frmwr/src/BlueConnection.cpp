#include "BlueConnection.hpp"


  BlueConnection::BlueConnection(BLECharacteristic* input) : _connected(false)
  {
    _input = input;
  }

  BlueConnection::~BlueConnection()
  {
  }
    void BlueConnection::onConnect(BLEServer* pServer)
    {
      _connected = true;
      Serial.println("connected");
      BLE2902* desc = (BLE2902*)_input->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
      desc->setNotifications(true);

    }

    void BlueConnection::onDisconnect(BLEServer* pServer) {
      _connected = false;
      Serial.println("diconnected");
      BLE2902* desc = (BLE2902*)_input->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
      desc->setNotifications(false);

    }
    bool BlueConnection::getConnectionStatus() {
      return _connected;
    }
