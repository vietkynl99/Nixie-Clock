#ifndef _HARDWARE_CONTROLLER_H_
#define _HARDWARE_CONTROLLER_H_

#include <Arduino.h>
#include "../common/Configs.h"
#include "../common/Log.h"
#include "../common/MessageEvent.h"
#include "../controller/LedController.h"
#include "../manager/SettingsManager.h"
#include "../fragment/BootFragment.h"

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
    static void initBuzzer();
    static void buttonHandler();
    static void buzzerHandler();
    static void setBuzzerState(bool state);
};

#endif