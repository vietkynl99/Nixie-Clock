#ifndef _LAUNCHER_MANAGER_H_
#define _LAUNCHER_MANAGER_H_

#include "Log.h"

enum FragmentType {
    FRAGMENT_TYPE_NONE,
    FRAGMENT_TYPE_CLOCK,
    FRAGMENT_TYPE_MENU,
    FRAGMENT_TYPE_MAX
};

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