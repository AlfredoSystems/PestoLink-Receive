#ifndef PESTOLINK_H
#define PESTOLINK_H

#include "Arduino.h"
#include <inttypes.h>
#include <ArduinoBLE.h>
#include "Keys.h"

class PestoLinkParser {
    public:
        void begin(const char *localName);
        bool update();
        float getAxis(uint8_t button_num);
        uint8_t getRawAxis(uint8_t button_num);
        bool buttonHeld(uint8_t button_num);
		bool keyHeld(Key key);

        void setBatteryVal(float batteryVoltage);
        void printBatteryVoltage(float batteryVoltage);
        void print(const char *telemetry, const char *hexCode = "0x808080");	
        void printToTerminal(const char *text);
        void printfToTerminal(const char * format, ... );
	private:
		uint32_t lastTelemetryMs;
    uint32_t lastTerminalMs;
};

extern PestoLinkParser PestoLink;

#endif //PESTOLINK_H
