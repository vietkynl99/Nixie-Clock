#ifndef _MENU_MANAGER_H_
#define _MENU_MANAGER_H_

#include <Arduino.h>
#include "../include/Log.h"
#include "../include/DisplayManager.h"
#include "../include/MenuItem.h"

#define MENU_ITEM_LIST_SIZE 10
#define MENU_FONT FSB18 // Serif Bold 18pt
#define MENU_HIGHTLIGHT_COLOR TFT_DARKCYAN
#define MENU_BACKGROUND_COLOR TFT_BLACK
#define MENU_HIGHTLIGHT_TOP_MARGIN 10
#define MENU_ITEM_LEFT_MARGIN 12

class MenuManager
{
private:
    static bool mIsInitialized;
    static bool mIsVisible;
    static bool mNeedsRedraw;
    static MenuItem *mMenuItemList[MENU_ITEM_LIST_SIZE];
    static int mMenuItemCount;
    static int mMenuItemNameMaxLength;
    static int mUserSelection;

public:
    static void init();
    static void loop();
    static void show();
    static void hide();
    static void up();
    static void down();

private:
    static void showHeader();
    static void showMenuList(bool firstDraw);
    static void addMenuItem(MenuItem *menuItem);
    static void createMenuList();
    static void addSpaceToEnd(String& string, int length);
};

#endif