#ifndef _SENSOR_CONTROLLER_H_
#define _SENSOR_CONTROLLER_H_

#include <Arduino.h>
#include <DHT_Async.h>
#include "../common/Configs.h"
#include "../common/Helper.h"
#include "../common/MessageEvent.h"

class SensorController
{
private:
    static DHT_Async mDHT;
    static float mTemperature;
    static float mHumidity;

public:
    static void init();
    static void loop();

    static float getTemperature();
    static float getHumidity();
    static bool isValidDhtValue(float value);

private:
    static void dhtHandler();
};

#endif