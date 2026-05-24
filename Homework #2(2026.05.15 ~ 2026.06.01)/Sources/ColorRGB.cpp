#include "Precompiled.h"
#include "ColorRGB.h"

#include "ColorRGBA.h"
#include "Vector3D.h"
#include "Vector4D.h"

ColorRGB::ColorRGB() noexcept
    : DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f)
{
}

ColorRGB::ColorRGB(float value_) noexcept
    : DirectX::XMFLOAT3(value_, value_, value_)
{
}

ColorRGB::ColorRGB(float r_, float g_, float b_) noexcept
    : DirectX::XMFLOAT3(r_, g_, b_)
{
}

ColorRGB::ColorRGB(const Vector3D& vector_) noexcept
    : DirectX::XMFLOAT3(vector_.x, vector_.y, vector_.z)
{
}

ColorRGB::ColorRGB(const Vector4D& vector_) noexcept
    : DirectX::XMFLOAT3(vector_.x, vector_.y, vector_.z)
{
}

ColorRGB::ColorRGB(const ColorRGB& color_) noexcept
    : DirectX::XMFLOAT3(color_)
{
}

ColorRGB::ColorRGB(ColorRGB&& color_) noexcept
    : DirectX::XMFLOAT3(color_)
{
}

ColorRGB& ColorRGB::operator=(const ColorRGB& other_) noexcept
{
    x = other_.x;
    y = other_.y;
    z = other_.z;
    return *this;
}

ColorRGB& ColorRGB::operator=(ColorRGB&& other_) noexcept
{
    x = other_.x;
    y = other_.y;
    z = other_.z;
    return *this;
}

ColorRGB ColorRGB::GetBlack() noexcept
{
    return ColorRGB(0.0f, 0.0f, 0.0f);
}

ColorRGB ColorRGB::GetWhite() noexcept
{
    return ColorRGB(1.0f, 1.0f, 1.0f);
}

ColorRGB ColorRGB::GetRed() noexcept
{
    return ColorRGB(1.0f, 0.0f, 0.0f);
}

ColorRGB ColorRGB::GetGreen() noexcept
{
    return ColorRGB(0.0f, 1.0f, 0.0f);
}

ColorRGB ColorRGB::GetBlue() noexcept
{
    return ColorRGB(0.0f, 0.0f, 1.0f);
}

ColorRGB ColorRGB::GetYellow() noexcept
{
    return ColorRGB(1.0f, 1.0f, 0.0f);
}

ColorRGB ColorRGB::GetCyan() noexcept
{
    return ColorRGB(0.0f, 1.0f, 1.0f);
}

ColorRGB ColorRGB::GetMagenta() noexcept
{
    return ColorRGB(1.0f, 0.0f, 1.0f);
}

ColorRGB ColorRGB::GetGray() noexcept
{
    return ColorRGB(0.5f, 0.5f, 0.5f);
}

ColorRGB ColorRGB::GetGrey() noexcept
{
    return GetGray();
}

float ColorRGB::LinearToGammaSpace(float value_) noexcept
{
    return std::pow(std::max(0.0f, value_), 1.0f / 2.2f);
}

float ColorRGB::GammaToLinearSpace(float value_) noexcept
{
    return std::pow(std::max(0.0f, value_), 2.2f);
}

ColorRGB ColorRGB::GetGamma() const noexcept
{
    return ColorRGB(
        LinearToGammaSpace(x),
        LinearToGammaSpace(y),
        LinearToGammaSpace(z));
}

ColorRGB ColorRGB::GetLinear() const noexcept
{
    return ColorRGB(
        GammaToLinearSpace(x),
        GammaToLinearSpace(y),
        GammaToLinearSpace(z));
}

float ColorRGB::GetGrayscale() const noexcept
{
    return 0.299f * x + 0.587f * y + 0.114f * z;
}

float ColorRGB::GetMaxColorComponent() const noexcept
{
    return std::max(x, std::max(y, z));
}

ColorRGB ColorRGB::HSVToRGB(float h_, float s_, float v_) noexcept
{
    return HSVToRGB(h_, s_, v_, false);
}

ColorRGB ColorRGB::HSVToRGB(float h_, float s_, float v_, bool hdr_) noexcept
{
    h_ = h_ - std::floor(h_);
    s_ = std::clamp(s_, 0.0f, 1.0f);
    if (!hdr_)
    {
        v_ = std::clamp(v_, 0.0f, 1.0f);
    }

    if (s_ <= Mathf::EPSILON)
    {
        return ColorRGB(v_, v_, v_);
    }

    const float scaledH{ h_ * 6.0f };
    const int sector{ static_cast<int>(std::floor(scaledH)) };
    const float f{ scaledH - static_cast<float>(sector) };
    const float p{ v_ * (1.0f - s_) };
    const float q{ v_ * (1.0f - s_ * f) };
    const float t{ v_ * (1.0f - s_ * (1.0f - f)) };

    switch (sector % 6)
    {
    case 0: return ColorRGB(v_, t, p);
    case 1: return ColorRGB(q, v_, p);
    case 2: return ColorRGB(p, v_, t);
    case 3: return ColorRGB(p, q, v_);
    case 4: return ColorRGB(t, p, v_);
    default: return ColorRGB(v_, p, q);
    }
}

void ColorRGB::RGBToHSV(const ColorRGB& rgbColor_, float& h_, float& s_, float& v_) noexcept
{
    const float r{ rgbColor_.x };
    const float g{ rgbColor_.y };
    const float b{ rgbColor_.z };

    const float maxV{ std::max(r, std::max(g, b)) };
    const float minV{ std::min(r, std::min(g, b)) };
    const float delta{ maxV - minV };

    v_ = maxV;

    if (delta <= Mathf::EPSILON)
    {
        h_ = 0.0f;
        s_ = 0.0f;
        return;
    }

    s_ = (maxV <= Mathf::EPSILON) ? 0.0f : (delta / maxV);

    if (r >= maxV)
    {
        h_ = (g - b) / delta;
    }
    else if (g >= maxV)
    {
        h_ = 2.0f + (b - r) / delta;
    }
    else
    {
        h_ = 4.0f + (r - g) / delta;
    }

    h_ /= 6.0f;
    if (h_ < 0.0f)
    {
        h_ += 1.0f;
    }
}

ColorRGB ColorRGB::Lerp(const ColorRGB& a_, const ColorRGB& b_, float t_) noexcept
{
    return LerpUnclamped(a_, b_, Mathf::Clamp(t_, 0.0f, 1.0f));
}

ColorRGB ColorRGB::LerpUnclamped(const ColorRGB& a_, const ColorRGB& b_, float t_) noexcept
{
    return ColorRGB(
        Mathf::Lerp(a_.x, b_.x, t_),
        Mathf::Lerp(a_.y, b_.y, t_),
        Mathf::Lerp(a_.z, b_.z, t_));
}

bool ColorRGB::IsApproximately(const ColorRGB& lhs_, const ColorRGB& rhs_, float epsilon_) noexcept
{
    return std::abs(lhs_.x - rhs_.x) <= epsilon_
        && std::abs(lhs_.y - rhs_.y) <= epsilon_
        && std::abs(lhs_.z - rhs_.z) <= epsilon_;
}

bool ColorRGB::operator==(const ColorRGB& other_) const noexcept
{
    return IsApproximately(*this, other_);
}

bool ColorRGB::operator!=(const ColorRGB& other_) const noexcept
{
    return !(*this == other_);
}

bool ColorRGB::IsFinite() const noexcept
{
    return std::isfinite(x) && std::isfinite(y) && std::isfinite(z);
}

bool ColorRGB::IsHDR() const noexcept
{
    return x > 1.0f || y > 1.0f || z > 1.0f;
}

Vector3D ColorRGB::ToVector3D() const noexcept
{
    return Vector3D(x, y, z);
}

Vector4D ColorRGB::ToVector4D(float alpha_) const noexcept
{
    return Vector4D(x, y, z, alpha_);
}

ColorRGB::ColorRGB(const ColorRGBA& color_) noexcept
    : DirectX::XMFLOAT3(color_.x, color_.y, color_.z)
{
}

ColorRGBA ColorRGB::ToColorRGBA(float alpha_) const noexcept
{
    return ColorRGBA(*this, alpha_);
}
