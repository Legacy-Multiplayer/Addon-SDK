#pragma once

#include "../Types.h"
#include "Value.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct VehicleAPI
{
    int (*Create)(int modelId, float x, float y, float z, float rotation, int color1, int color2, int respawnDelay, bool addSiren);

    int (*AddStatic)(int modelId, float x, float y, float z, float rotation, int color1, int color2);

    int (*AddStaticEx)(int modelId, float x, float y, float z, float rotation, int color1, int color2, int respawnDelay, bool addSiren);

    bool (*Destroy)(int vehicleId);

    bool (*IsValid)(int vehicleId);

    int (*GetModel)(int vehicleId);

    int (*GetInterior)(int vehicleId);

    bool (*LinkToInterior)(int vehicleId, int interior);

    bool (*GetPosition)(int vehicleId, float* outX, float* outY, float* outZ);

    bool (*SetPosition)(int vehicleId, float x, float y, float z);

    bool (*GetZAngle)(int vehicleId, float* outAngle);

    bool (*SetZAngle)(int vehicleId, float angle);

    bool (*GetHealth)(int vehicleId, float* outHealth);

    bool (*SetHealth)(int vehicleId, float health);

    bool (*SetToRespawn)(int vehicleId);

    bool (*GetColor)(int vehicleId, int* outColor1, int* outColor2);

    bool (*ChangeColor)(int vehicleId, int color1, int color2);

    bool (*ChangePaintjob)(int vehicleId, int paintjob);

    int (*GetPaintjob)(int vehicleId);

    bool (*SetNumberPlate)(int vehicleId, const char* plate);

    const char* (*GetNumberPlate)(int vehicleId);

    bool (*AttachTrailer)(int trailerId, int vehicleId);

    bool (*DetachTrailer)(int vehicleId);

    bool (*IsTrailerAttached)(int vehicleId);

    int (*GetTrailer)(int vehicleId);

    bool (*SetDimension)(int vehicleId, int dimension);

    int (*GetDimension)(int vehicleId);

    bool (*GetVelocity)(int vehicleId, float* outX, float* outY, float* outZ);

    bool (*SetVelocity)(int vehicleId, float x, float y, float z);

    bool (*SetAngularVelocity)(int vehicleId, float x, float y, float z);

    bool (*IsOnItsSide)(int vehicleId);

    bool (*IsUpsideDown)(int vehicleId);

    int (*GetSirenState)(int vehicleId);

    bool (*IsWrecked)(int vehicleId);

    bool (*IsSunked)(int vehicleId);

    int (*GetRespawnDelay)(int vehicleId);

    bool (*SetRespawnDelay)(int vehicleId, int delay);

    bool (*GetDamageStatus)(int vehicleId, int* outPanels, int* outDoors, uint8_t* outLights, uint8_t* outTires);

    bool (*UpdateDamageStatus)(int vehicleId, int panels, int doors, uint8_t lights, uint8_t tires);

    bool (*SetParamsForPlayer)(int vehicleId, int playerId, int objective, int locked);

    bool (*ManualEngineAndLights)(void);

    bool (*Repair)(int vehicleId);

    bool (*Explode)(int vehicleId);

    bool (*SetParamsCarDoors)(int vehicleId, int driver, int passenger, int backLeft, int backRight);

    bool (*GetParamsCarDoors)(int vehicleId, int* outDriver, int* outPassenger, int* outBackLeft, int* outBackRight);

    bool (*SetParamsCarWindows)(int vehicleId, int driver, int passenger, int backLeft, int backRight);

    bool (*GetParamsCarWindows)(int vehicleId, int* outDriver, int* outPassenger, int* outBackLeft, int* outBackRight);

    bool (*ToggleTaxiLight)(int vehicleId, bool toggle);

    bool (*SetEngineState)(int vehicleId, bool engineState);

    bool (*SetLightState)(int vehicleId, bool lightState);

    bool (*SetFeature)(int vehicleId, bool feature);

    bool (*SetVisibility)(int vehicleId, bool visible);

    bool (*SetHoodState)(int vehicleId, bool state);

    bool (*SetTrunkState)(int vehicleId, bool state);

    bool (*SetDoorState)(int vehicleId, int doorId, bool state);

    bool (*GetSpawnInfo)(int vehicleId, float* outX, float* outY, float* outZ, float* outRot, int* outCol1, int* outCol2);

    bool (*SetSpawnInfo)(int vehicleId, int modelId, float x, float y, float z, float rotation, int color1, int color2, int respawnDelay, int interior);

    bool (*GetSpawnPos)(int vehicleId, float* outX, float* outY, float* outZ);

    bool (*SetSpawnPos)(int vehicleId, float x, float y, float z);

    int (*GetComponentInSlot)(int vehicleId, int slot);

    int (*GetComponentType)(int componentId);

    bool (*AddComponent)(int vehicleId, int componentId);

    bool (*RemoveComponent)(int vehicleId, int componentId);

    bool (*GetModelInfo)(int modelId, int infoType, float* outX, float* outY, float* outZ);

    int (*GetParamsSirenState)(int vehicleId);

    bool (*GetRotationQuat)(int vehicleId, float* outW, float* outX, float* outY, float* outZ);

    float (*GetDistanceFromPoint)(int vehicleId, float x, float y, float z);

    int (*GetModelCount)(int modelId);

    int (*GetModelsUsed)(void);

    int (*GetCab)(int vehicleId);

    int (*GetDriver)(int vehicleId);

    int (*GetPassenger)(int vehicleId, int seatId);

    int (*GetSpawnModel)(int vehicleId);

    int (*GetSpawnInterior)(int vehicleId);

    bool (*IsStreamedIn)(int vehicleId, int forPlayerId);
} VehicleAPI;

#ifdef __cplusplus
}
#endif
