#include "../include/PreferencesManager.h"


Preferences *PreferencesManager::mPreferences = new Preferences();

static constexpr const char *const TAG = "PREFERENCES";

void PreferencesManager::clear(const char *name)
{
    mPreferences->begin(name, false);
    mPreferences->clear();
    mPreferences->end();
}

void PreferencesManager::putInt(const char *name, const char *key, int value)
{
    mPreferences->begin(name, false);
    mPreferences->putInt(key, value);
    mPreferences->end();

}

int PreferencesManager::getInt(const char *name, const char *key, int defaultValue)
{
    int ret = 0;
    mPreferences->begin(name, false);
    ret = mPreferences->getInt(key, defaultValue);
    mPreferences->end();
    return ret;
}