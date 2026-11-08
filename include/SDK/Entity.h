#pragma once

#include "API.h"
#include "Value.h"
#include <functional>
#include <iomanip>
#include <map>
#include <sstream>

namespace SDK
{
class CEntity
{
public:
    CEntity(const PluginAPI* api, EntityKind kind, int id)
        : m_api(api), m_kind(kind), m_id(id), m_generation(api ? api->GetEntityGeneration(kind, id) : 0)
    {
    }

    bool IsValid() const
    {
        return m_api && m_generation != 0 && m_api->GetEntityGeneration(m_kind, m_id) == m_generation;
    }

    int Id() const
    {
        if (!IsValid())
        {
            throw std::runtime_error("Entity no longer exists");
        }

        return m_id;
    }

private:
    const PluginAPI* m_api;

    EntityKind m_kind;
    int m_id;
    uint32_t m_generation;
};

} // namespace SDK
