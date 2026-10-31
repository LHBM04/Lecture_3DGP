#pragma once

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <cmath>
#include <compare>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <expected>
#include <filesystem>
#include <format>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <limits>
#include <memory>
#include <memory_resource>
#include <numbers>
#include <optional>
#include <print>
#include <ranges>
#include <span>
#include <stdfloat>
#include <string>
#include <string_view>
#include <tchar.h>
#include <tuple>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>

#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <windowsx.h>

#include <shellapi.h>

#include <wrl.h>

#ifndef _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC
#endif

#include <crtdbg.h>

#include <directx/d3d12.h>
#include <directx/d3d12sdklayers.h>
#include <directx/d3dx12.h>

#include <d3dcompiler.h>

#include <dxgi1_6.h>
#include <dxgidebug.h>

#include <directxmath/DirectXCollision.h>
#include <directxmath/DirectXColors.h>
#include <directxmath/DirectXMath.h>
#include <directxmath/DirectXPackedVector.h>

#include <directxtk12/Audio.h>
#include <directxtk12/BufferHelpers.h>
#include <directxtk12/CommonStates.h>
#include <directxtk12/DDSTextureLoader.h>
#include <directxtk12/DescriptorHeap.h>
#include <directxtk12/DirectXHelpers.h>
#include <directxtk12/EffectPipelineStateDescription.h>
#include <directxtk12/Effects.h>
#include <directxtk12/GamePad.h>
#include <directxtk12/GeometricPrimitive.h>
#include <directxtk12/GraphicsMemory.h>
#include <directxtk12/Keyboard.h>
#include <directxtk12/Model.h>
#include <directxtk12/Mouse.h>
#include <directxtk12/PostProcess.h>
#include <directxtk12/PrimitiveBatch.h>
#include <directxtk12/RenderTargetState.h>
#include <directxtk12/ResourceUploadBatch.h>
#include <directxtk12/ScreenGrab.h>
#include <directxtk12/SimpleMath.h>
#include <directxtk12/SpriteBatch.h>
#include <directxtk12/SpriteFont.h>
#include <directxtk12/VertexTypes.h>
#include <directxtk12/WICTextureLoader.h>
