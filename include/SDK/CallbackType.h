#pragma once

#include "Table.h"
#include "API.h"

namespace SDK
{
class CCallback
{
    struct State
    {
        State(const PluginAPI* host, void* instance, uint64_t token) : api(host), owner(instance), reference(token)
        {
        }

        const PluginAPI* api;
        void* owner;
        uint64_t reference;

        ~State()
        {
            api->ReleaseLuaCallback(owner, reference);
        }
    };

public:
    CCallback() = default;

    CCallback(const PluginAPI* api, void* owner, uint64_t borrowed);

    bool IsValid() const
    {
        return m_state && m_state->api->IsLuaCallbackValid(m_state->owner, m_state->reference);
    }

    void Reset()
    {
        m_state.reset();
    }

    template <typename... Args>
    bool Invoke(const Args&... arguments) const;

private:
    std::shared_ptr<State> m_state;
};

} // namespace SDK
