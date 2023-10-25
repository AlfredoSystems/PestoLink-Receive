#ifndef ALFREDOCONNECT_H
#define ALFREDOCONNECT_H

#include "Arduino.h"
#include <inttypes.h>
#include <String.h>
#include <ArduinoBLE.h>

class PestoLinkParser {
    public:
        void begin(char *localName);
        bool update();
        float getAxis(uint8_t button_num);
        uint8_t getRawAxis(uint8_t button_num);
        bool buttonHeld(uint8_t button_num);
		
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_PESTOLINK)
extern PestoLinkParser PestoLink;
#endif

#endif