#ifndef _LED_CONTROLLER_H_
#define _LED_CONTROLLER_H_

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "../common/PinDefines.h"

class LedController
{
private:
    static Adafruit_NeoPixel *mPixels;

public:
    static void init();
    static void loop();
};

#endif