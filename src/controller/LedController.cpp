#include "../../include/controller/LedController.h"

Adafruit_NeoPixel *LedController::mPixels = new Adafruit_NeoPixel(WS2812_LED_COUNT, WS2812_LED_PIN, NEO_GRB + NEO_KHZ800);
bool LedController::mNeedsRedraw = true;
int LedController::mMode = LED_MODE_MAX;
uint16_t LedController::mGHue = 0;

static constexpr const char *const TAG = "LED";

void LedController::init()
{
    mPixels->begin();
    turnOff();
    reloadBrightness();
    reloadMode();
}

void LedController::loop()
{
    switch (mMode)
    {
    case LED_MODE_WAVE:
        waveModeHandler();
        break;
    default:
        break;
    }

    if (mNeedsRedraw)
    {
        mNeedsRedraw = false;
        mPixels->show();
    }
}

void LedController::reloadBrightness()
{
    mPixels->setBrightness(SettingsManager::getLedBrightness() * 255 / 10);
}

void LedController::reloadMode()
{
    setMode(SettingsManager::getLedMode());
}

void LedController::setMode(int mode)
{
    if (mode > LED_MODE_MAX)
    {
        return;
    }
    if (mMode != mode)
    {
        mMode = mode;
        LOG("Mode changed to %d", mode);
        mGHue = 0;
        if (mMode == LED_MODE_MAX)
        {
            turnOff();
        }
    }
}

String LedController::getModeName(int mode)
{
    switch (mode)
    {
    case LED_MODE_WAVE:
        return "Wave";
    case LED_MODE_RAINBOW:
        return "Rainbow";
    case LED_MODE_MAX:
        return "Off";
    default:
        return "Unknown";
    }
}

void LedController::fillColor(uint16_t hue, uint8_t sat, uint8_t val)
{
    for (int i = 0; i < WS2812_LED_COUNT; i++)
    {
        mPixels->setPixelColor(i, mPixels->gamma32(mPixels->ColorHSV(hue, sat, val)));
    }
    mNeedsRedraw = true;
}

void LedController::turnOff()
{
    fillColor(0, 0, 0);
}

void LedController::waveModeHandler()
{
    static uint32_t timeTick = 0;
    if (xTaskGetTickCount() > timeTick)
    {
        timeTick = xTaskGetTickCount() + 20 / portTICK_PERIOD_MS;
        mGHue += map(SettingsManager::getLedSpeed(), 1, 10, 20, 500);
        fillColor(mGHue);
    }
}