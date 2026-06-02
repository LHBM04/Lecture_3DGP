#pragma once

#include <DirectXMath.h>
#include <iostream>

#include "MathF.h"

class ColorRGB;
class Vector3D;
class Vector4D;

class ColorRGBA : public DirectX::XMFLOAT4
{
public:
    ColorRGBA() noexcept;
    explicit ColorRGBA(float value_) noexcept;
    ColorRGBA(float r_, float g_, float b_, float a_ = 1.0f) noexcept;
    explicit ColorRGBA(const float* values_) noexcept;
    ColorRGBA(const ColorRGB& rgb_, float a_ = 1.0f) noexcept;
	explicit ColorRGBA(const Vector3D& vector_, float alpha_ = 1.0f) noexcept;
    ColorRGBA(const Vector4D& vector_) noexcept;

	ColorRGBA(const ColorRGBA& other_) noexcept = default;
	ColorRGBA(ColorRGBA&& other_) noexcept = default;

    ColorRGBA& operator=(const ColorRGBA& other_) noexcept;
    ColorRGBA& operator=(ColorRGBA&& other_) noexcept;
    ColorRGBA& operator=(const Vector4D& other_) noexcept;

    bool operator==(const ColorRGBA& other_) const noexcept;
    bool operator!=(const ColorRGBA& other_) const noexcept;

    ColorRGBA operator+(const ColorRGBA& other_) const noexcept;
    ColorRGBA& operator+=(const ColorRGBA& other_) noexcept;
    ColorRGBA operator-(const ColorRGBA& other_) const noexcept;
    ColorRGBA& operator-=(const ColorRGBA& other_) noexcept;
    ColorRGBA operator*(const ColorRGBA& other_) const noexcept;
    ColorRGBA& operator*=(const ColorRGBA& other_) noexcept;
    ColorRGBA operator*(float scalar_) const noexcept;
    ColorRGBA& operator*=(float scalar_) noexcept;
    ColorRGBA operator/(float scalar_) const noexcept;
    ColorRGBA& operator/=(float scalar_) noexcept;

    [[nodiscard]] ColorRGB ToColorRGB() const noexcept;
    [[nodiscard]] Vector4D ToVector4D() const noexcept;

    [[nodiscard]] static ColorRGBA Lerp(const ColorRGBA& start_, const ColorRGBA& end_, float t_) noexcept;
	[[nodiscard]] static bool IsApproximately(const ColorRGBA& lhs_, const ColorRGBA& rhs_, float epsilon_ = Mathf::Epsilon) noexcept;

    [[nodiscard]] static ColorRGBA GetBlack() noexcept;
    [[nodiscard]] static ColorRGBA GetWhite() noexcept;
    [[nodiscard]] static ColorRGBA GetRed() noexcept;
    [[nodiscard]] static ColorRGBA GetGreen() noexcept;
    [[nodiscard]] static ColorRGBA GetBlue() noexcept;
    [[nodiscard]] static ColorRGBA GetYellow() noexcept;
    [[nodiscard]] static ColorRGBA GetCyan() noexcept;
    [[nodiscard]] static ColorRGBA GetMagenta() noexcept;
    [[nodiscard]] static ColorRGBA GetClear() noexcept;

	[[nodiscard]] bool IsTransparent(float epsilon_ = Mathf::Epsilon) const noexcept;
	[[nodiscard]] bool IsOpaque(float epsilon_ = Mathf::Epsilon) const noexcept;

    static DirectX::XMVECTOR Load(const ColorRGBA& color_) noexcept;
    static void Store(ColorRGBA& destination_, DirectX::XMVECTOR source_) noexcept;

    operator Vector4D() const noexcept;
};
