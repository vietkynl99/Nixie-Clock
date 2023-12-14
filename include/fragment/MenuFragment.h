#ifndef _MENU_FRAGMENT_H_
#define _MENU_FRAGMENT_H_

#include <Arduino.h>
#include "../common/Log.h"
#include "../model/MenuItemList.h"

class MenuFragment
{
private:
    static MenuItemList *mMenuItemList;
    static bool mIsVisible;
    static bool mNeedsRedraw;
    static bool mIsFirstTime;
    static int mCurrentIndex;
    static bool mEditPanelVisible;
    static bool mDataChanged;

public:
    static void init();
    static void loop();
    static void show();
    static void hide();
    static bool isVisible();
    static void up();
    static void down();
    static void enter();
    static void back();

    static bool isWebServerEnable();

private:
    static void setEditPanelVisible(bool visible);
    static bool getEditPanelVisible();
    static void createMenuList();

    static void showHeader(const char *text);
    static void showMenuList();
    static void showEditPanel(MenuItem *item);
};

#endif