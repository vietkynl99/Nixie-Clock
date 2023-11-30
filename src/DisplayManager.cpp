#include "../include/clockDigit.h"
#include "../include/DisplayManager.h"
#include "../include/Log.h"

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

TFT_eSPI *DisplayManager::tft = new TFT_eSPI();
bool DisplayManager::mIsInitialized = false;
bool DisplayManager::mDrew = true;

static constexpr const char *const TAG = "DISPLAY";

void DisplayManager::init()
{
    if (!mIsInitialized)
    {
        LOG("init");
        mIsInitialized = true;
        tft->begin();
        tft->setRotation(DISPLAY_ROTATION);
        clear();
    }
}

void DisplayManager::loop()
{
    static uint32_t timeTick = 0;
    static int digit = 0;

    if (xTaskGetTickCount() > timeTick)
    {
        timeTick = xTaskGetTickCount() + 1000 / portTICK_PERIOD_MS;
        digit++;
        if (digit > 9)
        {
            digit = 0;
        }
        showDigit(digit);
    }
}

void DisplayManager::clear()
{
    if (mDrew)
    {
        mDrew = false;
        LOG("clear");
        tft->fillScreen(TFT_BLACK);
    }
}

void DisplayManager::setFont(const GFXfont *font)
{
    tft->setFreeFont(font);
}

void DisplayManager::setTextColor(uint16_t color)
{
    tft->setTextColor(color);
}

void DisplayManager::setTextColor(uint16_t fgcolor, uint16_t bgcolor, bool bgfill)
{
    tft->setTextColor(fgcolor, bgcolor, bgfill);
}

void DisplayManager::setCursor(int16_t x, int16_t y)
{
    tft->setCursor(x, y);
}

void DisplayManager::setTextDatum(uint8_t datum)
{
    tft->setTextDatum(datum);
}

void DisplayManager::drawString(const char *string, int32_t x, int32_t y)
{
    mDrew = true;
    tft->drawString(string, x, y);
}

void DisplayManager::drawString(const String &string, int32_t x, int32_t y)
{
    mDrew = true;
    tft->drawString(string, x, y);
}

void DisplayManager::setHeader(const char *text, uint16_t color)
{
    uint32_t height = tft->fontHeight(GFXFF);
    uint32_t x = TFT_WIDTH / 2;
    uint32_t y = height / 2;

    mDrew = true;
    tft->fillRect(0, 0, TFT_WIDTH, height, color);
    tft->setTextDatum(MC_DATUM);
    tft->drawString(text, x, y, GFXFF);
    // drawDatumMarker(x, y);
}

int16_t DisplayManager::getFontHeight()
{
    // return tft->fontHeight(GFXFF);
    return tft->fontHeight();
}

void DisplayManager::fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color)
{
    mDrew = true;
    tft->fillRect(x, y, w, h, color);
}

void DisplayManager::print(const char *text)
{
    mDrew = true;
    tft->print(text);
}

void DisplayManager::println(const char *text)
{
    mDrew = true;
    tft->println(text);
}

void DisplayManager::drawDatumMarker(int x, int y, uint16_t color)
{
    mDrew = true;
    tft->drawLine(x - 5, y, x + 5, y, color);
    tft->drawLine(x, y - 5, x, y + 5, color);
}

void DisplayManager::drawArrayJpeg(const uint8_t arrayname[], uint32_t array_size, int xpos, int ypos)
{
    int x = xpos;
    int y = ypos;

    mDrew = true;
    JpegDec.decodeArray(arrayname, array_size);
    renderJPEG(x, y);
}

void DisplayManager::drawArrayJpegInCenter(const uint8_t arrayname[], uint32_t array_size)
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

void DisplayManager::renderJPEG(int xpos, int ypos)
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

void DisplayManager::showTime(uint32_t msTime)
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

void DisplayManager::showDigit(int digit)
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