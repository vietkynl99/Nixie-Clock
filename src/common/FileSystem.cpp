#include "../../include/common/FileSystem.h"

static constexpr const char *const TAG = "FS";

#define FSYSTEM LittleFS
#define FORMAT_IF_FAILED true

void FileSystem::init()
{
    if (!FSYSTEM.begin(FORMAT_IF_FAILED))
    {
        LOGE("Mount Failed");
        return;
    }
}

void FileSystem::listDir(const char *dirname, uint8_t levels)
{
    LOG("Listing directory: %s", dirname);

    fs::File root = FSYSTEM.open(dirname);
    if (!root)
    {
        LOGE("Error: Failed to open directory");
        return;
    }
    if (!root.isDirectory())
    {
        LOGE("Error: Not a directory");
        return;
    }

    fs::File file = root.openNextFile();
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
    fs::File file = FSYSTEM.open(path);
    if (!file || file.isDirectory())
    {
        LOGE("Failed to open file %s for reading", path);
        return false;
    }
    content = file.readString();
    file.close();
    return true;
}

bool FileSystem::writeFile(const char *path, String &content)
{
    fs::File file = FSYSTEM.open(path, FILE_WRITE);
    if (!file)
    {
        LOGE("Failed to open file %s for writing", path);
        return false;
    }
    file.print(content);
    file.close();
    return true;
}

bool FileSystem::openFile(fs::File &file, const char *path)
{
    file = FSYSTEM.open(path);
    if (!file || file.isDirectory())
    {
        LOGE("Failed to open file %s for reading", path);
        return false;
    }
    return true;
}