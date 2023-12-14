#include "../../include/controller/WifiMaster.h"

WiFiManager WifiMaster::mWiFiManager;

static constexpr const char *const TAG = "WIFI";

void WifiMaster::init()
{
    WiFi.mode(WIFI_STA);

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
    // mWiFiManager.setAPClientCheck(true); // avoid timeout if client connected to softap

    // wifi scan settings
    // mWiFiManager.setRemoveDuplicateAPs(false); // do not remove duplicate ap names (true)
    // mWiFiManager.setMinimumSignalQuality(20);  // set min RSSI (percentage) to show in scans, null = 8%
    // mWiFiManager.setShowInfoErase(false);      // do not show erase button on info page
    // mWiFiManager.setScanDispPerc(true);       // show RSSI as percentage not graph icons

    // mWiFiManager.setBreakAfterConfig(true);   // always exit configportal even if wifi save fails

    // res = mWiFiManager.autoConnect(); // auto generated AP name from chipid
    // res = mWiFiManager.autoConnect("AutoConnectAP"); // anonymous ap
    mWiFiManager.autoConnect(AP_SSID, AP_PASSWORD); // password protected ap
}

void WifiMaster::loop()
{
    mWiFiManager.process();
    statusHandler();
}

void WifiMaster::printConnectedWifiInfo()
{
    LOG("SSID: %s, IP: %s", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
}

void WifiMaster::resetSettings()
{
    LOG("Reset WiFi settings");
    mWiFiManager.resetSettings();
    ESP.restart();
}

void WifiMaster::statusHandler()
{
    static uint32_t timeTick = 0;
    static bool status = false;

    if (xTaskGetTickCount() > timeTick)
    {
        timeTick = xTaskGetTickCount() + 1000 / portTICK_PERIOD_MS;
        if (status != WiFi.isConnected())
        {
            status = WiFi.isConnected();
            if (status)
            {
                LOG("WiFi connected");
                printConnectedWifiInfo();
            }
            else
            {
                LOG("WiFi disconnected");
            }
        }
    }
}