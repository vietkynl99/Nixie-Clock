#include "../include/MenuItem.h"

MenuItem::MenuItem(const String& name, int value)
{
    mName = name;
    mValue = value;
}

String MenuItem::getName()
{
    return mName;
}

int MenuItem::getValue()
{
    return mValue;
}