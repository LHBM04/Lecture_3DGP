#pragma once

#include <concepts>

#include <DirectXMath.h>

#include "Framework/Math/Vector2D.hpp"

namespace TUK::Framework
{
	template <IsNumeric TValue>
	[[nodiscard]] inline DirectX::XMVECTOR Load(const Vector2D<TValue>& source) noexcept
	{
		if constexpr (std::same_as<TValue, int>)
		{
			const DirectX::XMINT2 temp{ source.GetX(), source.GetY() };
			return DirectX::XMLoadSInt2(&temp);
		}
		else
		{
			const DirectX::XMFLOAT2 temp{ source.GetX(), source.GetY() };
			return DirectX::XMLoadFloat2(&temp);
		}
	}

	template <IsNumeric TValue>
	inline void Store(Vector2D<TValue>& destination, DirectX::FXMVECTOR source) noexcept
	{
		if constexpr (std::same_as<TValue, int>)
		{
			DirectX::XMINT2 temp{};
			DirectX::XMStoreSInt2(&temp, source);
			destination.Set(temp.x, temp.y);
		}
		else
		{
			DirectX::XMFLOAT2 temp{};
			DirectX::XMStoreFloat2(&temp, source);
			destination.Set(temp.x, temp.y);
		}
	}
}
