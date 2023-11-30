#include "../include/MenuItemList.h"

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