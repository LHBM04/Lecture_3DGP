#pragma once

#include "Math.hpp"

struct Vector3D;
struct Vector4D;
struct ColorRGB;

struct ColorRGBA : public DirectX::XMFLOAT4
{
    ColorRGBA();
    explicit ColorRGBA(float value_);
    explicit ColorRGBA(float r_, float g_, float b_);
    explicit ColorRGBA(float r_, float g_, float b_, float a_);
    explicit ColorRGBA(const ColorRGB& rgb_, float a_ = 1.0f);
	explicit ColorRGBA(const Vector3D& vector_, float alpha_ = 1.0f);
	explicit ColorRGBA(const Vector4D& vector_);
    ColorRGBA(const ColorRGBA& color_);
	ColorRGBA(ColorRGBA&& color_) noexcept;

	ColorRGBA& operator=(const ColorRGBA& other_);
    ColorRGBA& operator=(ColorRGBA&& other_) noexcept;
	operator COLORREF() const;

	static ColorRGBA GetClear();
	static ColorRGBA GetBlack();
	static ColorRGBA GetWhite();
	static ColorRGBA GetRed();
	static ColorRGBA GetGreen();
	static ColorRGBA GetBlue();
	static ColorRGBA GetYellow();
	static ColorRGBA GetCyan();
	static ColorRGBA GetMagenta();
	static ColorRGBA GetGray();
	static ColorRGBA GetGrey();

	ColorRGBA GetGamma() const;
	ColorRGBA GetLinear() const;

	float GetGrayscale() const;
	float GetMaxColorComponent() const;

	static ColorRGBA HSVToRGB(float h_, float s_, float v_);
	static ColorRGBA HSVToRGB(float h_, float s_, float v_, bool hdr_);
	static void RGBToHSV(const ColorRGBA& rgbColor_, float& h_, float& s_, float& v_);

	static ColorRGBA Lerp(const ColorRGBA& a_, const ColorRGBA& b_, float t_);
	static ColorRGBA LerpUnclamped(const ColorRGBA& a_, const ColorRGBA& b_, float t_);

	static bool IsApproximately(const ColorRGBA& lhs_, const ColorRGBA& rhs_, float epsilon_ = Mathf::EPSILON);

    bool operator==(const ColorRGBA& other_) const;
    bool operator!=(const ColorRGBA& other_) const;

	bool IsFinite() const;
	bool IsHDR() const;
	bool IsTransparent(float epsilon_ = Mathf::EPSILON) const;
	bool IsOpaque(float epsilon_ = Mathf::EPSILON) const;

	Vector3D ToVector3D() const;
	Vector4D ToVector4D() const;
	ColorRGB ToColorRGB() const;
};
