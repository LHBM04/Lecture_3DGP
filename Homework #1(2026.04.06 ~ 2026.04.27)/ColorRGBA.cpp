#include "Precompiled.hpp"
#include "ColorRGBA.hpp"

#include "ColorRGB.hpp"
#include "Math.hpp"
#include "Vector3D.hpp"
#include "Vector4D.hpp"

ColorRGBA::ColorRGBA()
    : XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)
{
}

ColorRGBA::ColorRGBA(float value_)
    : XMFLOAT4(value_, value_, value_, 1.0f)
{
}

ColorRGBA::ColorRGBA(float r_, float g_, float b_)
    : XMFLOAT4(r_, g_, b_, 1.0f)
{
}

ColorRGBA::ColorRGBA(float r_, float g_, float b_, float a_)
    : XMFLOAT4(r_, g_, b_, a_)
{
}

ColorRGBA::ColorRGBA(const ColorRGB& rgb_, float a_)
    : XMFLOAT4(rgb_.x, rgb_.y, rgb_.z, a_)
{
}

ColorRGBA::ColorRGBA(const Vector3D& vector_, float alpha_)
    : XMFLOAT4(vector_.x, vector_.y, vector_.z, alpha_)
{
}

ColorRGBA::ColorRGBA(const Vector4D& vector_)
    : XMFLOAT4(vector_.x, vector_.y, vector_.z, vector_.w)
{
}

ColorRGBA::ColorRGBA(const ColorRGBA& color_)
    : XMFLOAT4(color_)
{
}

ColorRGBA::ColorRGBA(ColorRGBA&& color_) noexcept
    : XMFLOAT4(color_)
{
}

ColorRGBA& ColorRGBA::operator=(const ColorRGBA& other_)
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

ColorRGBA::operator COLORREF() const
{
	const auto r = static_cast<unsigned char>(std::clamp(x, 0.0f, 1.0f) * 255.0f);
	const auto g = static_cast<unsigned char>(std::clamp(y, 0.0f, 1.0f) * 255.0f);
	const auto b = static_cast<unsigned char>(std::clamp(z, 0.0f, 1.0f) * 255.0f);
	return RGB(r, g, b);
}

ColorRGBA ColorRGBA::GetClear()
{
    return ColorRGBA(0.0f, 0.0f, 0.0f, 0.0f);
}

ColorRGBA ColorRGBA::GetBlack()
{
    return ColorRGBA(0.0f, 0.0f, 0.0f, 1.0f);
}

ColorRGBA ColorRGBA::GetWhite()
{
    return ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f);
}

ColorRGBA ColorRGBA::GetRed()
{
    return ColorRGBA(1.0f, 0.0f, 0.0f, 1.0f);
}

ColorRGBA ColorRGBA::GetGreen()
{
    return ColorRGBA(0.0f, 1.0f, 0.0f, 1.0f);
}

ColorRGBA ColorRGBA::GetBlue()
{
    return ColorRGBA(0.0f, 0.0f, 1.0f, 1.0f);
}

ColorRGBA ColorRGBA::GetYellow()
{
    return ColorRGBA(1.0f, 1.0f, 0.0f, 1.0f);
}

ColorRGBA ColorRGBA::GetCyan()
{
    return ColorRGBA(0.0f, 1.0f, 1.0f, 1.0f);
}

ColorRGBA ColorRGBA::GetMagenta()
{
    return ColorRGBA(1.0f, 0.0f, 1.0f, 1.0f);
}

ColorRGBA ColorRGBA::GetGray()
{
    return ColorRGBA(0.5f, 0.5f, 0.5f, 1.0f);
}

ColorRGBA ColorRGBA::GetGrey()
{
    return GetGray();
}

ColorRGBA ColorRGBA::GetGamma() const
{
    const ColorRGB gamma{ ColorRGB(x, y, z).GetGamma() };
    return ColorRGBA(gamma.x, gamma.y, gamma.z, w);
}

ColorRGBA ColorRGBA::GetLinear() const
{
    const ColorRGB linear{ ColorRGB(x, y, z).GetLinear() };
    return ColorRGBA(linear.x, linear.y, linear.z, w);
}

float ColorRGBA::GetGrayscale() const
{
    return ColorRGB(x, y, z).GetGrayscale();
}

float ColorRGBA::GetMaxColorComponent() const
{
    return ColorRGB(x, y, z).GetMaxColorComponent();
}

ColorRGBA ColorRGBA::HSVToRGB(float h_, float s_, float v_)
{
    return HSVToRGB(h_, s_, v_, false);
}

ColorRGBA ColorRGBA::HSVToRGB(float h_, float s_, float v_, bool hdr_)
{
    const ColorRGB rgb{ ColorRGB::HSVToRGB(h_, s_, v_, hdr_) };
    return ColorRGBA(rgb.x, rgb.y, rgb.z, 1.0f);
}

void ColorRGBA::RGBToHSV(const ColorRGBA& rgbColor_, float& h_, float& s_, float& v_)
{
    ColorRGB::RGBToHSV(ColorRGB(rgbColor_.x, rgbColor_.y, rgbColor_.z), h_, s_, v_);
}

ColorRGBA ColorRGBA::Lerp(const ColorRGBA& a_, const ColorRGBA& b_, float t_)
{
    return LerpUnclamped(a_, b_, Mathf::Clamp(t_, 0.0f, 1.0f));
}

ColorRGBA ColorRGBA::LerpUnclamped(const ColorRGBA& a_, const ColorRGBA& b_, float t_)
{
    return ColorRGBA(
        Mathf::Lerp(a_.x, b_.x, t_),
        Mathf::Lerp(a_.y, b_.y, t_),
        Mathf::Lerp(a_.z, b_.z, t_),
        Mathf::Lerp(a_.w, b_.w, t_));
}

bool ColorRGBA::IsApproximately(const ColorRGBA& lhs_, const ColorRGBA& rhs_, float epsilon_)
{
    return std::abs(lhs_.x - rhs_.x) <= epsilon_
        && std::abs(lhs_.y - rhs_.y) <= epsilon_
        && std::abs(lhs_.z - rhs_.z) <= epsilon_
        && std::abs(lhs_.w - rhs_.w) <= epsilon_;
}

bool ColorRGBA::operator==(const ColorRGBA& other_) const
{
    return IsApproximately(*this, other_);
}

bool ColorRGBA::operator!=(const ColorRGBA& other_) const
{
    return !(*this == other_);
}

bool ColorRGBA::IsFinite() const
{
    return std::isfinite(x) && std::isfinite(y) && std::isfinite(z) && std::isfinite(w);
}

bool ColorRGBA::IsHDR() const
{
    return x > 1.0f || y > 1.0f || z > 1.0f;
}

bool ColorRGBA::IsTransparent(float epsilon_) const
{
    return std::abs(w) <= epsilon_;
}

bool ColorRGBA::IsOpaque(float epsilon_) const
{
    return std::abs(w - 1.0f) <= epsilon_;
}

Vector3D ColorRGBA::ToVector3D() const
{
    return Vector3D(x, y, z);
}

Vector4D ColorRGBA::ToVector4D() const
{
    return Vector4D(x, y, z, w);
}

ColorRGB ColorRGBA::ToColorRGB() const
{
    return ColorRGB(x, y, z);
}
