#include "clockDigit.h"
#include "DisplayManager.h"

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

TFT_eSPI *DisplayManager::tft = new TFT_eSPI(DISPLAY_WIDTH, DISPLAY_HEIGHT);

void DisplayManager::init()
{
    tft->begin();

    tft->setRotation(DISPLAY_ROTATION);
    clear();

    drawArrayJpegInCenter(rgbTest, sizeof(rgbTest));
    delay(3000);

    clear();
}

void DisplayManager::loop()
{
    drawArrayJpegInCenter(digit_0, sizeof(digit_0));
    delay(1000);
    drawArrayJpegInCenter(digit_1, sizeof(digit_1));
    delay(1000);
    drawArrayJpegInCenter(digit_2, sizeof(digit_2));
    delay(1000);
    drawArrayJpegInCenter(digit_3, sizeof(digit_3));
    delay(1000);
    drawArrayJpegInCenter(digit_4, sizeof(digit_4));
    delay(1000);
    drawArrayJpegInCenter(digit_5, sizeof(digit_5));
    delay(1000);
    drawArrayJpegInCenter(digit_6, sizeof(digit_6));
    delay(1000);
    drawArrayJpegInCenter(digit_7, sizeof(digit_7));
    delay(1000);
    drawArrayJpegInCenter(digit_8, sizeof(digit_8));
    delay(1000);
    drawArrayJpegInCenter(digit_9, sizeof(digit_9));
    delay(1000);
}

void DisplayManager::clear()
{
    tft->fillScreen(TFT_BLACK);
}

void DisplayManager::drawArrayJpeg(const uint8_t arrayname[], uint32_t array_size, int xpos, int ypos)
{

    int x = xpos;
    int y = ypos;

    JpegDec.decodeArray(arrayname, array_size);

    // jpegInfo(); // Print information from the JPEG file (could comment this line out)

    renderJPEG(x, y);
}

void DisplayManager::drawArrayJpegInCenter(const uint8_t arrayname[], uint32_t array_size)
{

    int x = 0;
    int y = 0;

    JpegDec.decodeArray(arrayname, array_size);

    if (JpegDec.width < DISPLAY_WIDTH)
    {
        x = (DISPLAY_WIDTH - JpegDec.width) / 2;
    }
    if (JpegDec.height < DISPLAY_HEIGHT)
    {
        y = (DISPLAY_HEIGHT - JpegDec.height) / 2;
    }

    // jpegInfo(); // Print information from the JPEG file (could comment this line out)

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
    uint32_t drawTime = millis();

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
    showTime(millis() - drawTime);
#endif
}

void DisplayManager::jpegInfo()
{
    Serial.println(F("==============="));
    Serial.println(F("JPEG image info"));
    Serial.println(F("==============="));
    Serial.print(F("Width      :"));
    Serial.println(JpegDec.width);
    Serial.print(F("Height     :"));
    Serial.println(JpegDec.height);
    Serial.print(F("Components :"));
    Serial.println(JpegDec.comps);
    Serial.print(F("MCU / row  :"));
    Serial.println(JpegDec.MCUSPerRow);
    Serial.print(F("MCU / col  :"));
    Serial.println(JpegDec.MCUSPerCol);
    Serial.print(F("Scan type  :"));
    Serial.println(JpegDec.scanType);
    Serial.print(F("MCU width  :"));
    Serial.println(JpegDec.MCUWidth);
    Serial.print(F("MCU height :"));
    Serial.println(JpegDec.MCUHeight);
    Serial.println(F("==============="));
}

void DisplayManager::showTime(uint32_t msTime)
{
    tft->setCursor(0, 0);
    tft->setTextFont(1);
    tft->setTextSize(2);
    tft->setTextColor(TFT_WHITE, TFT_BLACK);
    tft->print(msTime);
    tft->println(F(" ms "));

    Serial.print(F("Render time:"));
    Serial.print(msTime);
    Serial.println(F(" ms "));
}
