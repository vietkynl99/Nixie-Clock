#include "../../include/controller/SensorController.h"

#define DHT_SENSOR_UPDATE_TIME 10000UL // (ms)
#define DHT_INVALID_VALUE -999

DHT_Async SensorController::mDHT(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);
float SensorController::mTemperature = DHT_INVALID_VALUE;
float SensorController::mHumidity = DHT_INVALID_VALUE;

static constexpr const char *const TAG = "SENSOR";

void SensorController::init()
{
}

void SensorController::loop()
{
    dhtHandler();
}

void SensorController::dhtHandler()
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

float SensorController::getTemperature()
{
    return mTemperature;
}

float SensorController::getHumidity()
{
    return mHumidity;
}

bool SensorController::isValidDhtValue(float value)
{
    return value > DHT_INVALID_VALUE;
}