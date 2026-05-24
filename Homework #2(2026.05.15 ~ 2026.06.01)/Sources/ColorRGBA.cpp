#include "Precompiled.h"
#include "ColorRGBA.h"

#include "ColorRGB.h"
#include "Mathf.h"
#include "Vector3D.h"
#include "Vector4D.h"

ColorRGBA::ColorRGBA() noexcept
    : DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)
{
}

ColorRGBA::ColorRGBA(float value_) noexcept
    : DirectX::XMFLOAT4(value_, value_, value_, 1.0f)
{
}

ColorRGBA::ColorRGBA(float r_, float g_, float b_) noexcept
    : DirectX::XMFLOAT4(r_, g_, b_, 1.0f)
{
}

ColorRGBA::ColorRGBA(float r_, float g_, float b_, float a_) noexcept
    : DirectX::XMFLOAT4(r_, g_, b_, a_)
{
}

ColorRGBA::ColorRGBA(const ColorRGB& rgb_, float a_) noexcept
    : DirectX::XMFLOAT4(rgb_.x, rgb_.y, rgb_.z, a_)
{
}

ColorRGBA::ColorRGBA(const Vector3D& vector_, float alpha_) noexcept
    : DirectX::XMFLOAT4(vector_.x, vector_.y, vector_.z, alpha_)
{
}

ColorRGBA::ColorRGBA(const Vector4D& vector_) noexcept
    : DirectX::XMFLOAT4(vector_.x, vector_.y, vector_.z, vector_.w)
{
}

ColorRGBA::ColorRGBA(const ColorRGBA& color_) noexcept
    : DirectX::XMFLOAT4(color_)
{
}

ColorRGBA::ColorRGBA(ColorRGBA&& color_) noexcept
    : DirectX::XMFLOAT4(color_)
{
}

ColorRGBA& ColorRGBA::operator=(const ColorRGBA& other_) noexcept
{
    x = other_.x;
    y = other_.y;
    z = other_.z;
    w = other_.w;
    return *this;
}

ColorRGBA& ColorRGBA::operator=(ColorRGBA&& other_) noexcept
{
    x = other_.x;
    y = other_.y;
    z = other_.z;
    w = other_.w;
    return *this;
}

ColorRGBA::operator COLORREF() const noexcept
{
	const auto r = static_cast<unsigned char>(std::clamp(x, 0.0f, 1.0f) * 255.0f);
	const auto g = static_cast<unsigned char>(std::clamp(y, 0.0f, 1.0f) * 255.0f);
	const auto b = static_cast<unsigned char>(std::clamp(z, 0.0f, 1.0f) * 255.0f);
	return RGB(r, g, b);
}

ColorRGBA ColorRGBA::GetClear() noexcept
{
    return ColorRGBA(0.0f, 0.0f, 0.0f, 0.0f);
}

ColorRGBA ColorRGBA::GetBlack() noexcept
{
    return ColorRGBA(0.0f, 0.0f, 0.0f, 1.0f);
}

ColorRGBA ColorRGBA::GetWhite() noexcept
{
    return ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f);
}

ColorRGBA ColorRGBA::GetRed() noexcept
{
    return ColorRGBA(1.0f, 0.0f, 0.0f, 1.0f);
}

ColorRGBA ColorRGBA::GetGreen() noexcept
{
    return ColorRGBA(0.0f, 1.0f, 0.0f, 1.0f);
}

ColorRGBA ColorRGBA::GetBlue() noexcept
{
    return ColorRGBA(0.0f, 0.0f, 1.0f, 1.0f);
}

ColorRGBA ColorRGBA::GetYellow() noexcept
{
    return ColorRGBA(1.0f, 1.0f, 0.0f, 1.0f);
}

ColorRGBA ColorRGBA::GetCyan() noexcept
{
    return ColorRGBA(0.0f, 1.0f, 1.0f, 1.0f);
}

ColorRGBA ColorRGBA::GetMagenta() noexcept
{
    return ColorRGBA(1.0f, 0.0f, 1.0f, 1.0f);
}

ColorRGBA ColorRGBA::GetGray() noexcept
{
    return ColorRGBA(0.5f, 0.5f, 0.5f, 1.0f);
}

ColorRGBA ColorRGBA::GetGrey() noexcept
{
    return GetGray();
}

ColorRGBA ColorRGBA::GetGamma() const noexcept
{
    const ColorRGB gamma{ ColorRGB(x, y, z).GetGamma() };
    return ColorRGBA(gamma.x, gamma.y, gamma.z, w);
}

ColorRGBA ColorRGBA::GetLinear() const noexcept
{
    const ColorRGB linear{ ColorRGB(x, y, z).GetLinear() };
    return ColorRGBA(linear.x, linear.y, linear.z, w);
}

float ColorRGBA::GetGrayscale() const noexcept
{
    return ColorRGB(x, y, z).GetGrayscale();
}

float ColorRGBA::GetMaxColorComponent() const noexcept
{
    return ColorRGB(x, y, z).GetMaxColorComponent();
}

ColorRGBA ColorRGBA::HSVToRGB(float h_, float s_, float v_) noexcept
{
    return HSVToRGB(h_, s_, v_, false);
}

ColorRGBA ColorRGBA::HSVToRGB(float h_, float s_, float v_, bool hdr_) noexcept
{
    const ColorRGB rgb{ ColorRGB::HSVToRGB(h_, s_, v_, hdr_) };
    return ColorRGBA(rgb.x, rgb.y, rgb.z, 1.0f);
}

void ColorRGBA::RGBToHSV(const ColorRGBA& rgbColor_, float& h_, float& s_, float& v_) noexcept
{
    ColorRGB::RGBToHSV(ColorRGB(rgbColor_.x, rgbColor_.y, rgbColor_.z), h_, s_, v_);
}

ColorRGBA ColorRGBA::Lerp(const ColorRGBA& a_, const ColorRGBA& b_, float t_) noexcept
{
    return LerpUnclamped(a_, b_, Mathf::Clamp(t_, 0.0f, 1.0f));
}

ColorRGBA ColorRGBA::LerpUnclamped(const ColorRGBA& a_, const ColorRGBA& b_, float t_) noexcept
{
    return ColorRGBA(
        Mathf::Lerp(a_.x, b_.x, t_),
        Mathf::Lerp(a_.y, b_.y, t_),
        Mathf::Lerp(a_.z, b_.z, t_),
        Mathf::Lerp(a_.w, b_.w, t_));
}

bool ColorRGBA::IsApproximately(const ColorRGBA& lhs_, const ColorRGBA& rhs_, float epsilon_) noexcept
{
    return std::abs(lhs_.x - rhs_.x) <= epsilon_
        && std::abs(lhs_.y - rhs_.y) <= epsilon_
        && std::abs(lhs_.z - rhs_.z) <= epsilon_
        && std::abs(lhs_.w - rhs_.w) <= epsilon_;
}

bool ColorRGBA::operator==(const ColorRGBA& other_) const noexcept
{
    return IsApproximately(*this, other_);
}

bool ColorRGBA::operator!=(const ColorRGBA& other_) const noexcept
{
    return !(*this == other_);
}

bool ColorRGBA::IsFinite() const noexcept
{
    return std::isfinite(x) && std::isfinite(y) && std::isfinite(z) && std::isfinite(w);
}

bool ColorRGBA::IsHDR() const noexcept
{
    return x > 1.0f || y > 1.0f || z > 1.0f;
}

bool ColorRGBA::IsTransparent(float epsilon_) const noexcept
{
    return std::abs(w) <= epsilon_;
}

bool ColorRGBA::IsOpaque(float epsilon_) const noexcept
{
    return std::abs(w - 1.0f) <= epsilon_;
}

Vector3D ColorRGBA::ToVector3D() const noexcept
{
    return Vector3D(x, y, z);
}

Vector4D ColorRGBA::ToVector4D() const noexcept
{
    return Vector4D(x, y, z, w);
}

ColorRGB ColorRGBA::ToColorRGB() const noexcept
{
    return ColorRGB(x, y, z);
}
