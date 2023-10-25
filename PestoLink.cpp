#include "PestoLink.h"

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_PESTOLINK)
PestoLinkParser PestoLink;
#endif

BLEService        ServicePestoBle("27df26c5-83f4-4964-bae0-d7b7cb0a1f54");
BLECharacteristic CharacteristicGamepad("452af57e-ad27-422c-88ae-76805ea641a9", BLEWriteWithoutResponse, 6, true);

void PestoLinkParser::begin(char *localName) {
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!");
    while (1);
  }

  BLE.setLocalName(localName);
  BLE.setAdvertisedService(ServicePestoBle);

  ServicePestoBle.addCharacteristic(CharacteristicGamepad);
  BLE.addService(ServicePestoBle);

  BLE.advertise();
}

//Todo: create a seperate "bool isConnected()" function?
bool PestoLinkParser::update() {
  BLEDevice central = BLE.central();

  if (central) {
    if (central.connected()) {
      return true;
    }
  }
  return false;
}

float PestoLinkParser::getAxis(uint8_t axis_num) {
  float axis_raw = (float) getRawAxis(axis_num);
  return (axis_raw / 127.5) - 1;
}

uint8_t PestoLinkParser::getRawAxis(uint8_t axis_num) {
  return (uint8_t)*( CharacteristicGamepad.value() + axis_num);
}

bool PestoLinkParser::buttonHeld(uint8_t button_num) {
  uint8_t raw_buttons = (uint8_t)*( CharacteristicGamepad.value() + 4);
  return (bool)((raw_buttons >> (button_num)) & 0x01);
}