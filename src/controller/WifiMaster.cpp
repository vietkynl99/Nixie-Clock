#include "../../include/controller/WifiMaster.h"

#ifdef USE_WIFI_MANAGER
WiFiManager WifiMaster::mWiFiManager;
#endif
bool WifiMaster::mIsScanning = false;

static constexpr const char *const TAG = "WIFI";

void WifiMaster::init()
{
    WiFi.mode(WIFI_STA);

#ifdef USE_WIFI_MANAGER
    // mWiFiManager.resetSettings(); // wipe settings

    mWiFiManager.setConfigPortalBlocking(false);

    // custom menu via array or vector
    // menu tokens, "wifi","wifinoscan","info","param","close","sep","erase","restart","exit" (sep is seperator) (if param is in menu, params will not show up in wifi page!)
    std::vector<const char *> menu = {"wifi", "param", "sep", "restart", "exit"};
    mWiFiManager.setMenu(menu);

    // set dark theme
    // mWiFiManager.setClass("invert");

    // set static ip
    //  mWiFiManager.setSTAStaticIPConfig(IPAddress(10,0,1,99), IPAddress(10,0,1,1), IPAddress(255,255,255,0)); // set static ip,gw,sn
    //  mWiFiManager.setShowStaticFields(true); // force show static ip fields
    //  mWiFiManager.setShowDnsFields(true);    // force show dns field always

    // mWiFiManager.setConnectTimeout(20); // how long to try to connect for before continuing
    mWiFiManager.setConfigPortalTimeout(120); // auto close configportal after n seconds
    // mWiFiManager.setCaptivePortalEnable(false); // disable captive portal redirection
    mWiFiManager.setAPClientCheck(true); // avoid timeout if client connected to softap

    // wifi scan settings
    // mWiFiManager.setRemoveDuplicateAPs(false); // do not remove duplicate ap names (true)
    // mWiFiManager.setMinimumSignalQuality(20);  // set min RSSI (percentage) to show in scans, null = 8%
    // mWiFiManager.setShowInfoErase(false);      // do not show erase button on info page
    // mWiFiManager.setScanDispPerc(true);       // show RSSI as percentage not graph icons

    // mWiFiManager.setBreakAfterConfig(true);   // always exit configportal even if wifi save fails

    // res = mWiFiManager.autoConnect(); // auto generated AP name from chipid
    // res = mWiFiManager.autoConnect("AutoConnectAP"); // anonymous ap
    mWiFiManager.autoConnect(AP_SSID, AP_PASSWORD); // password protected ap
#else
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    LOG("Connecting to " WIFI_SSID);
#endif
}

void WifiMaster::loop()
{
#ifdef USE_WIFI_MANAGER
    mWiFiManager.process();
#endif
    scanNetworksHandler();
}

void WifiMaster::startScanNetworks()
{
    if (!mIsScanning)
    {
        LOG("Start scan networks");
        mIsScanning = true;
        if (WiFi.status() == WL_CONNECTED)
        {
            LOG("Disconnecting networks");
        }
        if (WiFi.getMode() != WIFI_STA)
        {
            WiFi.mode(WIFI_STA);
        }
        WiFi.disconnect();
        WiFi.scanNetworks(true);
    }
}

void WifiMaster::stopScanNetworks()
{
    if (mIsScanning)
    {
        LOG("Stop scan networks");
        WiFi.scanDelete();
        mIsScanning = false;
    }
}

void WifiMaster::resetSettings()
{
#ifdef USE_WIFI_MANAGER
    LOG("Reset WiFi settings");
    mWiFiManager.resetSettings();
    ESP.restart();
#endif
}

String WifiMaster::getEncryptionTypeStr(uint8_t encType)
{
    switch (encType)
    {
    case WIFI_AUTH_OPEN:
        return "open";
    case WIFI_AUTH_WEP:
        return "WEP";
    case WIFI_AUTH_WPA_PSK:
        return "WPA";
    case WIFI_AUTH_WPA2_PSK:
        return "WPA2";
    case WIFI_AUTH_WPA_WPA2_PSK:
        return "WPA+WPA2";
    case WIFI_AUTH_WPA2_ENTERPRISE:
        return "WPA2-EAP";
    case WIFI_AUTH_WPA3_PSK:
        return "WPA3";
    case WIFI_AUTH_WPA2_WPA3_PSK:
        return "WPA2+WPA3";
    case WIFI_AUTH_WAPI_PSK:
        return "WAPI";
    default:
        return "unknown";
    }
}

void WifiMaster::printScannedNetWorks()
{
    int i = 0;
    String ssid;
    uint8_t encType;
    int32_t rssi;
    uint8_t *bssid;
    int32_t channel;

    while (WiFi.getNetworkInfo(i, ssid, encType, rssi, bssid, channel))
    {
        LOG("%2d. %-24s %4d | %s", i + 1, ssid.c_str(), rssi, getEncryptionTypeStr(encType).c_str());
        i++;
    }
}

void WifiMaster::scanNetworksHandler()
{
    static uint32_t timeTick = 0;
    if (xTaskGetTickCount() > timeTick)
    {
        timeTick = xTaskGetTickCount() + 500 / portTICK_PERIOD_MS;
        int n = WiFi.scanComplete();
        if (n == WIFI_SCAN_RUNNING)
        {
            LOG("WiFi scan running");
        }
        else if (n == 0)
        {
            LOG("No networks found");
            stopScanNetworks();
        }
        else if (n > 0)
        {
            LOG("Found %d networks", n);
            printScannedNetWorks();
            stopScanNetworks();
        }
    }
}