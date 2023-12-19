#ifndef _WEB_SERVER_MANAGER_H_
#define _WEB_SERVER_MANAGER_H_

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "../common/Log.h"
#include "../common/MessageEvent.h"
#include "../common/Helper.h"
#include "../controller/WifiMaster.h"
#include "../controller/RTCController.h"
#include "../manager/SettingsManager.h"

#define MDNS_SERVER_NAME "kynlclock"

#define SERVER_USERNAME "kynl"
#define SERVER_PASSWORD "123456"

class WebServerManager
{
private:
    static WebServer *mServer;
    static WiFiUDP mNtpUDP;
    static NTPClient *mTimeClient;

public:
    static void init();
    static void loop();

    static String getNTPTime();

private:
    static void startServer();
    static void startNTP();
    static void notFoundHandler();
    static void loginHandler();
    static void rootHandler();
    static bool isAuthentified();
    static void statusHandler();
    static void ntpHandler();
};
#endif