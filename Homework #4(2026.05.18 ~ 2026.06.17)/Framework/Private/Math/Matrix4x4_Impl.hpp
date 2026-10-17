#pragma once

#include <DirectXMath.h>

#include "Framework/Math/Matrix4x4.hpp"

namespace TUK::Framework
{
	[[nodiscard]] inline DirectX::XMMATRIX Load(const Matrix4x4<float>& source) noexcept
	{
		const DirectX::XMFLOAT4X4 temp(
			source[0],  source[1],  source[2],  source[3],
			source[4],  source[5],  source[6],  source[7],
			source[8],  source[9],  source[10], source[11],
			source[12], source[13], source[14], source[15]);
		return DirectX::XMLoadFloat4x4(&temp);
	}

	inline void Store(Matrix4x4<float>& destination, DirectX::CXMMATRIX source) noexcept
	{
		DirectX::XMFLOAT4X4 temp{};
		DirectX::XMStoreFloat4x4(&temp, source);
		destination = Matrix4x4<float>(&temp.m[0][0]);
	}
}
