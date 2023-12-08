#include "../../include/controller/HardwareController.h"

int HardwareController::mTouchPin[TOUCH_PIN_COUNT] = {TOUCH_PIN_ENTER, TOUCH_PIN_UP, TOUCH_PIN_DOWN};

static constexpr const char *const TAG = "HARDWARE";

void HardwareController::init()
{
    static bool initialized = false;

    if (!initialized)
    {
        initialized = true;
        pinMode(BUZZER_PIN, OUTPUT);
        digitalWrite(BUZZER_PIN, LOW);
    }
}

void HardwareController::loop()
{
    buttonHandler();
}

void HardwareController::buttonHandler()
{
    static uint32_t timeTick = 0;
    static bool prevState[TOUCH_PIN_COUNT] = {false, false, false};
    static bool newState[TOUCH_PIN_COUNT] = {false, false, false};
    static uint32_t risingTimeTick[TOUCH_PIN_COUNT] = {0, 0, 0};

    if (xTaskGetTickCount() > timeTick)
    {
        timeTick = xTaskGetTickCount() + 20 / portTICK_PERIOD_MS;
        for (int i = 0; i < TOUCH_PIN_COUNT; i++)
        {
            int data = touchRead(mTouchPin[i]);
            prevState[i] = newState[i];
            newState[i] = data < TOUCH_THRESHOLD;
#ifdef ENABLE_TOUCH_DEBUG
            if (abs(prevState[i] - newState[i]) > newState[i] * 0.2)
            {
                LOG("Touch index: %d, value: %d -> %d", i, data, newState[i]);
            }
#endif
            if (!prevState[i] && newState[i])
            {
                risingTimeTick[i] = xTaskGetTickCount();
            }
            if (prevState[i] && !newState[i])
            {
                uint32_t pressTime = xTaskGetTickCount() - risingTimeTick[i];
#ifdef ENABLE_TOUCH_DEBUG
                LOG("Button %d press time: %d", pressTime);
#endif
                if (pressTime > TOUCH_SHORT_PRESS_TIME && pressTime < TOUCH_SHORT_PRESS_TIMEOUT)
                {
                    LOG("Button %d short pressed", i + 1);
                }
            }
        }
    }
}