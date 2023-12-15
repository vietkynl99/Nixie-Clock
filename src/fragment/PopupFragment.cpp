#include "../../include/fragment/PopupFragment.h"

bool PopupFragment::mIsVisible = false;
bool PopupFragment::mNeedsRedraw = false;
bool PopupFragment::mIsFirstTime = false;
int PopupFragment::mType = POPUP_TYPE_CONFIRM_REBOOT;
bool PopupFragment::mSelection = false;
void (*PopupFragment::callbackFunc)(bool) = nullptr;

static constexpr const char *const TAG = "POPUP";

void PopupFragment::init()
{
    DisplayController::init();
}

void PopupFragment::loop()
{
    static uint32_t timeTick = 0;

    if (mIsVisible && mNeedsRedraw && xTaskGetTickCount() > timeTick)
    {
        timeTick = xTaskGetTickCount() + 10 / portTICK_PERIOD_MS;
        if (mIsFirstTime)
        {
            drawPopup();
        }
        else
        {
            updatePopupButton();
        }
        mNeedsRedraw = false;
        mIsFirstTime = false;
    }
}

void PopupFragment::show()
{
    if (!mIsVisible)
    {
        LOG("Show");
        mIsVisible = true;
        mIsFirstTime = true;
        mNeedsRedraw = true;
    }
}

void PopupFragment::hide()
{
    if (mIsVisible)
    {
        LOG("Hide");
        mIsVisible = false;
        mIsFirstTime = true;
        mNeedsRedraw = true;
    }
}

bool PopupFragment::isVisible()
{
    return mIsVisible;
}

void PopupFragment::handleEvent(const Message &message)
{
    switch (message.type)
    {
    case MESSAGE_TYPE_BUTTON_SHORT_PRESSED:
    {
        switch (message.value)
        {
        case BUTTON1_INDEX:
            if (callbackFunc)
            {
                callbackFunc(mSelection);
            }
            callbackFunc = nullptr;
            break;
        case BUTTON2_INDEX:
            setSelection(true);
            break;
        case BUTTON3_INDEX:
            setSelection(false);
            break;
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

void PopupFragment::setSelection(bool selection)
{
    if (mSelection != selection)
    {
        mSelection = selection;
        mNeedsRedraw = true;
    }
}

void PopupFragment::setType(int type)
{
    if (mType != type)
    {
        mType = type;
        mNeedsRedraw = true;
    }
}

void PopupFragment::setCallback(void (*callback)(bool))
{
    callbackFunc = callback;
}

void PopupFragment::drawPopup()
{
    LOG("Draw popup: %d", mType);

    String message = "Unknown message";
    switch (mType)
    {
    case POPUP_TYPE_CONFIRM_REBOOT:
        message = POPUP_MESSAGE_CONFIRM_REBOOT;
        break;
    default:
        break;
    }

    DisplayController::getTft()->fillRoundRect(POPUP_XPOS, POPUP_YPOS, POPUP_WIDTH, POPUP_HEIGHT, 10, TFT_WHITE);

    DisplayController::getTft()->setFreeFont(FSB12);
    DisplayController::getTft()->setTextSize(1);
    DisplayController::getTft()->setTextColor(TFT_BLACK);
    DisplayController::getTft()->setTextWrap(false);

    bool done = false;
    int yPosText = POPUP_YPOS + DisplayController::getTft()->fontHeight();
    while (!done)
    {
        String line;
        int index = message.indexOf('\n');
        if (index >= 0)
        {
            line = message.substring(0, index);
            message = message.substring(index + 1);
        }
        else
        {
            line = message;
            done = true;
        }
        DisplayController::getTft()->setCursor(POPUP_XPOS + TEXT_LEFT_MARGIN, yPosText);
        DisplayController::getTft()->print(line);
        yPosText += DisplayController::getTft()->fontHeight();
    }

    updatePopupButton();
}

void PopupFragment::updatePopupButton()
{
    DisplayController::drawButton(BUTTON_OK_XPOS, BUTTON_YPOS, BUTTON_WIDTH, BUTTON_HEIGHT, "OK", mSelection);
    DisplayController::drawButton(BUTTON_CANCEL_XPOS, BUTTON_YPOS, BUTTON_WIDTH, BUTTON_HEIGHT, "Cancel", !mSelection);
}
