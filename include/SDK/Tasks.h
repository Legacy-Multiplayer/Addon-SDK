#pragma once

#include "API.h"

namespace SDK
{
class CCancellation
{
public:
    CCancellation(const PluginAPI* api, const WorkContext* context) : m_api(api), m_context(context)
    {
    }

    bool IsCancelled() const
    {
        return m_api->IsWorkCancelled(m_context);
    }

private:
    const PluginAPI* m_api;
    const WorkContext* m_context;
};

} // namespace SDK
