#ifndef _WIFI_MASTER_H_
#define _WIFI_MASTER_H_

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <WiFiUdp.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <FS.h>
#include "../common/PinDefines.h"
#include "../common/Log.h"
#include "../common/FileSystem.h"

#define USE_WIFI_MANAGER

#ifdef USE_WIFI_MANAGER
#define AP_SSID "Kynl Clock"
#define AP_PASSWORD "12345678"
#define AP_IP_ADDR IPAddress(192, 168, 4, 1)

enum WifiMasterState
{
    WIFI_MASTER_STATE_NONE,
    WIFI_MASTER_STATE_CONNECTING,
    WIFI_MASTER_STATE_CONFIG_PORTAL,
    WIFI_MASTER_STATE_CONNECTED,
    WIFI_MASTER_STATE_DISCONNECTED
};
#else
#define WIFI_SSID "Wifi Chua"
#define WIFI_PASSWORD "khongcomatkhau"
#endif

class WifiMaster
{
private:
#ifdef USE_WIFI_MANAGER
    static String mSavedSSID;
    static String mSavedPassword;
    static int mState;
    static WebServer *mServer;
    static bool mStartedmDNS;
    static bool mIsScanning;
#endif

public:
    static void init();
    static void loop();
#ifdef USE_WIFI_MANAGER
    static void resetSettings();
    static String getEncryptionTypeStr(uint8_t encType);
    static void printScannedNetWorks();
    static int getState();
    static String getStateStr();
#endif

private:
#ifdef USE_WIFI_MANAGER
    static void setState(int state);
    static void startScanNetworks();
    static void stopScanNetworks();
    static bool getScannedWifiHtmlStr(String &str);

    static bool isValidWifiSettings();
    static void readSavedSettings();
    static void saveSettings();
    static void startConfigPortal();
    static void stopConfigPortal();
    static void startServer();
    static void stopServer();
    static void startMDNS();
    static void stopMDNS();
    static void startConnectToSavedWifi();
    static void stopConnectToSavedWifi();

    static void sendNotFound();
    static void notFoundHandler();
    static void rootHandler();
    static void saveDataHandler();

    static void processHandler();
#endif
};

#endif