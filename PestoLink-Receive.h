#ifndef PESTOLINK_H
#define PESTOLINK_H

#include "Arduino.h"
#include <inttypes.h>
#include <ArduinoBLE.h>
#include "Keys.h"

class PestoLinkParser {
    public:
        void begin(char *localName);
        bool update();
        float getAxis(uint8_t button_num);
        uint8_t getRawAxis(uint8_t button_num);
        bool buttonHeld(uint8_t button_num);
        void setBatteryVal(float battery_val);
		bool keyHeld(Key key);

	
	private:
		uint8_t _batteryVal = 0;
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_PESTOLINK)
extern PestoLinkParser PestoLink;
#endif

#endif //PESTOLINK_H