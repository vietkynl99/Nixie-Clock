#ifndef _MENU_MANAGER_H_
#define _MENU_MANAGER_H_

#include <Arduino.h>
#include "../include/Log.h"
#include "../include/DisplayManager.h"
#include "../include/MenuItem.h"

#define MENU_ITEM_LIST_SIZE             10
#define MENU_FONT                       FSB18 // Serif Bold 18pt
#define MENU_HEADER_COLOR               0x3C91
#define MENU_HIGHTLIGHT_COLOR           0x2124
#define MENU_BACKGROUND_COLOR           TFT_BLACK
#define MENU_HIGHTLIGHT_TOP_MARGIN      10
#define MENU_ITEM_LEFT_MARGIN           12

class MenuManager
{
private:
    static bool mIsInitialized;
    static bool mIsVisible;
    static bool mNeedsRedraw;
    static bool mIsFirstTime;
    static MenuItem *mMenuItemList[MENU_ITEM_LIST_SIZE];
    static int mMenuItemCount;
    static int mMenuItemNameMaxLength;
    static int mUserSelection;
    static bool mEditPanelVisible;

public:
    static void init();
    static void loop();
    static void show();
    static void hide();
    static void up();
    static void down();
    static void enter();
    static void back();

private:
    static void setEditPanelVisible(bool visible);
    static bool getEditPanelVisible();
    static void showHeader(const char* text);
    static void showMenuList(bool firstDraw);
    static void showEditPanel(bool isFirstTime);
    static void addMenuItem(MenuItem *menuItem);
    static void createMenuList();
    static void addSpaceToEnd(String& string, int length);
};

#endif