#include "PestoLink-Receive.h"

PestoLinkParser PestoLink;

#define TERMINAL_CHAR_LENGTH 64
#define TERMINAL_CTRL_LENGTH 1
#define TERMINAL_RESULT_LENGTH (TERMINAL_CHAR_LENGTH + TERMINAL_CTRL_LENGTH)

BLEService ServicePestoBle("27df26c5-83f4-4964-bae0-d7b7cb0a1f54");

BLECharacteristic CharacteristicGamepad("452af57e-ad27-422c-88ae-76805ea641a9", BLEWriteWithoutResponse, 18, true);
BLECharacteristic CharacteristicTelemetry("266d9d74-3e10-4fcd-88d2-cb63b5324d0c", BLERead | BLENotify, 12, true);
BLECharacteristic CharacteristicTerminal("433ec275-a494-40ab-98c2-4785a19bf830", BLERead | BLENotify, TERMINAL_RESULT_LENGTH, true);

char terminalText[TERMINAL_CHAR_LENGTH];

static uint8_t telemetryPacket[12];
static portMUX_TYPE telemetryMux = portMUX_INITIALIZER_UNLOCKED;

void taskUpdatePestoLink(void* pvParameters) {
  while (true) {
    BLEDevice central = BLE.central();
    bool connected = central && central.connected() && (*(CharacteristicGamepad.value()) == 0x01);

    PestoLink._isConnected = connected;

    static uint32_t lastTelemetrySend = 0;
    if (millis() - lastTelemetrySend >= 200) {
      lastTelemetrySend = millis();

      uint8_t telemetryData[12];
      portENTER_CRITICAL(&telemetryMux);
      memcpy(telemetryData, telemetryPacket, sizeof(telemetryPacket));
      if (telemetryPacket[11] > 0) {
        telemetryPacket[11] = 0;
      }
      portEXIT_CRITICAL(&telemetryMux);

      CharacteristicTelemetry.writeValue(telemetryData, 12, false);
    }

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}


void PestoLinkParser::begin(const char *localName) {
  _isConnected = false;
  _lastTerminalMs = 0;
  _TerminalPeriodMs = 200;
  _taskHandle = NULL;
  memset(telemetryPacket, 0, sizeof(telemetryPacket));

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

  if (_taskHandle == NULL) {
    xTaskCreatePinnedToCore(taskUpdatePestoLink, "taskUpdatePestoLink", 8192, NULL, 2, &_taskHandle, 1);
  }
}

float PestoLinkParser::getAxis(uint8_t axis_num) {
  float axis_raw = (float) getRawAxis(axis_num);
  return (axis_raw / 127.5) - 1;
}

uint8_t PestoLinkParser::getRawAxis(uint8_t axis_num) {
  if (axis_num >= 4) return 0;
  uint8_t value = (uint8_t)*(CharacteristicGamepad.value() + axis_num + 0x01);
  return value;
}

bool PestoLinkParser::buttonHeld(uint8_t button_num) {
  if (button_num >= 16) return false;
  uint8_t raw_buttons_LSB = (uint8_t)*(CharacteristicGamepad.value() + 5);
  uint8_t raw_buttons_MSB = (uint8_t)*(CharacteristicGamepad.value() + 6);

  uint16_t raw_buttons = (((uint16_t)(raw_buttons_MSB)) << 8) + (uint16_t)(raw_buttons_LSB);

  return (bool)((raw_buttons >> (button_num)) & 0x01);
}

bool PestoLinkParser::keyHeld(Key key) {
  for (int i = 7; i < 18; ++i) {
    uint8_t keyNum = (uint8_t)*(CharacteristicGamepad.value() + i);
    if (keyNum == static_cast<uint8_t>(key)) {
      return true;
    }
  }
  return false;
}

void PestoLinkParser::printBatteryVoltage(float batteryVoltage) {
  char voltageString[12];

  dtostrf(batteryVoltage, 5, 2, voltageString);
  strcat(voltageString, " V");

  if (batteryVoltage >= 7.6) {
    printTelemetry(voltageString, "00FF00");
  } else if (batteryVoltage >= 7) {
    printTelemetry(voltageString, "FFFF00");
  } else {
    printTelemetry(voltageString, "FF0000");
  }
}

void PestoLinkParser::printTelemetry(const char *telemetry, const char *hexCode) {
  if (hexCode[0] == '0' && hexCode[1] == 'x') hexCode += 2;
  long color = strtol(hexCode, nullptr, 16);

  portENTER_CRITICAL(&telemetryMux);
  memset(telemetryPacket, 0, 8);
  for (int i = 0; i < 8; i++) {
    if (telemetry[i] == '\0') break;
    telemetryPacket[i] = static_cast<uint8_t>(telemetry[i]);
  }
  telemetryPacket[8] = (color >> 16) & 0xFF;
  telemetryPacket[9] = (color >> 8) & 0xFF;
  telemetryPacket[10] = color & 0xFF;
  portEXIT_CRITICAL(&telemetryMux);
}

void PestoLinkParser::rumble() {
  portENTER_CRITICAL(&telemetryMux);
  telemetryPacket[11] = 1;
  portEXIT_CRITICAL(&telemetryMux);
}

void PestoLinkParser::clearTerminal() {
  if (millis() - _lastTerminalMs < _TerminalPeriodMs) return;

  uint8_t result[TERMINAL_RESULT_LENGTH];
  memset(result, 0, sizeof(result));
  result[0] = 2;

  CharacteristicTerminal.writeValue(result, TERMINAL_RESULT_LENGTH, false);
  _lastTerminalMs = millis();
}

void PestoLinkParser::printTerminal(const char *text) {
  if (millis() - _lastTerminalMs < _TerminalPeriodMs) return;

  uint8_t result[TERMINAL_RESULT_LENGTH];
  memset(result, 0, sizeof(result));
  result[0] = 1;

  int index = 0;
  while (index < TERMINAL_CHAR_LENGTH) {
    if (text[index] == '\0') break;
    result[index + 1] = static_cast<uint8_t>(text[index]);
    index++;
  }

  CharacteristicTerminal.writeValue(result, TERMINAL_RESULT_LENGTH, false);
  _lastTerminalMs = millis();
}

void PestoLinkParser::printfTerminal(const char *format, ...) {
  va_list args;
  va_start(args, format);
  vsnprintf(terminalText, TERMINAL_CHAR_LENGTH, format, args);
  va_end(args);
  printTerminal(terminalText);
}
