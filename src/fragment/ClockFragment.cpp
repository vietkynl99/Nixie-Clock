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
    static uint32_t timeTick = 0, digitTimeTick = 0;
    static uint32_t prevUnixTime = 0;
    static int digit[TFT_MAX];

    if (mIsVisible && xTaskGetTickCount() > digitTimeTick)
    {
        digitTimeTick = xTaskGetTickCount() + 300 / portTICK_PERIOD_MS;

        DateTime now = RTCController::getCurrentDateTime();
        if (RTCController::isValid(now))
        {
            if (prevUnixTime != now.unixtime())
            {
                prevUnixTime = now.unixtime();
                LOG("time: %s", RTCController::getString(now).c_str());

                for (int i = 0; i < TFT_MAX; i++)
                {
                    int number = -1;
                    switch (i)
                    {
                    case 0:
                        number = now.second() / 10;
                        break;
                    case 1:
                        number = now.second() % 10;
                        break;
                    default:
                        break;
                    }

                    if (number >= 0 && (mIsFirstTime || digit[i] != number))
                    {
                        digit[i] = number;
                        DisplayController::selectDisplay(i);
                        showDigit(digit[i]);
                    }
                }
            }
        }
        else
        {
            LOG("Invalid date time: %s", RTCController::getString(now).c_str());
        }
        mIsFirstTime = false;
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
        DisplayController::selectDisplay(TFT_MAX);
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