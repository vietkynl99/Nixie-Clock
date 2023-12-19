#ifndef _SETTINGS_MANAGER_H_
#define _SETTINGS_MANAGER_H_

#include <Arduino.h>
#include "../common/Log.h"
#include "../common/Helper.h"
#include "../model/MenuItemList.h"

class SettingsManager
{
private:
    static MenuItemList *mMenuItemList;

public:
    static void init();
    static MenuItemList* getItemList();
    static MenuItem* getItem(int index);
    static int getLength();
    
    static bool isWiFiEnabled();
    static bool isWebServerEnabled();
    static bool isNTPEnabled();
    static bool isRTCDebugEnabled();
    static int getBuzzerVolume();
};

#endif