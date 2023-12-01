#include "../include/ClockFragment.h"

bool ClockFragment::mIsVisible = false;
bool ClockFragment::mNeedsRedraw = false;
bool ClockFragment::mIsFirstTime = false;

static constexpr const char *const TAG = "CLOCK";

void ClockFragment::init()
{
    DisplayController::init();
}

void ClockFragment::loop()
{
    static uint32_t timeTick = 0, digitTimeTick = 0;
    static int digit = 0;

    if (mIsVisible && xTaskGetTickCount() > digitTimeTick)
    {
        digitTimeTick = xTaskGetTickCount() + 1000 / portTICK_PERIOD_MS;
        digit++;
        if (digit > 9)
        {
            digit = 0;
        }
        mNeedsRedraw = true;
    }
    if (mNeedsRedraw && xTaskGetTickCount() > timeTick)
    {
        timeTick = xTaskGetTickCount() + 10 / portTICK_PERIOD_MS;
        if (mIsVisible)
        {
            if (mIsFirstTime)
            {
                DisplayController::clear();
            }
            DisplayController::showDigit(digit);
        }

        mIsFirstTime = false;
        mNeedsRedraw = false;
    }
}

void ClockFragment::show()
{
    if (!mIsVisible)
    {
        LOG("Show");
        mIsVisible = true;
        mIsFirstTime = true;
        mNeedsRedraw = true;
    }
}

void ClockFragment::hide()
{
    if (mIsVisible)
    {
        LOG("Hide");
        mIsVisible = false;
        mIsFirstTime = true;
        mNeedsRedraw = true;
    }
}

bool ClockFragment::isVisible()
{
    return mIsVisible;
}