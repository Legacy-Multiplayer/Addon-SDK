#pragma once

#include "Native/Value.h"
#include "Native/Actor.h"
#include "Native/Player.h"
#include "Native/Vehicle.h"
#include "Native/Object.h"
#include "Native/Pickup.h"
#include "Native/GangZone.h"
#include "Native/Label.h"
#include "Native/Menu.h"
#include "Native/Messaging.h"
#include "Native/NetStats.h"
#include "Native/FileSystem.h"
#include "Native/World.h"
#include "Native/Timers.h"
#include "Native/Audio.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct PluginAPI
{
    uint32_t apiVersion;
    uint32_t structSize;

    // --- Addon Information ---
    const char* (*GetName)(void* addon);

    const char* (*GetPath)(void* addon);

    // --- Logging ---
    void (*Log)(void* addon, LogLevel level, const char* message);

    // --- Function Registration ---
    uint64_t (*RegisterFunction)(void* addon, const char* functionName, FunctionCallback callback, void* userData);

    bool (*UnregisterFunction)(void* addon, uint64_t handle);

    // --- Event Registration & Triggering ---
    uint64_t (*RegisterEvent)(void* addon, const char* eventName, EventCallback callback, void* userData);

    bool (*UnregisterEvent)(void* addon, uint64_t handle);

    bool (*TriggerEvent)(void* addon, const char* eventName, const Value* arguments, size_t argumentCount);

    // --- Dynamic Function Invocation (CallContext Builder) ---
    CallContext* (*BeginCall)(void* addon, const char* functionName);

    void (*EndCall)(CallContext* context);

    bool (*PushNil)(CallContext* context);

    bool (*PushBoolean)(CallContext* context, bool value);

    bool (*PushInteger)(CallContext* context, int64_t value);

    bool (*PushNumber)(CallContext* context, double value);

    bool (*PushString)(CallContext* context, const char* value);

    bool (*PushValue)(CallContext* context, const Value* value);

    bool (*ExecuteCall)(CallContext* context);

    size_t (*GetResultCount)(const CallContext* context);

    ValueType (*GetResultType)(const CallContext* context, size_t index);

    bool (*GetResultBoolean)(const CallContext* context, size_t index);

    int64_t (*GetResultInteger)(const CallContext* context, size_t index);

    double (*GetResultNumber)(const CallContext* context, size_t index);

    const char* (*GetResultString)(const CallContext* context, size_t index);

    bool (*GetResultValue)(const CallContext* context, size_t index, Value* outValue);

    bool (*HasError)(const CallContext* context);

    const char* (*GetError)(const CallContext* context);

    // --- Global Lua Value Registration ---
    bool (*SetGlobalInteger)(void* addon, const char* name, int64_t value);

    bool (*SetGlobalNumber)(void* addon, const char* name, double value);

    bool (*SetGlobalString)(void* addon, const char* name, const char* value);

    bool (*SetGlobalBoolean)(void* addon, const char* name, bool value);

    // --- Real Server Subsystem APIs ---
    const ActorAPI* actor;
    const PlayerAPI* player;
    const VehicleAPI* vehicle;
    const ObjectAPI* object;
    const PickupAPI* pickup;
    const GangZoneAPI* gangzone;
    const LabelAPI* label;
    const MenuAPI* menu;
    const MessagingAPI* messaging;
    const NetStatsAPI* netstats;
    const FileSystemAPI* filesystem;
    const WorldAPI* world;
    const TimersAPI* timers;

    // Exact SDK layout; rebuild all addons together with the server.
    uint64_t (*RegisterBuiltinFunction)(void* addon, const char* name, const char* builtinName);

    const AudioAPI* audio;

    // Owned native services. A successful registration transfers userData ownership
    // to the host, which calls dispose exactly once, including during unload.
    uint64_t (*CreateNativeTimer)(void* addon, uint32_t intervalMs, bool repeat, TaskCallback callback, void* userData, DisposeCallback dispose);

    bool (*CancelNativeTimer)(void* addon, uint64_t handle);

    uint64_t (*QueueWork)(void* addon, WorkCallback work, TaskCallback complete, void* userData, DisposeCallback dispose);

    bool (*IsWorkCancelled)(const WorkContext* context);

    void (*CancelOwnedTasks)(void* addon);

    uint32_t (*GetEntityGeneration)(EntityKind kind, int id);

    size_t (*CopyEntityIds)(EntityKind kind, int* ids, size_t capacity);

    // Borrowed function values are valid for their native callback scope only.
    // Retain returns an addon-owned token; release is safe from worker threads.
    uint64_t (*RetainLuaCallback)(void* addon, uint64_t borrowedReference);

    void (*ReleaseLuaCallback)(void* addon, uint64_t reference);

    bool (*IsLuaCallbackValid)(void* addon, uint64_t reference);

    bool (*InvokeLuaCallback)(void* addon, uint64_t reference, const Value* arguments, size_t count);
} PluginAPI;

// A rebuild/layout fingerprint, not a new addon API version. Old binaries are
// deliberately rejected before PluginMain can read the changed function tables.
#define PLUGIN_SDK_SIGNATURE (UINT64_C(0x4C47071300000000) ^ ((uint64_t)sizeof(Value) << 40) ^ (uint64_t)sizeof(PluginAPI) ^ ((uint64_t)sizeof(PlayerAPI) << 12) ^ ((uint64_t)sizeof(VehicleAPI) << 24))

// =====================================================================
// Addon Entry Point Types
// =====================================================================

typedef uint32_t (*PluginGetApiVersionFunction)(void);
typedef bool (*PluginMainFunction)(const PluginAPI* api, void* addon);
typedef void (*PluginUnloadFunction)(void);

#ifdef __cplusplus
}
#endif
