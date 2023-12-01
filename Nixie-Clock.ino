#include <SerialParser.h>
#include "include/Log.h"
#include "include/WifiMaster.h"
#include "include/ClockFragment.h"
#include "include/MenuFragment.h"
#include "include/LauncherManager.h"

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
			if (!strcmp(cmd, "RSWIFI"))
			{
				WifiMaster::resetWifiSettings();
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
	LOG("Start task 1");
	while (true)
	{
		if (mMutex != NULL && xSemaphoreTake(mMutex, portMAX_DELAY) == pdTRUE)
		{
			LauncherManager::loop();
			xSemaphoreGive(mMutex);
		}
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
}

void task2Handler(void *data)
{
	LOG("Start task 2");
	while (true)
	{
		debugHandler();
		vTaskDelay(20 / portTICK_PERIOD_MS);
	}
}

void setup()
{
	mMutex = xSemaphoreCreateMutex();

	Serial.begin(115200);
	SerialParser::setFeedbackEnable(true);
	SerialParser::setAllowEmptyCode(true);
	LauncherManager::init();
	delay(500);

	xTaskCreatePinnedToCore(task1Handler, "task1", TASK1_STACK_SIZE, NULL, 2, &task1, 0);
	xTaskCreatePinnedToCore(task2Handler, "task2", TASK2_STACK_SIZE, NULL, 1, &task2, 1);
}

void loop()
{
}