#ifndef _WIFI_MASTER_H_
#define _WIFI_MASTER_H_

#include <Arduino.h>
#include <WiFiManager.h>
#include "../common/Log.h"

#define USE_WIFI_MANAGER

#ifdef USE_WIFI_MANAGER
#define AP_SSID "Kynl Clock"
#define AP_PASSWORD "12345678"
#else
#define WIFI_SSID "Wifi Chua"
#define WIFI_PASSWORD "khongcomatkhau"
#endif

class WifiMaster
{
private:
    static WiFiManager mWiFiManager;

public:
    static void init();
    static void loop();
    static void resetSettings();
};

#endif