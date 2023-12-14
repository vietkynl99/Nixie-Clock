#include "../../include/manager/LauncherManager.h"

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

    BootFragment::show();
}

void LauncherManager::loop()
{
    if (BootFragment::isVisible())
    {
        if (BootFragment::isDone())
        {
            show(FRAGMENT_TYPE_DEFAULT);
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
    }
}

void LauncherManager::show(int type)
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
    mCurrentFragmentType = type;

    if (BootFragment::isVisible())
    {
        BootFragment::hide();
    }
    if (ReBootFragment::isVisible())
    {
        ReBootFragment::hide();
    }
    if (ClockFragment::isVisible())
    {
        ClockFragment::hide();
    }
    if (MenuFragment::isVisible())
    {
        MenuFragment::hide();
    }
    if (CubeFragment::isVisible())
    {
        CubeFragment::hide();
    }

    DisplayController::selectDisplay(TFT_MAX);
    DisplayController::clear();

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
    default:
        break;
    }
}

void LauncherManager::handleEvent(const Message &message)
{
    switch (message.type)
    {
    case MESSAGE_TYPE_BUTTON_LONG_PRESSED:
    {
        if (message.value == BUTTON_ENTER)
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
            case BUTTON_ENTER:
                MenuFragment::enter();
                break;
            case BUTTON_UP:
                MenuFragment::up();
                break;
            case BUTTON_DOWN:
                MenuFragment::down();
                break;
            default:
                break;
            }
        }
        break;
    }
    case MESSAGE_TYPE_REBOOT:
    {
        show(FRAGMENT_TYPE_REBOOT);
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