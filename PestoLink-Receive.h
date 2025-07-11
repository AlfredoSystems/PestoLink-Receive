#ifndef PESTOLINK_H
#define PESTOLINK_H

#include "Arduino.h"
#include <inttypes.h>
#include <ArduinoBLE.h>
#include "Keys.h"

class PestoLinkParser {
    public:
        void begin(const char *localName);
        bool update(){return _isConnected;};
        bool isConnected(){return _isConnected;};

        float getAxis(uint8_t button_num);
        uint8_t getRawAxis(uint8_t button_num);
        bool buttonHeld(uint8_t button_num);
		bool keyHeld(Key key);

        void printBatteryVoltage(float batteryVoltage);
        void printTelemetry(const char *telemetry, const char *hexCode = "0x808080");	

        void clearTerminal();
        void printTerminal(const char *text);
        void printfTerminal(const char * format, ... );
        void setTerminalPeriod(uint32_t period){_TerminalPeriodMs = period;};	
        
        volatile bool _isConnected;

    private:
        uint32_t _TerminalPeriodMs;
        uint32_t _lastTelemetryMs;
        uint32_t _lastTerminalMs;
};

extern PestoLinkParser PestoLink;

#endif //PESTOLINK_H