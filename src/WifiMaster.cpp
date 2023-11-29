#include <Arduino.h>
#include "../include/WifiMaster.h"

WiFiManager WifiMaster::wm;

static constexpr const char *const TAG = "WIFI";

void WifiMaster::printConnectedWifiInfo()
{
    LOG("SSID: '%s' IP: '%s'", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
}

void WifiMaster::init()
{
    WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
    delay(3000);

    LOG("Starting");

    // wm.resetSettings(); // wipe settings

    wm.setConfigPortalBlocking(false);

    // custom menu via array or vector
    // menu tokens, "wifi","wifinoscan","info","param","close","sep","erase","restart","exit" (sep is seperator) (if param is in menu, params will not show up in wifi page!)
    std::vector<const char *> menu = {"wifi", "param", "sep", "restart", "exit"};
    wm.setMenu(menu);

    // set dark theme
    // wm.setClass("invert");

    // set static ip
    //  wm.setSTAStaticIPConfig(IPAddress(10,0,1,99), IPAddress(10,0,1,1), IPAddress(255,255,255,0)); // set static ip,gw,sn
    //  wm.setShowStaticFields(true); // force show static ip fields
    //  wm.setShowDnsFields(true);    // force show dns field always

    // wm.setConnectTimeout(20); // how long to try to connect for before continuing
    wm.setConfigPortalTimeout(120); // auto close configportal after n seconds
    // wm.setCaptivePortalEnable(false); // disable captive portal redirection
    // wm.setAPClientCheck(true); // avoid timeout if client connected to softap

    // wifi scan settings
    // wm.setRemoveDuplicateAPs(false); // do not remove duplicate ap names (true)
    // wm.setMinimumSignalQuality(20);  // set min RSSI (percentage) to show in scans, null = 8%
    // wm.setShowInfoErase(false);      // do not show erase button on info page
    // wm.setScanDispPerc(true);       // show RSSI as percentage not graph icons

    // wm.setBreakAfterConfig(true);   // always exit configportal even if wifi save fails

    // res = wm.autoConnect(); // auto generated AP name from chipid
    // res = wm.autoConnect("AutoConnectAP"); // anonymous ap
    bool res = wm.autoConnect(AP_SSID, AP_PASSWORD); // password protected ap

    if (!res)
    {
        LOG("Failed to connect or hit timeout");
        // ESP.restart();
    }
    else
    {
        // if you get here you have connected to the WiFi
        LOG("Wifi connected!!!");
        printConnectedWifiInfo();
    }
}

void WifiMaster::resetWifiSettings()
{
    LOG("Restart Wifi Settings");
    wm.resetSettings();
    ESP.restart();

    // // start portal w delay
    // LOG("Starting config portal...");
    // wm.setConfigPortalTimeout(120);

    // if (!wm.startConfigPortal("OnDemandAP", "password"))
    // {
    //     LOG("Failed to connect or hit timeout");
    //     delay(3000);
    //     // ESP.restart();
    // }
    // else
    // {
    //     // if you get here you have connected to the WiFi
    //     LOG("Wifi connected!!!");
    //     printConnectedWifiInfo();
    // }
}

void WifiMaster::loop()
{
    wm.process();
}