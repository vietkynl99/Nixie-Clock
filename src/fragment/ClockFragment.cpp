#include "../../include/fragment/ClockFragment.h"
#include "../../include/common/ClockDigit.h"

bool ClockFragment::mIsVisible = false;
bool ClockFragment::mNeedsRedraw = false;
bool ClockFragment::mIsFirstTime = false;

static constexpr const char *const TAG = "CLOCK";

void ClockFragment::init()
{
    RTCController::init();
    DisplayController::init();
}

void ClockFragment::loop()
{
    static uint32_t timeTick = 0, digitTimeTick = 0, clockTimeTick = 0;
    static uint8_t displayCount = TFT_COUNT;
    static uint8_t digit[TFT_COUNT];
    static DateTime currentTime;
    static uint8_t transitionEffect = 0;
    static int effectNumber = 0;
    static uint8_t effectMask = 0;

    if (mIsVisible && xTaskGetTickCount() > clockTimeTick)
    {
        clockTimeTick = xTaskGetTickCount() + 300 / portTICK_PERIOD_MS;
        if (mIsFirstTime)
        {
            displayCount = SettingsManager::isFullDisplayClockMode() ? TFT_COUNT : 4;
            transitionEffect = 1;
            effectNumber = 9;
            effectMask = 0;
            for (int i = 0; i < displayCount; i++)
            {
                bitSet(effectMask, i);
            }
        }

        DateTime now = RTCController::getCurrentDateTime();
        if (Helper::isValidDateTime(now))
        {
            if (currentTime.unixtime() != now.unixtime())
            {
                currentTime = now;
                if (SettingsManager::isRTCDebugEnabled())
                {
                    LOG("%s", Helper::convertDateTimeToString(now).c_str());
                }

                for (int i = 0; i < displayCount; i++)
                {
                    int number = -1;
                    switch (i)
                    {
                    case 0:
                        number = now.hour() / 10;
                        break;
                    case 1:
                        number = now.hour() % 10;
                        break;
                    case 2:
                        number = now.minute() / 10;
                        break;
                    case 3:
                        number = now.minute() % 10;
                        break;
                    case 4:
                        number = now.second() / 10;
                        break;
                    case 5:
                        number = now.second() % 10;
                        break;
                    default:
                        break;
                    }

                    if (number >= 0 && (mIsFirstTime || digit[i] != number))
                    {
                        digit[i] = number;
                        if (!transitionEffect)
                        {
                            DisplayController::selectDisplay(i);
                            showDigit(digit[i]);
                        }
                    }
                }
            }
        }
        mIsFirstTime = false;
    }

    if (mIsVisible && transitionEffect && xTaskGetTickCount() > digitTimeTick)
    {
        digitTimeTick = xTaskGetTickCount() + 100 / portTICK_PERIOD_MS;

        if (transitionEffect)
        {
            if (transitionEffect == 2)
            {
                for (int i = 0; i < displayCount; i++)
                {
                    if (digit[i] < effectNumber)
                    {
                        bitClear(effectMask, i);
                    }
                }
            }
            DisplayController::selectMultiDisplay(effectMask);
            showDigit(effectNumber);

            if (transitionEffect == 1)
            {
                effectNumber--;
                if (effectNumber < 1)
                {
                    transitionEffect = 2;
                    effectNumber = 0;
                }
            }
            else
            {
                effectNumber++;
                if (effectNumber > 9 || effectMask == 0)
                {
                    transitionEffect = 0;
                    effectNumber = 0;
                    // redraw clock
                    clockTimeTick = 0;
                }
            }
        }
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
        DisplayController::selectDisplay(TFT_COUNT);
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

void ClockFragment::showDigit(int digit)
{
    switch (digit)
    {
    case 0:
        DisplayController::drawArrayJpegInCenter(image_digit_0, sizeof(image_digit_0));
        break;
    case 1:
        DisplayController::drawArrayJpegInCenter(image_digit_1, sizeof(image_digit_1));
        break;
    case 2:
        DisplayController::drawArrayJpegInCenter(image_digit_2, sizeof(image_digit_2));
        break;
    case 3:
        DisplayController::drawArrayJpegInCenter(image_digit_3, sizeof(image_digit_3));
        break;
    case 4:
        DisplayController::drawArrayJpegInCenter(image_digit_4, sizeof(image_digit_4));
        break;
    case 5:
        DisplayController::drawArrayJpegInCenter(image_digit_5, sizeof(image_digit_5));
        break;
    case 6:
        DisplayController::drawArrayJpegInCenter(image_digit_6, sizeof(image_digit_6));
        break;
    case 7:
        DisplayController::drawArrayJpegInCenter(image_digit_7, sizeof(image_digit_7));
        break;
    case 8:
        DisplayController::drawArrayJpegInCenter(image_digit_8, sizeof(image_digit_8));
        break;
    case 9:
        DisplayController::drawArrayJpegInCenter(image_digit_9, sizeof(image_digit_9));
        break;
    default:
        break;
    }
}