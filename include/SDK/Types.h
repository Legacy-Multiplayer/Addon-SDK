#pragma once

#include "Export.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct CallContext CallContext;

typedef struct Vector3
{
    float x;
    float y;
    float z;
} Vector3;

typedef struct Vector4
{
    float x;
    float y;
    float z;
    float w;
} Vector4;

// =====================================================================
// Log Levels
// =====================================================================

typedef enum LogLevel
{
    PLUGIN_LOG_INFO = 0,
    PLUGIN_LOG_WARN = 1,
    PLUGIN_LOG_ERROR = 2,
    PLUGIN_LOG_DEBUG = 3
} LogLevel;

// Callbacks run on the main server thread, except WorkCallback.
typedef struct WorkContext WorkContext;
typedef void (*TaskCallback)(void* userData);
typedef void (*WorkCallback)(const WorkContext* context, void* userData);
typedef void (*DisposeCallback)(void* userData);

typedef enum EntityKind
{
    PLUGIN_ENTITY_PLAYER = 0,
    PLUGIN_ENTITY_NPC = 1,
    PLUGIN_ENTITY_ACTOR = 2,
    PLUGIN_ENTITY_VEHICLE = 3,
    PLUGIN_ENTITY_OBJECT = 4
} EntityKind;

#ifdef __cplusplus
}
#endif
