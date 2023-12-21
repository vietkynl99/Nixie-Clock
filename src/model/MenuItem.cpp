#include "../../include/model/MenuItem.h"
#include "../../include/manager/PreferencesManager.h"
#include "../../include/controller/LedController.h"

MenuItem::MenuItem(const String &name, MenuItemType type, int value, int minValue, int maxValue, bool needToReboot, MessageType changedNotifyType, MessageType savedNotifyType)
{
    mName = name;
    mValue = value;
    mType = type;
    mNeedToReboot = needToReboot;
    mChangedNotifyType = changedNotifyType;
    mSavedNotifyType = savedNotifyType;
    if (mType == MENU_ITEM_TYPE_BOOL || mType == MENU_ITEM_TYPE_CLOCK_MODE)
    {
        mMinValue = 0;
        mMaxValue = 1;
    }
    else if (mType == MENU_ITEM_TYPE_RESET)
    {
        mMinValue = 0;
        mMaxValue = 0;
    }
    else
    {
        mMinValue = minValue;
        mMaxValue = maxValue;
    }
    if (mMaxValue < mMinValue)
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

MenuItemType MenuItem::getType()
{
    return mType;
}

String MenuItem::getName()
{
    return mName;
}

int MenuItem::getValue()
{
    return mValue;
}

bool MenuItem::setValue(int value)
{
    if (value >= mMinValue && value <= mMaxValue)
    {
        mValue = value;
        return true;
    }
    return false;
}

String MenuItem::getStringValue()
{
    if (mType == MENU_ITEM_TYPE_BOOL)
    {
        return mValue == 0 ? "disable" : "enable";
    }
    else if (mType == MENU_ITEM_TYPE_CLOCK_MODE)
    {
        return mValue == 0 ? "4 display" : "6 display";
    }
    else if (mType == MENU_ITEM_TYPE_LED_MODE)
    {
        return LedController::getModeName(mValue);
    }
    else if (mType == MENU_ITEM_TYPE_RESET)
    {
        return "";
    }
    else
    {
        return String(mValue);
    }
}

bool MenuItem::getBoolValue()
{
    return mValue != 0;
}

bool MenuItem::needToReboot()
{
    return mNeedToReboot;
}

MessageType MenuItem::getChangedNotifyType()
{
    return mChangedNotifyType;
}

MessageType MenuItem::getSavedNotifyType()
{
    return mSavedNotifyType;
}

bool MenuItem::isMinimum()
{
    return mValue == mMinValue;
}

bool MenuItem::isMaximum()
{
    return mValue == mMaxValue;
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

void MenuItem::load()
{
    mValue = PreferencesManager::getInt(PREFERENCES_NAME, mName.c_str(), mValue);
}

void MenuItem::save()
{
    PreferencesManager::putInt(PREFERENCES_NAME, mName.c_str(), mValue);
}