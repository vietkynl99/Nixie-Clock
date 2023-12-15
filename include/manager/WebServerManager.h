#ifndef _WEB_SERVER_MANAGER_H_
#define _WEB_SERVER_MANAGER_H_

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include "../common/Log.h"
#include "../common/MessageEvent.h"
#include "../controller/WifiMaster.h"

#define MDNS_SERVER_NAME "kynlclock"

#define SERVER_USERNAME "kynl"
#define SERVER_PASSWORD "123456"

class WebServerManager
{
private:
    static WebServer *mServer;

public:
    static void init();
    static void loop();

private:
    static void startServer();
    static void notFoundHandler();
    static void loginHandler();
    static void rootHandler();
    static bool isAuthentified();
    static void statusHandler();
};
#endif