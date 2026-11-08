#pragma once

#include "../Types.h"
#include "Value.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct WorldAPI
{
    bool (*SetWeather)(int weatherId);

    int (*GetWeather)(void);

    bool (*SetWorldTime)(int hour);

    int (*GetWorldTime)(void);

    bool (*SetGravity)(float gravity);

    float (*GetGravity)(void);

    int (*GetTickCount)(void);

    int (*GetServerTickCount)(void);

    int (*GetServerTickRate)(void);

    bool (*SetGameModeText)(const char* text);

    bool (*ShowNameTags)(bool show);

    bool (*ShowPlayerMarkers)(bool show);

    bool (*EnableZoneNames)(bool enable);

    bool (*EnableTirePopping)(bool enable);

    bool (*SetDefaultPlayerCJMovementEnabled)(bool enabled, bool applyToConnectedPlayers);

    bool (*DisableInteriorEnterExits)(void);

    bool (*DisableVehicleMarkers)(void);

    bool (*DisableNameTagLOS)(void);

    bool (*SetNameTagDrawDistance)(float distance);

    bool (*LimitGlobalChatRadius)(float radius);

    bool (*LimitPlayerMarkerRadius)(float radius);

    bool (*GameModeExit)(void);

    const char* (*GetWeaponName)(int weaponId);

    int (*FindWeaponId)(const char* name);

    const char* (*GetVehicleName)(int modelId);

    int (*FindVehicleModel)(const char* name);

    bool (*CreateExplosion)(float x, float y, float z, int type, float radius);

    bool (*CreateExplosionForPlayer)(int playerId, float x, float y, float z, int type, float radius);

    bool (*SetDisabledWeapons)(const int* weaponIds, size_t count);

    bool (*EnableStuntBonusForAll)(bool enable);

    bool (*EnableStuntBonusForPlayer)(int playerId, bool enable);

    bool (*Kick)(int playerId);

    bool (*Ban)(int playerId);

    bool (*BanEx)(int playerId, const char* reason);

    bool (*RemoveBan)(const char* ip);

    bool (*IsBanned)(const char* ip);

    bool (*BlockIpAddress)(const char* ip, int timeMs);

    bool (*UnblockIpAddress)(const char* ip);

    bool (*IsDefaultPlayerCJMovementEnabled)(void);
} WorldAPI;

#ifdef __cplusplus
}
#endif
