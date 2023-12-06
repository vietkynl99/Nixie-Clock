#ifndef _WIFI_MASTER_H_
#define _WIFI_MASTER_H_

// #include <EEPROM.h>
// #include <ArduinoJson.h>
// #include <ESP8266WiFi.h>
// #include <WiFiClient.h>
#include <WiFiManager.h>
// #include <ESP8266WebServer.h>
// #include <UART_Debug.h>
#include "../common/Log.h"

/* AP mode */
#define AP_SSID "Nixie Clock"
#define AP_PASSWORD "12345678"

class WifiMaster
{
private:
    static WiFiManager wm;

public:
    static void printConnectedWifiInfo();
    static void init();
    static void resetWifiSettings();
    static void loop();
};

#endif