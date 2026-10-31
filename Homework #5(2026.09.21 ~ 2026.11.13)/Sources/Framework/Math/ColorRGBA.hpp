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
	class ColorRGB;

	template <class TValue>
		requires ScalarType<TValue>
	class ColorRGBA
	{
	public:
		ColorRGBA() noexcept;
		explicit ColorRGBA(TValue value) noexcept;
		ColorRGBA(TValue r, TValue g, TValue b, TValue a = static_cast<TValue>(std::same_as<TValue, i32> ? 255 : 1)) noexcept;
		explicit ColorRGBA(const TValue* values) noexcept;

		ColorRGBA(const ColorRGBA& other) noexcept;
		ColorRGBA(ColorRGBA&& other) noexcept;

		ColorRGBA(const ColorRGB<TValue>& rgb, TValue a = static_cast<TValue>(std::same_as<TValue, i32> ? 255 : 1)) noexcept;
		explicit ColorRGBA(const Vector3D<TValue>& vector, TValue alpha = static_cast<TValue>(std::same_as<TValue, i32> ? 255 : 1)) noexcept;
		ColorRGBA(const Vector4D<TValue>& vector) noexcept;

		ColorRGBA& operator=(const ColorRGBA& other) noexcept;
		ColorRGBA& operator=(ColorRGBA&& other) noexcept;
		ColorRGBA& operator=(const Vector4D<TValue>& other) noexcept;

		operator Vector4D<TValue>() const noexcept;

		ColorRGBA operator+(const ColorRGBA& other) const noexcept;
		ColorRGBA& operator+=(const ColorRGBA& other) noexcept;

		ColorRGBA operator-(const ColorRGBA& other) const noexcept;
		ColorRGBA& operator-=(const ColorRGBA& other) noexcept;

		ColorRGBA operator*(const ColorRGBA& other) const noexcept;
		ColorRGBA operator*(TValue scalar) const noexcept;
		ColorRGBA& operator*=(const ColorRGBA& other) noexcept;
		ColorRGBA& operator*=(TValue scalar) noexcept;

		ColorRGBA operator/(TValue scalar) const noexcept;
		ColorRGBA& operator/=(TValue scalar) noexcept;

		bool operator==(const ColorRGBA& other) const noexcept;
		bool operator!=(const ColorRGBA& other) const noexcept;

		[[nodiscard]] TValue GetR() const noexcept;
		void SetR(TValue component) noexcept;

		[[nodiscard]] TValue GetG() const noexcept;
		void SetG(TValue component) noexcept;

		[[nodiscard]] TValue GetB() const noexcept;
		void SetB(TValue component) noexcept;

		[[nodiscard]] TValue GetA() const noexcept;
		void SetA(TValue component) noexcept;

		void Set(TValue r, TValue g, TValue b, TValue a) noexcept;

		[[nodiscard]] bool IsTransparent(f32 epsilon = std::numeric_limits<f32>::epsilon()) const noexcept;
		[[nodiscard]] bool IsOpaque(f32 epsilon = std::numeric_limits<f32>::epsilon()) const noexcept;

		[[nodiscard]] Vector4D<TValue> ToVector4D() const noexcept;
		[[nodiscard]] ColorRGB<TValue> ToColorRGB() const noexcept;

		[[nodiscard]] static ColorRGBA GetBlack() noexcept;
		[[nodiscard]] static ColorRGBA GetWhite() noexcept;

		[[nodiscard]] static ColorRGBA GetRed() noexcept;
		[[nodiscard]] static ColorRGBA GetGreen() noexcept;
		[[nodiscard]] static ColorRGBA GetBlue() noexcept;

		[[nodiscard]] static ColorRGBA GetYellow() noexcept;
		[[nodiscard]] static ColorRGBA GetCyan() noexcept;
		[[nodiscard]] static ColorRGBA GetMagenta() noexcept;

		[[nodiscard]] static ColorRGBA GetClear() noexcept;

		static DirectX::XMVECTOR Load(const ColorRGBA& color) noexcept requires std::same_as<TValue, f32>;
		static void Store(ColorRGBA& destination, DirectX::XMVECTOR source) noexcept requires std::same_as<TValue, f32>;

		[[nodiscard]] static bool IsApproximately(const ColorRGBA& lhs, const ColorRGBA& rhs, f32 epsilon = std::numeric_limits<f32>::epsilon()) noexcept requires std::same_as<TValue, f32>;

		[[nodiscard]] static ColorRGBA Lerp(const ColorRGBA& start, const ColorRGBA& end, f32 t) noexcept requires std::same_as<TValue, f32>;

	private:
		std::conditional_t<std::same_as<TValue, i32>, DirectX::XMINT4, DirectX::XMFLOAT4> value;
	};

	extern template class ColorRGBA<i32>;
	extern template class ColorRGBA<f32>;
}
