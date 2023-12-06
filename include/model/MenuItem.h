#ifndef _MENU_ITEM_H_
#define _MENU_ITEM_H_

#include <Arduino.h>

enum MenuItemType
{
    MENU_ITEM_TYPE_BOOL,
    MENU_ITEM_TYPE_INT
};

class MenuItem
{
private:
    String mName;
    MenuItemType mType;
    int mValue;
    int mMinValue;
    int mMaxValue;

public:
    MenuItem(const String& name, MenuItemType type, int value, int minValue = 0, int maxValue = 0);
    String getName();
    int getValue();
    bool setValue(int value);
    String getValueAsString();
    bool isMinimum();
    bool isMaximum();
    bool inc();
    bool dec();

    void load();
    void save();
};

#endif