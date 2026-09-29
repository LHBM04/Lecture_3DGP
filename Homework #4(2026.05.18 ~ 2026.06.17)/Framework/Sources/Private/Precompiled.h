#pragma once

#include <cstdlib>
#include <format>
#include <iostream>
#include <print>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>

#include <d3d12.h>
#include <dxgi1_6.h>

#ifdef NDEBUG
	#define ASSERT(condition, message) (static_cast<void>(0))
#else
    #define ASSERT(condition, message) \
            do { \
                if (!(condition)) { \
                    std::wcerr << L"Assertion failed: " << #condition \
                               << L", message: " << message \
                               << L", file: " << __FILE__ \
                               << L", line: " << __LINE__ << std::endl; \
                    std::abort(); \
                } \
            } while (false)
#endif
