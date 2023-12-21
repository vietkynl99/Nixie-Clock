#ifndef _SETTINGS_MANAGER_H_
#define _SETTINGS_MANAGER_H_

#include <Arduino.h>
#include "../common/Log.h"
#include "../common/Helper.h"
#include "../common/MessageEvent.h"
#include "../model/MenuItemList.h"

class SettingsManager
{
private:
    static MenuItemList *mMenuItemList;

public:
    static void init();
    static void reset();
    static MenuItemList* getItemList();
    static MenuItem* getItem(int index);
    static int getLength();
    
    static bool isFullDisplayClockMode();
    static bool isWiFiEnabled();
    static bool isWebServerEnabled();
    static bool isNTPEnabled();
    static bool isRTCDebugEnabled();
    static int getBuzzerVolume();
    static int getLedMode();
    static int getLedBrightness();
    static int getLedSpeed();
};

#endif