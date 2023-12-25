#include "../../include/controller/WifiMaster.h"

#ifdef USE_WIFI_MANAGER
String WifiMaster::mSavedSSID = "";
String WifiMaster::mSavedPassword = "";
int WifiMaster::mState = WIFI_MASTER_STATE_NONE;
WebServer *WifiMaster::mServer = nullptr;
bool WifiMaster::mStartedmDNS = false;

const char HTML_WIFI_ITEM1[] PROGMEM = "<div><a href='#p' onclick='c(this)'>";
const char HTML_WIFI_ITEM2[] PROGMEM = "</a><div class='q q-";
const char HTML_WIFI_LOCK[] PROGMEM = " l";
const char HTML_WIFI_ITEM3[] PROGMEM = "'></div></div>";
const char HTML_WIFI_LIST[] PROGMEM = "<!-- HTML_WIFI_LIST -->";
const char HTML_NO_NETWORKS_FOUND[] PROGMEM = "<label>No networks found</label><br>";

#define SSID_FILE_PATH "/ssid.txt"
#define PASS_FILE_PATH "/pass.txt"
#define CONFIG_WIFI_FILE_PATH "/wifimanager/configWifi.html"
#define CONFIG_WIFI_SUCCESS_FILE_PATH "/wifimanager/success.html"

#define CONNECT_WIFI_TIMEOUT 30000UL   // (ms)
#define CONFIG_PORTAL_TIMEOUT 120000UL // (ms)

bool WifiMaster::mIsScanning = false;
#endif

static constexpr const char *const TAG = "WIFI";

void WifiMaster::init()
{
    LOG("Init WiFi");
#ifdef USE_WIFI_MANAGER
    setState(WIFI_MASTER_STATE_NONE);
    readSavedSettings();
    if (!isValidWifiSettings())
    {
        startConfigPortal();
    }
    else
    {
        startConnectToSavedWifi();
    }
#else
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    LOG("Connecting to " WIFI_SSID);
    startMDNS();
#endif
}

void WifiMaster::loop()
{
#ifdef USE_WIFI_MANAGER
    if (mServer)
    {
        mServer->handleClient();
    }
    processHandler();
#endif
}

void WifiMaster::resetSettings()
{
#ifdef USE_WIFI_MANAGER
    LOG("Reset WiFi settings");
    mSavedSSID = "";
    mSavedPassword = "";
    FileSystem::writeFile(SSID_FILE_PATH, mSavedSSID);
    FileSystem::writeFile(PASS_FILE_PATH, mSavedPassword);
    ESP.restart();
#endif
}

#ifdef USE_WIFI_MANAGER
int WifiMaster::getState()
{
    return mState;
}

String WifiMaster::getStateStr()
{
    switch (mState)
    {
    case WIFI_MASTER_STATE_NONE:
        return "NONE";
    case WIFI_MASTER_STATE_CONNECTING:
        return "CONNECTING";
    case WIFI_MASTER_STATE_CONFIG_PORTAL:
        return "CONFIG_PORTAL";
    case WIFI_MASTER_STATE_CONNECTED:
        return "CONNECTED";
    case WIFI_MASTER_STATE_DISCONNECTED:
        return "DISCONNECTED";
    default:
        return "UNKNOWN";
    }
}

void WifiMaster::setState(int state)
{
    if (mState != state)
    {
        mState = state;
        LOG("State changed to %s", getStateStr().c_str());
        Message message = {MESSAGE_TYPE_UPDATE_NETWORK_STATE, 0};
        MessageEvent::send(message);
    }
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
            WiFi.disconnect();
        }
        if (WiFi.getMode() == WIFI_AP)
        {
            WiFi.mode(WIFI_AP_STA);
        }
        else
        {
            WiFi.mode(WIFI_STA);
        }
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
        LOG("%2d. %-24s %4ddBm | %s", i + 1, ssid.c_str(), rssi, getEncryptionTypeStr(encType).c_str());
        i++;
    }
}

bool WifiMaster::getScannedWifiHtmlStr(String &str)
{
    int i = 0;
    String ssid;
    uint8_t encType;
    int32_t rssi;
    uint8_t *bssid;
    int32_t channel;
    bool ret = false;

    while (WiFi.getNetworkInfo(i, ssid, encType, rssi, bssid, channel))
    {
        str += FPSTR(HTML_WIFI_ITEM1);
        str += ssid;
        str += FPSTR(HTML_WIFI_ITEM2);
        str += String(Helper::getRssiLevel(rssi));
        if (encType != WIFI_AUTH_OPEN)
        {
            str += FPSTR(HTML_WIFI_LOCK);
        }
        str += FPSTR(HTML_WIFI_ITEM3);
        str += "\n";
        i++;
        ret = true;
    }
    return ret;
}

bool WifiMaster::isValidWifiSettings()
{
    return mSavedSSID.length() > 0 && mSavedPassword.length() > 0;
}

void WifiMaster::readSavedSettings()
{
    if (!FileSystem::readFile(SSID_FILE_PATH, mSavedSSID) || !FileSystem::readFile(PASS_FILE_PATH, mSavedPassword))
    {
        LOGE("Failed to read saved network");
        mSavedSSID = "";
        mSavedPassword = "";
    }
}

void WifiMaster::saveSettings()
{
    if (!FileSystem::writeFile(SSID_FILE_PATH, mSavedSSID) || !FileSystem::writeFile(PASS_FILE_PATH, mSavedPassword))
    {
        LOGE("Failed to save network");
    }
    LOG("Saved network: %s", mSavedSSID.c_str());
}

void WifiMaster::startConfigPortal()
{
    if (mState != WIFI_MASTER_STATE_NONE)
    {
        LOGE("Cannot start config portal");
        return;
    }
    LOG("Start config portal");
    setState(WIFI_MASTER_STATE_CONFIG_PORTAL);
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID, AP_PASSWORD);
    WiFi.softAPConfig(AP_IP_ADDR, IPAddress(0, 0, 0, 0), IPAddress(255, 255, 255, 0));
    startServer();
    startMDNS();
    startScanNetworks();
}

void WifiMaster::stopConfigPortal()
{
    if (mState != WIFI_MASTER_STATE_CONFIG_PORTAL)
    {
        return;
    }
    LOG("Stop config portal");
    setState(WIFI_MASTER_STATE_DISCONNECTED);
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_OFF);
    stopServer();
    // stopMDNS();
    stopScanNetworks();
}

void WifiMaster::startServer()
{
    if (!mServer)
    {
        LOG("Start server");
        mServer = new WebServer(80);
        mServer->onNotFound(notFoundHandler);
        mServer->on("/", rootHandler);
        mServer->on("/save", saveDataHandler);
        mServer->begin();
    }
}

void WifiMaster::stopServer()
{
    if (mServer)
    {
        LOG("Stop server");
        mServer->stop();
        delete mServer;
        mServer = nullptr;
    }
}

void WifiMaster::startMDNS()
{
    if (mStartedmDNS)
    {
        return;
    }
    if (MDNS.begin(MDNS_SERVER_NAME))
    {
        LOG("mDNS responder started at http://" MDNS_SERVER_NAME ".local");
        // MDNS.addService("http", "tcp", 80);
    }
}

void WifiMaster::stopMDNS()
{
    if (!mStartedmDNS)
    {
        return;
    }
    LOG("Stop mDNS responder");
    MDNS.end();
}

void WifiMaster::startConnectToSavedWifi()
{
    if (mState != WIFI_MASTER_STATE_NONE)
    {
        LOGE("Cannot connnect to saved wifi");
        return;
    }
    setState(WIFI_MASTER_STATE_CONNECTING);
    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true);
    WiFi.begin(mSavedSSID.c_str(), mSavedPassword.c_str());
    LOG("Connecting to %s", mSavedSSID.c_str());
    startMDNS();
}

void WifiMaster::stopConnectToSavedWifi()
{
    if (mState != WIFI_MASTER_STATE_CONNECTING)
    {
        return;
    }
    LOG("Stop connect to saved Wifi");
    setState(WIFI_MASTER_STATE_NONE);
    WiFi.disconnect(true);
    stopServer();
    // stopMDNS();
}

void WifiMaster::sendNotFound()
{
    if (!mServer)
    {
        return;
    }
    mServer->send(404, "text/plain", "404 Not Found");
}

void WifiMaster::notFoundHandler()
{
    if (!mServer)
    {
        return;
    }
#ifdef DEBUG_HTTP_ARGUMENTS
    String message = "URI: ";
    message += mServer->uri();
    message += "\nMethod: ";
    message += (mServer->method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += mServer->args();
    message += "\n";
    for (uint8_t i = 0; i < mServer->args(); i++)
    {
        message += " " + mServer->argName(i) + ": " + mServer->arg(i) + "\n";
    }
    LOG("Http: %s", message.c_str());
#endif
    sendNotFound();
}

void WifiMaster::rootHandler()
{
    if (!mServer)
    {
        return;
    }
#ifdef DEBUG_HTTP_ARGUMENTS
    String message = "URI: ";
    message += mServer->uri();
    message += "\nMethod: ";
    message += (mServer->method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += mServer->args();
    message += "\n";
    for (uint8_t i = 0; i < mServer->args(); i++)
    {
        message += " " + mServer->argName(i) + ": " + mServer->arg(i) + "\n";
    }
    LOG("Http: %s", message.c_str());
#endif

    fs::File file;
    if (!FileSystem::openFile(file, CONFIG_WIFI_FILE_PATH))
    {
        LOGE("Cannot open file");
        sendNotFound();
        return;
    }

    String html = "";
    while (file.available())
    {
        html += (char)file.read();
    }
    file.close();

    String wifiList = "";
    if (getScannedWifiHtmlStr(wifiList))
    {
        html.replace(FPSTR(HTML_WIFI_LIST), wifiList);
    }
    else
    {
        html.replace(FPSTR(HTML_WIFI_LIST), FPSTR(HTML_NO_NETWORKS_FOUND));
    }

    mServer->send(200, "text/html", html);
}

void WifiMaster::saveDataHandler()
{
    if (!mServer)
    {
        return;
    }
    if (mServer->hasArg("s"))
    {
        mSavedSSID = mServer->arg("s");
    }
    if (mServer->hasArg("p"))
    {
        mSavedPassword = mServer->arg("p");
    }
    Helper::trim(mSavedSSID);
    Helper::trim(mSavedPassword);
    if (isValidWifiSettings())
    {
        fs::File file;
        if (FileSystem::openFile(file, CONFIG_WIFI_SUCCESS_FILE_PATH))
        {
            mServer->streamFile(file, "text/html");
            file.close();

            saveSettings();
            // ESP.restart();
            Message message = {MESSAGE_TYPE_REBOOT, 0};
            MessageEvent::send(message);
        }
        else
        {
            LOGE("Cannot open file");
            sendNotFound();
        }
    }
    else
    {
        mServer->send(200, "text/plain", F("Wifi network information is invalid. Please try again."));
    }
}

void WifiMaster::processHandler()
{
    static int prevState = WIFI_MASTER_STATE_NONE;
    static bool prevConnectStatus = false;
    static int prevWifiCount = -99;
    static uint32_t timeTick = 0, scanTimeTick = 0;

    if (prevConnectStatus != WiFi.isConnected())
    {
        prevConnectStatus = WiFi.isConnected();
        if (mState == WIFI_MASTER_STATE_CONNECTED && !WiFi.isConnected())
        {
            setState(WIFI_MASTER_STATE_DISCONNECTED);
        }
        else if (mState == WIFI_MASTER_STATE_DISCONNECTED && WiFi.isConnected())
        {
            setState(WIFI_MASTER_STATE_CONNECTED);
        }
    }

    if (mState == WIFI_MASTER_STATE_CONNECTING && WiFi.isConnected())
    {
        setState(WIFI_MASTER_STATE_CONNECTED);
        stopScanNetworks();
        stopServer();
    }

    if (prevState != mState)
    {
        prevState = mState;
        if (mState == WIFI_MASTER_STATE_CONFIG_PORTAL)
        {
            timeTick = xTaskGetTickCount() + CONFIG_PORTAL_TIMEOUT / portTICK_PERIOD_MS;
        }
        else if (mState == WIFI_MASTER_STATE_CONNECTING)
        {
            timeTick = xTaskGetTickCount() + CONNECT_WIFI_TIMEOUT / portTICK_PERIOD_MS;
        }
    }

    if ((mState == WIFI_MASTER_STATE_CONFIG_PORTAL || mState == WIFI_MASTER_STATE_CONNECTING) && !WiFi.isConnected() && xTaskGetTickCount() > timeTick)
    {
        if (mState == WIFI_MASTER_STATE_CONFIG_PORTAL)
        {
            LOG("Config portal timeout");
            stopConfigPortal();
        }
        else if (mState == WIFI_MASTER_STATE_CONNECTING)
        {
            LOG("Connect to saved Wifi timeout");
            stopConnectToSavedWifi();
            startConfigPortal();
        }
    }

    if (mIsScanning && xTaskGetTickCount() > scanTimeTick)
    {
        scanTimeTick = xTaskGetTickCount() + 100 / portTICK_PERIOD_MS;
        int wifiCount = WiFi.scanComplete();
        if (prevWifiCount != wifiCount)
        {
            prevWifiCount = wifiCount;
            if (wifiCount == 0)
            {
                LOG("No networks found");
            }
            else if (wifiCount > 0)
            {
                LOG("Found %d networks", wifiCount);
            }
            else if (wifiCount == WIFI_SCAN_RUNNING)
            {
                LOG("WiFi scan running");
            }
            else
            {
                LOG("WiFi scan disabled or failed");
            }
        }
    }
}
#endif