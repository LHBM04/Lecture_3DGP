#pragma once

#include <concepts>

#include <DirectXMath.h>

#include "Framework/Math/Vector3D.hpp"

namespace TUK::Framework
{
	template <IsNumeric TValue>
	[[nodiscard]] inline DirectX::XMVECTOR Load(const Vector3D<TValue>& source) noexcept
	{
		if constexpr (std::same_as<TValue, int>)
		{
			const DirectX::XMINT3 temp{ source.GetX(), source.GetY(), source.GetZ() };
			return DirectX::XMLoadSInt3(&temp);
		}
		else
		{
			const DirectX::XMFLOAT3 temp{ source.GetX(), source.GetY(), source.GetZ() };
			return DirectX::XMLoadFloat3(&temp);
		}
	}

	template <IsNumeric TValue>
	inline void Store(Vector3D<TValue>& destination, DirectX::FXMVECTOR source) noexcept
	{
		if constexpr (std::same_as<TValue, int>)
		{
			DirectX::XMINT3 temp{};
			DirectX::XMStoreSInt3(&temp, source);
			destination.Set(temp.x, temp.y, temp.z);
		}
		else
		{
			DirectX::XMFLOAT3 temp{};
			DirectX::XMStoreFloat3(&temp, source);
			destination.Set(temp.x, temp.y, temp.z);
		}
	}
}
