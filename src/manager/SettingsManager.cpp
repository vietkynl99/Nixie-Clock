#include "../../include/manager/SettingsManager.h"
#include "../../include/manager/PreferencesManager.h"
#include "../../include/controller/LedController.h"

MenuItemList *SettingsManager::mMenuItemList = nullptr;

static constexpr const char *const TAG = "SETTING";

void SettingsManager::init()
{
    if (mMenuItemList)
    {
        return;
    }
    mMenuItemList = new MenuItemList();
    mMenuItemList->add(new MenuItem("Clock mode", MENU_ITEM_TYPE_CLOCK_MODE, 1));
    mMenuItemList->add(new MenuItem("Web server", MENU_ITEM_TYPE_BOOL, false, 0, 1, true));
    mMenuItemList->add(new MenuItem("NTP service", MENU_ITEM_TYPE_BOOL, true, 0, 1, true));
    mMenuItemList->add(new MenuItem("Buzzer volume", MENU_ITEM_TYPE_INT, 8, 0, 10));
    mMenuItemList->add(new MenuItem("Led mode", MENU_ITEM_TYPE_LED_MODE, LED_MODE_MAX, 0, LED_MODE_MAX,
                                    false, MESSAGE_TYPE_LED_MODE_CHANGED, MESSAGE_TYPE_LED_MODE_CHANGED));
    mMenuItemList->add(new MenuItem("Led brightness", MENU_ITEM_TYPE_INT, 2, 0, 10,
                                    false, MESSAGE_TYPE_LED_BRIGHTNESS_CHANGED, MESSAGE_TYPE_LED_BRIGHTNESS_CHANGED));
    mMenuItemList->add(new MenuItem("Led speed", MENU_ITEM_TYPE_INT, 2, 1, 10));
    mMenuItemList->add(new MenuItem("Reset WiFi", MENU_ITEM_TYPE_RESET));
    mMenuItemList->add(new MenuItem("Reset settings", MENU_ITEM_TYPE_RESET));
    mMenuItemList->loadData();

    LOG("Settings list loaded:");
    for (int i = 0; i < mMenuItemList->length(); i++)
    {
        MenuItem *item = mMenuItemList->get(i);
        LOG("%d. %s: %s", i + 1, item->getName().c_str(), item->getStringValue().c_str());
    }
}

void SettingsManager::reset()
{
    LOG("Reset");
    PreferencesManager::clear(PREFERENCES_NAME);
    delete mMenuItemList;
    mMenuItemList = nullptr;
    // init();
}

MenuItemList *SettingsManager::getItemList()
{
    return mMenuItemList;
}

MenuItem *SettingsManager::getItem(int index)
{
    return mMenuItemList ? mMenuItemList->get(index) : nullptr;
}

int SettingsManager::getLength()
{
    return mMenuItemList ? mMenuItemList->length() : 0;
}

/* ATTENTION: The index of the setting MUST BE same as the list */
bool SettingsManager::isFullDisplayClockMode()
{
    return mMenuItemList ? mMenuItemList->get(0)->getBoolValue() : false;
}

bool SettingsManager::isWiFiEnabled()
{
    return isWebServerEnabled() || isNTPEnabled();
}

bool SettingsManager::isWebServerEnabled()
{
    return mMenuItemList ? mMenuItemList->get(1)->getBoolValue() : false;
}

bool SettingsManager::isNTPEnabled()
{
    return mMenuItemList ? mMenuItemList->get(2)->getBoolValue() : false;
}

int SettingsManager::getBuzzerVolume()
{
    return mMenuItemList ? mMenuItemList->get(3)->getValue() : 0;
}

int SettingsManager::getLedMode()
{
    return mMenuItemList ? mMenuItemList->get(4)->getValue() : 0;
}

int SettingsManager::getLedBrightness()
{
    return mMenuItemList ? mMenuItemList->get(5)->getValue() : 0;
}

int SettingsManager::getLedSpeed()
{
    return mMenuItemList ? mMenuItemList->get(6)->getValue() : 0;
}