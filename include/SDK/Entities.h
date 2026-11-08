#pragma once

#include "Entity.h"
#include <functional>
#include <iomanip>
#include <map>
#include <sstream>

namespace SDK
{
class CEntities
{
public:
    explicit CEntities(EntityKind kind) : m_kind(kind)
    {
    }

    void Initialize(const PluginAPI* api)
    {
        m_api = api;
    }

    CEntity Find(int id) const
    {
        return CEntity(m_api, m_kind, id);
    }

    template <typename Function>
    void ForEach(Function function) const
    {
        if (!m_api)
        {
            return;
        }

        std::vector<int> ids(m_api->CopyEntityIds(m_kind, nullptr, 0));
        const auto count = m_api->CopyEntityIds(m_kind, ids.data(), ids.size());

        if (count > ids.size())
        {
            throw std::runtime_error("Entity enumeration must run on the server thread");
        }

        std::vector<CEntity> snapshot;

        for (size_t index = 0; index < count; ++index)
        {
            snapshot.push_back(Find(ids[index]));
        }

        for (const auto& entity : snapshot)
        {
            if (entity.IsValid())
            {
                function(entity);
            }
        }
    }

private:
    EntityKind m_kind;
    const PluginAPI* m_api = nullptr;
};

} // namespace SDK
