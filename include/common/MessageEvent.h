#ifndef _MESSAGE_EVENT_H_
#define _MESSAGE_EVENT_H_

#include <Arduino.h>
#include "../../include/common/Log.h"

enum MessageType{
    MESSAGE_TYPE_NONE,
    MESSAGE_TYPE_BUTTON_SHORT_PRESSED,
    MESSAGE_TYPE_BUTTON_LONG_PRESSED,
    MESSAGE_TYPE_REBOOT,
    MESSAGE_TYPE_SHOW_POPUP,
    MESSAGE_TYPE_CHANGE_TO_PREVIOUS_FRAGMENT,
    MESSAGE_TYPE_UPDATE_TEMP_AND_RH,
    MESSAGE_TYPE_UPDATE_WIFI_STATUS,
    MESSAGE_TYPE_LED_MODE_CHANGED,
    MESSAGE_TYPE_LED_BRIGHTNESS_CHANGED
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