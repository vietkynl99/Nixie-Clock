#include "../../include/controller/DisplayController.h"
#include <TJpg_Decoder.h>

TFT_eSPI *DisplayController::tft = new TFT_eSPI();

static constexpr const char *const TAG = "DISPLAY";

void DisplayController::init()
{
    static bool initialized = false;
    if (!initialized)
    {
        LOG("init");
        initialized = true;

        pinMode(HC595_SHCP, OUTPUT);
        pinMode(HC595_STCP, OUTPUT);
        pinMode(HC595_DS, OUTPUT);

        selectDisplay(TFT_COUNT);
        tft->begin();
        tft->setRotation(DISPLAY_ROTATION);
        clear();

        tft->initDMA();
        TJpgDec.setJpgScale(1);
        tft->setSwapBytes(true);
        TJpgDec.setCallback(outputDMACallback);
    }
}

void DisplayController::clear()
{
    LOG("clear");
    tft->fillScreen(TFT_BLACK);
}

void DisplayController::setFont(const GFXfont *font, uint8_t size)
{
    tft->setFreeFont(font);
    tft->setTextSize(size);
}

void DisplayController::selectDisplay(int index)
{
    if (index < 0 || index > TFT_COUNT)
    {
        LOG("Invalid index %d", index);
        return;
    }
    selectMultiDisplay(index == TFT_COUNT ? 0xFF : 1 << index);
}

void DisplayController::selectMultiDisplay(uint8_t mask)
{
    static uint8_t prevMask = 0;

    if (prevMask == mask)
    {
        return;
    }
    prevMask = mask;

    digitalWrite(HC595_STCP, LOW);
    shiftOut(HC595_DS, HC595_SHCP, MSBFIRST, ~(mask << 1));
    digitalWrite(HC595_STCP, HIGH);
}

TFT_eSPI *DisplayController::getTft()
{
    return tft;
}

void DisplayController::drawArrayJpeg(const uint8_t array[], uint32_t arraySize, int xpos, int ypos)
{
#ifdef SHOW_RENDER_TIME
    uint32_t startTime = xTaskGetTickCount();
#endif

    tft->startWrite();
    TJpgDec.drawJpg(xpos, ypos, array, arraySize);
    tft->endWrite();

#ifdef SHOW_RENDER_TIME
    showTime((xTaskGetTickCount() - startTime) / portTICK_PERIOD_MS);
#endif
}

void DisplayController::drawArrayJpegInCenter(const uint8_t array[], uint32_t arraySize)
{
    int x = 0, y = 0;
    uint16_t width = 0, height = 0;

    TJpgDec.getJpgSize(&width, &height, array, arraySize);
    if (width < TFT_WIDTH)
    {
        x = (TFT_WIDTH - width) / 2;
    }
    if (height < TFT_HEIGHT)
    {
        y = (TFT_HEIGHT - height) / 2;
    }
    drawArrayJpeg(array, arraySize, x, y);
}

void DisplayController::drawButton(int x, int y, int width, int height, String label, bool hightlight)
{
    tft->fillRoundRect(x, y, width, height, 8, hightlight ? BUTTON_HIGHLIGH_BACKGROUND : BUTTON_BACKGROUND);
    tft->setFreeFont(FSB12);
    tft->setTextSize(1);
    tft->setTextColor(hightlight ? TFT_WHITE : TFT_BLACK);
    tft->setTextDatum(CC_DATUM);
    tft->drawString(label, x + width / 2, y + height / 2);
}

void DisplayController::drawWifiIcon(int x, int y, WifiIconType type, uint16_t color, bool clearBackground)
{
    x += 21;
    y += 40;
    if (clearBackground)
    {
        tft->fillRect(x - 21, y - 32, 42, 40, TFT_BLACK);
    }

    tft->fillCircle(x, y, 3, color);
    int strength = type >= WIFI_ICON_TYPE_0_SIGNAL && type <= WIFI_ICON_TYPE_3_SIGNAL ? type : WIFI_ICON_TYPE_3_SIGNAL;
    for (int i = 1; i <= 3; i++)
    {
        tft->drawArc(x, y, 8 * i + 2, 8 * i, 135, 225, i<=strength?color:TFT_BLACK, TFT_BLACK);
    }

    if (type == WIFI_ICON_TYPE_DISCONNECTED)
    {
        setFont(FF8, 1);
        tft->setTextColor(color);
        tft->setTextDatum(CC_DATUM);
        tft->drawString("\\", x, y - 16);
    }
}

bool DisplayController::outputDMACallback(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap)
{
    static uint16_t dmaBuffer1[16 * 16]; // Toggle buffer for 16*16 MCU block, 512bytes
    static uint16_t dmaBuffer2[16 * 16]; // Toggle buffer for 16*16 MCU block, 512bytes
    static uint16_t *dmaBufferPtr = dmaBuffer1;
    static bool dmaBufferSel = 0;

    if (y >= tft->height())
    {
        return 0;
    }

    // Double buffering is used, the bitmap is copied to the buffer by pushImageDMA() the
    // bitmap can then be updated by the jpeg decoder while DMA is in progress
    dmaBufferPtr = dmaBufferSel ? dmaBuffer2 : dmaBuffer1;
    dmaBufferSel = !dmaBufferSel; // Toggle buffer selection

    // pushImageDMA() will clip the image block at screen boundaries before initiating DMA
    // Initiate DMA - blocking only if last DMA is not complete
    // The DMA transfer of image block to the TFT is now in progress...
    tft->pushImageDMA(x, y, w, h, bitmap, dmaBufferPtr);

    // Return 1 to decode next block.
    return 1;
}

void DisplayController::showTime(uint32_t msTime)
{
    tft->setCursor(0, 0);
    tft->setTextFont(1);
    tft->setTextSize(2);
    tft->setTextColor(TFT_WHITE, TFT_BLACK);
    tft->print(msTime);
    tft->println(F(" ms "));

    LOG("Render time: %ld ms", msTime);
}