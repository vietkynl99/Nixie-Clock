#ifndef _SERVER_MANAGER_H_
#define _SERVER_MANAGER_H_

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "../common/Log.h"
#include "../common/MessageEvent.h"
#include "../common/Helper.h"
#include "../common/FileSystem.h"
#include "../controller/WifiMaster.h"
#include "../controller/RTCController.h"
#include "../controller/HardwareController.h"
#include "../manager/SettingsManager.h"

#define SERVER_USERNAME "kynl"
#define SERVER_PASSWORD "123456"

class ServerManager
{
private:
    static WebServer *mServer;
    static WiFiUDP mNtpUDP;
    static NTPClient *mTimeClient;
    static bool mWifiEnabled;

public:
    static void init();
    static void loop();

    static String getNTPTime();

private:
    static void startServer();
    static void startNTP();
    static bool isAuthentified();
    static void sendNotFound();

    static void notFoundHandler();
    static void loginHandler();
    static void rootHandler();
    static void statusHandler();
    static void ntpHandler();
};
#endif