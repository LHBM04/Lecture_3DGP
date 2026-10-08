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
	class ColorRGB;

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	class ColorRGBA
	{
	public:
		ColorRGBA() noexcept;
		explicit ColorRGBA(TValue value) noexcept;
		ColorRGBA(TValue r, TValue g, TValue b, TValue a = static_cast<TValue>(std::same_as<TValue, int> ? 255 : 1)) noexcept;
		explicit ColorRGBA(const TValue* values) noexcept;

		ColorRGBA(const ColorRGBA& other) noexcept;
		ColorRGBA(ColorRGBA&& other) noexcept;

		ColorRGBA(const ColorRGB<TValue>& rgb, TValue a = static_cast<TValue>(std::same_as<TValue, int> ? 255 : 1)) noexcept;
		explicit ColorRGBA(const Vector3D<TValue>& vector, TValue alpha = static_cast<TValue>(std::same_as<TValue, int> ? 255 : 1)) noexcept;
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

		[[nodiscard]] bool IsTransparent(float epsilon = std::numeric_limits<float>::epsilon()) const noexcept;
		[[nodiscard]] bool IsOpaque(float epsilon = std::numeric_limits<float>::epsilon()) const noexcept;

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

		static DirectX::XMVECTOR Load(const ColorRGBA& color) noexcept requires std::same_as<TValue, float>;
		static void Store(ColorRGBA& destination, DirectX::XMVECTOR source) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static bool IsApproximately(const ColorRGBA& lhs, const ColorRGBA& rhs, float epsilon = std::numeric_limits<float>::epsilon()) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static ColorRGBA Lerp(const ColorRGBA& start, const ColorRGBA& end, float t) noexcept requires std::same_as<TValue, float>;

	private:
		std::conditional_t<std::same_as<TValue, int>, DirectX::XMINT4, DirectX::XMFLOAT4> value;
	};

	extern template class ColorRGBA<int>;
	extern template class ColorRGBA<float>;
}
