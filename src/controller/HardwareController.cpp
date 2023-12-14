#include "../../include/controller/HardwareController.h"

int HardwareController::mTouchPin[TOUCH_PIN_COUNT] = {TOUCH_PIN_BUTTON1, TOUCH_PIN_BUTTON2, TOUCH_PIN_BUTTON3};
int HardwareController::mBuzzerCount = 0;

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
    buzzerHandler();
}

void HardwareController::buttonHandler()
{
    static uint32_t timeTick = 0;
    static bool prevState[TOUCH_PIN_COUNT] = {false, false, false};
    static bool newState[TOUCH_PIN_COUNT] = {false, false, false};
    static bool longState[TOUCH_PIN_COUNT] = {false, false, false};
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
                LOG("Button %d, value: %d -> %d", i, data, newState[i]);
            }
#endif
            if (!prevState[i] && newState[i])
            {
                risingTimeTick[i] = xTaskGetTickCount();
            }
            if (prevState[i] && !newState[i])
            {
                longState[i] = false;
                uint32_t pressTime = xTaskGetTickCount() - risingTimeTick[i];
                if (pressTime > TOUCH_SHORT_PRESS_TIME && pressTime < TOUCH_SHORT_PRESS_TIMEOUT)
                {
                    Message event = {MESSAGE_TYPE_BUTTON_SHORT_PRESSED, i};
                    MessageEvent::send(event);
                    LOG("Button %d short pressed", i);
                    bip(1);
                }
            }
            if (newState[i] && !longState[i] && xTaskGetTickCount() - risingTimeTick[i] > TOUCH_LONG_PRESS_TIME)
            {
                longState[i] = true;
                Message event = {MESSAGE_TYPE_BUTTON_LONG_PRESSED, i};
                MessageEvent::send(event);
                LOG("Button %d long pressed", i);
                bip(2);
            }
        }
    }
}

void HardwareController::buzzerHandler()
{
    static uint32_t timeTick = 0;

    if (xTaskGetTickCount() > timeTick)
    {
        timeTick = xTaskGetTickCount() + 70 / portTICK_PERIOD_MS;
        if (mBuzzerCount > 0)
        {
            mBuzzerCount--;
            digitalWrite(BUZZER_PIN, !digitalRead(BUZZER_PIN));
        }
        else if (digitalRead(BUZZER_PIN))
        {
            digitalWrite(BUZZER_PIN, LOW);
        }
    }
}

void HardwareController::bip(int n)
{
    if (mBuzzerCount == 0)
    {
        mBuzzerCount = 2 * n;
    }
}