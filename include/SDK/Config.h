#pragma once

#include "Files.h"
#include <functional>
#include <iomanip>
#include <map>
#include <sstream>

namespace SDK
{
class CConfig
{
public:
    void Initialize(CFiles* files)
    {
        m_files = files;
        m_values.clear();
    }

    bool Load(const std::string& path)
    {
        if (!m_files || !m_files->Exists(path))
        {
            return false;
        }

        std::map<std::string, std::string> values;
        std::istringstream input(m_files->ReadText(path));
        std::string line;

        while (std::getline(input, line))
        {
            std::istringstream entry(line);

            entry >> std::ws;

            if (entry.eof() || entry.peek() == '#' || entry.peek() == ';')
            {
                continue;
            }

            std::string key;

            if (!std::getline(entry, key, '='))
            {
                throw std::invalid_argument("Invalid configuration line");
            }

            const auto last = key.find_last_not_of(" \t\r");

            key = last == std::string::npos ? "" : key.substr(0, last + 1);
            ValidateKey(key);
            std::string value;

            if (!(entry >> std::quoted(value)) || !(entry >> std::ws).eof() || !values.emplace(key, value).second)
            {
                throw std::invalid_argument("Invalid or duplicate configuration key: " + key);
            }
        }

        m_values.swap(values);

        return true;
    }

    void Save(const std::string& path) const
    {
        if (!m_files)
        {
            throw std::logic_error("Configuration unavailable");
        }

        std::ostringstream output;

        for (const auto& entry : m_values)
        {
            output << entry.first << " = " << std::quoted(entry.second) << '\n';
        }

        m_files->WriteText(path, output.str());
    }

    template <typename T>
    T Get(const std::string& key, const T& fallback) const
    {
        const auto found = m_values.find(key);

        if (found == m_values.end())
        {
            return fallback;
        }

        if constexpr (std::is_same_v<T, std::string>)
        {
            return found->second;
        }
        else
        {
            if constexpr (std::is_unsigned_v<T>)
            {
                const auto first = found->second.find_first_not_of(" \t");

                if (first != std::string::npos && found->second[first] == '-')
                {
                    throw std::out_of_range("Negative unsigned configuration value");
                }
            }

            T value{};
            std::istringstream input(found->second);

            if constexpr (std::is_same_v<T, bool>)
            {
                input >> std::boolalpha;
            }

            if (!(input >> value) || !(input >> std::ws).eof())
            {
                throw std::invalid_argument("Invalid configuration value: " + key);
            }

            if constexpr (std::is_floating_point_v<T>)
            {
                if (!std::isfinite(value))
                {
                    throw std::invalid_argument("Non-finite configuration value");
                }
            }

            return value;
        }
    }

    template <typename T>
    void Set(const std::string& key, const T& value)
    {
        ValidateKey(key);
        std::ostringstream output;

        output << std::boolalpha << value;
        const auto text = output.str();

        if (text.find_first_of("\r\n") != std::string::npos || text.find('\0') != std::string::npos)
        {
            throw std::invalid_argument("Configuration values must fit one line");
        }

        m_values[key] = text;
    }

private:
    static void ValidateKey(const std::string& key)
    {
        if (key.empty() || key.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789._-") != std::string::npos)
        {
            throw std::invalid_argument("Invalid configuration key");
        }
    }

    CFiles* m_files = nullptr;
    std::map<std::string, std::string> m_values;
};

} // namespace SDK
