#include "../../include/controller/LedController.h"

Adafruit_NeoPixel *LedController::mPixels = new Adafruit_NeoPixel(WS2812_LED_COUNT, WS2812_LED_PIN, NEO_GRB + NEO_KHZ800);

void LedController::init()
{
    mPixels->begin();
}

void LedController::loop()
{
}