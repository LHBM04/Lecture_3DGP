#pragma once

#include <concepts>
#include <cstdint>

#include <DirectXMath.h>

#include "Framework/Math/Quaternion.hpp"

namespace TUK::Framework
{
	template <IsNumeric TValue>
	[[nodiscard]] inline DirectX::XMVECTOR Load(const Quaternion<TValue>& source) noexcept
	{
		if constexpr (std::same_as<TValue, std::int32_t>)
		{
			const DirectX::XMINT4 temp{ source.GetX(), source.GetY(), source.GetZ(), source.GetW() };
			return DirectX::XMLoadSInt4(&temp);
		}
		else
		{
			const DirectX::XMFLOAT4 temp{ source.GetX(), source.GetY(), source.GetZ(), source.GetW() };
			return DirectX::XMLoadFloat4(&temp);
		}
	}

	template <IsNumeric TValue>
	inline void Store(Quaternion<TValue>& destination, DirectX::FXMVECTOR source) noexcept
	{
		if constexpr (std::same_as<TValue, std::int32_t>)
		{
			DirectX::XMINT4 temp{};
			DirectX::XMStoreSInt4(&temp, source);
			destination.Set(temp.x, temp.y, temp.z, temp.w);
		}
		else
		{
			DirectX::XMFLOAT4 temp{};
			DirectX::XMStoreFloat4(&temp, source);
			destination.Set(temp.x, temp.y, temp.z, temp.w);
		}
	}
}
