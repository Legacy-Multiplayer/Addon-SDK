#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
#define PLUGIN_LINKAGE extern "C"
#else
#define PLUGIN_LINKAGE extern
#endif

#if defined(_WIN32) || defined(WIN32)
#define PLUGIN_EXPORT PLUGIN_LINKAGE __declspec(dllexport)
#else
#define PLUGIN_EXPORT PLUGIN_LINKAGE __attribute__((visibility("default")))
#endif

#define PLUGIN_API_VERSION 1
