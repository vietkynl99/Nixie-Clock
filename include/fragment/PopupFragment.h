#ifndef _POPUP_FRAGMENT_H_
#define _POPUP_FRAGMENT_H_

#include "../common/Log.h"
#include "../common/MessageEvent.h"
#include "../controller/HardwareController.h"
#include "../controller/DisplayController.h"

#define POPUP_MESSAGE_CONFIRM_REBOOT "Device needs to\nreboot to apply\nthis change."

enum PopupType {
    POPUP_TYPE_CONFIRM_REBOOT
};

class PopupFragment
{
private:
    static bool mIsVisible;
    static bool mNeedsRedraw;
    static bool mIsFirstTime;
    static int mType;
    static bool mSelection;
    static void (*callbackFunc)(bool);

public:
    static void init();
    static void loop();
    static void show();
    static void hide();
    static bool isVisible();
    static void handleEvent(const Message &message);
    static void setSelection(bool selection);
    static void setType(int type);
    static void setCallback(void (*callback)(bool));

private:
    static void drawPopup();
    static void updatePopupButton();
};

#endif