#pragma once

#include <concepts>

#include <DirectXMath.h>

#include "Framework/Math/ColorRGBA.hpp"

namespace TUK::Framework
{
	template <IsNumeric TValue>
	[[nodiscard]] inline DirectX::XMVECTOR Load(const ColorRGBA<TValue>& source) noexcept
	{
		if constexpr (std::same_as<TValue, int>)
		{
			const DirectX::XMINT4 temp{ source.GetR(), source.GetG(), source.GetB(), source.GetA() };
			return DirectX::XMLoadSInt4(&temp);
		}
		else
		{
			const DirectX::XMFLOAT4 temp{ source.GetR(), source.GetG(), source.GetB(), source.GetA() };
			return DirectX::XMLoadFloat4(&temp);
		}
	}

	template <IsNumeric TValue>
	inline void Store(ColorRGBA<TValue>& destination, DirectX::FXMVECTOR source) noexcept
	{
		if constexpr (std::same_as<TValue, int>)
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
