#ifndef _CUBE_FRAGMENT_H_
#define _CUBE_FRAGMENT_H_

#include "../common/Log.h"
#include "../controller/DisplayController.h"

#define USE_DMA_TO_TFT

// Color depth has to be 16 bits if DMA is used to render image
#define COLOR_DEPTH 16

// 128x128 for a 16 bit colour Sprite (32Kbytes RAM)
// Maximum is 181x181 (64Kbytes) for DMA -  restricted by processor design
#define IWIDTH 128
#define IHEIGHT 128

// Size of cube image
// 358 is max for 128x128 sprite, too big and pixel trails are drawn...
#define CUBE_SIZE 358

class CubeFragment
{
private:
    static bool mIsVisible;
    static bool mNeedsRedraw;
    static bool mIsFirstTime;

    static TFT_eSprite *mTftSpr[2];
    static bool mTftSprSel;
    static uint16_t *mTftSprPtr[2];
    static bool mSpin[3];
    static int mMinX;
    static int mMinY;
    static int mMaxX;
    static int mMaxY;

public:
    static void init();
    static void loop();
    static void show();
    static void hide();
    static bool isVisible();

private:
    static void forceStop();
    static void createInstance();
    static void destroyInstance();
    static int shoelace(int x1, int y1, int x2, int y2, int x3, int y3);
    static void drawCube();
};

#endif