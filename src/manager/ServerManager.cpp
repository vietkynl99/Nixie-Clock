#include "../../include/manager/ServerManager.h"

WebServer *ServerManager::mServer = nullptr;
WiFiUDP ServerManager::mNtpUDP;
NTPClient *ServerManager::mTimeClient = nullptr;

static constexpr const char *const TAG = "SERVER";

#define NTP_TIME_OFFSET (25200)				 // UTC+7 (Vietnam)
#define NTP_UPDATE_INTERVAL (24 * 3600000UL) // (ms) 24 hours
#define NTP_RTC_DIFF_TIME 60				 // (s) If the difference between NTP and RTC time is greater than this time, RTC time will be set according to NTP time

#define HTML_HOME_FILE_PATH "/home.html"

void ServerManager::init()
{
	AsyncWiFiManager::setAPInformation(DEVICE_NAME, DEVICE_PASSWORD);
	AsyncWiFiManager::setMDnsServerName(MDNS_SERVER_NAME);
	AsyncWiFiManager::setAutoConfigPortalEnable(false);

	AsyncWiFiManager::setOnStateChanged([](AsyncWiFiState state) {
		Message message = {MESSAGE_TYPE_UPDATE_NETWORK_STATE, (int)state};
		MessageEvent::send(message); 
	});

	AsyncWiFiManager::setOnWiFiInformationChanged([]() {
		Message message = {MESSAGE_TYPE_REBOOT, 0};
		MessageEvent::send(message); 
	});

	AsyncWiFiManager::begin();
}

void ServerManager::loop()
{
	AsyncWiFiManager::loop();
	statusHandler();
	ntpHandler();
	if (mServer)
	{
		mServer->handleClient();
	}
}

String ServerManager::getNTPTime()
{
	if (!mTimeClient || !mTimeClient->isTimeSet())
	{
		return "";
	}
	DateTime now(mTimeClient->getEpochTime());
	return Helper::convertDateTimeToString(now);
}

void ServerManager::startServer()
{
	static bool started = false;
	if (!started)
	{
		started = true;
		LOG("Starting server");
		mServer = new WebServer(80);
		mServer->onNotFound(notFoundHandler);
		mServer->on("/login", loginHandler);
		mServer->on("/", rootHandler);

		// here the list of headers to be recorded
		const char *headerkeys[] = {"Cookie"};
		size_t headerkeyssize = sizeof(headerkeys) / sizeof(char *);
		mServer->collectHeaders(headerkeys, headerkeyssize);

		mServer->begin();
	}
}

void ServerManager::startNTP()
{
	static bool started = false;
	if (!started)
	{
		started = true;
		LOG("Starting NTP")
		mTimeClient = new NTPClient(mNtpUDP);
		mTimeClient->begin();
#ifdef NTP_TIME_OFFSET
		mTimeClient->setTimeOffset(NTP_TIME_OFFSET);
#endif
#ifdef NTP_UPDATE_INTERVAL
		mTimeClient->setUpdateInterval(NTP_UPDATE_INTERVAL);
#endif
	}
}

bool ServerManager::isAuthentified()
{
	if (mServer->hasHeader("Cookie"))
	{
		String cookie = mServer->header("Cookie");
		LOG("Found cookie: %s", cookie.c_str());
		if (cookie.indexOf("ESPSESSIONID=1") != -1)
		{
			LOG("Authentification Successful");
			return true;
		}
	}
	LOG("Authentification Failed");
	return false;
}

void ServerManager::sendNotFound()
{
	mServer->send(404, "text/plain", "404 Not Found");
}

void ServerManager::notFoundHandler()
{
	sendNotFound();
}

void ServerManager::loginHandler()
{
	if (mServer->hasHeader("Cookie"))
	{
		String cookie = mServer->header("Cookie");
		LOG("Found cookie: %s", cookie.c_str());
	}
	if (mServer->hasArg("DISCONNECT"))
	{
		LOG("Log out");
		mServer->sendHeader("Location", "/login");
		mServer->sendHeader("Cache-Control", "no-cache");
		mServer->sendHeader("Set-Cookie", "ESPSESSIONID=0");
		mServer->send(301);
		return;
	}
	if (mServer->hasArg("USERNAME") && mServer->hasArg("PASSWORD"))
	{
		if (mServer->arg("USERNAME") == DEVICE_USERNAME && mServer->arg("PASSWORD") == DEVICE_PASSWORD)
		{
			mServer->sendHeader("Location", "/");
			mServer->sendHeader("Cache-Control", "no-cache");
			mServer->sendHeader("Set-Cookie", "ESPSESSIONID=1");
			mServer->send(301);
			LOG("Log in Successful");
			return;
		}
		LOG("Log in Failed");
	}

	fs::File file;
	if (FileSystem::openFile(file, "/login.html"))
	{
		mServer->streamFile(file, "text/html");
		file.close();
	}
	else
	{
		sendNotFound();
	}
}

void ServerManager::rootHandler()
{
	if (!isAuthentified())
	{
		mServer->sendHeader("Location", "/login");
		mServer->sendHeader("Cache-Control", "no-cache");
		mServer->send(301);
		return;
	}

	fs::File file;
	if (FileSystem::openFile(file, HTML_HOME_FILE_PATH))
	{
		mServer->streamFile(file, "text/html");
		file.close();
	}
	else
	{
		LOGE("Cannot open file");
		sendNotFound();
	}
}

void ServerManager::statusHandler()
{
	static uint32_t timeTick = 0;
	static bool status = false;

	if (xTaskGetTickCount() > timeTick)
	{
		timeTick = xTaskGetTickCount() + 500 / portTICK_PERIOD_MS;
		if (status != WiFi.isConnected())
		{
			status = WiFi.isConnected();
			if (status)
			{
				LOG("WiFi connected: SSID: %s, IP: %s", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
				if (SettingsManager::isWebServerEnabled())
				{
					startServer();
				}
				if (SettingsManager::isNTPEnabled())
				{
					startNTP();
				}
			}
			else
			{
				LOG("WiFi disconnected");
			}
			HardwareController::bip(3);
		}
	}
}

void ServerManager::ntpHandler()
{
	static uint32_t timeTick = 0;
	static bool timeSet = false;

	if (!mTimeClient)
	{
		return;
	}

	mTimeClient->update();

	if (xTaskGetTickCount() > timeTick)
	{
		timeTick = xTaskGetTickCount() + 500 / portTICK_PERIOD_MS;
		if (!timeSet && mTimeClient->isTimeSet())
		{
			timeSet = true;
			long long diff = mTimeClient->getEpochTime() - RTCController::getCurrentDateTime().unixtime();
			LOG("NTP time has been set to %s (%lds different from RTC time)", getNTPTime().c_str(), diff);
			if (abs(diff) >= NTP_RTC_DIFF_TIME)
			{
				RTCController::setDateTime(DateTime(mTimeClient->getEpochTime()));
			}
		}
	}
}