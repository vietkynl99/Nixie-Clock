#ifndef _DISPLAY_CONTROLLER_H_
#define _DISPLAY_CONTROLLER_H_

#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include "../common/PinDefines.h"
#include "../common/FreeFonts.h"
#include "../model/MenuItemList.h"
#include "../common/Log.h"

// #define SHOW_RENDER_TIME

#define MENU_FONT                       FSB18 // Serif Bold 18pt
#define MENU_FONT_SIZE                  1
#define MENU_HEADER_COLOR 0x3C91
#define MENU_HIGHTLIGHT_COLOR 0x2124
#define MENU_BACKGROUND_COLOR TFT_BLACK
#define MENU_HIGHTLIGHT_TOP_MARGIN 10
#define MENU_ITEM_LEFT_MARGIN 12

#define POPUP_WIDTH_MARGIN 20
#define POPUP_HEIGHT_MARGIN 40
#define TEXT_LEFT_MARGIN 10
#define BUTTON_WIDTH 83
#define BUTTON_HEIGHT 40
#define BUTTON_BOTTOM_MARGIN 10
#define BUTTON_HIGHLIGH_BACKGROUND 0x0413 // cyan
#define BUTTON_BACKGROUND 0xCE99          // gray

#define POPUP_WIDTH (TFT_WIDTH - 2 * POPUP_WIDTH_MARGIN)
#define POPUP_HEIGHT (TFT_HEIGHT - 2 * POPUP_HEIGHT_MARGIN)
#define POPUP_XPOS (POPUP_WIDTH_MARGIN)
#define POPUP_YPOS (POPUP_HEIGHT_MARGIN)
#define BUTTON_OK_XPOS (POPUP_XPOS + (POPUP_WIDTH - 2 * BUTTON_WIDTH) / 3)
#define BUTTON_CANCEL_XPOS (POPUP_XPOS + BUTTON_WIDTH + (POPUP_WIDTH - 2 * BUTTON_WIDTH) * 2 / 3)
#define BUTTON_YPOS (POPUP_YPOS + POPUP_HEIGHT - BUTTON_HEIGHT - BUTTON_BOTTOM_MARGIN)

class DisplayController
{
private:
    static TFT_eSPI *tft;
    // static int mCsPinList[TFT_COUNT];

public:
    static void init();
    static void clear();
    static TFT_eSPI *getTft();

    static void setFont(const GFXfont *font, uint8_t size);
    static void selectDisplay(int index);

    static void drawArrayJpeg(const uint8_t array[], uint32_t arraySize, int xpos, int ypos);
    static void drawArrayJpegInCenter(const uint8_t array[], uint32_t arraySize);
    static void drawButton(int x, int y, int width, int height, String label, bool hightlight);

private:
    static bool outputDMACallback(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap);
    static void showTime(uint32_t msTime);
};

#endif