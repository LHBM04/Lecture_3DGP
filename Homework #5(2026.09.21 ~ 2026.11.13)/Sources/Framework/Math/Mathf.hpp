#pragma once

#include "../Core/Common.hpp"

namespace TUK::Framework
{
	namespace Mathf
	{
		constexpr f32 Epsilon = 0.0001f;

		constexpr f32 PI = 3.141592f;

		constexpr f32 Deg2Rad = PI / 180.0f;

		constexpr f32 Rad2Deg = 180.0f / PI;

		constexpr f32 HalfPI = PI / 2;

		[[nodiscard]] constexpr f32 Clamp(f32 value, f32 min, f32 max) noexcept
		{
			return std::max(min, std::min(max, value));
		}

		[[nodiscard]] constexpr f32 Lerp(f32 start, f32 end, f32 t) noexcept
		{
			return start + t * (end - start);
		}

		[[nodiscard]] constexpr f32 SmootherStep(f32 edge0, f32 edge1, f32 x) noexcept
		{
			f32 t = Clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
			return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
		}
	}
}
