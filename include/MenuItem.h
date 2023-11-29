#ifndef _MENU_ITEM_H_
#define _MENU_ITEM_H_

#include <Arduino.h>

class MenuItem
{
private:
    String mName;
    int mValue;

public:
    MenuItem(const String& name, int value);
    String getName();
    int getValue();
};

#endif