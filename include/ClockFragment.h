#ifndef _CLOCK_FRAGMENT_H_
#define _CLOCK_FRAGMENT_H_

#include "Log.h"
#include "DisplayController.h"

class ClockFragment
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