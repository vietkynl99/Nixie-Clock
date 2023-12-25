#include "../../include/controller/HardwareController.h"

// #define ENABLE_TOUCH_DEBUG

#define BUZZER_PWM_VALUE_MIN 30
#define BUZZER_PWM_VALUE_MAX 254

#define TOUCH_THRESHOLD 17
#define TOUCH_SHORT_PRESS_TIMEOUT 700
#define TOUCH_LONG_PRESS_TIME 1000

#define DHT_SENSOR_UPDATE_TIME 10000UL // (ms)
#define DHT_INVALID_VALUE -999

int HardwareController::mTouchPin[TOUCH_PIN_COUNT] = {TOUCH_PIN_BUTTON1, TOUCH_PIN_BUTTON2, TOUCH_PIN_BUTTON3};
int HardwareController::mBuzzerCount = 0;
float HardwareController::mTemperature = DHT_INVALID_VALUE;
float HardwareController::mHumidity = DHT_INVALID_VALUE;
DHT_Async HardwareController::mDHT(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

static constexpr const char *const TAG = "HARDWARE";

void HardwareController::init()
{
    static bool initialized = false;

    if (!initialized)
    {
        initialized = true;
        ledcAttachPin(BUZZER_PIN, BUZZER_PWM_CHANNEL);
        ledcSetup(BUZZER_PWM_CHANNEL, BUZZER_PWM_FREQUENCY, BUZZER_PWM_RESOLUTION);
        ledcWrite(BUZZER_PWM_CHANNEL, 0);
        LedController::init();
    }
}

void HardwareController::loop()
{
    buttonHandler();
    buzzerHandler();
    dhtHandler();
    LedController::loop();
}

void HardwareController::bip(int n)
{
    if (mBuzzerCount == 0)
    {
        mBuzzerCount = 2 * n;
    }
}

float HardwareController::getTemperature()
{
    return mTemperature;
}

float HardwareController::getHumidity()
{
    return mHumidity;
}

bool HardwareController::isValidDhtValue(float value)
{
    return value > DHT_INVALID_VALUE;
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
            if (abs(prevState[i] - newState[i]) > newState[i] * 0.1)
            {
                LOG("Button %d, value: %d -> %d", i, data, newState[i]);
            }
#endif
            if (!prevState[i] && newState[i])
            {
                risingTimeTick[i] = xTaskGetTickCount();
            }
            if (!BootFragment::isVisible())
            {
                if (prevState[i] && !newState[i])
                {
                    longState[i] = false;
                    uint32_t pressTime = xTaskGetTickCount() - risingTimeTick[i];
                    if (pressTime < TOUCH_SHORT_PRESS_TIMEOUT)
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
}

void HardwareController::buzzerHandler()
{
    static uint32_t timeTick = 0;
    static bool state = false;

    if (xTaskGetTickCount() > timeTick)
    {
        timeTick = xTaskGetTickCount() + 70 / portTICK_PERIOD_MS;
        if (mBuzzerCount > 0)
        {
            mBuzzerCount--;
            state ^= 1;
            setBuzzerState(state);
        }
        else if (state)
        {
            state = false;
            setBuzzerState(state);
        }
    }
}

void HardwareController::dhtHandler()
{
    static uint32_t timeTick = 0;
    static float temperature, humidity;

    if (xTaskGetTickCount() > timeTick)
    {
        if (mDHT.measure(&temperature, &humidity))
        {
            timeTick = xTaskGetTickCount() + DHT_SENSOR_UPDATE_TIME / portTICK_PERIOD_MS;
            if (abs(mTemperature - temperature) >= 0.1 || abs(mHumidity - humidity) >= 0.1)
            {
                mTemperature = temperature;
                mHumidity = humidity;
                Message message = {MESSAGE_TYPE_UPDATE_TEMP_AND_RH, 0};
                MessageEvent::send(message);
            }
        }
    }
}

void HardwareController::setBuzzerState(bool state)
{
    int pwmValue = 0;
    int volume = SettingsManager::getBuzzerVolume();
    if (volume > 0)
    {
        pwmValue = (volume - 1) / 9.0 * (BUZZER_PWM_VALUE_MAX - BUZZER_PWM_VALUE_MIN) + BUZZER_PWM_VALUE_MIN;
    }
    ledcWrite(BUZZER_PWM_CHANNEL, state ? pwmValue : 0);
}