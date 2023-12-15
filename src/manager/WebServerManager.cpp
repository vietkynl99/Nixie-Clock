#include "../../include/manager/WebServerManager.h"

WebServer *WebServerManager::mServer = nullptr;

static constexpr const char *const TAG = "SERVER";

void WebServerManager::init()
{
	WifiMaster::init();
}

void WebServerManager::loop()
{
	WifiMaster::loop();
	statusHandler();
	if (mServer != nullptr)
	{
		mServer->handleClient();
	}
}

void WebServerManager::startServer()
{
	static bool started = false;

	if (!started)
	{
		started = true;
		mServer = new WebServer(80);
		mServer->onNotFound(notFoundHandler);
		mServer->on("/login", loginHandler);
		mServer->on("/", rootHandler);

		// here the list of headers to be recorded
		const char *headerkeys[] = {"Cookie"};
		size_t headerkeyssize = sizeof(headerkeys) / sizeof(char *);
		mServer->collectHeaders(headerkeys, headerkeyssize);

		mServer->begin();

		if (MDNS.begin(MDNS_SERVER_NAME))
		{
			LOG("mDNS responder started: http://" MDNS_SERVER_NAME ".local");
			// MDNS.addService("http", "tcp", 80);
		}
	}
}

void WebServerManager::notFoundHandler()
{
	mServer->send(404, "text/plain", "404 Not Found");
}

void WebServerManager::loginHandler()
{
	String msg;
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
		if (mServer->arg("USERNAME") == SERVER_USERNAME && mServer->arg("PASSWORD") == SERVER_PASSWORD)
		{
			mServer->sendHeader("Location", "/");
			mServer->sendHeader("Cache-Control", "no-cache");
			mServer->sendHeader("Set-Cookie", "ESPSESSIONID=1");
			mServer->send(301);
			LOG("Log in Successful");
			return;
		}
		msg = "Wrong username/password! try again.";
		LOG("Log in Failed");
	}
	String content = "<html><body><form action='/login' method='POST'>Login to access this page<br>";
	content += "User:<input type='text' name='USERNAME' placeholder='user name'><br>";
	content += "Password:<input type='password' name='PASSWORD' placeholder='password'><br>";
	content += "<input type='submit' name='SUBMIT' value='Submit'></form>" + msg + "<br>";
	// content += "You also can go <a href='/inline'>here</a></body></html>";
	mServer->send(200, "text/html", content);
}

void WebServerManager::rootHandler()
{
	if (!isAuthentified())
	{
		mServer->sendHeader("Location", "/login");
		mServer->sendHeader("Cache-Control", "no-cache");
		mServer->send(301);
		return;
	}

	char temp[400];
	int sec = millis() / 1000;
	int min = sec / 60;
	int hr = min / 60;

	snprintf(temp, 400,

			 "<html>\
  <head>\
    <meta http-equiv='refresh' content='5'/>\
    <title>ESP32 Demo</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>Hello from ESP32!</h1>\
    <p>Uptime: %02d:%02d:%02d</p>\
    <img src=\"/test.svg\" />\
  </body>\
</html>",
			 hr, min % 60, sec % 60);
	mServer->send(200, "text/html", temp);
}

bool WebServerManager::isAuthentified()
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

void WebServerManager::statusHandler()
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
				startServer();
			}
			else
			{
				LOG("WiFi disconnected");
			}
		}
	}
}