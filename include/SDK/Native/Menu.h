#pragma once

#include "../Types.h"
#include "Value.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct MenuAPI
{
    int (*Create)(const char* title, int columns, float x, float y, float col1Width, float col2Width);

    bool (*Destroy)(int menuId);

    int (*AddItem)(int menuId, int column, const char* text);

    bool (*SetColumnHeader)(int menuId, int column, const char* header);

    bool (*ShowForPlayer)(int menuId, int playerId);

    bool (*HideForPlayer)(int menuId, int playerId);

    bool (*IsValid)(int menuId);

    bool (*Disable)(int menuId);

    bool (*DisableRow)(int menuId, int row);

    int (*GetPlayerMenu)(int playerId);
} MenuAPI;

#ifdef __cplusplus
}
#endif
