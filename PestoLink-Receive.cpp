#include "PestoLink-Receive.h"

PestoLinkParser PestoLink;

#define TERMINAL_CHAR_LENGTH 64
#define TERMINAL_CTRL_LENGTH 1
#define TERMINAL_RESULT_LENGTH TERMINAL_CHAR_LENGTH + TERMINAL_CTRL_LENGTH

BLEService ServicePestoBle("27df26c5-83f4-4964-bae0-d7b7cb0a1f54");

BLECharacteristic CharacteristicGamepad("452af57e-ad27-422c-88ae-76805ea641a9", BLEWriteWithoutResponse, 18, true);
BLECharacteristic	CharacteristicTelemetry("266d9d74-3e10-4fcd-88d2-cb63b5324d0c", BLERead | BLENotify, 11, true);
BLECharacteristic CharacteristicTerminal("433ec275-a494-40ab-98c2-4785a19bf830", BLERead | BLENotify, TERMINAL_RESULT_LENGTH, true);

char terminalText[TERMINAL_CHAR_LENGTH]; // make space to create terminal text in for printfTerminal

void PestoLinkParser::begin(const char *localName) {
  _isConnected = false;
	_lastTelemetryMs = 0;
  _lastTerminalMs = 0;
  _TerminalPeriodMs = 200;

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

bool PestoLinkParser::update() {
  BLEDevice central = BLE.central();

  _isConnected = false;

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
  _isConnected = true;

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

void PestoLinkParser::printBatteryVoltage(float batteryVoltage){
  char voltageString[8];       // Array to hold the resulting string

  dtostrf(batteryVoltage, 5, 2, voltageString);  // 4 width, 2 decimal places
  strcat(voltageString, " V");  // Append " V" to the string

  if(batteryVoltage >= 7.6) {
      printTelemetry(voltageString, "00FF00");
  } else if (batteryVoltage >= 7) {
      printTelemetry(voltageString, "FFFF00");
  } else {
      printTelemetry(voltageString, "FF0000");
  }
}

void PestoLinkParser::printTelemetry(const char *telemetry,const char *hexCode){
  if(_lastTelemetryMs + 500 > millis()) return;

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

  _lastTelemetryMs = millis();
}

void PestoLinkParser::clearTerminal(){

  if(_lastTerminalMs + _TerminalPeriodMs > millis())return;

  uint8_t result[TERMINAL_RESULT_LENGTH];

  for(int i = 0; i < TERMINAL_RESULT_LENGTH ; i++){
    result[i] = 0;
  }

  //2 means clear the terminal
  result[0] = {2};

  CharacteristicTerminal.writeValue(result, TERMINAL_RESULT_LENGTH, false); 

  _lastTerminalMs = millis();
}

void PestoLinkParser::printTerminal(const char *text){
  if(_lastTerminalMs + _TerminalPeriodMs > millis()) return;

  uint8_t result[TERMINAL_RESULT_LENGTH];

  for(int i = 0; i < TERMINAL_RESULT_LENGTH ; i++){
    result[i] = 0;
  }

  //1 means normal message
  result[0] = 1;

  int index = 0;

  // Loop over the first 64 characters of the input
  while (index < TERMINAL_CHAR_LENGTH) {
      if (text[index] == '\0') break;

      // If there's a character at this position, use its ASCII value        
      result[index+1] = static_cast<uint8_t>(text[index]);

      index++;
  }

  CharacteristicTerminal.writeValue(result, TERMINAL_RESULT_LENGTH, false); 

  _lastTerminalMs = millis();
}

void PestoLinkParser::printfTerminal(const char * format, ... ){
  va_list args;
  va_start(args, format);
  // https://cplusplus.com/reference/cstdio/vsnprintf/
  vsnprintf(terminalText, TERMINAL_CHAR_LENGTH, format, args);
  va_end(args);
  printTerminal(terminalText);
}