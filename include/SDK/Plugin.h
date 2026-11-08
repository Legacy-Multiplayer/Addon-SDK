#pragma once

#include "Callback.h"
#include "Config.h"
#include "Entities.h"
#include "Log.h"
#include "Tasks.h"
#include <exception>

namespace SDK
{
// Derive once, register typed lambdas in OnLoad, and export with REGISTER_PLUGIN.
// Everything except the Async worker executes on the server's main thread.
class CPlugin
{
public:
    virtual ~CPlugin() = default;

    CPlugin(const CPlugin&) = delete;

    CPlugin& operator=(const CPlugin&) = delete;

    bool Load(const PluginAPI* api, void* owner) noexcept
    {
        if (m_api || !api || !owner || api->apiVersion != PLUGIN_API_VERSION || api->structSize != sizeof(PluginAPI))
        {
            return false;
        }

        m_api = api;
        m_owner = owner;

        Log.Initialize(api, owner);
        Files.Initialize(api->filesystem);
        Config.Initialize(&Files);

        Players.Initialize(api);
        NPCs.Initialize(api);
        Actors.Initialize(api);
        Vehicles.Initialize(api);
        Objects.Initialize(api);

        try
        {
            if (OnLoad())
            {
                return true;
            }
        }
        catch (const std::exception& error)
        {
            Log.Error("Initialization failed: {}", error.what());
        }
        catch (...)
        {
            Log.Error("Initialization failed");
        }

        Unload();

        return false;
    }

    void Unload() noexcept
    {
        if (!m_api || m_unloading)
        {
            return;
        }

        m_unloading = true;
        m_api->CancelOwnedTasks(m_owner);

        try
        {
            OnUnload();
        }
        catch (...)
        {
            Log.Error("OnUnload failed");
        }
        while (!m_cleanup.empty())
        {
            auto cleanup = std::move(m_cleanup.back());

            m_cleanup.pop_back();

            try
            {
                cleanup();
            }
            catch (...)
            {
                Log.Error("Cleanup callback failed");
            }
        }

        for (const auto handle : m_functions)
        {
            m_api->UnregisterFunction(m_owner, handle);
        }

        for (const auto& event : m_events)
        {
            m_api->UnregisterEvent(m_owner, event.first);
        }

        m_functions.clear();
        m_events.clear();
        m_bindings.clear();

        Players.Initialize(nullptr);
        NPCs.Initialize(nullptr);
        Actors.Initialize(nullptr);
        Vehicles.Initialize(nullptr);
        Objects.Initialize(nullptr);

        Config.Initialize(nullptr);
        Files.Initialize(nullptr);
        Log.Initialize(nullptr, nullptr);

        m_api = nullptr;
        m_owner = nullptr;
        m_unloading = false;
    }

    const PluginAPI* API() const
    {
        return m_api;
    }

    void* Handle() const
    {
        return m_owner;
    }

    void OnCleanup(std::function<void()> callback)
    {
        m_cleanup.push_back(std::move(callback));
    }

    void RawFunction(const char* name, FunctionCallback callback, void* data = nullptr)
    {
        RequireLoading();
        const auto handle = m_api->RegisterFunction(m_owner, name, callback, data);

        if (!handle)
        {
            throw std::runtime_error(std::string("Cannot register function: ") + name);
        }

        m_functions.push_back(handle);
    }

    void Builtin(const char* name)
    {
        RequireLoading();
        const auto target = std::string("__core_") + name;
        const auto handle = m_api->RegisterBuiltinFunction(m_owner, name, target.c_str());

        if (!handle)
        {
            throw std::runtime_error(std::string("Cannot expose built-in function: ") + name);
        }

        m_functions.push_back(handle);
    }

    template <typename FunctionType>
    void Function(const char* name, FunctionType function)
    {
        struct Binding
        {
            FunctionType function;
            CResults results;
            const PluginAPI* api;
            void* owner;

            static int Invoke(const Value* args, size_t count, Value* output, size_t capacity, void* data)
            {
                auto& self = *static_cast<Binding*>(data);
                constexpr auto arity = CCallable<FunctionType>::Count;

                if (count > arity)
                {
                    throw std::invalid_argument("Too many function arguments");
                }

                using Indices = std::make_index_sequence<arity>;

                if constexpr (std::is_void_v<decltype(InvokePlugin(self.function, args, count, Indices{}, self.api, self.owner))>)
                {
                    InvokePlugin(self.function, args, count, Indices{}, self.api, self.owner);

                    return 0;
                }
                else
                {
                    return self.results.Store(InvokePlugin(self.function, args, count, Indices{}, self.api, self.owner), output, capacity);
                }
            }
        };

        auto binding = std::make_shared<Binding>(Binding{std::move(function), {}, m_api, m_owner});

        m_bindings.push_back(binding);
        RawFunction(name, &Binding::Invoke, binding.get());
    }

    template <typename FunctionType>
    uint64_t Event(const char* name, FunctionType function)
    {
        return Listen(name, std::move(function), false);
    }

    template <typename FunctionType>
    uint64_t Once(const char* name, FunctionType function)
    {
        return Listen(name, std::move(function), true);
    }

    bool Off(uint64_t handle)
    {
        if (!m_api || !m_api->UnregisterEvent(m_owner, handle))
        {
            return false;
        }

        m_events.erase(handle);

        return true;
    }

    template <typename... Args>
    void Call(const char* name, const Args&... args)
    {
        auto call = Begin(name, args...);
    }

    template <typename Result, typename... Args>
    Result CallResult(const char* name, const Args&... args)
    {
        static_assert(!std::is_pointer_v<Result> && !std::is_reference_v<Result> &&
                          !std::is_same_v<Result, std::optional<const char*>>,
            "CallResult requires an owned result type such as std::string");
        auto call = Begin(name, args...);
        Value value = ValueNil();

        if (!m_api->GetResultValue(call.get(), 0, &value))
        {
            throw std::runtime_error("Function returned no result");
        }

        return CValues::Read<Result>(&value, 1, 0, m_api, m_owner);
    }

    template <typename FunctionType>
    uint64_t Every(uint32_t intervalMs, FunctionType function)
    {
        return Timer(intervalMs, true, std::move(function));
    }

    template <typename FunctionType>
    uint64_t After(uint32_t delayMs, FunctionType function)
    {
        return Timer(delayMs, false, std::move(function));
    }

    bool CancelTimer(uint64_t handle)
    {
        return m_api && m_api->CancelNativeTimer(m_owner, handle);
    }

    template <typename Work, typename Complete>
    uint64_t Async(Work worker, Complete complete)
    {
        RequireLoading();
        using Result = std::invoke_result_t<Work, CCancellation>;

        static_assert(!std::is_void_v<Result>, "Async workers return a value for the main-thread completion");

        struct Job
        {
            const PluginAPI* api;

            Work worker;
            Complete complete;
            std::optional<Result> result;

            static void Run(const WorkContext* context, void* data)
            {
                auto& job = *static_cast<Job*>(data);
                job.result.emplace(job.worker(CCancellation(job.api, context)));
            }

            static void Finish(void* data)
            {
                auto& job = *static_cast<Job*>(data);
                job.complete(std::move(*job.result));
            }

            static void Dispose(void* data)
            {
                delete static_cast<Job*>(data);
            }
        };

        auto job = std::make_unique<Job>(Job{m_api, std::move(worker), std::move(complete), {}});
        const auto handle = m_api->QueueWork(m_owner, &Job::Run, &Job::Finish, job.get(), &Job::Dispose);

        if (!handle)
        {
            throw std::runtime_error("Addon work queue is full or shutting down");
        }

        job.release();

        return handle;
    }

    CLog Log;
    CFiles Files;
    CConfig Config;
    CEntities Players{PLUGIN_ENTITY_PLAYER};
    CEntities NPCs{PLUGIN_ENTITY_NPC};
    CEntities Actors{PLUGIN_ENTITY_ACTOR};
    CEntities Vehicles{PLUGIN_ENTITY_VEHICLE};
    CEntities Objects{PLUGIN_ENTITY_OBJECT};

protected:
    CPlugin() = default;
    virtual bool OnLoad() = 0;

    virtual void OnUnload()
    {
    }

private:
    void RequireLoading() const
    {
        if (!m_api || m_unloading)
        {
            throw std::logic_error("Addon is not active");
        }
    }

    template <typename FunctionType>
    uint64_t Listen(const char* name, FunctionType function, bool once)
    {
        RequireLoading();

        struct Binding : std::enable_shared_from_this<Binding>
        {
            Binding(CPlugin* addon, FunctionType callback, bool onceOnly)
                : owner(addon), function(std::move(callback)), once(onceOnly)
            {
            }

            CPlugin* owner;
            FunctionType function;
            uint64_t handle = 0;
            bool once;
            bool fired = false;

            static void Invoke(const char*, const Value* args, size_t count, void* data)
            {
                auto& self = *static_cast<Binding*>(data);
                const auto keepAlive = self.shared_from_this();

                if (self.once)
                {
                    if (self.fired)
                    {
                        return;
                    }

                    self.fired = true;
                    self.owner->Off(self.handle);
                }

                // Events may gain trailing arguments; listeners select a typed prefix.
                InvokePlugin(self.function, args, count, std::make_index_sequence<CCallable<FunctionType>::Count>{}, self.owner->API(), self.owner->Handle());
            }
        };

        auto binding = std::make_shared<Binding>(this, std::move(function), once);
        const auto handle = m_api->RegisterEvent(m_owner, name, &Binding::Invoke, binding.get());

        if (!handle)
        {
            throw std::runtime_error(std::string("Cannot register event: ") + name);
        }

        binding->handle = handle;

        try
        {
            m_events.emplace(handle, binding);
        }
        catch (...)
        {
            m_api->UnregisterEvent(m_owner, handle);
            throw;
        }

        return handle;
    }

    template <typename FunctionType>
    uint64_t Timer(uint32_t interval, bool repeat, FunctionType function)
    {
        RequireLoading();

        struct Task
        {
            FunctionType function;

            static void Invoke(void* data)
            {
                static_cast<Task*>(data)->function();
            }

            static void Dispose(void* data)
            {
                delete static_cast<Task*>(data);
            }
        };

        auto task = std::make_unique<Task>(Task{std::move(function)});
        const auto handle = m_api->CreateNativeTimer(m_owner, interval, repeat, &Task::Invoke, task.get(), &Task::Dispose);

        if (!handle)
        {
            throw std::runtime_error("Cannot create addon timer");
        }

        task.release();

        return handle;
    }

    template <typename... Args>
    auto Begin(const char* name, const Args&... args)
    {
        RequireLoading();
        auto call = std::unique_ptr<CallContext, void (*)(CallContext*)>(m_api->BeginCall(m_owner, name), m_api->EndCall);

        if (!call)
        {
            throw std::runtime_error(std::string("Cannot call: ") + name);
        }

        CValueStorage storage;
        const auto push = [&](const auto& argument)
        {
            const auto value = CValues::Write(argument, storage.strings, &storage);

            if (!m_api->PushValue(call.get(), &value))
            {
                throw std::runtime_error("Cannot push function argument");
            }
        };
        (push(args), ...);

        if (!m_api->ExecuteCall(call.get()))
        {
            throw std::runtime_error(m_api->GetError(call.get()));
        }

        return call;
    }

    const PluginAPI* m_api = nullptr;
    void* m_owner = nullptr;
    bool m_unloading = false;

    std::vector<uint64_t> m_functions;
    std::map<uint64_t, std::shared_ptr<void>> m_events;
    std::vector<std::shared_ptr<void>> m_bindings;
    std::vector<std::function<void()>> m_cleanup;
};

} // namespace SDK

#define REGISTER_PLUGIN(Type) \
    static Type g_pluginInstance; \
\
    PLUGIN_EXPORT uint32_t GetPluginApiVersion(void) \
    { \
        return PLUGIN_API_VERSION; \
    } \
\
    PLUGIN_EXPORT uint64_t GetPluginSDKSignature(void) \
    { \
        return PLUGIN_SDK_SIGNATURE; \
    } \
\
    PLUGIN_EXPORT bool PluginMain(const PluginAPI* api, void* owner) \
    { \
        return g_pluginInstance.Load(api, owner); \
    } \
\
    PLUGIN_EXPORT void PluginUnload(void) \
    { \
        g_pluginInstance.Unload(); \
    }
