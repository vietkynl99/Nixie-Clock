#include "../include/MenuItemList.h"
#include "../include/PreferencesManager.h"

#define PREFERENCES_NAME "MenuItemList"

static constexpr const char *const TAG = "MenuItemList";

MenuItemList::MenuItemList()
{
    mLength = 0;
    for (int i = 0; i < mLength; i++)
    {
        mList[i] = nullptr;
    }
}

void MenuItemList::add(MenuItem *item)
{
    if (mLength < MENU_ITEM_LIST_SIZE)
    {
        mList[mLength] = item;
        mLength++;
    }
    else
    {
        LOG("Menu item list is full");
        if (item)
        {
            delete item;
            item = nullptr;
        }
    }
}

int MenuItemList::length()
{
    return mLength;
}

MenuItem *MenuItemList::get(int index)
{
    return index >= 0 && index < mLength ? mList[index] : nullptr;
}

void MenuItemList::loadData()
{
    for (int i = 0; i < mLength; i++)
    {
        mList[i]->setValue(PreferencesManager::getInt(PREFERENCES_NAME, mList[i]->getName().c_str(), mList[i]->getValue()));
    }
}

void MenuItemList::saveData(int index)
{
    PreferencesManager::putInt(PREFERENCES_NAME, mList[index]->getName().c_str(), mList[index]->getValue());
}

void MenuItemList::saveData()
{
    for (int i = 0; i < mLength; i++)
    {
        saveData(i);
    }
}