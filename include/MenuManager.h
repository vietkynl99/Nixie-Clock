#ifndef _MENU_MANAGER_H_
#define _MENU_MANAGER_H_

#include <Arduino.h>
#include "../include/Log.h"
#include "../include/DisplayController.h"
#include "../include/MenuItemList.h"



class MenuManager
{
private:
    static MenuItemList *mMenuItemList;
    static bool mIsInitialized;
    static bool mIsVisible;
    static bool mNeedsRedraw;
    static bool mIsFirstTime;
    static int mCurrentIndex;
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
    static void createMenuList();
};

#endif