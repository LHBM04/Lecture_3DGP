#pragma once

#include <concepts>
#include <cstdint>
#include <limits>
#include <type_traits>

#include "Mathf.hpp"
#include "Vector3D.hpp"
#include "Vector4D.hpp"

namespace TUK::Framework
{
	template <IsNumeric TValue>
	class ColorRGB;

	template <IsNumeric TValue>
	class ColorRGBA
	{
	public:
		ColorRGBA() noexcept;
		explicit ColorRGBA(TValue value) noexcept;
		ColorRGBA(TValue r, TValue g, TValue b, TValue a = static_cast<TValue>(std::same_as<TValue, std::int32_t> ? 255 : 1)) noexcept;
		explicit ColorRGBA(const TValue* values) noexcept;

		ColorRGBA(const ColorRGBA& other) noexcept;
		ColorRGBA(ColorRGBA&& other) noexcept;

		ColorRGBA(const ColorRGB<TValue>& rgb, TValue a = static_cast<TValue>(std::same_as<TValue, std::int32_t> ? 255 : 1)) noexcept;
		explicit ColorRGBA(const Vector3D<TValue>& vector, TValue alpha = static_cast<TValue>(std::same_as<TValue, std::int32_t> ? 255 : 1)) noexcept;
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

		/** R Get/Setter */
		[[nodiscard]] TValue GetR() const noexcept;
		void SetR(TValue component) noexcept;

		/** G Get/Setter */
		[[nodiscard]] TValue GetG() const noexcept;
		void SetG(TValue component) noexcept;

		/** B Get/Setter */
		[[nodiscard]] TValue GetB() const noexcept;
		void SetB(TValue component) noexcept;

		/** A Get/Setter */
		[[nodiscard]] TValue GetA() const noexcept;
		void SetA(TValue component) noexcept;

		/** 전체 값 수정 */
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

		[[nodiscard]] static bool IsApproximately(const ColorRGBA& lhs, const ColorRGBA& rhs, float epsilon = std::numeric_limits<float>::epsilon()) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static ColorRGBA Lerp(const ColorRGBA& start, const ColorRGBA& end, float t) noexcept requires std::same_as<TValue, float>;

	private:
		TValue values[4];
	};

	extern template class ColorRGBA<std::int32_t>;
	extern template class ColorRGBA<float>;
}
