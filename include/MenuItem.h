#ifndef _MENU_ITEM_H_
#define _MENU_ITEM_H_

#include <Arduino.h>

class MenuItem
{
private:
    String mName;
    int mValue;
    int mMinValue;
    int mMaxValue;

public:
    MenuItem(const String& name, int value, int minValue, int maxValue);
    String getName();
    int getValue();
    bool inc();
    bool dec();
};

#endif