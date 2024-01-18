#include <SerialParser.h>
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

void showHelp()
{
	LOG("Available commands:");
	LOG("1. HELP: Show help");
	LOG("2. RESTART: Restart");
	LOG("3. RSWIFI: Reset WiFi Settings");
	LOG("4. LAUNCHER <type>: Show fragment with launcher");
	LOG("5. REFRESH: Refresh current fragment");
	LOG("6. TIME: Set RTC time");
	LOG("\tEx: TIME 190523 (Set time to 19:05:23)");
	LOG("7. DATE: Set RTC date");
	LOG("\tEx: DATE 20230108 (Set date to 2023/01/08)");
	LOG("8. NTP: Get current NTP time");
	LOG("9. RTC: Get current RTC time");
	LOG("10. DHT: Get data from DHT sensor (temperature and humidity)");
	LOG("11. FSLIST: List files in filesystem");
	LOG("12. WFLIST: List scanned WiFis");
	LOG("13. WF <SSID> <PASSWORD>: Set WiFi");
	LOG("14. TEST <value>: Set test value");
}

void debugHandler()
{
	static String cmd;
	static long value;
	static String valueStr;

	if (SerialParser::run(&cmd, &value, &valueStr))
	{
		if (mMutex != NULL && xSemaphoreTake(mMutex, portMAX_DELAY) == pdTRUE)
		{
			if (cmd.equals("HELP"))
			{
				showHelp();
			}
			else if (cmd.equals("RESTART"))
			{
				ESP.restart();
			}
			else if (cmd.equals("RSWIFI"))
			{
				AsyncWiFiManager::resetSettings();
				ESP.restart();
			}
			else if (cmd.equals("LAUNCHER"))
			{
				LauncherManager::show((FragmentType)value);
			}
			else if (cmd.equals("REFRESH"))
			{
				LauncherManager::refresh();
			}
			// set time: Ex 19:25:23 -> 190523
			else if (cmd.equals("TIME"))
			{
				RTCController::setTime(value / 10000, value % 10000 / 100, value % 100);
			}
			// set date: Ex 2023/01/08 -> 20230108 (year: 2000 -> 2099)
			else if (cmd.equals("DATE"))
			{
				RTCController::setDate(value / 10000, value % 10000 / 100, value % 100);
			}
			else if (cmd.equals("NTP"))
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
			}
			else if (cmd.equals("RTC"))
			{
				LOG("RTC time: %s", RTCController::getCurrentDateTimeStr().c_str());
			}
			else if (cmd.equals("DHT"))
			{
				LOG("Temp: %.1fC, Hum: %.1f%%", SensorController::getTemperature(), SensorController::getHumidity());
			}
			else if (cmd.equals("FSLIST"))
			{
				FileSystem::listDir("/", 0);
			}
			else if (cmd.equals("WFLIST"))
			{
				AsyncWiFiManager::printScannedNetWorks();
			}
			// WF <ssid>,<password>
			else if (cmd.equals("WF"))
			{
				if (valueStr.length() > 0)
				{
					int index = valueStr.indexOf(',');
					if (index > 0)
					{
						String ssid = valueStr.substring(0, index);
						String password = valueStr.substring(index + 1);
						LOG("Set wifi information: ssid: '%s', password: '%s'", ssid.c_str(), password.c_str());
						AsyncWiFiManager::setWifiInformation(ssid, password);
						ESP.restart();
					}
				}
			}
			else if (cmd.equals("TEST"))
			{
				Helper::mTestValue = value;
			}
			else
			{
				LOG("Unknown command: '%s'", cmd);
			}
			xSemaphoreGive(mMutex);
		}
	}
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

	while (true)
	{
		debugHandler();
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
	SerialParser::setFeedbackEnable(true);
	SerialParser::setAllowEmptyCode(true);
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