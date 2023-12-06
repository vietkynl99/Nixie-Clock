#ifndef _LOG_H_
#define _LOG_H_

#include <Arduino.h>

#define USE_MUTEX
#define LOG(...) Log::print(TAG, __VA_ARGS__);

class Log
{
private:
#ifdef USE_MUTEX
    static SemaphoreHandle_t mMutex;
#endif

public:
    static void print(const char *tag, const char *pFormat, ...);
};
#endif