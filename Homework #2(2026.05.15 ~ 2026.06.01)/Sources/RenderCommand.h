#pragma once

#include <cstdint>

#include <DirectXMath.h>

#include "CameraConstants.h"

struct DrawCall final
{
	uint64_t sortKey{ 0 };
	DirectX::XMFLOAT4X4 worldMatrix{};
	const void* vertexBufferViewPtr{ nullptr };
	const void* indexBufferViewPtr{ nullptr };
	uint32_t indexCount{ 0 };
	uint32_t startIndexLocation{ 0 };
	int32_t baseVertexLocation{ 0 };
};

[[nodiscard]] constexpr uint64_t MakeRenderSortKey(
	RenderPassType passType_,
	uint32_t shaderId_,
	uint32_t materialId_) noexcept
{
	const uint64_t pass{ static_cast<uint64_t>(static_cast<uint8_t>(passType_)) & 0xFFull };
	const uint64_t shader{ static_cast<uint64_t>(shaderId_) & 0x00FF'FFFFull };
	const uint64_t material{ static_cast<uint64_t>(materialId_) & 0xFFFF'FFFFull };

	return (pass << 56) | (shader << 32) | material;
}

[[nodiscard]] constexpr RenderPassType GetRenderPassType(uint64_t sortKey_) noexcept
{
	return static_cast<RenderPassType>((sortKey_ >> 56) & 0xFFull);
}
