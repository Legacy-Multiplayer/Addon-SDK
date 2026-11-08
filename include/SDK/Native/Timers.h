#pragma once

#include "../Types.h"
#include "Value.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct TimersAPI
{
    int (*SetTimer)(int interval, bool repeating, const char* funcName);

    bool (*KillTimer)(int timerId);
} TimersAPI;

#ifdef __cplusplus
}
#endif
