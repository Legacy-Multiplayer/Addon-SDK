#pragma once

#include "../Types.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum ValueType
{
    PLUGIN_VALUE_NIL = 0,
    PLUGIN_VALUE_BOOLEAN = 1,
    PLUGIN_VALUE_INTEGER = 2,
    PLUGIN_VALUE_NUMBER = 3,
    PLUGIN_VALUE_STRING = 4,
    PLUGIN_VALUE_USERDATA = 5,
    PLUGIN_VALUE_TABLE = 6,
    PLUGIN_VALUE_FUNCTION = 7
} ValueType;

typedef struct TableEntry TableEntry;

typedef struct Value
{
    ValueType type;

    union
    {
        bool booleanValue;
        int64_t integerValue;
        double numberValue;
        const char* stringValue;

        void* pointerValue;
        uint64_t referenceValue;

        struct
        {
            const TableEntry* entries;
            size_t count;
        } tableValue;
    };

    // Strings are length-delimited; embedded NUL bytes are preserved.
    size_t stringLength;
} Value;

typedef struct TableEntry
{
    Value key;
    Value value;
} TableEntry;

// =====================================================================
// Value Helper Constructors
// =====================================================================

static inline Value ValueNil(void)
{
    Value value = {PLUGIN_VALUE_NIL, {false}, 0};

    value.type = PLUGIN_VALUE_NIL;
    value.integerValue = 0;

    return value;
}

static inline Value ValueBoolean(bool booleanValue)
{
    Value value = {PLUGIN_VALUE_NIL, {false}, 0};

    value.type = PLUGIN_VALUE_BOOLEAN;
    value.booleanValue = booleanValue;

    return value;
}

static inline Value ValueInteger(int64_t integerValue)
{
    Value value = {PLUGIN_VALUE_NIL, {false}, 0};

    value.type = PLUGIN_VALUE_INTEGER;
    value.integerValue = integerValue;

    return value;
}

static inline Value ValueNumber(double numberValue)
{
    Value value = {PLUGIN_VALUE_NIL, {false}, 0};

    value.type = PLUGIN_VALUE_NUMBER;
    value.numberValue = numberValue;

    return value;
}

static inline Value ValueString(const char* stringValue)
{
    Value value = {PLUGIN_VALUE_NIL, {false}, 0};

    value.type = PLUGIN_VALUE_STRING;
    value.stringValue = stringValue;
    value.stringLength = stringValue ? strlen(stringValue) : 0;

    return value;
}

static inline Value ValueStringSized(const char* data, size_t length)
{
    Value value = ValueNil();
    value.type = PLUGIN_VALUE_STRING;
    value.stringValue = data;
    value.stringLength = length;
    return value;
}

static inline Value ValueTable(const TableEntry* entries, size_t count)
{
    Value value = ValueNil();
    value.type = PLUGIN_VALUE_TABLE;
    value.tableValue.entries = entries;
    value.tableValue.count = count;
    return value;
}

static inline Value ValueCallback(uint64_t reference)
{
    Value value = ValueNil();
    value.type = PLUGIN_VALUE_FUNCTION;
    value.referenceValue = reference;
    return value;
}

static inline Value ValuePointer(void* pointerValue)
{
    Value value = {PLUGIN_VALUE_NIL, {false}, 0};

    value.type = PLUGIN_VALUE_USERDATA;
    value.pointerValue = pointerValue;

    return value;
}

// =====================================================================
// Function & Event Callback Signatures
// =====================================================================

typedef int (*FunctionCallback)(
    const Value* arguments,
    size_t argumentCount,
    Value* results,
    size_t maxResults,
    void* userData);

typedef void (*EventCallback)(
    const char* eventName,
    const Value* arguments,
    size_t argumentCount,
    void* userData);

#ifdef __cplusplus
}
#endif
