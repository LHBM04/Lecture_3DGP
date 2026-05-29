#pragma once

#include "MathF.h"

struct Vector3D;
struct Vector4D;
struct ColorRGBA;

struct ColorRGB : public DirectX::XMFLOAT3
{
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
