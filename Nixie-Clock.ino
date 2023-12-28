#include <SerialParser.h>
#include "include/common/Log.h"
#include "include/common/MessageEvent.h"
#include "include/common/Helper.h"
#include "include/common/FileSystem.h"
#include "include/controller/HardwareController.h"
#include "include/manager/SettingsManager.h"
#include "include/manager/ServerManager.h"
#include "include/manager/LauncherManager.h"

// #define DEBUG_FREE_MEMORY
// #define DEBUG_TASK_FPS
// #define DEBUG_SHOW_EVENT_INFORMATION

#define UI_TASK_STACK_SIZE 10000
#define BG_TASK_STACK_SIZE 10000

TaskHandle_t uiTask, bgTask;
SemaphoreHandle_t mMutex;

static constexpr const char *const TAG = "SYSTEM";

void debugHandler()
{
	static String cmd;
	static long value;
	static String valueStr;

	if (SerialParser::run(&cmd, &value, &valueStr))
	{
		if (mMutex != NULL && xSemaphoreTake(mMutex, portMAX_DELAY) == pdTRUE)
		{
			if (cmd.equals("RESTART"))
			{
				ESP.restart();
			}
			else if (cmd.equals("RSWIFI"))
			{
				AsyncWiFiManager::resetSettings();
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
				LOG("Temp: %.1fC, Hum: %.1f%%", HardwareController::getTemperature(), HardwareController::getHumidity());
			}
			else if (cmd.equals("TEST"))
			{
				Helper::mTestValue = value;
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

	LOGF("Start UI task");
	Helper::showFreeMemory();

	while (true)
	{
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

void backgroundTaskHandler(void *data)
{
	uint32_t timeTick = 0;
	int fps = 0;

	LOGF("Start background task");
	ServerManager::init();
	Helper::showFreeMemory();

	while (true)
	{
		fps++;
		if (xTaskGetTickCount() > timeTick)
		{
			timeTick = xTaskGetTickCount() + 1000 / portTICK_PERIOD_MS;
#ifdef DEBUG_TASK_FPS
			LOG("Task 2: %d fps", fps);
#endif
#ifdef DEBUG_FREE_MEMORY
			Helper::showFreeMemory();
#endif
			fps = 0;
		}
		debugHandler();
		HardwareController::loop();
		ServerManager::loop();
		vTaskDelay(5 / portTICK_PERIOD_MS);
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
	HardwareController::init();
	LauncherManager::init();
	delay(500);

	Helper::showFreeMemory();

	int ret;
	if ((ret = xTaskCreatePinnedToCore(uiTaskHandler, "ui_task", UI_TASK_STACK_SIZE, NULL, 2, &uiTask, 0)) != pdPASS)
	{
		LOGE("Failed to create UI task: %d", ret);
		Helper::showFreeMemory();
	}
	if ((ret = xTaskCreatePinnedToCore(backgroundTaskHandler, "bg_task", BG_TASK_STACK_SIZE, NULL, 1, &bgTask, 1)) != pdPASS)
	{
		LOGE("Failed to create background task: %d", ret);
		Helper::showFreeMemory();
	}
}

void loop()
{
	vTaskDelay(100);
}