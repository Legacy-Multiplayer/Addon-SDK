#pragma once

#include "../Types.h"
#include "Value.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct FileSystemAPI
{
    int (*Open)(const char* path, const char* mode);

    bool (*Close)(int handle);

    const char* (*Read)(int handle, size_t bytes);

    int (*Write)(int handle, const char* data);

    int (*Seek)(int handle, int offset, int origin);

    int (*Tell)(int handle);

    bool (*Flush)(int handle);

    bool (*Eof)(int handle);

    int (*Size)(int handle);

    bool (*Exists)(const char* path);

    bool (*Delete)(const char* path);

    bool (*DirExists)(const char* path);

    bool (*DirCreate)(const char* path);

    bool (*DirDelete)(const char* path);

    bool (*Rename)(const char* oldPath, const char* newPath);
} FileSystemAPI;

#ifdef __cplusplus
}
#endif
