#pragma once

#include <algorithm>
#include <limits>
#include <numbers>
#include <numeric>

namespace TUK::Framework
{
	namespace Mathf
	{
		/** 앱실론 */
		constexpr float Epsilon = 0.0001f;

		/** 원주율 */
		constexpr float PI = 3.141592f;

		/** Degree -> Radian */
		constexpr float Deg2Rad = PI / 180.0f;

		/** Radian -> Degree */
		constexpr float Rad2Deg = 180.0f / PI;

		/** 원주율 / 2 */
		constexpr float HalfPI = PI / 2;

		/** 값을 범위 내로 고정 */
		[[nodiscard]] constexpr float Clamp(float value, float min, float max) noexcept
		{
			return std::max(min, std::min(max, value));
		}
		
		/** 선형 보간 */
		[[nodiscard]] constexpr float Lerp(float start, float end, float t) noexcept
		{
			return start + t * (end - start);
		}

		/** 부드러운 보간 */
		[[nodiscard]] constexpr float SmootherStep(float edge0, float edge1, float x) noexcept
		{
			float t = Clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
			return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
		}
	}
}
