#include "../../include/manager/LauncherManager.h"

FragmentType LauncherManager::mCurrentFragmentType = FRAGMENT_TYPE_NONE;

static constexpr const char *const TAG = "LAUNCHER";

void LauncherManager::init()
{
    DisplayController::init();
    ClockFragment::init();
    MenuFragment::init();
    CubeFragment::init();
    show(FRAGMENT_TYPE_DEFAULT);
}

void LauncherManager::loop()
{
    ClockFragment::loop();
    MenuFragment::loop();
    CubeFragment::loop();
}

void LauncherManager::show(FragmentType type)
{
    if (type < FRAGMENT_TYPE_NONE || type >= FRAGMENT_TYPE_MAX)
    {
        LOG("Invalid fragment type: %d", type);
        return;
    }
    if (mCurrentFragmentType == type)
    {
        return;
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

    DisplayController::clear();

    switch (type)
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
    default:
        break;
    }

    mCurrentFragmentType = type;
}