#pragma once

#include <concepts>
#include <limits>
#include <type_traits>

#include "Mathf.hpp"
#include "Vector3D.hpp"
#include "Vector4D.hpp"

namespace TUK::Framework
{
	template <IsNumeric TValue>
	class ColorRGBA;

	template <IsNumeric TValue>
	class ColorRGB
	{
	public:
		ColorRGB() noexcept;
		explicit ColorRGB(TValue value) noexcept;
		ColorRGB(TValue r, TValue g, TValue b) noexcept;

		ColorRGB(const ColorRGB& color) noexcept;
		ColorRGB(ColorRGB&& color) noexcept;

		explicit ColorRGB(const Vector3D<TValue>& vector) noexcept;
		explicit ColorRGB(const Vector4D<TValue>& vector) noexcept;
		explicit ColorRGB(const ColorRGBA<TValue>& color) noexcept;

		ColorRGB& operator=(const ColorRGB& other) noexcept;
		ColorRGB& operator=(ColorRGB&& other) noexcept;

		[[nodiscard]] bool operator==(const ColorRGB& other) const noexcept;
		[[nodiscard]] bool operator!=(const ColorRGB& other) const noexcept;

		[[nodiscard]] TValue GetR() const noexcept;
		void SetR(TValue component) noexcept;

		[[nodiscard]] TValue GetG() const noexcept;
		void SetG(TValue component) noexcept;

		[[nodiscard]] TValue GetB() const noexcept;
		void SetB(TValue component) noexcept;

		void Set(TValue r, TValue g, TValue b) noexcept;

		[[nodiscard]] ColorRGB GetGamma() const noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] ColorRGB GetLinear() const noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] float GetGrayscale() const noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] TValue GetMaxColorComponent() const noexcept;

		[[nodiscard]] bool IsFinite() const noexcept;
		[[nodiscard]] bool IsHDR() const noexcept;

		[[nodiscard]] Vector3D<TValue> ToVector3D() const noexcept;
		[[nodiscard]] Vector4D<TValue> ToVector4D(TValue alpha) const noexcept;
		[[nodiscard]] ColorRGBA<TValue> ToColorRGBA(TValue alpha = static_cast<TValue>(std::same_as<TValue, int> ? 255 : 1)) const noexcept;

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

		[[nodiscard]] static bool IsApproximately(const ColorRGB& lhs, const ColorRGB& rhs, float epsilon = std::numeric_limits<float>::epsilon()) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static float LinearToGammaSpace(float value) noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] static float GammaToLinearSpace(float value) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static ColorRGB HSVToRGB(float h, float s, float v) noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] static ColorRGB HSVToRGB(float h, float s, float v, bool hdr) noexcept requires std::same_as<TValue, float>;
		static void RGBToHSV(const ColorRGB& rgbColor, float& h, float& s, float& v) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static ColorRGB Lerp(const ColorRGB& a, const ColorRGB& b, float t) noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] static ColorRGB LerpUnclamped(const ColorRGB& a, const ColorRGB& b, float t) noexcept requires std::same_as<TValue, float>;

	private:
		TValue values[3];
	};

	extern template class ColorRGB<int>;
	extern template class ColorRGB<float>;
}
