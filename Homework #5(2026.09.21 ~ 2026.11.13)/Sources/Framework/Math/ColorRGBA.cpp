#include "Precompiled.hpp"
#include "ColorRGBA.hpp"

#include "ColorRGB.hpp"

namespace TUK::Framework
{
	template <ScalarType TScalar>
	ColorRGBA<TScalar>::ColorRGBA() noexcept
		: value(static_cast<TScalar>(0), static_cast<TScalar>(0), static_cast<TScalar>(0), static_cast<TScalar>(std::same_as<TScalar, std::int32_t> ? 255 : 1))
	{
	}

	template <ScalarType TScalar>
	ColorRGBA<TScalar>::ColorRGBA(TScalar value) noexcept
		: value(value, value, value, value)
	{
	}

	template <ScalarType TScalar>
	ColorRGBA<TScalar>::ColorRGBA(TScalar r, TScalar g, TScalar b, TScalar a) noexcept
		: value(r, g, b, a)
	{
	}

	template <ScalarType TScalar>
	ColorRGBA<TScalar>::ColorRGBA(const TScalar* values) noexcept
		: value(static_cast<TScalar>(0), static_cast<TScalar>(0), static_cast<TScalar>(0), static_cast<TScalar>(std::same_as<TScalar, std::int32_t> ? 255 : 1))
	{
		assert(values);
		value = decltype(value)(values);
	}

	template <ScalarType TScalar>
	ColorRGBA<TScalar>::ColorRGBA(const ColorRGBA<TScalar>& other) noexcept
		: value(other.value)
	{
	}

	template <ScalarType TScalar>
	ColorRGBA<TScalar>::ColorRGBA(ColorRGBA<TScalar>&& other) noexcept
		: value(std::move(other.value))
	{
	}

	template <ScalarType TScalar>
	ColorRGBA<TScalar>::ColorRGBA(const ColorRGB<TScalar>& rgb, TScalar a) noexcept
		: value(rgb.GetR(), rgb.GetG(), rgb.GetB(), a)
	{
	}

	template <ScalarType TScalar>
	ColorRGBA<TScalar>::ColorRGBA(const Vector3D<TScalar>& vector, TScalar alpha) noexcept
		: value(vector.GetX(), vector.GetY(), vector.GetZ(), alpha)
	{
	}

	template <ScalarType TScalar>
	ColorRGBA<TScalar>::ColorRGBA(const Vector4D<TScalar>& vector) noexcept
		: value(vector.GetX(), vector.GetY(), vector.GetZ(), vector.GetW())
	{
	}

	template <ScalarType TScalar>
	ColorRGBA<TScalar>& ColorRGBA<TScalar>::operator=(const ColorRGBA<TScalar>& other) noexcept
	{
		value.x = other.value.x;
		value.y = other.value.y;
		value.z = other.value.z;
		value.w = other.value.w;
		return *this;
	}

	template <ScalarType TScalar>
	ColorRGBA<TScalar>& ColorRGBA<TScalar>::operator=(ColorRGBA<TScalar>&& other) noexcept
	{
		value.x = other.value.x;
		value.y = other.value.y;
		value.z = other.value.z;
		value.w = other.value.w;
		return *this;
	}

	template <ScalarType TScalar>
	ColorRGBA<TScalar>& ColorRGBA<TScalar>::operator=(const Vector4D<TScalar>& other) noexcept
	{
		value.x = other.GetX();
		value.y = other.GetY();
		value.z = other.GetZ();
		value.w = other.GetW();
		return *this;
	}

	template <ScalarType TScalar>
	ColorRGBA<TScalar>::operator Vector4D<TScalar>() const noexcept
	{
		return Vector4D<TScalar>(value.x, value.y, value.z, value.w);
	}

	template <ScalarType TScalar>
	ColorRGBA<TScalar> ColorRGBA<TScalar>::operator+(const ColorRGBA<TScalar>& other) const noexcept
	{
		if constexpr (std::same_as<TScalar, std::int32_t>)
		{
			return ColorRGBA<TScalar>(value.x + other.value.x, value.y + other.value.y, value.z + other.value.z, value.w + other.value.w);
		}
		else
		{
			ColorRGBA<TScalar> result{};
			Store(result, DirectX::XMVectorAdd(Load(*this), Load(other)));
			return result;
		}
	}

	template <ScalarType TScalar>
	ColorRGBA<TScalar>& ColorRGBA<TScalar>::operator+=(const ColorRGBA<TScalar>& other) noexcept
	{
		*this = *this + other;
		return *this;
	}

	template <ScalarType TScalar>
	ColorRGBA<TScalar> ColorRGBA<TScalar>::operator-(const ColorRGBA<TScalar>& other) const noexcept
	{
		if constexpr (std::same_as<TScalar, std::int32_t>)
		{
			return ColorRGBA<TScalar>(value.x - other.value.x, value.y - other.value.y, value.z - other.value.z, value.w - other.value.w);
		}
		else
		{
			ColorRGBA<TScalar> result{};
			Store(result, DirectX::XMVectorSubtract(Load(*this), Load(other)));
			return result;
		}
	}

	template <ScalarType TScalar>
	ColorRGBA<TScalar>& ColorRGBA<TScalar>::operator-=(const ColorRGBA<TScalar>& other) noexcept
	{
		*this = *this - other;
		return *this;
	}

	template <ScalarType TScalar>
	ColorRGBA<TScalar> ColorRGBA<TScalar>::operator*(const ColorRGBA<TScalar>& other) const noexcept
	{
		if constexpr (std::same_as<TScalar, std::int32_t>)
		{
			return ColorRGBA<TScalar>(value.x * other.value.x, value.y * other.value.y, value.z * other.value.z, value.w * other.value.w);
		}
		else
		{
			ColorRGBA<TScalar> result{};
			Store(result, DirectX::XMVectorMultiply(Load(*this), Load(other)));
			return result;
		}
	}

	template <ScalarType TScalar>
	ColorRGBA<TScalar> ColorRGBA<TScalar>::operator*(TScalar scalar) const noexcept
	{
		if constexpr (std::same_as<TScalar, std::int32_t>)
		{
			return ColorRGBA<TScalar>(value.x * scalar, value.y * scalar, value.z * scalar, value.w * scalar);
		}
		else
		{
			ColorRGBA<TScalar> result{};
			Store(result, DirectX::XMVectorScale(Load(*this), scalar));
			return result;
		}
	}

	template <ScalarType TScalar>
	ColorRGBA<TScalar>& ColorRGBA<TScalar>::operator*=(const ColorRGBA<TScalar>& other) noexcept
	{
		*this = *this * other;
		return *this;
	}

	template <ScalarType TScalar>
	ColorRGBA<TScalar>& ColorRGBA<TScalar>::operator*=(TScalar scalar) noexcept
	{
		*this = *this * scalar;
		return *this;
	}

	template <ScalarType TScalar>
	ColorRGBA<TScalar> ColorRGBA<TScalar>::operator/(TScalar scalar) const noexcept
	{
		if constexpr (std::same_as<TScalar, std::int32_t>)
		{
			assert(scalar != TScalar{});
			return ColorRGBA<TScalar>(value.x / scalar, value.y / scalar, value.z / scalar, value.w / scalar);
		}
		else
		{
			assert(scalar != 0.0f);
			ColorRGBA<TScalar> result{};
			Store(result, DirectX::XMVectorScale(Load(*this), 1.0f / scalar));
			return result;
		}
	}

	template <ScalarType TScalar>
	ColorRGBA<TScalar>& ColorRGBA<TScalar>::operator/=(TScalar scalar) noexcept
	{
		*this = *this / scalar;
		return *this;
	}

	template <ScalarType TScalar>
	bool ColorRGBA<TScalar>::operator==(const ColorRGBA<TScalar>& other) const noexcept
	{
		if constexpr (std::same_as<TScalar, std::int32_t>)
		{
			return value.x == other.value.x && value.y == other.value.y && value.z == other.value.z && value.w == other.value.w;
		}
		else
		{
			return std::abs(value.x - other.value.x) < std::numeric_limits<float>::epsilon() &&
			std::abs(value.y - other.value.y) < std::numeric_limits<float>::epsilon() &&
			std::abs(value.z - other.value.z) < std::numeric_limits<float>::epsilon() &&
			std::abs(value.w - other.value.w) < std::numeric_limits<float>::epsilon();
		}
	}

	template <ScalarType TScalar>
	bool ColorRGBA<TScalar>::operator!=(const ColorRGBA<TScalar>& other) const noexcept
	{
		return !(*this == other);
	}

	template <ScalarType TScalar>
	TScalar ColorRGBA<TScalar>::GetR() const noexcept
	{
		return value.x;
	}

	template <ScalarType TScalar>
	void ColorRGBA<TScalar>::SetR(TScalar component) noexcept
	{
		value.x = component;
	}

	template <ScalarType TScalar>
	TScalar ColorRGBA<TScalar>::GetG() const noexcept
	{
		return value.y;
	}

	template <ScalarType TScalar>
	void ColorRGBA<TScalar>::SetG(TScalar component) noexcept
	{
		value.y = component;
	}

	template <ScalarType TScalar>
	TScalar ColorRGBA<TScalar>::GetB() const noexcept
	{
		return value.z;
	}

	template <ScalarType TScalar>
	void ColorRGBA<TScalar>::SetB(TScalar component) noexcept
	{
		value.z = component;
	}

	template <ScalarType TScalar>
	TScalar ColorRGBA<TScalar>::GetA() const noexcept
	{
		return value.w;
	}

	template <ScalarType TScalar>
	void ColorRGBA<TScalar>::SetA(TScalar component) noexcept
	{
		value.w = component;
	}

	template <ScalarType TScalar>
	void ColorRGBA<TScalar>::Set(TScalar r, TScalar g, TScalar b, TScalar a) noexcept
	{
		value.x = r;
		value.y = g;
		value.z = b;
		value.w = a;
	}

	template <ScalarType TScalar>
	bool ColorRGBA<TScalar>::IsTransparent(float epsilon) const noexcept
	{
		return value.w <= epsilon;
	}

	template <ScalarType TScalar>
	bool ColorRGBA<TScalar>::IsOpaque(float epsilon) const noexcept
	{
		constexpr TScalar maximum = static_cast<TScalar>(std::same_as<TScalar, std::int32_t> ? 255 : 1);
		return value.w >= maximum - epsilon;
	}

	template <ScalarType TScalar>
	Vector4D<TScalar> ColorRGBA<TScalar>::ToVector4D() const noexcept
	{
		return Vector4D<TScalar>(value.x, value.y, value.z, value.w);
	}

	template <ScalarType TScalar>
	ColorRGB<TScalar> ColorRGBA<TScalar>::ToColorRGB() const noexcept
	{
		return ColorRGB<TScalar>(value.x, value.y, value.z);
	}

	template <ScalarType TScalar>
	ColorRGBA<TScalar> ColorRGBA<TScalar>::GetBlack() noexcept
	{
		if constexpr (std::same_as<TScalar, std::int32_t>)
		{
			return ColorRGBA<TScalar>(0, 0, 0, 255);
		}
		else
		{
			return ColorRGBA<TScalar>(0.0f, 0.0f, 0.0f, 1.0f);
		}
	}

	template <ScalarType TScalar>
	ColorRGBA<TScalar> ColorRGBA<TScalar>::GetWhite() noexcept
	{
		if constexpr (std::same_as<TScalar, std::int32_t>)
		{
			return ColorRGBA<TScalar>(255, 255, 255, 255);
		}
		else
		{
			return ColorRGBA<TScalar>(1.0f, 1.0f, 1.0f, 1.0f);
		}
	}

	template <ScalarType TScalar>
	ColorRGBA<TScalar> ColorRGBA<TScalar>::GetRed() noexcept
	{
		if constexpr (std::same_as<TScalar, std::int32_t>)
		{
			return ColorRGBA<TScalar>(255, 0, 0, 255);
		}
		else
		{
			return ColorRGBA<TScalar>(1.0f, 0.0f, 0.0f, 1.0f);
		}
	}

	template <ScalarType TScalar>
	ColorRGBA<TScalar> ColorRGBA<TScalar>::GetGreen() noexcept
	{
		if constexpr (std::same_as<TScalar, std::int32_t>)
		{
			return ColorRGBA<TScalar>(0, 255, 0, 255);
		}
		else
		{
			return ColorRGBA<TScalar>(0.0f, 1.0f, 0.0f, 1.0f);
		}
	}

	template <ScalarType TScalar>
	ColorRGBA<TScalar> ColorRGBA<TScalar>::GetBlue() noexcept
	{
		if constexpr (std::same_as<TScalar, std::int32_t>)
		{
			return ColorRGBA<TScalar>(0, 0, 255, 255);
		}
		else
		{
			return ColorRGBA<TScalar>(0.0f, 0.0f, 1.0f, 1.0f);
		}
	}

	template <ScalarType TScalar>
	ColorRGBA<TScalar> ColorRGBA<TScalar>::GetYellow() noexcept
	{
		if constexpr (std::same_as<TScalar, std::int32_t>)
		{
			return ColorRGBA<TScalar>(255, 235, 4, 255);
		}
		else
		{
			return ColorRGBA<TScalar>(1.0f, 0.92f, 0.016f, 1.0f);
		}
	}

	template <ScalarType TScalar>
	ColorRGBA<TScalar> ColorRGBA<TScalar>::GetCyan() noexcept
	{
		if constexpr (std::same_as<TScalar, std::int32_t>)
		{
			return ColorRGBA<TScalar>(0, 255, 255, 255);
		}
		else
		{
			return ColorRGBA<TScalar>(0.0f, 1.0f, 1.0f, 1.0f);
		}
	}

	template <ScalarType TScalar>
	ColorRGBA<TScalar> ColorRGBA<TScalar>::GetMagenta() noexcept
	{
		if constexpr (std::same_as<TScalar, std::int32_t>)
		{
			return ColorRGBA<TScalar>(255, 0, 255, 255);
		}
		else
		{
			return ColorRGBA<TScalar>(1.0f, 0.0f, 1.0f, 1.0f);
		}
	}

	template <ScalarType TScalar>
	ColorRGBA<TScalar> ColorRGBA<TScalar>::GetClear() noexcept
	{
		if constexpr (std::same_as<TScalar, std::int32_t>)
		{
			return ColorRGBA<TScalar>(0, 0, 0, 0);
		}
		else
		{
			return ColorRGBA<TScalar>(0.0f, 0.0f, 0.0f, 0.0f);
		}
	}

	template <ScalarType TScalar>
	DirectX::XMVECTOR ColorRGBA<TScalar>::Load(const ColorRGBA<TScalar>& color) noexcept
		requires std::same_as<TScalar, f32>
	{
		return DirectX::XMLoadFloat4(&color.value);
	}

	template <ScalarType TScalar>
	void ColorRGBA<TScalar>::Store(ColorRGBA<TScalar>& destination, DirectX::XMVECTOR source) noexcept
		requires std::same_as<TScalar, f32>
	{
		DirectX::XMStoreFloat4(&destination.value, source);
	}

	template <ScalarType TScalar>
	bool ColorRGBA<TScalar>::IsApproximately(const ColorRGBA<TScalar>& lhs, const ColorRGBA<TScalar>& rhs, float epsilon) noexcept
		requires std::same_as<TScalar, f32>
	{
		return std::abs(lhs.value.x - rhs.value.x) <= epsilon &&
		std::abs(lhs.value.y - rhs.value.y) <= epsilon &&
		std::abs(lhs.value.z - rhs.value.z) <= epsilon &&
		std::abs(lhs.value.w - rhs.value.w) <= epsilon;
	}

	template <ScalarType TScalar>
	ColorRGBA<TScalar> ColorRGBA<TScalar>::Lerp(const ColorRGBA<TScalar>& start, const ColorRGBA<TScalar>& end, float t) noexcept
		requires std::same_as<TScalar, f32>
	{
		ColorRGBA<TScalar> result{};
		Store(result, DirectX::XMVectorLerp(Load(start), Load(end), t));
		return result;
	}

	template class ColorRGBA<std::int32_t>;
	template class ColorRGBA<float>;
}
