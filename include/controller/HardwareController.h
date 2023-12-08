#ifndef _HARDWARE_CONTROLLER_H_
#define _HARDWARE_CONTROLLER_H_

#include <Arduino.h>
#include "../common/Log.h"

#define TOUCH_PIN_COUNT 3

#define BUZZER_PIN 2
#define TOUCH_PIN_ENTER T6
#define TOUCH_PIN_UP T5
#define TOUCH_PIN_DOWN T4

#define TOUCH_THRESHOLD 40
#define TOUCH_SHORT_PRESS_TIME 70
#define TOUCH_SHORT_PRESS_TIMEOUT 700
#define TOUCH_LONG_PRESS_TIME 3000

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