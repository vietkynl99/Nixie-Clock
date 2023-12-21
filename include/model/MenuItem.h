#ifndef _MENU_ITEM_H_
#define _MENU_ITEM_H_

#include <Arduino.h>

#define PREFERENCES_NAME "MenuItem"

enum MenuItemType
{
    MENU_ITEM_TYPE_BOOL,
    MENU_ITEM_TYPE_INT,
    MENU_ITEM_TYPE_CLOCK_MODE,
    MENU_ITEM_TYPE_LED_MODE,
    MENU_ITEM_TYPE_RESET
};

class MenuItem
{
private:
    String mName;
    MenuItemType mType;
    int mValue;
    int mMinValue;
    int mMaxValue;
    bool mNeedToReboot;

public:
    MenuItem(const String &name, MenuItemType type, int value = 0, int minValue = 0, int maxValue = 0, bool needToReboot = false);

    bool setValue(int value);

    MenuItemType getType();
    String getName();
    int getValue();
    String getStringValue();
    bool getBoolValue();
    bool needToReboot();

    bool isMinimum();
    bool isMaximum();
    bool inc();
    bool dec();

    void load();
    void save();
};

#endif