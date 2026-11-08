#pragma once

#include "../Types.h"
#include "Value.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct NetStatsAPI
{
    const char* (*GetNetworkStats)(void);

    const char* (*GetPlayerNetworkStats)(int playerId);

    int (*GetBytesReceived)(int playerId);

    int (*GetBytesSent)(int playerId);

    int (*GetMessagesReceived)(int playerId);

    int (*GetMessagesSent)(int playerId);

    int (*GetMessagesRecvPerSecond)(int playerId);

    float (*GetPacketLossPercent)(int playerId);

    int (*GetConnectionStatus)(int playerId);

    int (*GetConnectedTime)(int playerId);

    const char* (*GetIpPort)(int playerId);
} NetStatsAPI;

#ifdef __cplusplus
}
#endif
