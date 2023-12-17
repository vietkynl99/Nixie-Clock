#ifndef _HARDWARE_CONTROLLER_H_
#define _HARDWARE_CONTROLLER_H_

#include <Arduino.h>
#include "../common/PinDefines.h"
#include "../common/Log.h"
#include "../common/MessageEvent.h"

#define TOUCH_THRESHOLD 40
#define TOUCH_SHORT_PRESS_TIME 70
#define TOUCH_SHORT_PRESS_TIMEOUT 700
#define TOUCH_LONG_PRESS_TIME 1000

// #define ENABLE_TOUCH_DEBUG

class HardwareController
{
private:
    static int mTouchPin[TOUCH_PIN_COUNT];
    static int mBuzzerCount;

public:
    static void init();
    static void loop();
    static void bip(int n);

private:
    static void buttonHandler();
    static void buzzerHandler();
};

#endif