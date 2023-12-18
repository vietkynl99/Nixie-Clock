#ifndef _LOG_H_
#define _LOG_H_

#include <Arduino.h>

#define ENABLE_LOG
#define USE_MUTEX

#ifdef ENABLE_LOG
#define LOG(...) Log::print(TAG, __VA_ARGS__);
#define LOGF(...) Log::print(TAG, __VA_ARGS__);
#else
#define LOG(...)
#define LOGF(...) Log::print(TAG, __VA_ARGS__);
#endif

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