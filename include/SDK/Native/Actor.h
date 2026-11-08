#pragma once

#include "../Types.h"
#include "Value.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct ActorAPI
{
    int (*Create)(int modelId, float x, float y, float z, float rotation);

    bool (*Destroy)(int actorId);

    bool (*IsValid)(int actorId);

    bool (*SetPosition)(int actorId, float x, float y, float z);

    bool (*GetPosition)(int actorId, float* outX, float* outY, float* outZ);

    bool (*SetFacingAngle)(int actorId, float angle);

    bool (*GetFacingAngle)(int actorId, float* outAngle);

    bool (*SetDimension)(int actorId, int dimension);

    int (*GetDimension)(int actorId);

    bool (*ApplyAnimation)(int actorId, const char* animLib, const char* animName, float speed, bool loop, bool lockX, bool lockY, bool freeze, int time);

    bool (*ClearAnimations)(int actorId);
} ActorAPI;

#ifdef __cplusplus
}
#endif
