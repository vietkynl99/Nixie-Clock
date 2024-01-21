#ifndef _CONFIGS_H_
#define _CONFIGS_H_

#include <TFT_eSPI.h>

// Name
#define DEVICE_NAME "Kynl Clock"
#define DEVICE_USERNAME "kynl"
#define DEVICE_PASSWORD "12345678"

// Web server
#define MDNS_SERVER_NAME "kynlclock"

// Hardware Controller
#define TOUCH_PIN_COUNT 3
#define BUZZER_PIN 27
#define BUZZER_PWM_CHANNEL 0
#define BUZZER_PWM_FREQUENCY 500 // 12kHz
#define BUZZER_PWM_RESOLUTION 8 // 8bit
#define TOUCH_PIN_BUTTON1 T4
#define TOUCH_PIN_BUTTON2 T5
#define TOUCH_PIN_BUTTON3 T6

#define BUTTON1_INDEX 0
#define BUTTON2_INDEX 1
#define BUTTON3_INDEX 2

// DHT11
#define DHT_SENSOR_PIN 32
#define DHT_SENSOR_TYPE DHT_TYPE_11

// WS2812 led
#define WS2812_LED_COUNT 6
#define WS2812_LED_PIN 26

// TFT display
// DON'T FORGET TO UPDATE THE User_Setup.h FILE IN THE LIBRARY TFT_eSPI
#define TFT_COUNT     6 // Number of TFT displays
#define DISPLAY_ROTATION    2 // rotation 180 degrees

#define HC595_SHCP 15
#define HC595_STCP 2
#define HC595_DS 5

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
#if TFT_DC != 33
#error TFT_DC must be 33
#endif
#if TFT_RST != 4
#error TFT_RST must be 4
#endif

// Render time
// #define SHOW_RENDER_TIME

#endif