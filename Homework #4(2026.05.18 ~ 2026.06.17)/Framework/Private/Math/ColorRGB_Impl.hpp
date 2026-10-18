#pragma once

#include <concepts>
#include <cstdint>

#include <DirectXMath.h>

#include "Framework/Math/ColorRGB.hpp"

namespace TUK::Framework
{
	template <IsNumeric TValue>
	[[nodiscard]] inline DirectX::XMVECTOR Load(const ColorRGB<TValue>& source) noexcept
	{
		if constexpr (std::same_as<TValue, std::int32_t>)
		{
			const DirectX::XMINT3 temp{ source.GetR(), source.GetG(), source.GetB() };
			return DirectX::XMLoadSInt3(&temp);
		}
		else
		{
			const DirectX::XMFLOAT3 temp{ source.GetR(), source.GetG(), source.GetB() };
			return DirectX::XMLoadFloat3(&temp);
		}
	}

	template <IsNumeric TValue>
	inline void Store(ColorRGB<TValue>& destination, DirectX::FXMVECTOR source) noexcept
	{
		if constexpr (std::same_as<TValue, std::int32_t>)
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
