#ifndef _DISPLAY_MANAGER_H_
#define _DISPLAY_MANAGER_H_

#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <JPEGDecoder.h>

// DO NOT FORGET EDIT "User_Setup.h" FILE IN TFT_eSPI LIBRARY
#ifndef ST7789_DRIVER
#error ST7789_DRIVER must be choosen
#endif
#if TFT_RGB_ORDER != TFT_BGR
#error TFT_RGB_ORDER must be TFT_BGR
#endif

#define DISPLAY_WIDTH 240
#define DISPLAY_HEIGHT 240
#define DISPLAY_ROTATION 2 // rotation 180 degrees

#define SHOW_RENDER_TIME

class DisplayManager
{
private:
    static TFT_eSPI *tft;

public:
    static void init();
    static void loop();
    static void clear();
    static void showDigit(int digit);

private:
    static void drawArrayJpeg(const uint8_t arrayname[], uint32_t array_size, int xpos, int ypos);
    static void drawArrayJpegInCenter(const uint8_t arrayname[], uint32_t array_size);
    static void renderJPEG(int xpos, int ypos);
    static void jpegInfo();
    static void showTime(uint32_t msTime);
};

#endif