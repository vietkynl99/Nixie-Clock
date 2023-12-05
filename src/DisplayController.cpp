#include "../include/clockDigit.h"
#include "../include/DisplayController.h"
#include "../include/Log.h"

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

TFT_eSPI *DisplayController::tft = new TFT_eSPI();
bool DisplayController::mIsInitialized = false;
bool DisplayController::mDrew = true;
int DisplayController::mCsPinList[TFT_MAX] = {TFT1_CS, TFT2_CS, TFT3_CS, TFT4_CS, TFT5_CS, TFT6_CS};

static constexpr const char *const TAG = "DISPLAY";

void DisplayController::init()
{
    if (!mIsInitialized)
    {
        LOG("init");
        mIsInitialized = true;

        for (int i = 0; i < TFT_MAX; i++)
        {
            pinMode(mCsPinList[i], OUTPUT);
            digitalWrite(mCsPinList[i], LOW);
        }

        selectDisplay(TFT_MAX);
        tft->begin();
        tft->setRotation(DISPLAY_ROTATION);
        clear();
    }
}

void DisplayController::clear()
{
    if (mDrew)
    {
        LOG("clear");
        mDrew = false;
        tft->fillScreen(TFT_BLACK);
    }
}

void DisplayController::setFont(const GFXfont *font, uint8_t size)
{
    tft->setFreeFont(font);
    tft->setTextSize(size);
}

void DisplayController::selectDisplay(int index)
{
    static int mPrevIndex = -1;

    if (index < 0 || index > TFT_MAX)
    {
        LOG("Invalid index %d", index);
        return;
    }

    if (mPrevIndex != index)
    {
        mPrevIndex = index;
        for (int i = 0; i < TFT_MAX; i++)
        {
            digitalWrite(mCsPinList[i], !(index == TFT_MAX || i == index));
        }
    }
}

void DisplayController::drawArrayJpeg(const uint8_t arrayname[], uint32_t array_size, int xpos, int ypos)
{
    int x = xpos;
    int y = ypos;

    mDrew = true;
    JpegDec.decodeArray(arrayname, array_size);
    renderJPEG(x, y);
}

void DisplayController::drawArrayJpegInCenter(const uint8_t arrayname[], uint32_t array_size)
{
    int x = 0;
    int y = 0;

    mDrew = true;
    JpegDec.decodeArray(arrayname, array_size);

    if (JpegDec.width < TFT_WIDTH)
    {
        x = (TFT_WIDTH - JpegDec.width) / 2;
    }
    if (JpegDec.height < TFT_HEIGHT)
    {
        y = (TFT_HEIGHT - JpegDec.height) / 2;
    }
    renderJPEG(x, y);
}

void DisplayController::renderJPEG(int xpos, int ypos)
{
    // retrieve infomration about the image
    uint16_t *pImg;
    uint16_t mcu_w = JpegDec.MCUWidth;
    uint16_t mcu_h = JpegDec.MCUHeight;
    uint32_t max_x = JpegDec.width;
    uint32_t max_y = JpegDec.height;

    // Jpeg images are draw as a set of image block (tiles) called MIN Coding Units (MCUs)
    // Typically these MCUs are 16x16 pixel blocks
    // Determine the width and height of the right and bottom edge image blocks
    uint32_t min_w = MIN(mcu_w, max_x % mcu_w);
    uint32_t min_h = MIN(mcu_h, max_y % mcu_h);

    // save the current image block size
    uint32_t win_w = mcu_w;
    uint32_t win_h = mcu_h;

    // record the current time so we can measure how long it takes to draw an image
    uint32_t drawTimeTick = xTaskGetTickCount();

    // save the coordinate of the right and bottom edges to assist image cropping
    // to the screen size
    max_x += xpos;
    max_y += ypos;

    // read each MCU block until there are no more
    while (JpegDec.readSwappedBytes())
    {
        // save a pointer to the image block
        pImg = JpegDec.pImage;

        // calculate where the image block should be drawn on the screen
        int mcu_x = JpegDec.MCUx * mcu_w + xpos; // Calculate coordinates of top left corner of current MCU
        int mcu_y = JpegDec.MCUy * mcu_h + ypos;

        // check if the image block size needs to be changed for the right edge
        win_w = mcu_x + mcu_w <= max_x ? mcu_w : min_w;

        // check if the image block size needs to be changed for the bottom edge
        win_h = mcu_y + mcu_h <= max_y ? mcu_h : min_h;

        // copy pixels into a contiguous block
        if (win_w != mcu_w)
        {
            uint16_t *cImg;
            int p = 0;
            cImg = pImg + win_w;
            for (int h = 1; h < win_h; h++)
            {
                p += mcu_w;
                for (int w = 0; w < win_w; w++)
                {
                    *cImg = *(pImg + w + p);
                    cImg++;
                }
            }
        }

        // draw image MCU block only if it will fit on the screen
        if ((mcu_x + win_w) <= tft->width() && (mcu_y + win_h) <= tft->height())
        {
            tft->pushRect(mcu_x, mcu_y, win_w, win_h, pImg);
        }
        else if ((mcu_y + win_h) >= tft->height())
        {
            JpegDec.abort(); // Image has run off bottom of screen so abort decoding
        }
    }

    // calculate how long it took to draw the image
#ifdef SHOW_RENDER_TIME
    showTime((xTaskGetTickCount() - drawTimeTick) / portTICK_PERIOD_MS);
#endif
}

void DisplayController::showTime(uint32_t msTime)
{
    mDrew = true;
    tft->setCursor(0, 0);
    tft->setTextFont(1);
    tft->setTextSize(2);
    tft->setTextColor(TFT_WHITE, TFT_BLACK);
    tft->print(msTime);
    tft->println(F(" ms "));

    LOG("Render time: %ld ms", msTime);
}

void DisplayController::showDigit(int digit)
{
    switch (digit)
    {
    case 0:
        drawArrayJpegInCenter(image_digit_0, sizeof(image_digit_0));
        break;
    case 1:
        drawArrayJpegInCenter(image_digit_1, sizeof(image_digit_1));
        break;
    case 2:
        drawArrayJpegInCenter(image_digit_2, sizeof(image_digit_2));
        break;
    case 3:
        drawArrayJpegInCenter(image_digit_3, sizeof(image_digit_3));
        break;
    case 4:
        drawArrayJpegInCenter(image_digit_4, sizeof(image_digit_4));
        break;
    case 5:
        drawArrayJpegInCenter(image_digit_5, sizeof(image_digit_5));
        break;
    case 6:
        drawArrayJpegInCenter(image_digit_6, sizeof(image_digit_6));
        break;
    case 7:
        drawArrayJpegInCenter(image_digit_7, sizeof(image_digit_7));
        break;
    case 8:
        drawArrayJpegInCenter(image_digit_8, sizeof(image_digit_8));
        break;
    case 9:
        drawArrayJpegInCenter(image_digit_9, sizeof(image_digit_9));
        break;
    default:
        break;
    }
}

void DisplayController::showHeader(const char *text)
{
    mDrew = true;
    tft->setTextColor(TFT_WHITE);

    uint32_t height = tft->fontHeight();
    uint32_t x = TFT_WIDTH / 2;
    uint32_t y = height / 2;

    tft->fillRect(0, 0, TFT_WIDTH, height, MENU_HEADER_COLOR);
    tft->setTextDatum(MC_DATUM);
    tft->drawString(text, x, y);
}

void DisplayController::showMenuList(MenuItemList *itemList, int currentIndex, bool isFirstTime)
{
    static int mPrevIndex = -1;
    static int startIndex = 0;

    LOG("showMenuList %d", isFirstTime);
    if (!itemList)
    {
        LOG("itemList is null");
        return;
    }

    mDrew = true;
    if (isFirstTime)
    {
        clear();
        showHeader("MENU");
    }

    uint16_t headerHeight = tft->fontHeight();
    uint16_t itemHeight = tft->fontHeight() * 1.3;
    uint16_t ypos = headerHeight;
    int maxDrawableItems = round((TFT_HEIGHT - headerHeight) * 1.0 / itemHeight);

    tft->setTextDatum(CL_DATUM);

    if (isFirstTime)
    {
        startIndex = 0;
        mPrevIndex = -1;
        currentIndex = 0;
    }
    else
    {
        if (currentIndex > mPrevIndex && startIndex + maxDrawableItems <= itemList->length() && currentIndex > startIndex + maxDrawableItems - 2)
        {
            startIndex++;
        }
        else if (currentIndex < mPrevIndex && startIndex > 0 && currentIndex < startIndex + 1)
        {
            startIndex--;
        }
    }

    for (int i = startIndex; i < itemList->length() && i < startIndex + maxDrawableItems; i++)
    {
        if (currentIndex != mPrevIndex)
        {
            if (mPrevIndex >= 0 && i == mPrevIndex)
            {
                tft->fillRect(0, ypos, TFT_WIDTH, itemHeight, MENU_BACKGROUND_COLOR);
            }
            else if (currentIndex >= 0 && i == currentIndex)
            {
                tft->fillRect(0, ypos, TFT_WIDTH, itemHeight, MENU_HIGHTLIGHT_COLOR);
            }
        }
        if (i == currentIndex)
        {
            tft->setTextColor(TFT_WHITE);
        }
        else
        {
            tft->setTextColor(TFT_WHITE, MENU_BACKGROUND_COLOR);
        }
        String itemName = itemList->get(i)->getName() + "    ";
        tft->drawString(itemName.c_str(), MENU_ITEM_LEFT_MARGIN, ypos + itemHeight / 2);
        ypos += itemHeight;
    }
    if (startIndex + maxDrawableItems > itemList->length())
    {
        tft->fillRect(0, ypos, TFT_WIDTH, TFT_HEIGHT - ypos, MENU_BACKGROUND_COLOR);
    }
    mPrevIndex = currentIndex;
}

void DisplayController::showEditPanel(MenuItem *item, bool isFirstTime)
{
    LOG("showEditPanel %d", isFirstTime);
    if (!item)
    {
        LOG("Item is null");
        return;
    }

    mDrew = true;
    if (isFirstTime)
    {
        clear();
        showHeader(item->getName().c_str());
    }

    uint16_t headerHeight = tft->fontHeight();
    tft->setTextColor(TFT_WHITE, MENU_BACKGROUND_COLOR);
    tft->setTextDatum(CC_DATUM);
    String before = item->isMinimum() ? "     " : "  <  ";
    String after = item->isMaximum() ? "     " : "  >  ";
    String str = before + item->getValueAsString() + after;
    tft->drawString(str.c_str(), TFT_WIDTH / 2, (TFT_HEIGHT + headerHeight) / 2);
}