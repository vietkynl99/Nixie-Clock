#ifndef _MENU_FRAGMENT_H_
#define _MENU_FRAGMENT_H_

#include <Arduino.h>
#include <AsyncWiFiManager.h>
#include "../common/Configs.h"
#include "../common/Log.h"
#include "../common/MessageEvent.h"
#include "../fragment/PopupFragment.h"

class MenuFragment
{
private:
    static bool mIsVisible;
    static bool mNeedsRedraw;
    static bool mIsFirstTime;
    static int mCurrentIndex;
    static bool mEditPanelVisible;
    static int mPrevValue;
    static int mWrapTextLength;

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

private:
    static void setEditPanelVisible(bool visible);
    static bool getEditPanelVisible();

    static void showHeader(const char *text);
    static void showMenuList();
    static void showEditPanel(MenuItem *item);

    static void onDataChanged(MenuItem *item);
    static void onDataSaved(MenuItem *item);

    static void caculateWrapTextLength();
    static void wrapText(String &text, int length);
};

#endif