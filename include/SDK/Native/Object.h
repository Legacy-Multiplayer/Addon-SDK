#pragma once

#include "../Types.h"
#include "Value.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct ObjectAPI
{
    int (*Create)(int modelId, float x, float y, float z, float rx, float ry, float rz, float drawDistance);

    bool (*Destroy)(int objectId);

    bool (*IsValid)(int objectId);

    int (*GetModel)(int objectId);

    bool (*GetPos)(int objectId, float* outX, float* outY, float* outZ);

    bool (*SetPos)(int objectId, float x, float y, float z);

    bool (*GetRot)(int objectId, float* outRX, float* outRY, float* outRZ);

    bool (*SetRot)(int objectId, float rx, float ry, float rz);

    float (*Move)(int objectId, float x, float y, float z, float speed);

    bool (*Stop)(int objectId);

    bool (*IsMoving)(int objectId);

    bool (*SetScale)(int objectId, float scale);

    bool (*AttachToPlayer)(int objectId, int playerId, float offsetX, float offsetY, float offsetZ, float rx, float ry, float rz);

    int (*CreatePlayer)(int playerId, int modelId, float x, float y, float z, float rx, float ry, float rz, float drawDistance);

    bool (*DestroyPlayer)(int playerId, int objectId);

    bool (*IsValidPlayer)(int playerId, int objectId);

    int (*GetPlayerModel)(int playerId, int objectId);

    bool (*GetPlayerPos)(int playerId, int objectId, float* outX, float* outY, float* outZ);

    bool (*SetPlayerPos)(int playerId, int objectId, float x, float y, float z);

    bool (*GetPlayerRot)(int playerId, int objectId, float* outRX, float* outRY, float* outRZ);

    bool (*SetPlayerRot)(int playerId, int objectId, float rx, float ry, float rz);

    float (*MovePlayer)(int playerId, int objectId, float x, float y, float z, float speed);

    bool (*StopPlayer)(int playerId, int objectId);

    bool (*IsPlayerMoving)(int playerId, int objectId);

    bool (*AttachPlayerToPlayer)(int playerId, int objectId, int attachToPlayerId, float offsetX, float offsetY, float offsetZ, float rx, float ry, float rz);

    // Dimension-aware world-object helpers.  The legacy Create callback above
    // remains available for existing addons and creates an object in dimension 0.
    int (*CreateInDimension)(int modelId, float x, float y, float z, float rx, float ry, float rz, float drawDistance, int dimension);

    bool (*SetDimension)(int objectId, int dimension);

    int (*GetDimension)(int objectId);

    bool (*AttachToPlayerBone)(int objectId, int playerId, float offsetX, float offsetY, float offsetZ,
        float rx, float ry, float rz, int boneId);
    bool (*AttachPlayerToPlayerBone)(int playerId, int objectId, int targetPlayerId,
        float offsetX, float offsetY, float offsetZ, float rx, float ry, float rz, int boneId);
    bool (*SetScaleAxes)(int objectId, float x, float y, float z);

    bool (*GetScaleAxes)(int objectId, float* x, float* y, float* z);

    bool (*SetPlayerScaleAxes)(int playerId, int objectId, float x, float y, float z);

    bool (*GetPlayerScaleAxes)(int playerId, int objectId, float* x, float* y, float* z);
} ObjectAPI;

#ifdef __cplusplus
}
#endif
