#ifndef _LAUNCHER_MANAGER_H_
#define _LAUNCHER_MANAGER_H_

#include "../common/Log.h"
#include "../common/MessageEvent.h"
#include "../controller/HardwareController.h"
#include "../fragment/BootFragment.h"
#include "../fragment/ClockFragment.h"
#include "../fragment/MenuFragment.h"
#include "../fragment/CubeFragment.h"
#include "../fragment/ReBootFragment.h"
#include "../fragment/PopupFragment.h"

enum FragmentType
{
    FRAGMENT_TYPE_CLOCK,
    FRAGMENT_TYPE_MENU,
    FRAGMENT_TYPE_CUBE,
    FRAGMENT_TYPE_REBOOT,
    FRAGMENT_TYPE_POPUP,
    FRAGMENT_TYPE_MAX
};

#define FRAGMENT_TYPE_DEFAULT (FRAGMENT_TYPE_CLOCK)

class LauncherManager
{
private:
    static int mPrevFragmentType;
    static int mCurrentFragmentType;

public:
    static void init();
    static void loop();
    static void show(int type, bool clearDisplay = true);
    static void handleEvent(const Message &message);

private:
    static void changeToNextFragment();
};

#endif