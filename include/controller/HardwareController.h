#ifndef _HARDWARE_CONTROLLER_H_
#define _HARDWARE_CONTROLLER_H_

#include <Arduino.h>
#include <DHT_Async.h>
#include "../common/PinDefines.h"
#include "../common/Log.h"
#include "../common/MessageEvent.h"
#include "../controller/LedController.h"
#include "../manager/SettingsManager.h"

class HardwareController
{
private:
    static int mTouchPin[TOUCH_PIN_COUNT];
    static int mBuzzerCount;
    static float mTemperature;
    static float mHumidity;
    static DHT_Async mDHT;

public:
    static void init();
    static void loop();
    static void bip(int n);
    static float getTemperature();
    static float getHumidity();
    static bool isValidDhtValue(float value);

private:
    static void buttonHandler();
    static void buzzerHandler();
    static void dhtHandler();
    static void setBuzzerState(bool state);
};

#endif