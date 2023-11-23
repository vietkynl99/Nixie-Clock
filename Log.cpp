#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include "Log.h"

SemaphoreHandle_t Log::mMutex = xSemaphoreCreateMutex();

void Log::print(const char *tag, const char *pFormat, ...)
{
    static char mStringBuffer[256];

    if (xSemaphoreTake(mMutex, portMAX_DELAY))
    {
        va_list pVlist;
        va_start(pVlist, pFormat);
        vsnprintf(mStringBuffer, sizeof(mStringBuffer) - 1, pFormat, pVlist);
        va_end(pVlist);
        Serial.print("[");
        Serial.print(tag);
        Serial.print("] ");
        Serial.println(mStringBuffer);
        xSemaphoreGive(mMutex);
    }
}