#pragma once

#include <concepts>
#include <limits>
#include <type_traits>

#include <directxmath/DirectXMath.h>

#include "Vector3D.hpp"
#include "Vector4D.hpp"

#include "../Core/Common.hpp"

namespace TUK::Framework
{
	template <class TValue>
		requires ScalarType<TValue>
	class ColorRGBA;

	template <class TValue>
		requires ScalarType<TValue>
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

		[[nodiscard]] ColorRGB GetGamma() const noexcept requires std::same_as<TValue, f32>;
		[[nodiscard]] ColorRGB GetLinear() const noexcept requires std::same_as<TValue, f32>;

		[[nodiscard]] f32 GetGrayscale() const noexcept requires std::same_as<TValue, f32>;
		[[nodiscard]] TValue GetMaxColorComponent() const noexcept;

		[[nodiscard]] bool IsFinite() const noexcept;
		[[nodiscard]] bool IsHDR() const noexcept;

		[[nodiscard]] Vector3D<TValue> ToVector3D() const noexcept;
		[[nodiscard]] Vector4D<TValue> ToVector4D(TValue alpha) const noexcept;
		[[nodiscard]] ColorRGBA<TValue> ToColorRGBA(TValue alpha = static_cast<TValue>(std::same_as<TValue, i32> ? 255 : 1)) const noexcept;

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

		[[nodiscard]] static DirectX::XMVECTOR Load(const ColorRGB& color) noexcept requires std::same_as<TValue, f32>;
		static void Store(ColorRGB& destination, DirectX::XMVECTOR source) noexcept requires std::same_as<TValue, f32>;

		[[nodiscard]] static bool IsApproximately(const ColorRGB& lhs, const ColorRGB& rhs, f32 epsilon = std::numeric_limits<f32>::epsilon()) noexcept requires std::same_as<TValue, f32>;

		[[nodiscard]] static f32 LinearToGammaSpace(f32 value) noexcept requires std::same_as<TValue, f32>;
		[[nodiscard]] static f32 GammaToLinearSpace(f32 value) noexcept requires std::same_as<TValue, f32>;

		[[nodiscard]] static ColorRGB HSVToRGB(f32 h, f32 s, f32 v) noexcept requires std::same_as<TValue, f32>;
		[[nodiscard]] static ColorRGB HSVToRGB(f32 h, f32 s, f32 v, bool hdr) noexcept requires std::same_as<TValue, f32>;
		static void RGBToHSV(const ColorRGB& rgbColor, f32& h, f32& s, f32& v) noexcept requires std::same_as<TValue, f32>;

		[[nodiscard]] static ColorRGB Lerp(const ColorRGB& a, const ColorRGB& b, f32 t) noexcept requires std::same_as<TValue, f32>;
		[[nodiscard]] static ColorRGB LerpUnclamped(const ColorRGB& a, const ColorRGB& b, f32 t) noexcept requires std::same_as<TValue, f32>;

	private:
		std::conditional_t<std::same_as<TValue, i32>, DirectX::XMINT3, DirectX::XMFLOAT3> value;
	};

	extern template class ColorRGB<i32>;
	extern template class ColorRGB<f32>;
}
