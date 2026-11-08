#pragma once

#include "../Types.h"
#include "Value.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct GangZoneAPI
{
    int (*Create)(float minX, float minY, float maxX, float maxY);

    bool (*Destroy)(int zoneId);

    bool (*ShowForPlayer)(int playerId, int zoneId, uint32_t color);

    bool (*ShowForAll)(int zoneId, uint32_t color);

    bool (*HideForPlayer)(int playerId, int zoneId);

    bool (*HideForAll)(int zoneId);

    bool (*FlashForPlayer)(int playerId, int zoneId, uint32_t color);

    bool (*FlashForAll)(int zoneId, uint32_t color);

    bool (*StopFlashForPlayer)(int playerId, int zoneId);

    bool (*StopFlashForAll)(int zoneId);

    bool (*IsValid)(int zoneId);
} GangZoneAPI;

#ifdef __cplusplus
}
#endif
