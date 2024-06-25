#include <CommandLineParser.h>
#include "include/common/Log.h"
#include "include/common/MessageEvent.h"
#include "include/common/Helper.h"
#include "include/common/FileSystem.h"
#include "include/controller/HardwareController.h"
#include "include/controller/SensorController.h"
#include "include/manager/SettingsManager.h"
#include "include/manager/ServerManager.h"
#include "include/manager/LauncherManager.h"

// #define DEBUG_FREE_MEMORY
// #define DEBUG_TASK_FPS
// #define DEBUG_SHOW_EVENT_INFORMATION

#define TASK_STACK_SIZE 10000

TaskHandle_t uiTask, hwTask, nwTask, ssTask;
SemaphoreHandle_t mMutex;

static constexpr const char *const TAG = "SYSTEM";

bool onCommandReboot(String params)
{
	ESP.restart();
	return true;
}

bool onCommandRsWifi(String params)
{
	AsyncWiFiManager::resetSettings();
	ESP.restart();
	return true;
}

bool onCommandRefresh(String params)
{
	LauncherManager::refresh();
	return true;
}

bool onCommandTime(String params)
{
	int hour, minute, second = 0;
	if (sscanf(params.c_str(), "%d:%d:%d", &hour, &minute, &second) == 3)
	{
		RTCController::setTime(hour, minute, second);
		return true;
	}
	return false;
}

bool onCommandDate(String params)
{
	int year, month, day = 0;
	if (sscanf(params.c_str(), "%d/%d/%d", &day, &month, &year) == 3)
	{
		RTCController::setDate(year, month, day);
		return true;
	}
	return false;
}

bool onCommandNTP(String params)
{
	if (!SettingsManager::isWiFiEnabled())
	{
		LOG("WiFi is not enabled");
	}
	else if (!SettingsManager::isNTPEnabled())
	{
		LOG("NTP is not enabled");
	}
	else
	{
		LOG("NTP time: %s", ServerManager::getNTPTime().c_str());
	}
	return true;
}

bool onCommandRTC(String params)
{
	LOG("RTC time: %s", RTCController::getCurrentDateTimeStr().c_str());
	return true;
}

bool onCommandDHT(String params)
{
	LOG("Temp: %.1fC, Hum: %.1f%%", SensorController::getTemperature(), SensorController::getHumidity());
	return true;
}

bool onCommandWfList(String params)
{
	AsyncWiFiManager::printScannedNetWorks();
	return true;
}

bool onCommandWifi(String params)
{
	if (params.length() > 0)
	{
		int index = params.indexOf(',');
		if (index > 0)
		{
			String ssid = params.substring(0, index);
			String password = params.substring(index + 1);
			LOG("Set wifi information: ssid: '%s', password: '%s'", ssid.c_str(), password.c_str());
			AsyncWiFiManager::setWifiInformation(ssid, password);
			ESP.restart();
			return true;
		}
	}
	return false;
}

void uiTaskHandler(void *data)
{
	uint32_t timeTick = 0;
	int fps = 0;
	Message message;

	LauncherManager::init();
	Helper::showFreeMemory();

	while (true)
	{
#if defined(DEBUG_TASK_FPS) || defined(DEBUG_FREE_MEMORY)
		fps++;
		if (xTaskGetTickCount() > timeTick)
		{
			timeTick = xTaskGetTickCount() + 1000 / portTICK_PERIOD_MS;
#ifdef DEBUG_TASK_FPS
			LOG("Task 1: %d fps", fps);
#endif
#ifdef DEBUG_FREE_MEMORY
			Helper::showFreeMemory();
#endif
			fps = 0;
		}
#endif
		if (mMutex && xSemaphoreTake(mMutex, portMAX_DELAY) == pdTRUE)
		{
			if (MessageEvent::get(message))
			{
#ifdef DEBUG_SHOW_EVENT_INFORMATION
				LOG("Received message: type: %d, value: %d", message.type, message.value);
#endif
				LauncherManager::handleEvent(message);
			}

			LauncherManager::loop();
			xSemaphoreGive(mMutex);
		}
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
}

void hwTaskHandler(void *data)
{
	HardwareController::init();

	CommandLineParser::init();
	CommandLineParser::install("reboot", onCommandReboot, "reboot");
	CommandLineParser::install("reset-wifi", onCommandRsWifi, "reset wifi");
	CommandLineParser::install("refresh", onCommandRefresh, "refresh UI");
	CommandLineParser::install("time", onCommandTime, "time hh:mm:ss\t: set time");
	CommandLineParser::install("date", onCommandDate, "date dd/MM/yyyy\t: set date");
	CommandLineParser::install("ntp", onCommandNTP, "get NTP time");
	CommandLineParser::install("rtc", onCommandRTC, "get RTC time");
	CommandLineParser::install("dht", onCommandDHT, "get DHT sensor values");
	CommandLineParser::install("wifi-list", onCommandWfList, "get scanned Wifi list");
	CommandLineParser::install("wifi", onCommandWifi, "set Wifi");

	while (true)
	{
		CommandLineParser::run();
		HardwareController::loop();
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
}

void ssTaskHandler(void *data)
{
	SensorController::init();

	while (true)
	{
		SensorController::loop();
		vTaskDelay(1 / portTICK_PERIOD_MS);
	}
}

void nwTaskHandler(void *data)
{
	ServerManager::init();
	Helper::showFreeMemory();

	while (true)
	{
		ServerManager::loop();
		vTaskDelay(5 / portTICK_PERIOD_MS);
	}
}

void createTask(TaskFunction_t pvTaskCode, const char *const pcName, UBaseType_t uxPriority, TaskHandle_t *const pvCreatedTask, const BaseType_t xCoreID)
{
	int ret;
	if ((ret = xTaskCreatePinnedToCore(pvTaskCode, pcName, TASK_STACK_SIZE, NULL, uxPriority, pvCreatedTask, xCoreID)) != pdPASS)
	{
		LOGE("Failed to create %s. Error %d", pcName, ret);
		Helper::showFreeMemory();
	}
}

void setup()
{
	mMutex = xSemaphoreCreateMutex();
	Serial.begin(115200);
	FileSystem::init();
	MessageEvent::init();
	SettingsManager::init();
	delay(500);

	Helper::showFreeMemory();

	// Core 0
	createTask(uiTaskHandler, "uiTask", 2, &uiTask, 0);
	createTask(hwTaskHandler, "hwTask", 1, &hwTask, 0);

	// Core 1
	createTask(ssTaskHandler, "ssTask", 2, &ssTask, 1);
	if (SettingsManager::isWiFiEnabled())
	{
		createTask(nwTaskHandler, "nwTask", 1, &nwTask, 1);
	}
	else
	{
		LOG("WiFi is disabled");
	}
}

void loop()
{
	vTaskDelay(100);
}