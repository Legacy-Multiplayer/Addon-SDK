#pragma once

#include "Value.h"
#include <functional>
#include <iomanip>
#include <map>
#include <sstream>

namespace SDK
{
class CLog
{
public:
    void Initialize(const PluginAPI* api, void* owner)
    {
        m_api = api;
        m_owner = owner;
    }

    template <typename... Args>
    void Info(const std::string& format, const Args&... args) const
    {
        Write(PLUGIN_LOG_INFO, format, args...);
    }

    template <typename... Args>
    void Warning(const std::string& format, const Args&... args) const
    {
        Write(PLUGIN_LOG_WARN, format, args...);
    }

    template <typename... Args>
    void Error(const std::string& format, const Args&... args) const
    {
        Write(PLUGIN_LOG_ERROR, format, args...);
    }

private:
    template <typename... Args>
    void Write(LogLevel level, const std::string& format, const Args&... args) const
    {
        if (!m_api)
        {
            return;
        }

        std::ostringstream output;
        size_t position = 0;
        const auto append = [&](const auto& value)
        {
            const auto marker = format.find("{}", position);

            if (marker == std::string::npos)
            {
                throw std::invalid_argument("Log format has too few placeholders");
            }

            output << format.substr(position, marker - position) << value;
            position = marker + 2;
        };
        (append(args), ...);
        output << format.substr(position);
        m_api->Log(m_owner, level, output.str().c_str());
    }

    const PluginAPI* m_api = nullptr;
    void* m_owner = nullptr;
};

// Resource-relative UTF-8 text operations; binary assets belong to the asset API.

} // namespace SDK
