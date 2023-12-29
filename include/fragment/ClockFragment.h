#ifndef _CLOCK_FRAGMENT_H_
#define _CLOCK_FRAGMENT_H_

#include <WiFi.h>
#include <AsyncWiFiManager.h>
#include "../common/Log.h"
#include "../common/Helper.h"
#include "../common/MessageEvent.h"
#include "../controller/SensorController.h"
#include "../controller/DisplayController.h"
#include "../controller/RTCController.h"
#include "../manager/SettingsManager.h"

class ClockFragment
{
private:
    static bool mIsVisible;
    static bool mNeedsRedraw;
    static bool mIsFirstTime;

public:
    static void init();
    static void loop();
    static void show();
    static void hide();
    static bool isVisible();
    static void handleEvent(const Message &message);

private:
    static void showDigit(int digit);
    static void updateNetworkIcon(bool force);
    static void updateInformationScreen(bool force);
};

#endif