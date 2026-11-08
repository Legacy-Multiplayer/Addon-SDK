# Legacy Plugin SDK

Shared C/C++17 SDK for Legacy Multiplayer server plugins. This repository uses `main` and addon API version **1**. The exact native layout/export fingerprint must match the server; rebuild plugins when updating the pinned SDK revision.

## Include once

```cpp
#include "SDK/Plugin.h"

class CExample final : public SDK::CPlugin
{
    bool OnLoad() override
    {
        Function("echo", [](SDK::CTable options)
        {
            return options;
        });

        return true;
    }
};

REGISTER_PLUGIN(CExample)
```

`SDK/Plugin.h` is the managed entry point. `SDK/API.h` is the C-compatible API descriptor. Native player, actor, vehicle, object and other services have their own headers under `SDK/Native`. Focused C++ headers provide table/value conversion, callbacks, tasks, logging, files, configuration and checked entity references.

Use this repository as a pinned Git submodule. Consumer projects add `SDK` with CMake and link `Legacy::SDK`; the shared `cmake/Plugin.cmake` helper defines `legacy_plugin`. Build with a 32-bit toolchain matching Legacy Server.

```sh
git clone --recurse-submodules <consumer-repository-url>
# For an existing checkout:
git submodule update --init --recursive
```

Owned table data can cross worker boundaries. Lua callbacks run on the main thread and are invalidated when their script, addon or Lua runtime is released. Full examples and lifecycle documentation are maintained in [Addons-Template](https://github.com/Legacy-Multiplayer/Addons-Template). The SDK does not contain the server implementation or link another Lua runtime into plugins.
