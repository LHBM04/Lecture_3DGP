#pragma once

#include <algorithm>
#include <cmath>
#include <limits>
#include <numbers>

namespace Mathf
{
	constexpr float PI{ std::numbers::pi_v<float> };

	constexpr float EPSILON{ std::numeric_limits<float>::epsilon() };

	constexpr float DEG2RAD{ PI / 180.0f };
	constexpr float RAD2DEG{ 180.0f / PI };

	[[nodiscard]] constexpr float Clamp(float value_, float min_, float max_) noexcept
	{
		return std::max(min_, std::min(max_, value_));
	}

	[[nodiscard]] constexpr float Lerp(float start_, float end_, float t_) noexcept
	{
		return start_ + t_ * (end_ - start_);
	}

	[[nodiscard]] constexpr float SmootherStep(float edge0_, float edge1_, float x_) noexcept
	{
		const float t{ Clamp((x_ - edge0_) / (edge1_ - edge0_), 0.0f, 1.0f) };
		return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
	}
}
