#pragma once

#include "Value.h"

namespace SDK
{
inline CCallback::CCallback(const PluginAPI* api, void* owner, uint64_t borrowed)
{
    if (!api || !api->RetainLuaCallback || !api->ReleaseLuaCallback)
    {
        throw std::runtime_error("Lua callback services are unavailable");
    }

    const auto retained = api->RetainLuaCallback(owner, borrowed);
    if (!retained)
    {
        throw std::runtime_error("Cannot retain this Lua callback");
    }

    try
    {
        m_state = std::make_shared<State>(api, owner, retained);
    }
    catch (...)
    {
        api->ReleaseLuaCallback(owner, retained);
        throw;
    }
}

template <typename... Args>
bool CCallback::Invoke(const Args&... arguments) const
{
    const auto state = m_state;
    if (!state || !IsValid())
    {
        return false;
    }

    CValueStorage storage;
    std::vector<Value> values;
    values.reserve(sizeof...(Args));
    (values.push_back(CValues::Write(arguments, storage.strings, &storage)), ...);
    return state->api->InvokeLuaCallback(state->owner, state->reference, values.data(), values.size());
}

} // namespace SDK
