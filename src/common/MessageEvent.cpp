#include "../../include/common/MessageEvent.h"

#define QUEUE_ELEMENT_SIZE 10

QueueHandle_t MessageEvent::mQueueHandle = nullptr;

static constexpr const char *const TAG = "MESSAGE";

bool MessageEvent::init()
{
    mQueueHandle = xQueueCreate(QUEUE_ELEMENT_SIZE, sizeof(Message));
    return mQueueHandle != nullptr;
}

bool MessageEvent::send(const Message &message)
{
    if (!mQueueHandle || uxQueueSpacesAvailable(mQueueHandle) <= 0)
    {
        return false;
    }

    int ret = xQueueSend(mQueueHandle, (void *)&message, 0);
    if (ret == pdTRUE)
    {
        return true;
    }
    else
    {
        LOG("Cannot send message, code: %d", ret);
        return false;
    }
}

bool MessageEvent::get(Message &message)
{
    if (!mQueueHandle || uxQueueMessagesWaiting(mQueueHandle) <= 0)
    {
        return false;
    }

    int ret = xQueueReceive(mQueueHandle, &message, portMAX_DELAY);
    if (ret == pdPASS)
    {
        return true;
    }
    else
    {
        LOG("Cannot get message, code: %d", ret);
        return false;
    }
}