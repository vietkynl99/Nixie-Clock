#ifndef _WIFI_MASTER_H_
#define _WIFI_MASTER_H_

#include <Arduino.h>
#include <WiFi.h>
#include "../common/Log.h"

#define USE_WIFI_MANAGER

#ifdef USE_WIFI_MANAGER
#include <WiFiManager.h>

#define AP_SSID "Kynl Clock"
#define AP_PASSWORD "12345678"
#else
#define WIFI_SSID "Wifi Chua"
#define WIFI_PASSWORD "khongcomatkhau"
#endif

class WifiMaster
{
private:
#ifdef USE_WIFI_MANAGER
    static WiFiManager mWiFiManager;
#endif
static bool mIsScanning;

public:
    static void init();
    static void loop();
    static void startScanNetworks();
    static void stopScanNetworks();
    static void resetSettings();

private:
    static String getEncryptionTypeStr(uint8_t encType);
    static void printScannedNetWorks();
    static void scanNetworksHandler();
};

#endif