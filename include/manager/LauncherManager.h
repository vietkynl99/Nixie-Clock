#ifndef _LAUNCHER_MANAGER_H_
#define _LAUNCHER_MANAGER_H_

#include "../common/Log.h"
#include "../fragment/BootFragment.h"
#include "../fragment/ClockFragment.h"
#include "../fragment/MenuFragment.h"
#include "../fragment/CubeFragment.h"

enum FragmentType {
    FRAGMENT_TYPE_NONE,
    FRAGMENT_TYPE_CLOCK,
    FRAGMENT_TYPE_MENU,
    FRAGMENT_TYPE_CUBE,
    FRAGMENT_TYPE_MAX
};

#define FRAGMENT_TYPE_DEFAULT (FRAGMENT_TYPE_CLOCK)

class LauncherManager
{
private:
    static FragmentType mCurrentFragmentType;

public:
    static void init();
    static void loop();
    static void show(FragmentType type);
};

#endif