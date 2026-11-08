#pragma once

#include "CallbackType.h"
#include <algorithm>
#include <optional>
#include <tuple>
#include <utility>

namespace SDK
{
// Stable storage for length-delimited strings and recursive C table views.
struct CValueStorage
{
    std::deque<std::string> strings;
    std::deque<std::vector<TableEntry>> tables;
    size_t nodes = 0;
    size_t bytes = 0;

    void Clear()
    {
        strings.clear();
        tables.clear();
        nodes = 0;
        bytes = 0;
    }

    void Account(size_t length = 0)
    {
        if (++nodes > 8192 || length > 4 * 1024 * 1024 - bytes)
        {
            throw std::length_error("Addon value exceeds 8192 nodes or 4 MiB");
        }

        bytes += length;
    }

    Value Write(const CValue& value, size_t depth = 0)
    {
        if (depth > 16)
        {
            throw std::length_error("Addon table nesting exceeds 16 levels");
        }

        Account();
        return std::visit([this, depth](const auto& item) -> Value
        {
            using Type = std::decay_t<decltype(item)>;
            if constexpr (std::is_same_v<Type, std::monostate>)
            {
                return ValueNil();
            }
            else if constexpr (std::is_same_v<Type, bool>)
            {
                return ValueBoolean(item);
            }
            else if constexpr (std::is_same_v<Type, int64_t>)
            {
                return ValueInteger(item);
            }
            else if constexpr (std::is_same_v<Type, double>)
            {
                return ValueNumber(item);
            }
            else if constexpr (std::is_same_v<Type, std::string>)
            {
                Account(item.size());
                strings.push_back(item);
                return ValueStringSized(strings.back().data(), strings.back().size());
            }
            else
            {
                if (!item || item->Size() > 4096)
                {
                    throw std::length_error("Addon table exceeds 4096 entries");
                }

                tables.emplace_back(item->Size());
                auto& table = tables.back();
                size_t index = 0;
                for (const auto& entry : item->GetEntries())
                {
                    table[index].key = std::visit([this, depth](const auto& key)
                    {
                        return Write(CValue(key), depth + 1);
                    }, entry.first);
                    table[index].value = Write(entry.second, depth + 1);
                    ++index;
                }

                return ValueTable(table.data(), table.size());
            }
        }, value.GetData());
    }

    CValue Read(const Value& value, size_t depth = 0)
    {
        if (depth > 16)
        {
            throw std::length_error("Addon table nesting exceeds 16 levels");
        }

        Account();
        switch (value.type)
        {
        case PLUGIN_VALUE_NIL:
            return {};

        case PLUGIN_VALUE_BOOLEAN:
            return value.booleanValue;

        case PLUGIN_VALUE_INTEGER:
            return value.integerValue;

        case PLUGIN_VALUE_NUMBER:
            return value.numberValue;

        case PLUGIN_VALUE_STRING:
            Account(value.stringLength);
            if (!value.stringValue && value.stringLength)
            {
                throw std::invalid_argument("Invalid addon string");
            }

            return std::string(value.stringValue ? value.stringValue : "", value.stringLength);

        case PLUGIN_VALUE_TABLE:
        {
            if (value.tableValue.count > 4096 || (!value.tableValue.entries && value.tableValue.count))
            {
                throw std::invalid_argument("Invalid addon table");
            }

            CTable table;
            for (size_t index = 0; index < value.tableValue.count; ++index)
            {
                const auto& entry = value.tableValue.entries[index];
                auto item = Read(entry.value, depth + 1);
                if (entry.key.type == PLUGIN_VALUE_INTEGER)
                {
                    table.Set(entry.key.integerValue, std::move(item));
                }
                else if (entry.key.type == PLUGIN_VALUE_STRING)
                {
                    table.Set(std::get<std::string>(Read(entry.key, depth + 1).GetData()), std::move(item));
                }
                else
                {
                    throw std::invalid_argument("Table keys must be integers or strings");
                }
            }

            return table;
        }

        default:
            throw std::invalid_argument("Data tables cannot contain functions, userdata or threads");
        }
    }
};

// All typed bindings use the same checked conversions. Values are never silently
// truncated to IDs or converted from strings to numbers.
class CValues
{
    template <typename T>
    struct Optional : std::false_type
    {
    };

    template <typename T>
    struct Optional<std::optional<T>> : std::true_type
    {
        using Type = T;
    };

public:
    template <typename T>
    static std::decay_t<T> Read(const Value* values, size_t count, size_t index, const PluginAPI* api = nullptr, void* owner = nullptr)
    {
        using Type = std::decay_t<T>;

        if constexpr (Optional<Type>::value)
        {
            if (!values || index >= count || values[index].type == PLUGIN_VALUE_NIL)
            {
                return std::nullopt;
            }

            return Read<typename Optional<Type>::Type>(values, count, index, api, owner);
        }
        else
        {
            if (!values || index >= count)
            {
                throw std::invalid_argument("Missing argument " + std::to_string(index + 1));
            }

            const auto& value = values[index];

            if constexpr (std::is_same_v<Type, bool>)
            {
                if (value.type == PLUGIN_VALUE_BOOLEAN)
                {
                    return value.booleanValue;
                }
            }
            else if constexpr (std::is_integral_v<Type>)
            {
                int64_t integer = 0;

                if (value.type == PLUGIN_VALUE_INTEGER)
                {
                    integer = value.integerValue;
                }
                else if (value.type == PLUGIN_VALUE_NUMBER && std::isfinite(value.numberValue) &&
                         std::trunc(value.numberValue) == value.numberValue &&
                         value.numberValue >= -9223372036854775808.0 && value.numberValue < 9223372036854775808.0)
                {
                    integer = static_cast<int64_t>(value.numberValue);
                }
                else
                {
                    throw std::invalid_argument("Expected an integer at argument " + std::to_string(index + 1));
                }

                if constexpr (std::is_unsigned_v<Type>)
                {
                    if (integer < 0 || static_cast<uint64_t>(integer) > std::numeric_limits<Type>::max())
                    {
                        throw std::out_of_range("Unsigned integer argument out of range");
                    }
                }
                else if (integer < std::numeric_limits<Type>::min() || integer > std::numeric_limits<Type>::max())
                {
                    throw std::out_of_range("Integer argument out of range");
                }

                return static_cast<Type>(integer);
            }
            else if constexpr (std::is_floating_point_v<Type>)
            {
                if (value.type != PLUGIN_VALUE_INTEGER && value.type != PLUGIN_VALUE_NUMBER)
                {
                    throw std::invalid_argument("Expected a number");
                }

                const auto number = value.type == PLUGIN_VALUE_INTEGER ? static_cast<double>(value.integerValue) : value.numberValue;

                if ((value.type == PLUGIN_VALUE_INTEGER || value.type == PLUGIN_VALUE_NUMBER) &&
                    std::isfinite(number) && number >= -std::numeric_limits<Type>::max() && number <= std::numeric_limits<Type>::max())
                {
                    return static_cast<Type>(number);
                }
            }
            else if constexpr (std::is_same_v<Type, CTable> || std::is_same_v<Type, CValue>)
            {
                CValueStorage storage;
                auto copied = storage.Read(value);
                if constexpr (std::is_same_v<Type, CTable>)
                {
                    return copied.As<CTable>();
                }
                else
                {
                    return copied;
                }
            }
            else if constexpr (std::is_same_v<Type, CCallback>)
            {
                if (value.type != PLUGIN_VALUE_FUNCTION)
                {
                    throw std::invalid_argument("Expected a Lua callback");
                }

                return CCallback(api, owner, value.referenceValue);
            }
            else if constexpr (std::is_same_v<Type, const char*>)
            {
                if (value.type == PLUGIN_VALUE_STRING && value.stringValue)
                {
                    return value.stringValue;
                }
            }
            else if constexpr (std::is_same_v<Type, std::string>)
            {
                if (value.type == PLUGIN_VALUE_STRING && value.stringValue)
                {
                    return std::string(value.stringValue, value.stringLength);
                }
            }
            else
            {
                static_assert(!sizeof(Type), "Unsupported addon argument type; use scalar, table, value, callback or optional");
            }

            if constexpr ((!std::is_integral_v<Type> || std::is_same_v<Type, bool>) &&
                          !std::is_same_v<Type, CTable> && !std::is_same_v<Type, CValue> && !std::is_same_v<Type, CCallback>)
            {
                throw std::invalid_argument("Wrong type at argument " + std::to_string(index + 1));
            }
        }
    }

    template <typename T>
    static Value Write(const T& value, std::deque<std::string>& strings, CValueStorage* storage = nullptr)
    {
        using Type = std::decay_t<T>;

        if constexpr (Optional<Type>::value)
        {
            return value ? Write(*value, strings, storage) : ValueNil();
        }
        else if constexpr (std::is_same_v<Type, bool>)
        {
            return ValueBoolean(value);
        }
        else if constexpr (std::is_integral_v<Type>)
        {
            if constexpr (std::is_unsigned_v<Type>)
            {
                if (value > static_cast<uint64_t>(std::numeric_limits<int64_t>::max()))
                {
                    throw std::out_of_range("Lua integer result out of range");
                }
            }

            return ValueInteger(static_cast<int64_t>(value));
        }
        else if constexpr (std::is_floating_point_v<Type>)
        {
            if (!std::isfinite(value))
            {
                throw std::invalid_argument("Non-finite addon result");
            }

            return ValueNumber(value);
        }
        else if constexpr (std::is_same_v<Type, CTable> || std::is_same_v<Type, CValue>)
        {
            if (!storage)
            {
                throw std::invalid_argument("Complex values require owned value storage");
            }

            return storage->Write(CValue(value));
        }
        else if constexpr (std::is_convertible_v<Type, std::string>)
        {
            strings.emplace_back(value);

            return ValueStringSized(strings.back().data(), strings.back().size());
        }
        else
        {
            static_assert(!sizeof(Type), "Unsupported addon result type");
        }
    }
};

template <typename T>
struct CCallable : CCallable<decltype(&T::operator())>
{
};

template <typename R, typename... Args>
struct CCallable<R (*)(Args...)>
{
    using Arguments = std::tuple<Args...>;
    static constexpr size_t Count = sizeof...(Args);
};

template <typename C, typename R, typename... Args>
struct CCallable<R (C::*)(Args...) const> : CCallable<R (*)(Args...)>
{
};

template <typename C, typename R, typename... Args>
struct CCallable<R (C::*)(Args...)> : CCallable<R (*)(Args...)>
{
};

template <typename Function, size_t... I>
static decltype(auto) InvokePlugin(Function& function, const Value* args, size_t count, std::index_sequence<I...>, const PluginAPI* api = nullptr, void* owner = nullptr)
{
    using Arguments = typename CCallable<Function>::Arguments;

    return function(CValues::Read<std::tuple_element_t<I, Arguments>>(args, count, I, api, owner)...);
}

class CResults
{
public:
    template <typename T>
    int Store(T&& result, Value* output, size_t capacity)
    {
        m_storage.Clear();
        m_values.clear();
        Append(std::forward<T>(result));

        if (!output || m_values.size() > capacity)
        {
            throw std::length_error("Too many addon results");
        }

        std::copy(m_values.begin(), m_values.end(), output);

        return static_cast<int>(m_values.size());
    }

private:
    template <typename T>
    void Append(const T& value)
    {
        m_values.push_back(CValues::Write(value, m_storage.strings, &m_storage));
    }

    template <typename... T>
    void Append(const std::tuple<T...>& values)
    {
        std::apply([this](const auto&... value)
        {
            (Append(value), ...);
        }, values);
    }

    CValueStorage m_storage;
    std::vector<Value> m_values;
};

} // namespace SDK
