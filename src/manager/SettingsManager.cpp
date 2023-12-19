#include "../../include/manager/SettingsManager.h"

MenuItemList *SettingsManager::mMenuItemList = nullptr;

static constexpr const char *const TAG = "SETTING";

void SettingsManager::init()
{
    if (mMenuItemList)
    {
        return;
    }
    mMenuItemList = new MenuItemList();
    mMenuItemList->add(new MenuItem("Web server", MENU_ITEM_TYPE_BOOL, false, 0, 1, true));
    mMenuItemList->add(new MenuItem("NTP Service", MENU_ITEM_TYPE_BOOL, true, 0, 1, true));
    mMenuItemList->add(new MenuItem("RTC debug", MENU_ITEM_TYPE_BOOL, false));
    mMenuItemList->add(new MenuItem("Buzzer volume", MENU_ITEM_TYPE_INT, 8, 0, 10));
    mMenuItemList->loadData();

    LOG("Settings list loaded:");
    for (int i = 0; i < mMenuItemList->length(); i++)
    {
        MenuItem *item = mMenuItemList->get(i);
        LOG("%d. %s: %s", i + 1, item->getName().c_str(), item->getStringValue().c_str());
    }
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
bool SettingsManager::isWiFiEnabled()
{
    return isWebServerEnabled() || isNTPEnabled();
}

bool SettingsManager::isWebServerEnabled()
{
    return mMenuItemList ? mMenuItemList->get(0)->getBoolValue() : false;
}

bool SettingsManager::isNTPEnabled()
{
    return mMenuItemList ? mMenuItemList->get(1)->getBoolValue() : false;
}

bool SettingsManager::isRTCDebugEnabled()
{
    return mMenuItemList ? mMenuItemList->get(2)->getBoolValue() : false;
}

int SettingsManager::getBuzzerVolume()
{
    return mMenuItemList ? mMenuItemList->get(3)->getValue() : 0;
}