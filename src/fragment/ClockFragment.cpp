#include "../../include/fragment/ClockFragment.h"
#include "../../include/common/Images.h"

bool ClockFragment::mIsVisible = false;
bool ClockFragment::mNeedsRedraw = false;
bool ClockFragment::mIsFirstTime = false;

static constexpr const char *const TAG = "CLOCK";

#define INFORMATION_SCREEN_TEXT_COLOR 0x94B2 // #969696

void ClockFragment::init()
{
    RTCController::init();
    DisplayController::init();
}

void ClockFragment::loop()
{
    static uint32_t digitTimeTick = 0, clockTimeTick = 0;
    static uint8_t displayCount = TFT_COUNT;
    static uint8_t digit[TFT_COUNT];
    static DateTime currentTime;
    static uint8_t transitionEffect = 0;
    static int effectNumber = 0;
    static uint8_t effectMask = 0;
    static int networkEffectNumber = 0;

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

        // Update RTC time
        DateTime now = RTCController::getCurrentDateTime();
        if (Helper::isValidDateTime(now))
        {
            if (currentTime.unixtime() != now.unixtime())
            {
                currentTime = now;
                // LOG("%s", Helper::convertDateTimeToString(now).c_str());
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

        // Information screen
        if (!SettingsManager::isFullDisplayClockMode())
        {
            if (mIsFirstTime)
            {
                networkEffectNumber = 0;
                updateInformationScreen(true);
                updateNetworkIcon(true);
            }
            else if (AsyncWiFiManager::getState() == ASYNC_WIFI_STATE_CONNECTING)
            {
                networkEffectNumber = !networkEffectNumber;
                if (networkEffectNumber)
                {
                    updateNetworkIcon(true);
                }
            }
            else if (AsyncWiFiManager::getState() == ASYNC_WIFI_STATE_CONNECTED)
            {
                networkEffectNumber++;
                if (networkEffectNumber > 30)
                {
                    networkEffectNumber = 0;
                    updateNetworkIcon(false);
                }
            }
        }
        mIsFirstTime = false;
    }

    // Transition effect
    // TODO: Fix the error of time changing while the effect is running
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

void ClockFragment::handleEvent(const Message &message)
{
    if (!mIsVisible)
    {
        return;
    }

    switch (message.type)
    {
    case MESSAGE_TYPE_UPDATE_TEMP_AND_RH:
    {
        if (!SettingsManager::isFullDisplayClockMode())
        {
            updateInformationScreen(false);
        }
        break;
    }
    case MESSAGE_TYPE_UPDATE_NETWORK_STATE:
    {
        if (!SettingsManager::isFullDisplayClockMode())
        {
            updateNetworkIcon(false);
        }
        break;
    }
    default:
        break;
    }
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

void ClockFragment::updateNetworkIcon(bool force)
{
    static int state = -1;
    static int type = WIFI_ICON_TYPE_0_SIGNAL;
    static int prevRssiLevel = -1;

    if (force || state != AsyncWiFiManager::getState())
    {
        if (state != AsyncWiFiManager::getState())
        {
            state = AsyncWiFiManager::getState();
            type = WIFI_ICON_TYPE_0_SIGNAL;
        }
        DisplayController::selectDisplay(TFT_COUNT - 1);
        DisplayController::getTft()->fillRect(TFT_WIDTH - 45, 5, 45, 50, TFT_BLACK);
        switch (state)
        {
        case ASYNC_WIFI_STATE_CONFIG_PORTAL:
            DisplayController::drawArrayJpeg(image_hotspot, sizeof(image_hotspot), TFT_WIDTH - 45, 5);
            break;
        case ASYNC_WIFI_STATE_CONNECTING:
            type++;
            if (type > WIFI_ICON_TYPE_3_SIGNAL)
            {
                type = WIFI_ICON_TYPE_0_SIGNAL;
            }
            DisplayController::drawWifiIcon(TFT_WIDTH - 45, 0, (WifiIconType)type, INFORMATION_SCREEN_TEXT_COLOR, false);
            break;
        case ASYNC_WIFI_STATE_CONNECTED:
            prevRssiLevel = Helper::getRssiLevel(WiFi.RSSI()); // 0-4
            type = prevRssiLevel > WIFI_ICON_TYPE_0_SIGNAL ? prevRssiLevel - 1 : WIFI_ICON_TYPE_0_SIGNAL;
            DisplayController::drawWifiIcon(TFT_WIDTH - 45, 0, (WifiIconType)type, INFORMATION_SCREEN_TEXT_COLOR);
            break;
        case ASYNC_WIFI_STATE_DISCONNECTED:
            DisplayController::drawWifiIcon(TFT_WIDTH - 45, 0, WIFI_ICON_TYPE_DISCONNECTED, INFORMATION_SCREEN_TEXT_COLOR);
            break;
        default:
            break;
        }
    }
    else if (state == ASYNC_WIFI_STATE_CONNECTED && prevRssiLevel != Helper::getRssiLevel(WiFi.RSSI()))
    {
        prevRssiLevel = Helper::getRssiLevel(WiFi.RSSI());
        type = prevRssiLevel > WIFI_ICON_TYPE_0_SIGNAL ? prevRssiLevel - 1 : WIFI_ICON_TYPE_0_SIGNAL;
        DisplayController::selectDisplay(TFT_COUNT - 1);
        DisplayController::drawWifiIcon(TFT_WIDTH - 45, 0, (WifiIconType)type, INFORMATION_SCREEN_TEXT_COLOR);
    }
}

void ClockFragment::updateInformationScreen(bool force)
{
    int xpos = 10;
    int ypos = 20;
    char buffer[16];

    TFT_eSPI *tft = DisplayController::getTft();

    DisplayController::selectDisplay(TFT_COUNT - 1);
    DisplayController::setFont(FSB18, 1);
    tft->setTextColor(INFORMATION_SCREEN_TEXT_COLOR, TFT_BLACK);
    tft->setTextDatum(TL_DATUM);

    tft->drawString(Helper::convertDateToString(RTCController::getCurrentDateTime()), xpos, ypos);

    ypos = 90;
    DisplayController::setFont(FSB24, 1);
    if (SensorController::isValidDhtValue(SensorController::getTemperature()))
    {
        snprintf(buffer, sizeof(buffer), "T: %.1f", SensorController::getTemperature());
    }
    else
    {
        strncpy(buffer, "T: --", sizeof(buffer));
    }
    tft->drawString(buffer, xpos, ypos);
    xpos += tft->textWidth(buffer) + 12;
    DisplayController::setFont(FSB18, 1);
    tft->drawString("o", xpos, ypos - 5);
    xpos += tft->textWidth("o") + 4;
    DisplayController::setFont(FSB24, 1);
    tft->drawString("C", xpos, ypos);

    xpos = 10;
    ypos += tft->fontHeight() * 1.1;
    if (SensorController::isValidDhtValue(SensorController::getHumidity()))
    {
        snprintf(buffer, sizeof(buffer), "H: %d %%", (int)SensorController::getHumidity());
    }
    else
    {
        strncpy(buffer, "H: -- %", sizeof(buffer));
    }
    tft->drawString(buffer, xpos, ypos);
}