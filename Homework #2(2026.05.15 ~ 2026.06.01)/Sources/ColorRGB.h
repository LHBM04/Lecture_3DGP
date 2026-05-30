#pragma once

#include "MathF.h"

class Vector3D;
class Vector4D;
class ColorRGBA;

class ColorRGB : public DirectX::XMFLOAT3
{
public:
    ColorRGB() noexcept;
    explicit ColorRGB(float value_) noexcept;
    ColorRGB(float r_, float g_, float b_) noexcept;
    explicit ColorRGB(const Vector3D& vector_) noexcept;
    explicit ColorRGB(const Vector4D& vector_) noexcept;
    explicit ColorRGB(const ColorRGBA& color_) noexcept;
    ColorRGB(const ColorRGB& color_) noexcept;
    ColorRGB(ColorRGB&& color_) noexcept;

    ColorRGB& operator=(const ColorRGB& other_) noexcept;
    ColorRGB& operator=(ColorRGB&& other_) noexcept;

    [[nodiscard]] static ColorRGB GetBlack() noexcept;
    [[nodiscard]] static ColorRGB GetWhite() noexcept;
    [[nodiscard]] static ColorRGB GetRed() noexcept;
    [[nodiscard]] static ColorRGB GetGreen() noexcept;
    [[nodiscard]] static ColorRGB GetBlue() noexcept;
    [[nodiscard]] static ColorRGB GetYellow() noexcept;
    [[nodiscard]] static ColorRGB GetCyan() noexcept;
    [[nodiscard]] static ColorRGB GetMagenta() noexcept;
    [[nodiscard]] static ColorRGB GetGray() noexcept;
    [[nodiscard]] static ColorRGB GetGrey() noexcept;

    [[nodiscard]] static float LinearToGammaSpace(float value_) noexcept;
    [[nodiscard]] static float GammaToLinearSpace(float value_) noexcept;

    [[nodiscard]] ColorRGB GetGamma() const noexcept;
    [[nodiscard]] ColorRGB GetLinear() const noexcept;

    [[nodiscard]] float GetGrayscale() const noexcept;
    [[nodiscard]] float GetMaxColorComponent() const noexcept;

    [[nodiscard]] static ColorRGB HSVToRGB(float h_, float s_, float v_) noexcept;
    [[nodiscard]] static ColorRGB HSVToRGB(float h_, float s_, float v_, bool hdr_) noexcept;
    static void RGBToHSV(const ColorRGB& rgbColor_, float& h_, float& s_, float& v_) noexcept;

    [[nodiscard]] static ColorRGB Lerp(const ColorRGB& a_, const ColorRGB& b_, float t_) noexcept;
    [[nodiscard]] static ColorRGB LerpUnclamped(const ColorRGB& a_, const ColorRGB& b_, float t_) noexcept;

    [[nodiscard]] static bool IsApproximately(const ColorRGB& lhs_, const ColorRGB& rhs_, float epsilon_ = Mathf::Epsilon) noexcept;

    [[nodiscard]] bool operator==(const ColorRGB& other_) const noexcept;
    [[nodiscard]] bool operator!=(const ColorRGB& other_) const noexcept;

    [[nodiscard]] bool IsFinite() const noexcept;
    [[nodiscard]] bool IsHDR() const noexcept;

    [[nodiscard]] Vector3D ToVector3D() const noexcept;
    [[nodiscard]] Vector4D ToVector4D(float alpha_) const noexcept;
    [[nodiscard]] ColorRGBA ToColorRGBA(float alpha_ = 1.0f) const noexcept;
};

inline ColorRGB::ColorRGB() noexcept
    : DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f)
{
}

inline ColorRGB::ColorRGB(float value_) noexcept
    : DirectX::XMFLOAT3(value_, value_, value_)
{
}

inline ColorRGB::ColorRGB(float r_, float g_, float b_) noexcept
    : DirectX::XMFLOAT3(r_, g_, b_)
{
}

inline ColorRGB::ColorRGB(const ColorRGB& color_) noexcept
    : DirectX::XMFLOAT3(color_)
{
}

inline ColorRGB::ColorRGB(ColorRGB&& color_) noexcept
    : DirectX::XMFLOAT3(color_)
{
}

inline ColorRGB& ColorRGB::operator=(const ColorRGB& other_) noexcept
{
    x = other_.x;
    y = other_.y;
    z = other_.z;
    return *this;
}

inline ColorRGB& ColorRGB::operator=(ColorRGB&& other_) noexcept
{
    x = other_.x;
    y = other_.y;
    z = other_.z;
    return *this;
}

inline ColorRGB ColorRGB::GetBlack() noexcept
{
    return ColorRGB(0.0f, 0.0f, 0.0f);
}

inline ColorRGB ColorRGB::GetWhite() noexcept
{
    return ColorRGB(1.0f, 1.0f, 1.0f);
}

inline ColorRGB ColorRGB::GetRed() noexcept
{
    return ColorRGB(1.0f, 0.0f, 0.0f);
}

inline ColorRGB ColorRGB::GetGreen() noexcept
{
    return ColorRGB(0.0f, 1.0f, 0.0f);
}

inline ColorRGB ColorRGB::GetBlue() noexcept
{
    return ColorRGB(0.0f, 0.0f, 1.0f);
}

inline ColorRGB ColorRGB::GetYellow() noexcept
{
    return ColorRGB(1.0f, 1.0f, 0.0f);
}

inline ColorRGB ColorRGB::GetCyan() noexcept
{
    return ColorRGB(0.0f, 1.0f, 1.0f);
}

inline ColorRGB ColorRGB::GetMagenta() noexcept
{
    return ColorRGB(1.0f, 0.0f, 1.0f);
}

inline ColorRGB ColorRGB::GetGray() noexcept
{
    return ColorRGB(0.5f, 0.5f, 0.5f);
}

inline ColorRGB ColorRGB::GetGrey() noexcept
{
    return GetGray();
}

inline float ColorRGB::LinearToGammaSpace(float value_) noexcept
{
    return std::pow(std::max(0.0f, value_), 1.0f / 2.2f);
}

inline float ColorRGB::GammaToLinearSpace(float value_) noexcept
{
    return std::pow(std::max(0.0f, value_), 2.2f);
}

inline ColorRGB ColorRGB::GetGamma() const noexcept
{
    return ColorRGB(
        LinearToGammaSpace(x),
        LinearToGammaSpace(y),
        LinearToGammaSpace(z));
}

inline ColorRGB ColorRGB::GetLinear() const noexcept
{
    return ColorRGB(
        GammaToLinearSpace(x),
        GammaToLinearSpace(y),
        GammaToLinearSpace(z));
}

inline float ColorRGB::GetGrayscale() const noexcept
{
    return 0.299f * x + 0.587f * y + 0.114f * z;
}

inline float ColorRGB::GetMaxColorComponent() const noexcept
{
    return std::max(x, std::max(y, z));
}

inline ColorRGB ColorRGB::HSVToRGB(float h_, float s_, float v_) noexcept
{
    return HSVToRGB(h_, s_, v_, false);
}

inline ColorRGB ColorRGB::HSVToRGB(float h_, float s_, float v_, bool hdr_) noexcept
{
    h_ = h_ - std::floor(h_);
    s_ = std::clamp(s_, 0.0f, 1.0f);
    if (!hdr_)
    {
        v_ = std::clamp(v_, 0.0f, 1.0f);
    }

    if (s_ <= Mathf::Epsilon)
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

inline void ColorRGB::RGBToHSV(const ColorRGB& rgbColor_, float& h_, float& s_, float& v_) noexcept
{
    const float r{ rgbColor_.x };
    const float g{ rgbColor_.y };
    const float b{ rgbColor_.z };

    const float maxV{ std::max(r, std::max(g, b)) };
    const float minV{ std::min(r, std::min(g, b)) };
    const float delta{ maxV - minV };

    v_ = maxV;

    if (delta <= Mathf::Epsilon)
    {
        h_ = 0.0f;
        s_ = 0.0f;
        return;
    }

    s_ = (maxV <= Mathf::Epsilon) ? 0.0f : (delta / maxV);

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

inline ColorRGB ColorRGB::Lerp(const ColorRGB& a_, const ColorRGB& b_, float t_) noexcept
{
    return LerpUnclamped(a_, b_, Mathf::Clamp(t_, 0.0f, 1.0f));
}

inline ColorRGB ColorRGB::LerpUnclamped(const ColorRGB& a_, const ColorRGB& b_, float t_) noexcept
{
    return ColorRGB(
        Mathf::Lerp(a_.x, b_.x, t_),
        Mathf::Lerp(a_.y, b_.y, t_),
        Mathf::Lerp(a_.z, b_.z, t_));
}

inline bool ColorRGB::IsApproximately(const ColorRGB& lhs_, const ColorRGB& rhs_, float epsilon_) noexcept
{
    return std::abs(lhs_.x - rhs_.x) <= epsilon_
        && std::abs(lhs_.y - rhs_.y) <= epsilon_
        && std::abs(lhs_.z - rhs_.z) <= epsilon_;
}

inline bool ColorRGB::operator==(const ColorRGB& other_) const noexcept
{
    return IsApproximately(*this, other_);
}

inline bool ColorRGB::operator!=(const ColorRGB& other_) const noexcept
{
    return !(*this == other_);
}

inline bool ColorRGB::IsFinite() const noexcept
{
    return std::isfinite(x) && std::isfinite(y) && std::isfinite(z);
}

inline bool ColorRGB::IsHDR() const noexcept
{
    return x > 1.0f || y > 1.0f || z > 1.0f;
}

#include "Vector3D.h"
#include "Vector4D.h"
#include "ColorRGBA.h"

inline ColorRGB::ColorRGB(const Vector3D& vector_) noexcept
    : DirectX::XMFLOAT3(vector_.x, vector_.y, vector_.z)
{
}

inline ColorRGB::ColorRGB(const Vector4D& vector_) noexcept
    : DirectX::XMFLOAT3(vector_.x, vector_.y, vector_.z)
{
}

inline ColorRGB::ColorRGB(const ColorRGBA& color_) noexcept
    : DirectX::XMFLOAT3(color_.x, color_.y, color_.z)
{
}

inline Vector3D ColorRGB::ToVector3D() const noexcept
{
    return Vector3D(x, y, z);
}

inline Vector4D ColorRGB::ToVector4D(float alpha_) const noexcept
{
    return Vector4D(x, y, z, alpha_);
}

inline ColorRGBA ColorRGB::ToColorRGBA(float alpha_) const noexcept
{
    return ColorRGBA(*this, alpha_);
}

