#include <SerialParser.h>
#include "include/common/Log.h"
#include "include/common/MessageEvent.h"
#include "include/controller/HardwareController.h"
#include "include/manager/WebServerManager.h"
#include "include/manager/LauncherManager.h"

// #define DEBUG_FREE_MEMORY
// #define DEBUG_TASK_FPS

#define TASK1_STACK_SIZE 10000
#define TASK2_STACK_SIZE 10000

TaskHandle_t task1, task2;
SemaphoreHandle_t mMutex;
size_t totalHeap = 0;

static constexpr const char *const TAG = "SYSTEM";

void showFreeMemory()
{
	size_t freeHeap = ESP.getFreeHeap();
	LOGF("Free Heap: %d bytes (%d%% free)", freeHeap, (freeHeap * 100) / (totalHeap + 1));
}

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
			else if (!strcmp(cmd, "NTP"))
			{
				LOG("NTP time: %s", WebServerManager::getNTPTime().c_str());
			}
			else if (!strcmp(cmd, "RTC"))
			{
				LOG("RTC time: %s", RTCController::getCurrentDateTimeStr().c_str());
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
	uint32_t timeTick = 0;
	int fps = 0;
	Message message;

	LOGF("Start task 1");
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
			showFreeMemory();
#endif
			fps = 0;
		}
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
	uint32_t timeTick = 0;
	int fps = 0;

	LOGF("Start task 2");
	WebServerManager::init();
	
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
			showFreeMemory();
#endif
			fps = 0;
		}
		debugHandler();
		HardwareController::loop();
		WebServerManager::loop();
		vTaskDelay(5 / portTICK_PERIOD_MS);
	}
}

void setup()
{
	mMutex = xSemaphoreCreateMutex();
	Serial.begin(115200);

	totalHeap = ESP.getHeapSize();
	LOGF("Total Heap: %d bytes", totalHeap);

	SerialParser::setFeedbackEnable(true);
	SerialParser::setAllowEmptyCode(true);
	MessageEvent::init();
	HardwareController::init();
	LauncherManager::init();
	delay(500);

	showFreeMemory();

	int ret;
	if ((ret = xTaskCreatePinnedToCore(task1Handler, "task1", TASK1_STACK_SIZE, NULL, 2, &task1, 0)) != pdPASS)
	{
		LOGF("Failed to create task1: %d", ret);
		showFreeMemory();
	}
	if ((ret = xTaskCreatePinnedToCore(task2Handler, "task2", TASK2_STACK_SIZE, NULL, 1, &task2, 1)) != pdPASS)
	{
		LOGF("Failed to create task2: %d", ret);
		showFreeMemory();
	}
}

void loop()
{
	vTaskDelay(100);
}