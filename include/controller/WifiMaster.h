#ifndef _WIFI_MASTER_H_
#define _WIFI_MASTER_H_

#include <Arduino.h>
#include <WiFiManager.h>
#include "../common/Log.h"

/* AP mode */
#define AP_SSID "Kynl Clock"
#define AP_PASSWORD "12345678"

class WifiMaster
{
private:
    static WiFiManager mWiFiManager;

public:
    static void init();
    static void loop();
    static void printConnectedWifiInfo();
    static void resetSettings();

private:
    static void statusHandler();
};

#endif