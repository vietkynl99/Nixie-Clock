#include "../../include/common/FileSystem.h"

static constexpr const char *const TAG = "FS";

#define FORMAT_IF_FAILED true

void FileSystem::init()
{
    if (!FS.begin(FORMAT_IF_FAILED))
    {
        LOGE("FS Mount Failed");
        return;
    }
}

void FileSystem::listDir(const char *dirname, uint8_t levels)
{
    LOG("Listing directory: %s", dirname);

    fs::File root = FS.open(dirname);
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
    fs::File file = FS.open(path);
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
    fs::File file = FS.open(path, FILE_WRITE);
    if (!file)
    {
        LOGE("Failed to open file %s for writing", path);
        return false;
    }
    bool ret = false;
    if (file.print(content))
    {
        ret = true;
    }
    else
    {
        LOGE("Write to %s failed", path);
    }
    file.close();
    return ret;
}

bool FileSystem::openFile(fs::File &file, const char *path)
{
    file = FS.open(path);
    if (!file || file.isDirectory())
    {
        LOGE("Failed to open file %s for reading", path);
        return false;
    }
    return true;
}