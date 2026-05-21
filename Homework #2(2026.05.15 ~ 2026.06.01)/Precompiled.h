#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <compare>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <functional>
#include <limits>
#include <map>
#include <memory>
#include <numbers>
#include <ranges>
#include <set>
#include <span>
#include <sstream>
#include <string>
#include <string_view>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <windowsx.h>
 
#include <wrl.h>

#include <d3d12.h>
#include <d3dcompiler.h>
#include <dxgi1_6.h>

#include <DirectXCollision.h>
#include <DirectXColors.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

#include <xaudio2.h>

#define STATIC_CLASS(type)                      \
public:                                         \
	type() = delete;                            \
	~type() = delete;                           \
                                                \
	type(const type&) = delete;                 \
	type& operator=(const type&) = delete;      \
                                                \
	type(type&&) = delete;                      \
	type& operator=(type&&) = delete;           \
                                                \
	void* operator new(size_t) = delete;        \
	void* operator new[](size_t) = delete;      \
	void operator delete(void*) = delete;       \
	void operator delete[](void*) = delete
