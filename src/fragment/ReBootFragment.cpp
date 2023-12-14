#include "../../include/fragment/ReBootFragment.h"

bool ReBootFragment::mIsVisible = false;
bool ReBootFragment::mNeedsRedraw = false;
bool ReBootFragment::mIsFirstTime = false;

static constexpr const char *const TAG = "REBOOT";

#define REBOOT_DELAY_TIME 2000

void ReBootFragment::init()
{
    DisplayController::init();
}

void ReBootFragment::loop()
{
    static uint32_t timeTick = 0;

    if (mIsVisible && xTaskGetTickCount() > timeTick)
    {
        if (mIsFirstTime)
        {
            mIsFirstTime = false;
            timeTick = xTaskGetTickCount() + REBOOT_DELAY_TIME / portTICK_PERIOD_MS;
            DisplayController::setFont(MENU_FONT, MENU_FONT_SIZE);
            DisplayController::getTft()->setTextColor(TFT_WHITE, MENU_BACKGROUND_COLOR);
            DisplayController::getTft()->setTextDatum(CC_DATUM);
            DisplayController::getTft()->drawString("Rebooting...", TFT_WIDTH / 2, TFT_HEIGHT / 2);
        }
        else
        {
            LOG("Rebooting...");
            ESP.restart();
        }
    }
}

void ReBootFragment::show()
{
    if (!mIsVisible)
    {
        LOG("Show");
        mIsVisible = true;
        mIsFirstTime = true;
        mNeedsRedraw = true;
        DisplayController::selectDisplay(TFT_MAX);
    }
}

void ReBootFragment::hide()
{
    if (mIsVisible)
    {
        LOG("Hide");
        mIsVisible = false;
        mIsFirstTime = true;
        mNeedsRedraw = true;
    }
}

bool ReBootFragment::isVisible()
{
    return mIsVisible;
}