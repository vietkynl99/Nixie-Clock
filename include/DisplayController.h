#ifndef _DISPLAY_CONTROLLER_H_
#define _DISPLAY_CONTROLLER_H_

#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <JPEGDecoder.h>
#include "FreeFonts.h"
#include "MenuItemList.h"

/*
  #########################################################################
  ###### DON'T FORGET TO UPDATE THE User_Setup.h FILE IN THE LIBRARY ######
  ######       TO SELECT YOUR DISPLAY TYPE AND ENABLE FONTS          ######
  #########################################################################
*/

#ifndef ST7789_DRIVER
#error ST7789_DRIVER must be choosen
#endif
#if TFT_RGB_ORDER != TFT_BGR
#error TFT_RGB_ORDER must be TFT_BGR
#endif
#if TFT_WIDTH != 240
#error TFT_WIDTH must be 240
#endif
#if TFT_HEIGHT != 240
#error TFT_HEIGHT must be 240
#endif
#if TFT_MISO != -1
#error TFT_MISO must be -1
#endif
#if TFT_MOSI != 23
#error TFT_MOSI must be 23
#endif
#if TFT_SCLK != 18
#error TFT_SCLK must be 18
#endif
#if TFT_CS != -1
#error TFT_CS must be -1
#endif
#if TFT_DC != 2
#error TFT_DC must be 2
#endif
#if TFT_RST != 4
#error TFT_RST must be 4
#endif

#define TFT_MAX     6 // Number of TFT displays
#define TFT1_CS     22
#define TFT2_CS     21
#define TFT3_CS     13
#define TFT4_CS     14
#define TFT5_CS     25
#define TFT6_CS     26

#define DISPLAY_ROTATION    2 // rotation 180 degrees

// #define SHOW_RENDER_TIME

#define MENU_FONT                       FSB18 // Serif Bold 18pt
#define MENU_FONT_SIZE                  1
#define MENU_HEADER_COLOR               0x3C91
#define MENU_HIGHTLIGHT_COLOR           0x2124
#define MENU_BACKGROUND_COLOR           TFT_BLACK
#define MENU_HIGHTLIGHT_TOP_MARGIN      10
#define MENU_ITEM_LEFT_MARGIN           12

class DisplayController
{
private:
    static TFT_eSPI *tft;
    static bool mIsInitialized;
    static int mCsPinList[TFT_MAX];

public:
    static void init();
    static void clear();

    static void setFont(const GFXfont *font, uint8_t size);
    static void selectDisplay(int index);

    static TFT_eSPI *getTft();

    static void showDigit(int digit);
    static void showHeader(const char *text);
    static void showMenuList(MenuItemList *itemList, int currentIndex, bool isFirstTime);
    static void showEditPanel(MenuItem *item, bool isFirstTime);

private:
    static void drawArrayJpeg(const uint8_t arrayname[], uint32_t array_size, int xpos, int ypos);
    static void drawArrayJpegInCenter(const uint8_t arrayname[], uint32_t array_size);
    static void renderJPEG(int xpos, int ypos);
    static void jpegInfo();
    static void showTime(uint32_t msTime);
};

#endif