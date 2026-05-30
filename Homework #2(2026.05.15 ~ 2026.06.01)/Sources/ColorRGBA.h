#pragma once

#include "MathF.h"

class Vector3D;
class Vector4D;
class ColorRGB;

class ColorRGBA : public DirectX::XMFLOAT4
{
public:
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

	[[nodiscard]] static bool IsApproximately(const ColorRGBA& lhs_, const ColorRGBA& rhs_, float epsilon_ = Mathf::Epsilon) noexcept;

    [[nodiscard]] bool operator==(const ColorRGBA& other_) const noexcept;
    [[nodiscard]] bool operator!=(const ColorRGBA& other_) const noexcept;

	[[nodiscard]] bool IsFinite() const noexcept;
	[[nodiscard]] bool IsHDR() const noexcept;
	[[nodiscard]] bool IsTransparent(float epsilon_ = Mathf::Epsilon) const noexcept;
	[[nodiscard]] bool IsOpaque(float epsilon_ = Mathf::Epsilon) const noexcept;

	[[nodiscard]] Vector3D ToVector3D() const noexcept;
	[[nodiscard]] Vector4D ToVector4D() const noexcept;
	[[nodiscard]] ColorRGB ToColorRGB() const noexcept;
};

inline ColorRGBA::ColorRGBA() noexcept
    : DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)
{
}

inline ColorRGBA::ColorRGBA(float value_) noexcept
    : DirectX::XMFLOAT4(value_, value_, value_, 1.0f)
{
}

inline ColorRGBA::ColorRGBA(float r_, float g_, float b_) noexcept
    : DirectX::XMFLOAT4(r_, g_, b_, 1.0f)
{
}

inline ColorRGBA::ColorRGBA(float r_, float g_, float b_, float a_) noexcept
    : DirectX::XMFLOAT4(r_, g_, b_, a_)
{
}

inline ColorRGBA::ColorRGBA(const ColorRGBA& color_) noexcept
    : DirectX::XMFLOAT4(color_)
{
}

inline ColorRGBA::ColorRGBA(ColorRGBA&& color_) noexcept
    : DirectX::XMFLOAT4(color_)
{
}

inline ColorRGBA& ColorRGBA::operator=(const ColorRGBA& other_) noexcept
{
    x = other_.x;
    y = other_.y;
    z = other_.z;
    w = other_.w;
    return *this;
}

inline ColorRGBA& ColorRGBA::operator=(ColorRGBA&& other_) noexcept
{
    x = other_.x;
    y = other_.y;
    z = other_.z;
    w = other_.w;
    return *this;
}

inline ColorRGBA::operator COLORREF() const noexcept
{
	const auto r = static_cast<unsigned char>(std::clamp(x, 0.0f, 1.0f) * 255.0f);
	const auto g = static_cast<unsigned char>(std::clamp(y, 0.0f, 1.0f) * 255.0f);
	const auto b = static_cast<unsigned char>(std::clamp(z, 0.0f, 1.0f) * 255.0f);
	return RGB(r, g, b);
}

inline ColorRGBA ColorRGBA::GetClear() noexcept
{
    return ColorRGBA(0.0f, 0.0f, 0.0f, 0.0f);
}

inline ColorRGBA ColorRGBA::GetBlack() noexcept
{
    return ColorRGBA(0.0f, 0.0f, 0.0f, 1.0f);
}

inline ColorRGBA ColorRGBA::GetWhite() noexcept
{
    return ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f);
}

inline ColorRGBA ColorRGBA::GetRed() noexcept
{
    return ColorRGBA(1.0f, 0.0f, 0.0f, 1.0f);
}

inline ColorRGBA ColorRGBA::GetGreen() noexcept
{
    return ColorRGBA(0.0f, 1.0f, 0.0f, 1.0f);
}

inline ColorRGBA ColorRGBA::GetBlue() noexcept
{
    return ColorRGBA(0.0f, 0.0f, 1.0f, 1.0f);
}

inline ColorRGBA ColorRGBA::GetYellow() noexcept
{
    return ColorRGBA(1.0f, 1.0f, 0.0f, 1.0f);
}

inline ColorRGBA ColorRGBA::GetCyan() noexcept
{
    return ColorRGBA(0.0f, 1.0f, 1.0f, 1.0f);
}

inline ColorRGBA ColorRGBA::GetMagenta() noexcept
{
    return ColorRGBA(1.0f, 0.0f, 1.0f, 1.0f);
}

inline ColorRGBA ColorRGBA::GetGray() noexcept
{
    return ColorRGBA(0.5f, 0.5f, 0.5f, 1.0f);
}

inline ColorRGBA ColorRGBA::GetGrey() noexcept
{
    return GetGray();
}

inline ColorRGBA ColorRGBA::Lerp(const ColorRGBA& a_, const ColorRGBA& b_, float t_) noexcept
{
    return LerpUnclamped(a_, b_, Mathf::Clamp(t_, 0.0f, 1.0f));
}

inline ColorRGBA ColorRGBA::LerpUnclamped(const ColorRGBA& a_, const ColorRGBA& b_, float t_) noexcept
{
    return ColorRGBA(
        Mathf::Lerp(a_.x, b_.x, t_),
        Mathf::Lerp(a_.y, b_.y, t_),
        Mathf::Lerp(a_.z, b_.z, t_),
        Mathf::Lerp(a_.w, b_.w, t_));
}

inline bool ColorRGBA::IsApproximately(const ColorRGBA& lhs_, const ColorRGBA& rhs_, float epsilon_) noexcept
{
    return std::abs(lhs_.x - rhs_.x) <= epsilon_
        && std::abs(lhs_.y - rhs_.y) <= epsilon_
        && std::abs(lhs_.z - rhs_.z) <= epsilon_
        && std::abs(lhs_.w - rhs_.w) <= epsilon_;
}

inline bool ColorRGBA::operator==(const ColorRGBA& other_) const noexcept
{
    return IsApproximately(*this, other_);
}

inline bool ColorRGBA::operator!=(const ColorRGBA& other_) const noexcept
{
    return !(*this == other_);
}

inline bool ColorRGBA::IsFinite() const noexcept
{
    return std::isfinite(x) && std::isfinite(y) && std::isfinite(z) && std::isfinite(w);
}

inline bool ColorRGBA::IsHDR() const noexcept
{
    return x > 1.0f || y > 1.0f || z > 1.0f;
}

inline bool ColorRGBA::IsTransparent(float epsilon_) const noexcept
{
    return std::abs(w) <= epsilon_;
}

inline bool ColorRGBA::IsOpaque(float epsilon_) const noexcept
{
    return std::abs(w - 1.0f) <= epsilon_;
}

#include "Vector3D.h"
#include "Vector4D.h"
#include "ColorRGB.h"

inline ColorRGBA::ColorRGBA(const ColorRGB& rgb_, float a_) noexcept
    : DirectX::XMFLOAT4(rgb_.x, rgb_.y, rgb_.z, a_)
{
}

inline ColorRGBA::ColorRGBA(const Vector3D& vector_, float alpha_) noexcept
    : DirectX::XMFLOAT4(vector_.x, vector_.y, vector_.z, alpha_)
{
}

inline ColorRGBA::ColorRGBA(const Vector4D& vector_) noexcept
    : DirectX::XMFLOAT4(vector_.x, vector_.y, vector_.z, vector_.w)
{
}

inline ColorRGBA ColorRGBA::GetGamma() const noexcept
{
    const ColorRGB gamma{ ColorRGB(x, y, z).GetGamma() };
    return ColorRGBA(gamma.x, gamma.y, gamma.z, w);
}

inline ColorRGBA ColorRGBA::GetLinear() const noexcept
{
    const ColorRGB linear{ ColorRGB(x, y, z).GetLinear() };
    return ColorRGBA(linear.x, linear.y, linear.z, w);
}

inline float ColorRGBA::GetGrayscale() const noexcept
{
    return ColorRGB(x, y, z).GetGrayscale();
}

inline float ColorRGBA::GetMaxColorComponent() const noexcept
{
    return ColorRGB(x, y, z).GetMaxColorComponent();
}

inline ColorRGBA ColorRGBA::HSVToRGB(float h_, float s_, float v_) noexcept
{
    return HSVToRGB(h_, s_, v_, false);
}

inline ColorRGBA ColorRGBA::HSVToRGB(float h_, float s_, float v_, bool hdr_) noexcept
{
    const ColorRGB rgb{ ColorRGB::HSVToRGB(h_, s_, v_, hdr_) };
    return ColorRGBA(rgb.x, rgb.y, rgb.z, 1.0f);
}

inline void ColorRGBA::RGBToHSV(const ColorRGBA& rgbColor_, float& h_, float& s_, float& v_) noexcept
{
    ColorRGB::RGBToHSV(ColorRGB(rgbColor_.x, rgbColor_.y, rgbColor_.z), h_, s_, v_);
}

inline Vector3D ColorRGBA::ToVector3D() const noexcept
{
    return Vector3D(x, y, z);
}

inline Vector4D ColorRGBA::ToVector4D() const noexcept
{
    return Vector4D(x, y, z, w);
}

inline ColorRGB ColorRGBA::ToColorRGB() const noexcept
{
    return ColorRGB(x, y, z);
}

