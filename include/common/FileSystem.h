#ifndef _FILE_SYSTEM_H_
#define _FILE_SYSTEM_H_

#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>
#include "../common/Log.h"
#include "../common/Helper.h"

class FileSystem
{
public:
    static void init();
    static void listDir(const char *dirname, uint8_t levels);
    static bool readFile(const char *path, String &content);
};

#endif