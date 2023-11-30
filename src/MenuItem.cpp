#include "../include/MenuItem.h"

MenuItem::MenuItem(const String &name, int value, int minValue, int maxValue)
{
    mName = name;
    mValue = value;
    mMinValue = minValue;
    mMaxValue = maxValue;
    if(mMaxValue < mMinValue)
    {
        mMaxValue = mMinValue;
    }
    if (mValue < mMinValue)
    {
        mValue = mMinValue;
    }
    if (mValue > mMaxValue)
    {
        mValue = mMaxValue;
    }
}

String MenuItem::getName()
{
    return mName;
}

int MenuItem::getValue()
{
    return mValue;
}

bool MenuItem::inc()
{
    if (mValue < mMaxValue)
    {
        mValue++;
        return true;
    }
    return false;
}

bool MenuItem::dec()
{
    if (mValue > mMinValue)
    {
        mValue--;
        return true;
    }
    return false;
}