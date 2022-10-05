#pragma once

#if NO_LOG_OUTPUT
#define LOG_INFO(...) ((void)0)
#define LOG_INFO_SIMPLE(...) ((void)0)
#define LOG_ERROR(...) ((void)0)
#else
#include <cstdio>
#include <Windows.h>
#define LOG_INFO(f_, ...) { char buffer[1024]; sprintf_s(buffer, 1024, "INFO [%s:%d] " f_ "\n", (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__), __LINE__, ##__VA_ARGS__); printf(buffer); OutputDebugString(buffer); }
#define LOG_INFO_SIMPLE(f_, ...) { char buffer[1024]; sprintf_s(buffer, 1024, "" f_, ##__VA_ARGS__); printf(buffer); OutputDebugString(buffer); }
#define LOG_ERROR(f_, ...) { char buffer[1024]; sprintf_s(buffer, 1024, "ERROR [%s:%d] " f_ "\n", (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__), __LINE__, ##__VA_ARGS__); printf(buffer); OutputDebugString(buffer); }
#endif