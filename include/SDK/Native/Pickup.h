#pragma once

#include "../Types.h"
#include "Value.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct PickupAPI
{
    int (*Create)(int model, int type, float x, float y, float z, int dimension);

    bool (*AddStatic)(int model, int type, float x, float y, float z, int dimension);

    bool (*Destroy)(int pickupId);

    bool (*DestroyAll)(void);

    bool (*IsValid)(int pickupId);

    bool (*IsStatic)(int pickupId);

    bool (*GetPosition)(int pickupId, float* outX, float* outY, float* outZ);

    int (*GetModel)(int pickupId);

    int (*GetType)(int pickupId);

    int (*GetCount)(void);

    int (*GetDimension)(int pickupId);
} PickupAPI;

#ifdef __cplusplus
}
#endif
