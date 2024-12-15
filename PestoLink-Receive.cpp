#include "PestoLink-Receive.h"

PestoLinkParser PestoLink;

BLEService ServicePestoBle("27df26c5-83f4-4964-bae0-d7b7cb0a1f54");

BLECharacteristic CharacteristicGamepad("452af57e-ad27-422c-88ae-76805ea641a9", BLEWriteWithoutResponse, 18, true);
BLECharacteristic	CharacteristicTelemetry("266d9d74-3e10-4fcd-88d2-cb63b5324d0c", BLERead | BLENotify, 11, true);
BLECharacteristic CharacteristicTerminal("433ec275-a494-40ab-98c2-4785a19bf830", BLERead | BLENotify, 1024, true);

#define MAX_TERMINAL_LENGTH 64
char terminalText[MAX_TERMINAL_LENGTH]; // make space to create terminal text in

void PestoLinkParser::begin(const char *localName) {
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!");
    while (1);
  }

  BLE.setLocalName(localName);
  BLE.setAdvertisedService(ServicePestoBle);

  ServicePestoBle.addCharacteristic(CharacteristicGamepad);
  ServicePestoBle.addCharacteristic(CharacteristicTelemetry);
  ServicePestoBle.addCharacteristic(CharacteristicTerminal);
  BLE.addService(ServicePestoBle);
  
  int8_t emptyGamepad[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  CharacteristicGamepad.writeValue(emptyGamepad, 18, false); 
  
  BLE.advertise();
}

//Todo: create a seperate "bool isConnected()" function?
bool PestoLinkParser::update() {
  BLEDevice central = BLE.central();

  if (!central) {
	return false;
  }
  
  if (!central.connected()) {
	return false;
  }
  
  if((uint8_t)*(CharacteristicGamepad.value()) != 0x01){
    return false;
  }
  
  //for(int i = 0; i < 20; i++){
  //  Serial.print((uint8_t)*(CharacteristicGamepad.value() + i)); Serial.print(", ");
  //}
  //Serial.println(" ");
  
  return true;
}

float PestoLinkParser::getAxis(uint8_t axis_num) {
  float axis_raw = (float) getRawAxis(axis_num);
  return (axis_raw / 127.5) - 1;
}

uint8_t PestoLinkParser::getRawAxis(uint8_t axis_num) {
  return (uint8_t)*( CharacteristicGamepad.value() + axis_num + 0x01);
}

bool PestoLinkParser::buttonHeld(uint8_t button_num) {
  
  uint8_t raw_buttons_LSB = (uint8_t)*( CharacteristicGamepad.value() + 5);
  uint8_t raw_buttons_MSB = (uint8_t)*( CharacteristicGamepad.value() + 6);

  uint16_t raw_buttons = (((uint16_t)(raw_buttons_MSB)) << 8) + (uint16_t)(raw_buttons_LSB);
  
  return (bool)((raw_buttons >> (button_num)) & 0x01);
}

bool PestoLinkParser::keyHeld(Key key) {

    // Start checking from the 7th byte (index 7) to the 17th byte (index 17)
    for (int i = 7; i < 18; ++i) {
		uint8_t keyNum = (uint8_t)*(CharacteristicGamepad.value() + i);
        if (keyNum == static_cast<uint8_t>(key)) {
            return true;
        }
    }

    // If the key is not found in the last 11 bytes, return false
    return false;
}

void PestoLinkParser::setBatteryVal(float batteryVoltage){
  printBatteryVoltage(batteryVoltage);
}


void PestoLinkParser::printBatteryVoltage(float batteryVoltage){
  char voltageString[8];       // Array to hold the resulting string

  dtostrf(batteryVoltage, 5, 2, voltageString);  // 4 width, 2 decimal places
  strcat(voltageString, " V");  // Append " V" to the string

  if(batteryVoltage >= 7.6) {
      print(voltageString, "00FF00");
  } else if (batteryVoltage >= 7) {
      print(voltageString, "FFFF00");
  } else {
      print(voltageString, "FF0000");
  }
}

void PestoLinkParser::print(const char *telemetry,const char *hexCode){
  if(lastTelemetryMs + 500 > millis()){
    return;
  }

  uint8_t result[11];

  // Loop over the first eight characters of the input
  for (int i = 0; i < 8; i++) {
      // If there's a character at this position, use its ASCII value
      if (telemetry[i] != '\0') {
          result[i] = static_cast<uint8_t>(telemetry[i]);
      } else {
          // If we're out of characters, set the rest to null (0)
          result[i] = 0;
      }
  }

  // Adjust pointer if the hex code starts with "0x"
  if (hexCode[0] == '0' && hexCode[1] == 'x') hexCode += 2;
  long color = strtol(hexCode, nullptr, 16);
  result[8] = (color >> 16) & 0xFF;
  result[9] = (color >> 8) & 0xFF;
  result[10] = color & 0xFF;
  
  CharacteristicTelemetry.writeValue(result, 11, false); 

  lastTelemetryMs = millis();
}

/**
  example:
  int number = millis(); // just an example of a number
  //  snprintf is a great way to add numbers to text in C++ https://www.geeksforgeeks.org/sprintf-in-c/ It's a bit strange but I think you'll find it useful. ("text"+number doesn't work in C++)
  snprintf(text, 64, "distance: %d", number);
  PestoLink.printToTerminal(text);
 */
void PestoLinkParser::printToTerminal(const char *text){
  if(lastTerminalMs + 500 > millis()){
    return;
  }

  uint8_t result[MAX_TERMINAL_LENGTH];

  // Loop over the first 64 characters of the input
  for (int i = 0; i < MAX_TERMINAL_LENGTH; i++) {
      // If there's a character at this position, use its ASCII value
      if (text[i] != '\0') {
          result[i] = static_cast<uint8_t>(text[i]);
      } else {
          // If we're out of characters, set the rest to null (0)
          for( ; i < MAX_TERMINAL_LENGTH; i++){
	          result[i] = 0;
	      }
      }
  }
  
  CharacteristicTerminal.writeValue(result, 64, false); 

  lastTerminalMs = millis();
}

/**
  example:
  int number = millis(); // just an example of a number
  PestoLink.printfToTerminal("distance: %d", number);
*/
void PestoLinkParser::printfToTerminal(const char * format, ... ){
  va_list args;
  va_start(args, format);
  // https://cplusplus.com/reference/cstdio/vsnprintf/
  vsnprintf(terminalText, MAX_TERMINAL_LENGTH, format, args);
  va_end(args);
  printToTerminal(terminalText);
}
