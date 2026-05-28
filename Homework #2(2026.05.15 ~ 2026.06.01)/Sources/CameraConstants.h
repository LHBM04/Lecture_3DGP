#pragma once

#include <array>
#include <cstdint>

#include <DirectXMath.h>

enum class RenderPassType : uint8_t
{
	Opaque3D = 0,
	Transparent3D = 1,
	UI = 2,
};

enum class CameraClearFlags : uint8_t
{
	SolidColor,
	OnlyDepth,
	None,
};

struct CameraViewport final
{
	float topLeftX{ 0.0f };
	float topLeftY{ 0.0f };
	float width{ 1.0f };
	float height{ 1.0f };
	float minDepth{ 0.0f };
	float maxDepth{ 1.0f };
};

struct CameraConstants final
{
	RenderPassType passType{ RenderPassType::Opaque3D };
	CameraClearFlags clearFlags{ CameraClearFlags::SolidColor };
	DirectX::XMFLOAT4 backgroundColor{ 0.0f, 0.0f, 0.0f, 1.0f };
	CameraViewport viewport{};
	DirectX::XMFLOAT4X4 viewProj{};
	DirectX::XMFLOAT3 position{ 0.0f, 0.0f, 0.0f };
};
