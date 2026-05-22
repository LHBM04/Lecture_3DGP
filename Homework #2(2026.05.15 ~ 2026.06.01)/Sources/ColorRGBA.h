#pragma once

#include "Math.h"

struct Vector3D;
struct Vector4D;
struct ColorRGB;

struct ColorRGBA : public DirectX::XMFLOAT4
{
    ColorRGBA() noexcept;
    explicit ColorRGBA(float value_) noexcept;
    explicit ColorRGBA(float r_, float g_, float b_) noexcept;
    explicit ColorRGBA(float r_, float g_, float b_, float a_) noexcept;
    explicit ColorRGBA(const ColorRGB& rgb_, float a_ = 1.0f) noexcept;
	explicit ColorRGBA(const Vector3D& vector_, float alpha_ = 1.0f) noexcept;
	explicit ColorRGBA(const Vector4D& vector_) noexcept;
    ColorRGBA(const ColorRGBA& color_) noexcept;
	ColorRGBA(ColorRGBA&& color_) noexcept;

	ColorRGBA& operator=(const ColorRGBA& other_) noexcept;
    ColorRGBA& operator=(ColorRGBA&& other_) noexcept;
	[[nodiscard]] operator COLORREF() const noexcept;

	[[nodiscard]] static ColorRGBA GetClear() noexcept;
	[[nodiscard]] static ColorRGBA GetBlack() noexcept;
	[[nodiscard]] static ColorRGBA GetWhite() noexcept;
	[[nodiscard]] static ColorRGBA GetRed() noexcept;
	[[nodiscard]] static ColorRGBA GetGreen() noexcept;
	[[nodiscard]] static ColorRGBA GetBlue() noexcept;
	[[nodiscard]] static ColorRGBA GetYellow() noexcept;
	[[nodiscard]] static ColorRGBA GetCyan() noexcept;
	[[nodiscard]] static ColorRGBA GetMagenta() noexcept;
	[[nodiscard]] static ColorRGBA GetGray() noexcept;
	[[nodiscard]] static ColorRGBA GetGrey() noexcept;

	[[nodiscard]] ColorRGBA GetGamma() const noexcept;
	[[nodiscard]] ColorRGBA GetLinear() const noexcept;

	[[nodiscard]] float GetGrayscale() const noexcept;
	[[nodiscard]] float GetMaxColorComponent() const noexcept;

	[[nodiscard]] static ColorRGBA HSVToRGB(float h_, float s_, float v_) noexcept;
	[[nodiscard]] static ColorRGBA HSVToRGB(float h_, float s_, float v_, bool hdr_) noexcept;
	static void RGBToHSV(const ColorRGBA& rgbColor_, float& h_, float& s_, float& v_) noexcept;

	[[nodiscard]] static ColorRGBA Lerp(const ColorRGBA& a_, const ColorRGBA& b_, float t_) noexcept;
	[[nodiscard]] static ColorRGBA LerpUnclamped(const ColorRGBA& a_, const ColorRGBA& b_, float t_) noexcept;

	[[nodiscard]] static bool IsApproximately(const ColorRGBA& lhs_, const ColorRGBA& rhs_, float epsilon_ = Mathf::EPSILON) noexcept;

    [[nodiscard]] bool operator==(const ColorRGBA& other_) const noexcept;
    [[nodiscard]] bool operator!=(const ColorRGBA& other_) const noexcept;

	[[nodiscard]] bool IsFinite() const noexcept;
	[[nodiscard]] bool IsHDR() const noexcept;
	[[nodiscard]] bool IsTransparent(float epsilon_ = Mathf::EPSILON) const noexcept;
	[[nodiscard]] bool IsOpaque(float epsilon_ = Mathf::EPSILON) const noexcept;

	[[nodiscard]] Vector3D ToVector3D() const noexcept;
	[[nodiscard]] Vector4D ToVector4D() const noexcept;
	[[nodiscard]] ColorRGB ToColorRGB() const noexcept;
};
