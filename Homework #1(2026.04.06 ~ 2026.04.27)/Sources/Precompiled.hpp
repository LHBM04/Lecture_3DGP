#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <sdkddkver.h>
#include <Windows.h>

#undef RegisterClass
#undef CreateWindow

#include <DirectXCollision.h>
#include <DirectXColors.h>
#include <DirectXMath.h>

#include <array>
#include <compare>
#include <algorithm>
#include <cmath>
#include <concepts>
#include <iostream>
#include <cstdarg>
#include <limits>
#include <numbers>
#include <numeric>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <typeindex>
#include <ranges>
#include <typeinfo>
#include <random>

using Vector = DirectX::XMVECTOR;
using Matrix = DirectX::XMMATRIX;

// C#의 정적 클래스를 흉내.
#define STATIC_CLASS(type)                      \
	public:                                     \
	    type() = delete;                        \
        ~type() = delete;                       \
                                                \
        type(const type&) = delete;             \
        type& operator=(const type&) = delete;  \
                                                \
        type(type&&) = delete;                  \
        type& operator=(type&&) = delete;       \
                                                \
        void* operator new(size_t) = delete;    \
        void* operator new[](size_t) = delete;  \
        void operator delete(void*) = delete;   \
        void operator delete[](void*) = delete; 