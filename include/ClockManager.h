#ifndef _CLOCK_MANAGER_H_
#define _CLOCK_MANAGER_H_

#include "../include/Log.h"
#include "../include/DisplayController.h"

class ClockManager
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
};

#endif