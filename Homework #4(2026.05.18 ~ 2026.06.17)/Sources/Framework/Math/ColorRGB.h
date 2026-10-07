#pragma once

#include <concepts>
#include <limits>
#include <type_traits>

#include <DirectXMath.h>

#include "Vector3D.h"
#include "Vector4D.h"

namespace TUK::Framework
{
	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	class ColorRGBA;

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	class ColorRGB
	{
	public:
		ColorRGB() noexcept;
		explicit ColorRGB(TValue value_) noexcept;
		ColorRGB(TValue r_, TValue g_, TValue b_) noexcept;

		ColorRGB(const ColorRGB& color_) noexcept;
		ColorRGB(ColorRGB&& color_) noexcept;

		explicit ColorRGB(const Vector3D<TValue>& vector_) noexcept;
		explicit ColorRGB(const Vector4D<TValue>& vector_) noexcept;
		explicit ColorRGB(const ColorRGBA<TValue>& color_) noexcept;

		ColorRGB& operator=(const ColorRGB& other_) noexcept;
		ColorRGB& operator=(ColorRGB&& other_) noexcept;

		[[nodiscard]] bool operator==(const ColorRGB& other_) const noexcept;
		[[nodiscard]] bool operator!=(const ColorRGB& other_) const noexcept;

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
		[[nodiscard]] Vector4D<TValue> ToVector4D(TValue alpha_) const noexcept;
		[[nodiscard]] ColorRGBA<TValue> ToColorRGBA(TValue alpha_ = static_cast<TValue>(std::same_as<TValue, int> ? 255 : 1)) const noexcept;

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

		[[nodiscard]] static DirectX::XMVECTOR Load(const ColorRGB& color) noexcept requires std::same_as<TValue, float>;
		static void Store(ColorRGB& destination, DirectX::XMVECTOR source) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static bool IsApproximately(const ColorRGB& lhs_, const ColorRGB& rhs_, float epsilon_ = std::numeric_limits<float>::epsilon()) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static float LinearToGammaSpace(float value_) noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] static float GammaToLinearSpace(float value_) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static ColorRGB HSVToRGB(float h_, float s_, float v_) noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] static ColorRGB HSVToRGB(float h_, float s_, float v_, bool hdr_) noexcept requires std::same_as<TValue, float>;
		static void RGBToHSV(const ColorRGB& rgbColor_, float& h_, float& s_, float& v_) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static ColorRGB Lerp(const ColorRGB& a_, const ColorRGB& b_, float t_) noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] static ColorRGB LerpUnclamped(const ColorRGB& a_, const ColorRGB& b_, float t_) noexcept requires std::same_as<TValue, float>;

	private:
		std::conditional_t<std::same_as<TValue, int>, DirectX::XMINT3, DirectX::XMFLOAT3> value;
	};

	extern template class ColorRGB<int>;
	extern template class ColorRGB<float>;
}
