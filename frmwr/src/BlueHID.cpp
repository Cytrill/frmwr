#include "BlueHID.hpp"

BLEHIDDevice* BlueHID::_hid = NULL;
BLECharacteristic* BlueHID::_input = NULL;
BLECharacteristic* BlueHID::_output = NULL;
BlueConnection* _inputCallback = NULL;
BlueHID::BlueHID()
{

}

BlueHID::~BlueHID()
{
}



void BlueHID::taskServer(void*) {


  BLEDevice::init("cytrill");



  BLEServer *pServer = BLEDevice::createServer();



  _hid = new BLEHIDDevice(pServer);
  _input = _hid->inputReport(1); // <-- input REPORTID from report map
  _output = _hid->outputReport(1); // <-- output REPORTID from report map
  _inputCallback = new BlueConnection(_input);
  pServer->setCallbacks(_inputCallback);


  std::string name = "shguro";
  _hid->manufacturer()->setValue(name);

  _hid->pnp(0x02, 0xe502, 0xa111, 0x0210);
  _hid->hidInfo(0x00, 0x02);

  BLESecurity *pSecurity = new BLESecurity();
  // pSecurity->setKeySize();
  pSecurity->setAuthenticationMode(ESP_LE_AUTH_BOND);

  const uint8_t report[] = {
    USAGE_PAGE(1),      0x01,       // Generic Desktop Ctrls
    USAGE(1),           0x05,       // Game Pad
    COLLECTION(1),      0x01,       // Application
    REPORT_ID(1),       0x01,        //   Report ID (4)
    USAGE_PAGE(1),      0x09,       //   button
    USAGE_MINIMUM(1),   0x01,       //Usage_Minimum(Button 1)
    USAGE_MAXIMUM(1),   0x08,       //Usage_maximum(Button 8)
    LOGICAL_MINIMUM(1), 0x00,
    LOGICAL_MAXIMUM(1), 0x01,
    REPORT_SIZE(1),     0x01,       //   1 byte (Modifier)
    REPORT_COUNT(1),    0x08,
    HIDINPUT(1),        0x02,       //   Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position
    END_COLLECTION(0)
  };

  _hid->reportMap((uint8_t*)report, sizeof(report));
  _hid->startServices();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->setAppearance(HID_GAMEPAD);
  pAdvertising->addServiceUUID(_hid->hidService()->getUUID());
  pAdvertising->start();
  _hid->setBatteryLevel(7);



  delay(portMAX_DELAY);

};


void BlueHID::setup()
{
  xTaskCreatePinnedToCore(taskServer, "server", 10000, NULL, 5, NULL,1);
  Cytrill.setLedLeft(0x00, 0x00, 0x55, 1);
  Cytrill.setLedRight(0x00, 0x00, 0x55, 1);
  delay(1000);
  lastMsg = 0;

}

void BlueHID::loop()
{
    Cytrill.loop();
    uint8_t msg[] {Cytrill.getButtons()};
    if(_inputCallback->getConnectionStatus() && msg[0]!=lastMsg) {
      lastMsg = msg[0];
      _input->setValue(msg, sizeof(msg));
      _input->notify();
    }

}
