#pragma once

#include "Types.h"
#include <cmath>
#include <deque>
#include <limits>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

namespace SDK
{
class CTable;

// Owned, Lua-independent data. Nested tables are immutable shared snapshots.
class CValue
{
public:
    using Data = std::variant<std::monostate, bool, int64_t, double, std::string, std::shared_ptr<const CTable>>;

    CValue() = default;

    CValue(bool value) : m_data(value)
    {
    }

    template <typename T, std::enable_if_t<std::is_integral_v<T> && !std::is_same_v<T, bool>, int> = 0>
    CValue(T value)
    {
        if constexpr (std::is_unsigned_v<T>)
        {
            if (value > static_cast<uint64_t>(std::numeric_limits<int64_t>::max()))
            {
                throw std::out_of_range("Lua integer out of range");
            }
        }

        m_data = static_cast<int64_t>(value);
    }

    CValue(double value) : m_data(value)
    {
        if (!std::isfinite(value))
        {
            throw std::invalid_argument("Non-finite addon number");
        }
    }

    CValue(std::string value) : m_data(std::move(value))
    {
    }

    CValue(const char* value) : m_data(std::string(value ? value : ""))
    {
    }

    CValue(const CTable& value);

    bool IsNil() const
    {
        return std::holds_alternative<std::monostate>(m_data);
    }

    template <typename T>
    T As() const;

    const Data& GetData() const
    {
        return m_data;
    }

private:
    Data m_data;
};

class CTable
{
public:
    using Key = std::variant<int64_t, std::string>;
    using Entries = std::map<Key, CValue>;

    void Set(std::string key, CValue value)
    {
        m_entries.insert_or_assign(std::move(key), std::move(value));
    }

    void Set(int64_t key, CValue value)
    {
        m_entries.insert_or_assign(key, std::move(value));
    }

    bool Contains(const std::string& key) const
    {
        return m_entries.count(key) != 0;
    }

    bool Contains(int64_t key) const
    {
        return m_entries.count(key) != 0;
    }

    template <typename T, typename KeyType>
    T Get(const KeyType& key) const
    {
        return m_entries.at(Key(key)).template As<T>();
    }

    template <typename T, typename KeyType>
    T Get(const KeyType& key, T fallback) const
    {
        const auto found = m_entries.find(Key(key));
        return found == m_entries.end() ? std::move(fallback) : found->second.template As<T>();
    }

    size_t Size() const
    {
        return m_entries.size();
    }

    const Entries& GetEntries() const
    {
        return m_entries;
    }

    static CTable Array(std::initializer_list<CValue> values)
    {
        CTable table;
        int64_t index = 1;
        for (const auto& value : values)
        {
            table.Set(index++, value);
        }

        return table;
    }

private:
    Entries m_entries;
};

inline CValue::CValue(const CTable& value) : m_data(std::make_shared<const CTable>(value))
{
}

template <typename T>
T CValue::As() const
{
    static_assert(!std::is_pointer_v<T> && !std::is_reference_v<T>, "Use an owned result type");
    if constexpr (std::is_same_v<T, CTable>)
    {
        return *std::get<std::shared_ptr<const CTable>>(m_data);
    }
    else if constexpr (std::is_same_v<T, CValue>)
    {
        return *this;
    }
    else if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, bool>)
    {
        return std::get<T>(m_data);
    }
    else if constexpr (std::is_integral_v<T>)
    {
        int64_t value;
        if (const auto* integer = std::get_if<int64_t>(&m_data))
        {
            value = *integer;
        }
        else
        {
            const double number = std::get<double>(m_data);
            if (!std::isfinite(number) || std::trunc(number) != number || number < -9223372036854775808.0 || number >= 9223372036854775808.0)
            {
                throw std::out_of_range("Expected an integer");
            }
            value = static_cast<int64_t>(number);
        }
        if constexpr (std::is_unsigned_v<T>)
        {
            if (value < 0 || static_cast<uint64_t>(value) > std::numeric_limits<T>::max())
            {
                throw std::out_of_range("Integer out of range");
            }
        }
        else
        {
            if (value < std::numeric_limits<T>::min() || value > std::numeric_limits<T>::max())
            {
                throw std::out_of_range("Integer out of range");
            }
        }
        return static_cast<T>(value);
    }
    else if constexpr (std::is_floating_point_v<T>)
    {
        const auto* integer = std::get_if<int64_t>(&m_data);
        const double value = integer ? static_cast<double>(*integer) : std::get<double>(m_data);
        if (!std::isfinite(value) || value < -std::numeric_limits<T>::max() || value > std::numeric_limits<T>::max())
        {
            throw std::out_of_range("Number out of range");
        }
        return static_cast<T>(value);
    }
    else
    {
        static_assert(!sizeof(T), "Unsupported owned value conversion");
    }
}

} // namespace SDK
