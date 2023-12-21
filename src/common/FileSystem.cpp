#include "../../include/common/FileSystem.h"

static constexpr const char *const TAG = "FS";

#define FORMAT_SPIFFS_IF_FAILED true

void FileSystem::init()
{
    if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED))
    {
        LOG("SPIFFS Mount Failed");
        return;
    }
    LOG("SPIFFS Mount Success");
}

void FileSystem::listDir(const char *dirname, uint8_t levels)
{
    LOG("Listing directory: %s", dirname);

    File root = SPIFFS.open(dirname);
    if (!root)
    {
        LOG("Error: Failed to open directory");
        return;
    }
    if (!root.isDirectory())
    {
        LOG("Error: Not a directory");
        return;
    }

    File file = root.openNextFile();
    while (file)
    {
        if (file.isDirectory())
        {
            LOG("Dir: %s", file.name());
            if (levels)
            {
                listDir(file.path(), levels - 1);
            }
        }
        else
        {
            LOG("File: %s, size: %d Bytes", file.name(), file.size());
        }
        file = root.openNextFile();
    }
}

bool FileSystem::readFile(const char *path, String &content)
{
    File file = SPIFFS.open(path);
    if (!file || file.isDirectory())
    {
        LOG("Failed to open file %s for reading", path);
        return false;
    }
    LOG("File Content: %s", file.readString().c_str());
    content = file.readString();
    file.close();
    return true;
}