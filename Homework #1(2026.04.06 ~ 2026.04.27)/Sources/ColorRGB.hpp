#pragma once

#include "Math.hpp"

struct Vector3D;
struct Vector4D;
struct ColorRGBA;

struct ColorRGB : public DirectX::XMFLOAT3
{
    ColorRGB();
    ColorRGB(float value_);
    ColorRGB(float r_, float g_, float b_);
    ColorRGB(const Vector3D& vector_);
    ColorRGB(const Vector4D& vector_);
    ColorRGB(const ColorRGBA& color_);
    ColorRGB(const ColorRGB& color_);
    ColorRGB(ColorRGB&& color_) noexcept;

    ColorRGB& operator=(const ColorRGB& other_);
    ColorRGB& operator=(ColorRGB&& other_) noexcept;

    static ColorRGB GetBlack();
    static ColorRGB GetWhite();
    static ColorRGB GetRed();
    static ColorRGB GetGreen();
    static ColorRGB GetBlue();
    static ColorRGB GetYellow();
    static ColorRGB GetCyan();
    static ColorRGB GetMagenta();
    static ColorRGB GetGray();
    static ColorRGB GetGrey();

    static float LinearToGammaSpace(float value_);
    static float GammaToLinearSpace(float value_);

    ColorRGB GetGamma() const;
    ColorRGB GetLinear() const;

    float GetGrayscale() const;
    float GetMaxColorComponent() const;

    static ColorRGB HSVToRGB(float h_, float s_, float v_);
    static ColorRGB HSVToRGB(float h_, float s_, float v_, bool hdr_);
    static void RGBToHSV(const ColorRGB& rgbColor_, float& h_, float& s_, float& v_);

    static ColorRGB Lerp(const ColorRGB& a_, const ColorRGB& b_, float t_);
    static ColorRGB LerpUnclamped(const ColorRGB& a_, const ColorRGB& b_, float t_);

    static bool IsApproximately(const ColorRGB& lhs_, const ColorRGB& rhs_, float epsilon_ = Mathf::EPSILON);

    bool operator==(const ColorRGB& other_) const;
    bool operator!=(const ColorRGB& other_) const;

    bool IsFinite() const;
    bool IsHDR() const;

    Vector3D ToVector3D() const;
    Vector4D ToVector4D(float alpha_) const;
    ColorRGBA ToColorRGBA(float alpha_ = 1.0f) const;
};
