#include <SerialParser.h>
#include "include/Log.h"
#include "include/MenuManager.h"
#include "include/WifiMaster.h"

#define TASK1_STACK_SIZE 10000
#define TASK2_STACK_SIZE 10000

TaskHandle_t task1, task2;

static constexpr const char *const TAG = "SYSTEM";

void debugHandler()
{
	static char cmd[20];
	static long code;

	if (SerialParser::run(cmd, code))
	{
		if (!strcmp(cmd, "RSWIFI"))
		{
			WifiMaster::resetWifiSettings();
		}
		else if (!strcmp(cmd, "MENU"))
		{
			if (code)
			{
				MenuManager::show();
			}
			else
			{
				MenuManager::hide();
			}
		}
		else if (!strcmp(cmd, "UP"))
		{
			MenuManager::up();
		}
		else if (!strcmp(cmd, "DOWN"))
		{
			MenuManager::down();
		}
		else
		{
			LOG("Unknown command: '%s'", cmd);
		}
		
	}
}

void task1Handler(void *data)
{
	LOG("Start task 1");
	MenuManager::show();
	while (true)
	{
		MenuManager::loop();
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
	Serial.begin(115200);
	SerialParser::setFeedbackEnable(true);
	SerialParser::setAllowEmptyCode(true);
	MenuManager::init();
	LOG("portTICK_PERIOD_MS: %d", portTICK_PERIOD_MS);
	delay(500);

	xTaskCreatePinnedToCore(task1Handler, "task1", TASK1_STACK_SIZE, NULL, 2, &task1, 0);
	xTaskCreatePinnedToCore(task2Handler, "task2", TASK2_STACK_SIZE, NULL, 1, &task2, 1);
}

void loop()
{
}