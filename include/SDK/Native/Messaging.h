#pragma once

#include "../Types.h"
#include "Value.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct MessagingAPI
{
    bool (*SendClientMessage)(int playerId, uint32_t color, const char* message);

    bool (*SendClientMessageToAll)(uint32_t color, const char* message);

    bool (*SendPlayerMessageToPlayer)(int playerId, int senderId, const char* message);

    bool (*SendPlayerMessageToAll)(int senderId, const char* message);

    bool (*GameTextForAll)(const char* text, int time, int style);

    bool (*GameTextForPlayer)(int playerId, const char* text, int time, int style);
} MessagingAPI;

#ifdef __cplusplus
}
#endif
