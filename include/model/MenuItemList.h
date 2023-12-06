#ifndef _MENU_ITEM_LIST_H_
#define _MENU_ITEM_LIST_H_

#include "MenuItem.h"
#include "../common/Log.h"

#define MENU_ITEM_LIST_SIZE 10

class MenuItemList
{
private:
    int mLength;
    MenuItem *mList[MENU_ITEM_LIST_SIZE];

public:
    MenuItemList();
    void add(MenuItem *item);
    int length();
    MenuItem *get(int index);

    void loadData();
    void saveData();
};

#endif