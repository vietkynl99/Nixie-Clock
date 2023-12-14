#ifndef _MESSAGE_EVENT_H_
#define _MESSAGE_EVENT_H_

#include <Arduino.h>
#include "../../include/common/Log.h"

enum MessageType{
    MESSAGE_TYPE_BUTTON_SHORT_PRESSED,
    MESSAGE_TYPE_BUTTON_LONG_PRESSED,
    MESSAGE_TYPE_REBOOT
};

typedef struct
{
    MessageType type;
    int value;
} Message;

class MessageEvent
{
private:
    static QueueHandle_t mQueueHandle;

public:
    static bool init();
    static bool send(const Message &message);
    static bool get(Message &message);
};

#endif