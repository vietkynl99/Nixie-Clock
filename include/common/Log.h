#ifndef _LOG_H_
#define _LOG_H_

#include <Arduino.h>

enum LogLevel
{
    LOG_LEVEL_NONE,
    LOG_LEVEL_ERROR
};

#define ENABLE_LOG
#define USE_MUTEX

#ifdef ENABLE_LOG
#define LOG(...) Log::print(LOG_LEVEL_NONE, TAG, __VA_ARGS__);
#define LOGF(...) Log::print(LOG_LEVEL_NONE, TAG, __VA_ARGS__);
#define LOGE(...) Log::print(LOG_LEVEL_ERROR, TAG, __VA_ARGS__);
#else
#define LOG(...)
#define LOGF(...) Log::print(LOG_LEVEL_NONE, TAG, __VA_ARGS__);
#define LOGE(...) Log::print(LOG_LEVEL_ERROR, TAG, __VA_ARGS__);
#endif

class Log
{
private:
#ifdef USE_MUTEX
    static SemaphoreHandle_t mMutex;
#endif

public:
    static void print(uint8_t level, const char *tag, const char *pFormat, ...);
};
#endif