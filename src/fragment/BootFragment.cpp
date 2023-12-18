#include "../../include/fragment/BootFragment.h"

bool BootFragment::mIsVisible = false;
bool BootFragment::mNeedsRedraw = false;
bool BootFragment::mIsFirstTime = false;
bool BootFragment::mIsDone = false;
TFT_eSprite *BootFragment::mSprGraph = nullptr;
int *BootFragment::mXPosList = nullptr;
int *BootFragment::mYPosList = nullptr;
int *BootFragment::mLengthList = nullptr;
uint16_t *BootFragment::mColorList = nullptr;
int BootFragment::mLengthMax = 0;

#define GREEN_COLOR_LIST_SIZE 6
uint16_t greenColorList[] = {0x0201, 0x0341, 0x04a1, 0x05e1, 0x06e1, 0x07a1};

static constexpr const char *const TAG = "BOOT";

#define NUM_EFFECT_COLUMNS 15
#define BOOTING_DELAY_TIME 1200 // (ms)

void BootFragment::init()
{
    DisplayController::init();
}

void BootFragment::loop()
{
    static uint32_t timeTick = 0, doneTimeTick = 0;
    static int length = 0;

    if (mIsVisible && xTaskGetTickCount() > timeTick)
    {
        timeTick = xTaskGetTickCount() + 70 / portTICK_PERIOD_MS;
        if (mIsFirstTime)
        {
            mIsFirstTime = false;
            length = 0;
        }

        mSprGraph->pushSprite(0, 0);
        mSprGraph->scroll(0, -16);

        if (length < mLengthMax)
        {
            length++;
            doneTimeTick = xTaskGetTickCount() + BOOTING_DELAY_TIME / portTICK_PERIOD_MS;
            for (int i = 0; i < NUM_EFFECT_COLUMNS; i++)
            {
                if (length < mLengthList[i])
                {
                    mSprGraph->setTextColor(mColorList[i]);
                    mSprGraph->drawString(String((char)random('A', 'z')), mXPosList[i], mYPosList[i] + length * 3);
                }
            }
        }
        else if (!mIsDone && xTaskGetTickCount() > doneTimeTick)
        {
            mIsDone = true;
        }
    }
}

void BootFragment::show()
{
    if (!mIsVisible)
    {
        LOG("Show");
        mIsVisible = true;
        mIsFirstTime = true;
        mNeedsRedraw = true;
        mIsDone = false;
        createInstance();
    }
}

void BootFragment::hide()
{
    if (mIsVisible)
    {
        LOG("Hide");
        mIsVisible = false;
        mIsFirstTime = true;
        mNeedsRedraw = true;
        mIsDone = true;
        destroyInstance();
    }
}

bool BootFragment::isVisible()
{
    return mIsVisible;
}

bool BootFragment::isDone()
{
    return mIsDone;
}

void BootFragment::createInstance()
{
    if (!mSprGraph)
    {
        mSprGraph = new TFT_eSprite(DisplayController::getTft());
        mSprGraph->setColorDepth(8);
        mSprGraph->createSprite(TFT_WIDTH, TFT_HEIGHT);
        mSprGraph->fillSprite(TFT_BLACK);
        mSprGraph->setTextFont(4);
    }
    if (!mXPosList)
    {
        mXPosList = new int[NUM_EFFECT_COLUMNS];
        for (int i = 0; i < NUM_EFFECT_COLUMNS; i++)
        {
            mXPosList[i] = random(5, TFT_WIDTH - 5);
        }
    }
    if (!mYPosList)
    {
        mYPosList = new int[NUM_EFFECT_COLUMNS];
        for (int i = 0; i < NUM_EFFECT_COLUMNS; i++)
        {
            mYPosList[i] = random(0, 2 * TFT_HEIGHT / 3);
        }
    }
    if (!mLengthList)
    {
        mLengthList = new int[NUM_EFFECT_COLUMNS];
        for (int i = 0; i < NUM_EFFECT_COLUMNS; i++)
        {
            mLengthList[i] = random(10, 60);
            if (mLengthList[i] > mLengthMax)
            {
                mLengthMax = mLengthList[i];
            }
        }
    }
    if (!mColorList)
    {
        mColorList = new uint16_t[NUM_EFFECT_COLUMNS];
        for (int i = 0; i < NUM_EFFECT_COLUMNS; i++)
        {
            mColorList[i] = greenColorList[random(0, GREEN_COLOR_LIST_SIZE - 1)];
        }
    }
}

void BootFragment::destroyInstance()
{
    if (mSprGraph)
    {
        delete mSprGraph;
        mSprGraph = nullptr;
    }
    if (mXPosList)
    {
        delete[] mXPosList;
        mXPosList = nullptr;
    }
    if (mYPosList)
    {
        delete[] mYPosList;
        mYPosList = nullptr;
    }
    if (mLengthList)
    {
        delete[] mLengthList;
        mLengthList = nullptr;
    }
    if (mColorList)
    {
        delete[] mColorList;
        mColorList = nullptr;
    }
}