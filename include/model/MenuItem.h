#ifndef _MENU_ITEM_H_
#define _MENU_ITEM_H_

#include <Arduino.h>
#include "../common/MessageEvent.h"

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
    MessageType mChangedNotifyType;
    MessageType mSavedNotifyType;

public:
    MenuItem(const String &name, MenuItemType type, int value = 0, int minValue = 0, int maxValue = 0,
             bool needToReboot = false, MessageType changedNotifyType = MESSAGE_TYPE_NONE, MessageType savedNotifyType = MESSAGE_TYPE_NONE);

    bool setValue(int value);

    MenuItemType getType();
    String getName();
    int getValue();
    String getStringValue();
    bool getBoolValue();
    bool needToReboot();
    MessageType getChangedNotifyType();
    MessageType getSavedNotifyType();

    bool isMinimum();
    bool isMaximum();
    bool inc();
    bool dec();

    void load();
    void save();
};

#endif