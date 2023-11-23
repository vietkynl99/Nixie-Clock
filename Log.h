#ifndef _LOG_H_
#define _LOG_H_

#include <Arduino.h>

#define LOG(TAG, ...) Log::print(TAG, __VA_ARGS__);
#define LOG_WIFI(...) LOG("WIFI", __VA_ARGS__)
#define LOG_SYSTEM(...) LOG("SYSTEM", __VA_ARGS__)
#define LOG_SERVER(...) LOG("SERVER", __VA_ARGS__)
#define LOG_DISPLAY(...) LOG("DISPLAY", __VA_ARGS__)

class Log
{
private:
    static SemaphoreHandle_t mMutex;

public:
    static void print(const char *tag, const char *pFormat, ...);
};
#endif