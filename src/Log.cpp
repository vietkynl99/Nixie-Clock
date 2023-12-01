#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include "../include/Log.h"

#ifdef USE_MUTEX
SemaphoreHandle_t Log::mMutex = xSemaphoreCreateMutex();
#endif

void Log::print(const char *tag, const char *pFormat, ...)
{
    static char mStringBuffer[256];

#ifdef USE_MUTEX
    if (mMutex != NULL && xSemaphoreTake(mMutex, portMAX_DELAY) == pdTRUE)
    {
#endif
        va_list pVlist;
        va_start(pVlist, pFormat);
        vsnprintf(mStringBuffer, sizeof(mStringBuffer) - 1, pFormat, pVlist);
        va_end(pVlist);
        Serial.print("[");
        Serial.print(tag);
        Serial.print("] ");
        Serial.println(mStringBuffer);
#ifdef USE_MUTEX
        xSemaphoreGive(mMutex);
    }
#endif
}