#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include "../../include/common/Log.h"

#ifdef USE_MUTEX
SemaphoreHandle_t Log::mMutex = xSemaphoreCreateMutex();
#endif

void Log::print(uint8_t level, const char *tag, const char *pFormat, ...)
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
        if (level == LOG_LEVEL_ERROR)
        {
            Serial.print(F("[ERROR]"));
        }
        Serial.print(F("["));
        Serial.print(tag);
        Serial.print(F("] "));
        Serial.println(mStringBuffer);
#ifdef USE_MUTEX
        xSemaphoreGive(mMutex);
    }
#endif
}