#ifndef _DISPLAY_MANAGER_H_
#define _DISPLAY_MANAGER_H_

#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <JPEGDecoder.h>
#include "FreeFonts.h"

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

#define DISPLAY_ROTATION 2 // rotation 180 degrees

#define SHOW_RENDER_TIME

class DisplayManager
{
private:
    static bool mIsInitialized;
    static TFT_eSPI *tft;

public:
    static void init();
    static void loop();
    static void clear();
    static void setFont(const GFXfont *font);
    static void setTextColor(uint16_t color);
    static void setTextColor(uint16_t fgcolor, uint16_t bgcolor, bool bgfill = false);
    static void setCursor(int16_t x, int16_t y);
    static void setTextDatum(uint8_t datum);
    static void drawString(const char *string, int32_t x, int32_t y);
    static void drawString(const String& string, int32_t x, int32_t y);
    static void setHeader(const char *text, uint16_t color);
    static int16_t getFontHeight();
    static void fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color);
    static void print(const char *text);
    static void println(const char *text);
    static void showDigit(int digit);

private:
    static void drawDatumMarker(int x, int y, uint16_t color = TFT_GREEN); // Draw a + mark centred on x,y
    static void drawArrayJpeg(const uint8_t arrayname[], uint32_t array_size, int xpos, int ypos);
    static void drawArrayJpegInCenter(const uint8_t arrayname[], uint32_t array_size);
    static void renderJPEG(int xpos, int ypos);
    static void jpegInfo();
    static void showTime(uint32_t msTime);
};

#endif