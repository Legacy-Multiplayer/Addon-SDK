#pragma once

#include "../Types.h"
#include "Value.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct LabelAPI
{
    int (*Create3DText)(const char* text, uint32_t color, float x, float y, float z, float drawDistance, int dimension, bool testLOS);

    bool (*Delete3DText)(int labelId);

    bool (*Attach3DTextToPlayer)(int labelId, int playerId, float offsetX, float offsetY, float offsetZ);

    bool (*Attach3DTextToVehicle)(int labelId, int vehicleId, float offsetX, float offsetY, float offsetZ);

    bool (*Update3DText)(int labelId, uint32_t color, const char* text);

    int (*CreatePlayer3DText)(int playerId, const char* text, uint32_t color, float x, float y, float z, float drawDistance, int attachedPlayerId, int attachedVehicleId, bool testLOS);

    bool (*DeletePlayer3DText)(int playerId, int labelId);

    bool (*UpdatePlayer3DText)(int playerId, int labelId, uint32_t color, const char* text);

    bool (*IsValid3DText)(int labelId);

    bool (*IsValidPlayer3DText)(int playerId, int labelId);

    bool (*Attach3DTextToPlayerBone)(int labelId, int playerId, float offsetX, float offsetY,
        float offsetZ, int boneId);
    int (*CreatePlayer3DTextBone)(int playerId, const char* text, uint32_t color, float x, float y,
        float z, float drawDistance, int attachedPlayerId, int attachedVehicleId, bool testLOS, int boneId);
} LabelAPI;

#ifdef __cplusplus
}
#endif
