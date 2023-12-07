#ifndef _BOOT_FRAGMENT_H_
#define _BOOT_FRAGMENT_H_

#include "../common/Log.h"
#include "../controller/DisplayController.h"

class BootFragment
{
private:
    static bool mIsVisible;
    static bool mNeedsRedraw;
    static bool mIsFirstTime;
    static bool mIsDone;

    static TFT_eSprite *mSprGraph;
    static int *mXPosList;
    static int *mYPosList;
    static int *mLengthList;
    static uint16_t *mColorList;
    static int mLengthMax;

public:
    static void init();
    static void loop();
    static void show();
    static void hide();
    static bool isVisible();
    static bool isDone();

private:
    static void createInstance();
    static void destroyInstance();
};

#endif