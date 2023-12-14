#ifndef _REBOOT_FRAGMENT_H_
#define _REBOOT_FRAGMENT_H_

#include "../common/Log.h"
#include "../controller/DisplayController.h"

class ReBootFragment
{
private:
    static bool mIsVisible;
    static bool mNeedsRedraw;
    static bool mIsFirstTime;

public:
    static void init();
    static void loop();
    static void show();
    static void hide();
    static bool isVisible();
};

#endif