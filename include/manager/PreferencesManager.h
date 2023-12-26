#ifndef _PREFERENCES_MANAGER_H_
#define _PREFERENCES_MANAGER_H_

#include <Arduino.h>
#include <Preferences.h>
#include "../common/Log.h"

class PreferencesManager
{
private:
    static Preferences *mPreferences;

public:
    static void clear(const char *name);

    static void putInt(const char *name, const char *key, int value);
    static void putString(const char *name, const char *key, String value);

    static int getInt(const char *name, const char *key, int defaultValue);
    static String getString(const char *name, const char *key, String defaultValue = "");
};

#endif