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
	template <ScalarType TScalar>
	class ColorRGB;

	template <ScalarType TScalar>
	class ColorRGBA
	{
	public:
		ColorRGBA() noexcept;
		explicit ColorRGBA(TScalar value) noexcept;
		ColorRGBA(TScalar r, TScalar g, TScalar b, TScalar a = static_cast<TScalar>(std::same_as<TScalar, i32> ? 255 : 1)) noexcept;
		explicit ColorRGBA(const TScalar* values) noexcept;

		ColorRGBA(const ColorRGBA& other) noexcept;
		ColorRGBA(ColorRGBA&& other) noexcept;

		ColorRGBA(const ColorRGB<TScalar>& rgb, TScalar a = static_cast<TScalar>(std::same_as<TScalar, i32> ? 255 : 1)) noexcept;
		explicit ColorRGBA(const Vector3D<TScalar>& vector, TScalar alpha = static_cast<TScalar>(std::same_as<TScalar, i32> ? 255 : 1)) noexcept;
		ColorRGBA(const Vector4D<TScalar>& vector) noexcept;

		ColorRGBA& operator=(const ColorRGBA& other) noexcept;
		ColorRGBA& operator=(ColorRGBA&& other) noexcept;
		ColorRGBA& operator=(const Vector4D<TScalar>& other) noexcept;

		operator Vector4D<TScalar>() const noexcept;

		ColorRGBA operator+(const ColorRGBA& other) const noexcept;
		ColorRGBA& operator+=(const ColorRGBA& other) noexcept;

		ColorRGBA operator-(const ColorRGBA& other) const noexcept;
		ColorRGBA& operator-=(const ColorRGBA& other) noexcept;

		ColorRGBA operator*(const ColorRGBA& other) const noexcept;
		ColorRGBA operator*(TScalar scalar) const noexcept;
		ColorRGBA& operator*=(const ColorRGBA& other) noexcept;
		ColorRGBA& operator*=(TScalar scalar) noexcept;

		ColorRGBA operator/(TScalar scalar) const noexcept;
		ColorRGBA& operator/=(TScalar scalar) noexcept;

		bool operator==(const ColorRGBA& other) const noexcept;
		bool operator!=(const ColorRGBA& other) const noexcept;

		[[nodiscard]] TScalar GetR() const noexcept;
		void SetR(TScalar component) noexcept;

		[[nodiscard]] TScalar GetG() const noexcept;
		void SetG(TScalar component) noexcept;

		[[nodiscard]] TScalar GetB() const noexcept;
		void SetB(TScalar component) noexcept;

		[[nodiscard]] TScalar GetA() const noexcept;
		void SetA(TScalar component) noexcept;

		void Set(TScalar r, TScalar g, TScalar b, TScalar a) noexcept;

		[[nodiscard]] bool IsTransparent(f32 epsilon = std::numeric_limits<f32>::epsilon()) const noexcept;
		[[nodiscard]] bool IsOpaque(f32 epsilon = std::numeric_limits<f32>::epsilon()) const noexcept;

		[[nodiscard]] Vector4D<TScalar> ToVector4D() const noexcept;
		[[nodiscard]] ColorRGB<TScalar> ToColorRGB() const noexcept;

		[[nodiscard]] static ColorRGBA GetBlack() noexcept;
		[[nodiscard]] static ColorRGBA GetWhite() noexcept;

		[[nodiscard]] static ColorRGBA GetRed() noexcept;
		[[nodiscard]] static ColorRGBA GetGreen() noexcept;
		[[nodiscard]] static ColorRGBA GetBlue() noexcept;

		[[nodiscard]] static ColorRGBA GetYellow() noexcept;
		[[nodiscard]] static ColorRGBA GetCyan() noexcept;
		[[nodiscard]] static ColorRGBA GetMagenta() noexcept;

		[[nodiscard]] static ColorRGBA GetClear() noexcept;

		static DirectX::XMVECTOR Load(const ColorRGBA& color) noexcept requires std::same_as<TScalar, f32>;
		static void Store(ColorRGBA& destination, DirectX::XMVECTOR source) noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] static bool IsApproximately(const ColorRGBA& lhs, const ColorRGBA& rhs, f32 epsilon = std::numeric_limits<f32>::epsilon()) noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] static ColorRGBA Lerp(const ColorRGBA& start, const ColorRGBA& end, f32 t) noexcept requires std::same_as<TScalar, f32>;

	private:
		std::conditional_t<std::same_as<TScalar, i32>, DirectX::XMINT4, DirectX::XMFLOAT4> value;
	};

	extern template class ColorRGBA<i32>;
	extern template class ColorRGBA<f32>;
}
