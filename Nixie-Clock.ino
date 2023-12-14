#include <SerialParser.h>
#include "include/common/Log.h"
#include "include/common/MessageEvent.h"
#include "include/controller/HardwareController.h"
#include "include/controller/WifiMaster.h"
#include "include/manager/LauncherManager.h"

#define TASK1_STACK_SIZE 10000
#define TASK2_STACK_SIZE 10000

TaskHandle_t task1, task2;
SemaphoreHandle_t mMutex;

static constexpr const char *const TAG = "SYSTEM";

void debugHandler()
{
	static char cmd[20];
	static long code;

	if (SerialParser::run(cmd, code))
	{
		if (mMutex != NULL && xSemaphoreTake(mMutex, portMAX_DELAY) == pdTRUE)
		{
			if (!strcmp(cmd, "RESTART"))
			{
				ESP.restart();
			}
			else if (!strcmp(cmd, "RSWIFI"))
			{
				WifiMaster::resetSettings();
			}
			else if (!strcmp(cmd, "LAUNCHER"))
			{
				LauncherManager::show((FragmentType)code);
			}
			else if (!strcmp(cmd, "MENU"))
			{
				if (code)
				{
					MenuFragment::show();
				}
				else
				{
					MenuFragment::hide();
				}
			}
			else if (!strcmp(cmd, "CLOCK"))
			{
				if (code)
				{
					ClockFragment::show();
				}
				else
				{
					ClockFragment::hide();
				}
			}
			else if (!strcmp(cmd, "CLEAR"))
			{
				DisplayController::clear();
			}
			else if (!strcmp(cmd, "SELECT"))
			{
				DisplayController::selectDisplay(code);
			}
			else if (!strcmp(cmd, "UP"))
			{
				MenuFragment::up();
			}
			else if (!strcmp(cmd, "DOWN"))
			{
				MenuFragment::down();
			}
			else if (!strcmp(cmd, "ENTER"))
			{
				MenuFragment::enter();
			}
			else if (!strcmp(cmd, "BACK"))
			{
				MenuFragment::back();
			}
			// set time: Ex 19:25:23 -> 190523
			else if (!strcmp(cmd, "TIME"))
			{
				RTCController::setTime(code / 10000, code % 10000 / 100, code % 100);
			}
			// set date: Ex 2023/01/08 -> 20230108 (year: 2000 -> 2099)
			else if (!strcmp(cmd, "DATE"))
			{
				RTCController::setDate(code / 10000, code % 10000 / 100, code % 100);
			}
			else
			{
				LOG("Unknown command: '%s'", cmd);
			}
			xSemaphoreGive(mMutex);
		}
	}
}

void task1Handler(void *data)
{
	Message message;

	LOG("Start task 1");
	while (true)
	{
		if (mMutex && xSemaphoreTake(mMutex, portMAX_DELAY) == pdTRUE)
		{
			if (MessageEvent::get(message))
			{
				LOG("Received message: type: %d, value: %d", message.type, message.value);
				LauncherManager::handleEvent(message);
			}

			LauncherManager::loop();
			xSemaphoreGive(mMutex);
		}
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
}

void task2Handler(void *data)
{
	LOG("Start task 2");

	bool webServerEnabled = MenuFragment::isWebServerEnabled();
	if (webServerEnabled)
	{
		WifiMaster::init();
	}
	while (true)
	{
		debugHandler();
		HardwareController::loop();
		if (webServerEnabled)
		{
			WifiMaster::loop();
		}
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
}

void setup()
{
	delay(500);
	mMutex = xSemaphoreCreateMutex();
	Serial.begin(115200);
	SerialParser::setFeedbackEnable(true);
	SerialParser::setAllowEmptyCode(true);
	MessageEvent::init();
	HardwareController::init();
	LauncherManager::init();
	delay(500);

	xTaskCreatePinnedToCore(task1Handler, "task1", TASK1_STACK_SIZE, NULL, 2, &task1, 0);
	xTaskCreatePinnedToCore(task2Handler, "task2", TASK2_STACK_SIZE, NULL, 1, &task2, 1);
}

void loop()
{
}