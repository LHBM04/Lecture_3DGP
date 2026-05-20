#pragma once

#include <algorithm>
#include <cmath>
#include <compare>
#include <cstddef>
#include <limits>
#include <numbers>

#include <DirectXMath.h>

namespace Mathf
{
    constexpr float PI{ std::numbers::pi_v<float> };

    constexpr float EPSILON{ std::numeric_limits<float>::epsilon() };

    constexpr float DEG2RAD{ PI / 180.0f };
    constexpr float RAD2DEG{ 180.0f / PI };

    [[nodiscard]] constexpr float Clamp(float _value, float _min, float _max) noexcept
    {
        return std::max(_min, std::min(_max, _value));
    }

    [[nodiscard]] constexpr float Lerp(float _start, float _end, float _t) noexcept
    {
        return _start + _t * (_end - _start);
    }

    [[nodiscard]] constexpr float SmootherStep(float _edge0, float _edge1, float _x) noexcept
    {
        float t{ Clamp((_x - _edge0) / (_edge1 - _edge0), 0.0f, 1.0f) };
        return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
    }
}
