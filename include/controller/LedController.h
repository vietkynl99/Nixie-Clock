#ifndef _LED_CONTROLLER_H_
#define _LED_CONTROLLER_H_

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "../common/Configs.h"
#include "../common/Helper.h"
#include "../manager/SettingsManager.h"

enum LedMode
{
    LED_MODE_WAVE,
    LED_MODE_RAINBOW,
    LED_MODE_MAX
};

class LedController
{
private:
    static Adafruit_NeoPixel *mPixels;
    static bool mNeedsRedraw;
    static int mMode;
    static uint16_t mGHue;

public:
    static void init();
    static void loop();
    static void reloadBrightness();
    static void reloadMode();
    static void setMode(int mode);
    static String getModeName(int mode);

private:
    static void fillColor(uint16_t hue, uint8_t sat = 255, uint8_t val = 255);
    static void turnOff();
    static void waveModeHandler();
};

#endif