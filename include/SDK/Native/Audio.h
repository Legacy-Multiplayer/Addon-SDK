#pragma once

#include "../Types.h"
#include "Value.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct AudioAPI
{
    bool (*Play)(int playerId, const char* assetPathOrName, float volume, bool loop, bool usePosition, float x, float y, float z, float distance);

    bool (*Stop)(int playerId, const char* assetPathOrName);
} AudioAPI;

#ifdef __cplusplus
}
#endif
