#include <SerialParser.h>
#include "DisplayManager.h"
#include "Log.h"

#define TASK1_STACK_SIZE 10000
#define TASK2_STACK_SIZE 10000

TaskHandle_t task1, task2;

void debugHandler()
{
	static char cmd[20];
	static long code;

	if (SerialParser::run(cmd, code))
	{
		if (!strcmp(cmd, "FUNCTION_NAME"))
		{
			// do something
		}
	}
}

void task1Handler(void *data)
{
	LOG_SYSTEM("Start task 1");
	while (true)
	{
		DisplayManager::loop();
		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}

void task2Handler(void *data)
{
	LOG_SYSTEM("Start task 2");
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
	DisplayManager::init();
	delay(500);

	xTaskCreatePinnedToCore(task1Handler, "task1", TASK1_STACK_SIZE, NULL, 2, &task1, 0);
	xTaskCreatePinnedToCore(task2Handler, "task2", TASK2_STACK_SIZE, NULL, 1, &task2, 1);
}

void loop()
{
}