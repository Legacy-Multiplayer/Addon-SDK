#pragma once

#include "../Types.h"
#include "Value.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct PlayerAPI
{
    bool (*IsConnected)(int playerId);

    const char* (*GetName)(int playerId);

    const char* (*GetIp)(int playerId);

    bool (*GetPosition)(int playerId, float* outX, float* outY, float* outZ);

    bool (*GetVelocity)(int playerId, float* outX, float* outY, float* outZ);

    bool (*GetCameraPosition)(int playerId, float* outX, float* outY, float* outZ);

    bool (*GetCameraFrontVector)(int playerId, float* outX, float* outY, float* outZ);

    bool (*GetWorldBounds)(int playerId, float* outXMax, float* outYMax, float* outXMin, float* outYMin);

    bool (*GetKeys)(int playerId, int* outKeys, int* outUpDown, int* outLeftRight);

    bool (*GetTime)(int playerId, int* outHour, int* outMinute);

    bool (*GetWeaponData)(int playerId, int slot, int* outWeapon, int* outAmmo);

    bool (*GetHealth)(int playerId, float* outHealth);

    bool (*GetArmour)(int playerId, float* outArmour);

    bool (*GetFacingAngle)(int playerId, float* outAngle);

    int (*GetPing)(int playerId);

    float (*GetCameraZoom)(int playerId);

    float (*GetCameraAspectRatio)(int playerId);

    int (*GetSkin)(int playerId);

    uint32_t (*GetColor)(int playerId);

    int (*GetInterior)(int playerId);

    int (*GetDimension)(int playerId);

    int (*GetState)(int playerId);

    int (*GetVehicleId)(int playerId);

    int (*GetScore)(int playerId);

    int (*GetMoney)(int playerId);

    int (*GetWeapon)(int playerId);

    int (*GetAmmo)(int playerId);

    int (*GetWantedLevel)(int playerId);

    int (*GetSpecialAction)(int playerId);

    int (*GetDrunkLevel)(int playerId);

    int (*GetTargetPlayer)(int playerId);

    int (*GetTargetActor)(int playerId);

    int (*GetCameraMode)(int playerId);

    int (*GetVehicleSeat)(int playerId);

    int (*GetFightingStyle)(int playerId);

    int (*GetFightingMove)(int playerId);

    int (*GetWeaponState)(int playerId);

    int (*GetSurfingVehicleId)(int playerId);

    bool (*IsInCheckpoint)(int playerId);

    bool (*IsInRaceCheckpoint)(int playerId);

    bool (*IsInVehicle)(int playerId, int vehicleId);

    bool (*IsInAnyVehicle)(int playerId);

    bool (*IsTyping)(int playerId);

    int (*GetCount)(void);

    int (*GetIdFromName)(const char* name);

    bool (*IsInRangeOfPoint)(int playerId, float range, float x, float y, float z);

    float (*GetDistanceFromPoint)(int playerId, float x, float y, float z);

    bool (*IsPickupStreamedIn)(int pickupId, int playerId);

    bool (*IsActorStreamedIn)(int actorId, int playerId);

    bool (*SetSpawnInfo)(int playerId, int skin, float x, float y, float z, float rotation, int weapon1, int ammo1, int weapon2, int ammo2, int weapon3, int ammo3);

    bool (*Spawn)(int playerId);

    bool (*SetPosition)(int playerId, float x, float y, float z);

    bool (*SetPositionFindZ)(int playerId, float x, float y, float z);

    bool (*SetHealth)(int playerId, float health);

    bool (*SetArmour)(int playerId, float armour);

    bool (*SetFacingAngle)(int playerId, float angle);

    bool (*SetInterior)(int playerId, int interiorId);

    bool (*SetVelocity)(int playerId, float x, float y, float z);

    bool (*SetSkin)(int playerId, int skinId);

    bool (*SetColor)(int playerId, uint32_t color);

    bool (*SetDimension)(int playerId, int dimension);

    bool (*SetTime)(int playerId, int hour, int minute);

    bool (*SetScore)(int playerId, int score);

    bool (*GiveMoney)(int playerId, int money);

    bool (*SetMoney)(int playerId, int money);

    bool (*ResetMoney)(int playerId);

    int (*SetName)(int playerId, const char* name);

    bool (*SetDrunkLevel)(int playerId, int level);

    bool (*SetWantedLevel)(int playerId, int level);

    bool (*PutInVehicle)(int playerId, int vehicleId, int seatId);

    bool (*RemoveFromVehicle)(int playerId);

    bool (*ToggleControllable)(int playerId, bool toggle);

    bool (*PlaySound)(int playerId, int soundId, float x, float y, float z);

    bool (*ApplyAnimation)(int playerId, const char* animLib, const char* animName, float speed, bool loop, bool lockX, bool lockY, bool lockF, int time);

    bool (*ClearAnimations)(int playerId);

    bool (*SetSpecialAction)(int playerId, int actionId);

    bool (*SetCheckpoint)(int playerId, float x, float y, float z, float size);

    bool (*DisableCheckpoint)(int playerId);

    bool (*SetRaceCheckpoint)(int playerId, int type, float x, float y, float z, float nextX, float nextY, float nextZ, float size);

    bool (*DisableRaceCheckpoint)(int playerId);

    bool (*SetWorldBounds)(int playerId, float xMax, float yMax, float xMin, float yMin);

    bool (*ClearWorldBounds)(int playerId);

    bool (*SetMarkerForPlayer)(int playerId, int showPlayerId, uint32_t color);

    bool (*ShowNameTagForPlayer)(int playerId, int showPlayerId, bool show);

    bool (*SetMapIcon)(int playerId, int iconId, float x, float y, float z, int markerType, uint32_t color, int style);

    bool (*RemoveMapIcon)(int playerId, int iconId);

    bool (*SetCameraPos)(int playerId, float x, float y, float z, float rx, float ry, float rz);

    bool (*SetCameraLookAt)(int playerId, float x, float y, float z, int cut);

    bool (*SetCameraBehindPlayer)(int playerId);

    bool (*ToggleSpectating)(int playerId, bool toggle);

    bool (*SpectatePlayer)(int playerId, int targetPlayerId, int mode);

    bool (*SpectateVehicle)(int playerId, int vehicleId, int mode);

    bool (*SetAmmo)(int playerId, int weaponSlot, int ammo);

    bool (*GiveWeapon)(int playerId, int weaponId, int ammo);

    bool (*ResetWeapons)(int playerId);

    bool (*SetSkillLevel)(int playerId, int skill, int level);

    int (*SetArmedWeapon)(int playerId, int weaponId);

    bool (*SetFightingStyle)(int playerId, int style, int moves);

    bool (*SetMaxHealth)(int playerId, float maxHealth);

    bool (*InterpolateCameraPos)(int playerId, float fromX, float fromY, float fromZ, float toX, float toY, float toZ, int time, int cut);

    bool (*InterpolateCameraLookAt)(int playerId, float fromX, float fromY, float fromZ, float toX, float toY, float toZ, int time, int cut);

    bool (*SetBlurLevel)(int playerId, int blurLevel);

    int (*SetGameSpeed)(int playerId, float speed);

    bool (*ToggleWidescreen)(int playerId, bool on);

    bool (*SetShopName)(int playerId, const char* shopName);

    // Deprecated ABI v1 slots: callable false-return stubs for older binaries.
    bool (*PlayCrimeReport)(int playerId, int suspectId, int crimeId);

    bool (*DisableRemoteVehicleCollisions)(int playerId, bool disable);

    bool (*SetChatBubble)(int playerId, const char* text, uint32_t color, float drawDistance, int expireTime);

    bool (*SetWeather)(int playerId, int weatherId);

    bool (*ToggleClock)(int playerId, bool toggle);

    bool (*CreatePlayerPickup)(int pickupId, int playerId, int model, int type, float x, float y, float z);

    bool (*DestroyPlayerPickup)(int pickupId, int playerId);

    bool (*SetCJMovementEnabled)(int playerId, bool enabled);

    bool (*IsCJMovementEnabled)(int playerId);

    bool (*IsAFK)(int playerId);

    int (*GetFPS)(int playerId);

    int (*GetClipAmmo)(int playerId);

    bool (*IsNPC)(int playerId);

    bool (*IsStreamedIn)(int playerId, int forPlayerId);
} PlayerAPI;

#ifdef __cplusplus
}
#endif
