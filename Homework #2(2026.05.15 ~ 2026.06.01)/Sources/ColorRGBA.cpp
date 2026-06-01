#include "Precompiled.h"
#include "ColorRGBA.h"
#include "ColorRGB.h"
#include "Vector3D.h"
#include "Vector4D.h"

ColorRGBA::ColorRGBA() noexcept
    : DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)
{
}

ColorRGBA::ColorRGBA(float value_) noexcept
    : DirectX::XMFLOAT4(value_, value_, value_, value_)
{
}

ColorRGBA::ColorRGBA(float r_, float g_, float b_, float a_) noexcept
    : DirectX::XMFLOAT4(r_, g_, b_, a_)
{
}

ColorRGBA::ColorRGBA(const float* values_) noexcept
    : DirectX::XMFLOAT4(values_)
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

ColorRGBA& ColorRGBA::operator=(const Vector4D& other_) noexcept
{
    x = other_.x;
    y = other_.y;
    z = other_.z;
    w = other_.w;
    return *this;
}

bool ColorRGBA::operator==(const ColorRGBA& other_) const noexcept
{
    return std::abs(x - other_.x) < Mathf::Epsilon &&
           std::abs(y - other_.y) < Mathf::Epsilon &&
           std::abs(z - other_.z) < Mathf::Epsilon &&
           std::abs(w - other_.w) < Mathf::Epsilon;
}

bool ColorRGBA::operator!=(const ColorRGBA& other_) const noexcept
{
    return !(*this == other_);
}

ColorRGBA ColorRGBA::operator+(const ColorRGBA& other_) const noexcept
{
    ColorRGBA result;
    Store(result, DirectX::XMVectorAdd(Load(*this), Load(other_)));
    return result;
}

ColorRGBA& ColorRGBA::operator+=(const ColorRGBA& other_) noexcept
{
    Store(*this, DirectX::XMVectorAdd(Load(*this), Load(other_)));
    return *this;
}

ColorRGBA ColorRGBA::operator-(const ColorRGBA& other_) const noexcept
{
    ColorRGBA result;
    Store(result, DirectX::XMVectorSubtract(Load(*this), Load(other_)));
    return result;
}

ColorRGBA& ColorRGBA::operator-=(const ColorRGBA& other_) noexcept
{
    Store(*this, DirectX::XMVectorSubtract(Load(*this), Load(other_)));
    return *this;
}

ColorRGBA ColorRGBA::operator*(const ColorRGBA& other_) const noexcept
{
    ColorRGBA result;
    Store(result, DirectX::XMVectorMultiply(Load(*this), Load(other_)));
    return result;
}

ColorRGBA& ColorRGBA::operator*=(const ColorRGBA& other_) noexcept
{
    Store(*this, DirectX::XMVectorMultiply(Load(*this), Load(other_)));
    return *this;
}

ColorRGBA ColorRGBA::operator*(float scalar_) const noexcept
{
    ColorRGBA result;
    Store(result, DirectX::XMVectorScale(Load(*this), scalar_));
    return result;
}

ColorRGBA& ColorRGBA::operator*=(float scalar_) noexcept
{
    Store(*this, DirectX::XMVectorScale(Load(*this), scalar_));
    return *this;
}

ColorRGBA ColorRGBA::operator/(float scalar_) const noexcept
{
    ColorRGBA result;
    Store(result, DirectX::XMVectorScale(Load(*this), 1.0f / scalar_));
    return result;
}

ColorRGBA& ColorRGBA::operator/=(float scalar_) noexcept
{
    Store(*this, DirectX::XMVectorScale(Load(*this), 1.0f / scalar_));
    return *this;
}

ColorRGB ColorRGBA::ToColorRGB() const noexcept
{
    return ColorRGB(x, y, z);
}

Vector4D ColorRGBA::ToVector4D() const noexcept
{
    return Vector4D(x, y, z, w);
}

ColorRGBA ColorRGBA::Lerp(const ColorRGBA& start_, const ColorRGBA& end_, float t_) noexcept
{
    ColorRGBA result;
    Store(result, DirectX::XMVectorLerp(Load(start_), Load(end_), t_));
    return result;
}

bool ColorRGBA::IsApproximately(const ColorRGBA& lhs_, const ColorRGBA& rhs_, float epsilon_) noexcept
{
	return std::abs(lhs_.x - rhs_.x) <= epsilon_ &&
		   std::abs(lhs_.y - rhs_.y) <= epsilon_ &&
		   std::abs(lhs_.z - rhs_.z) <= epsilon_ &&
		   std::abs(lhs_.w - rhs_.w) <= epsilon_;
}

ColorRGBA ColorRGBA::GetBlack() noexcept { return ColorRGBA(0.0f, 0.0f, 0.0f, 1.0f); }
ColorRGBA ColorRGBA::GetWhite() noexcept { return ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f); }
ColorRGBA ColorRGBA::GetRed() noexcept { return ColorRGBA(1.0f, 0.0f, 0.0f, 1.0f); }
ColorRGBA ColorRGBA::GetGreen() noexcept { return ColorRGBA(0.0f, 1.0f, 0.0f, 1.0f); }
ColorRGBA ColorRGBA::GetBlue() noexcept { return ColorRGBA(0.0f, 0.0f, 1.0f, 1.0f); }
ColorRGBA ColorRGBA::GetYellow() noexcept { return ColorRGBA(1.0f, 0.92f, 0.016f, 1.0f); }
ColorRGBA ColorRGBA::GetCyan() noexcept { return ColorRGBA(0.0f, 1.0f, 1.0f, 1.0f); }
ColorRGBA ColorRGBA::GetMagenta() noexcept { return ColorRGBA(1.0f, 0.0f, 1.0f, 1.0f); }
ColorRGBA ColorRGBA::GetClear() noexcept { return ColorRGBA(0.0f, 0.0f, 0.0f, 0.0f); }

bool ColorRGBA::IsTransparent(float epsilon_) const noexcept
{
	return w <= epsilon_;
}

bool ColorRGBA::IsOpaque(float epsilon_) const noexcept
{
	return w >= 1.0f - epsilon_;
}

DirectX::XMVECTOR ColorRGBA::Load(const ColorRGBA& color_) noexcept
{
    return DirectX::XMLoadFloat4(&color_);
}

void ColorRGBA::Store(ColorRGBA& destination_, DirectX::XMVECTOR source_) noexcept
{
    DirectX::XMStoreFloat4(&destination_, source_);
}

ColorRGBA::operator Vector4D() const noexcept
{
    return Vector4D(x, y, z, w);
}
