#include "../../include/manager/LauncherManager.h"

int LauncherManager::mPrevFragmentType = FRAGMENT_TYPE_MAX;
int LauncherManager::mCurrentFragmentType = FRAGMENT_TYPE_MAX;

static constexpr const char *const TAG = "LAUNCHER";

void LauncherManager::init()
{
    DisplayController::init();

    BootFragment::init();
    ReBootFragment::init();
    ClockFragment::init();
    MenuFragment::init();
    CubeFragment::init();
    PopupFragment::init();

    BootFragment::show();
}

void LauncherManager::loop()
{
    if (BootFragment::isVisible())
    {
        if (BootFragment::isDone())
        {
            show(FRAGMENT_TYPE_DEFAULT);
            Helper::showFreeMemory();
        }
        else
        {
            BootFragment::loop();
        }
    }
    else
    {
        ReBootFragment::loop();
        ClockFragment::loop();
        MenuFragment::loop();
        CubeFragment::loop();
        PopupFragment::loop();
    }
}

void LauncherManager::refresh(bool clearDisplay)
{
    BootFragment::hide();
    ReBootFragment::hide();
    ClockFragment::hide();
    MenuFragment::hide();
    CubeFragment::hide();
    PopupFragment::hide();

    if (clearDisplay)
    {
        DisplayController::selectDisplay(TFT_COUNT);
        DisplayController::clear();
    }

    switch (mCurrentFragmentType)
    {
    case FRAGMENT_TYPE_CLOCK:
        ClockFragment::show();
        break;
    case FRAGMENT_TYPE_MENU:
        MenuFragment::show();
        break;
    case FRAGMENT_TYPE_CUBE:
        CubeFragment::show();
        break;
    case FRAGMENT_TYPE_REBOOT:
        ReBootFragment::show();
        break;
    case FRAGMENT_TYPE_POPUP:
        PopupFragment::show();
        break;
    default:
        break;
    }
}

void LauncherManager::show(int type, bool clearDisplay)
{
    if (type < 0 || type >= FRAGMENT_TYPE_MAX)
    {
        LOG("Invalid fragment type: %d", type);
        return;
    }
    if (mCurrentFragmentType == type)
    {
        return;
    }
    mPrevFragmentType = mCurrentFragmentType;
    mCurrentFragmentType = type;

    refresh(clearDisplay);
}

void LauncherManager::handleEvent(const Message &message)
{
    switch (message.type)
    {
    case MESSAGE_TYPE_BUTTON_LONG_PRESSED:
    {
        if (message.value == BUTTON1_INDEX)
        {
            changeToNextFragment();
        }
        break;
    }
    case MESSAGE_TYPE_BUTTON_SHORT_PRESSED:
    {
        if (mCurrentFragmentType == FRAGMENT_TYPE_MENU)
        {
            switch (message.value)
            {
            case BUTTON1_INDEX:
                MenuFragment::enter();
                break;
            case BUTTON2_INDEX:
                MenuFragment::down();
                break;
            case BUTTON3_INDEX:
                MenuFragment::up();
                break;
            default:
                break;
            }
        }
        else if (mCurrentFragmentType == FRAGMENT_TYPE_POPUP)
        {
            PopupFragment::handleEvent(message);
        }
        break;
    }
    case MESSAGE_TYPE_REBOOT:
    {
        show(FRAGMENT_TYPE_REBOOT);
        break;
    }
    case MESSAGE_TYPE_SHOW_POPUP:
    {
        PopupFragment::setType(message.value);
        show(FRAGMENT_TYPE_POPUP, false);
        break;
    }
    case MESSAGE_TYPE_CHANGE_TO_PREVIOUS_FRAGMENT:
    {
        show(mPrevFragmentType);
        break;
    }
    case MESSAGE_TYPE_UPDATE_TEMP_AND_RH:
    case MESSAGE_TYPE_UPDATE_WIFI_STATUS:
    {
        if (mCurrentFragmentType == FRAGMENT_TYPE_CLOCK)
        {
            ClockFragment::handleEvent(message);
        }
        break;
    }
    default:
        break;
    }
}

void LauncherManager::changeToNextFragment()
{
    LOG("Change to next fragment");
    int type = mCurrentFragmentType + 1;
    if (type >= FRAGMENT_TYPE_REBOOT)
    {
        type = 0;
    }
    show(type);
}