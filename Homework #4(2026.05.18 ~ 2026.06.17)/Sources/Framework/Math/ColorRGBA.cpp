#include "Precompiled.hpp"
#include "ColorRGBA.hpp"

#include "ColorRGB.hpp"

namespace TUK::Framework
{
	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue>::ColorRGBA() noexcept
		: value(static_cast<TValue>(0), static_cast<TValue>(0), static_cast<TValue>(0), static_cast<TValue>(std::same_as<TValue, int> ? 255 : 1))
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue>::ColorRGBA(TValue value) noexcept
		: value(value, value, value, value)
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue>::ColorRGBA(TValue r, TValue g, TValue b, TValue a) noexcept
		: value(r, g, b, a)
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue>::ColorRGBA(const TValue* values) noexcept
		: value(static_cast<TValue>(0), static_cast<TValue>(0), static_cast<TValue>(0), static_cast<TValue>(std::same_as<TValue, int> ? 255 : 1))
	{
		assert(values);
		value = decltype(value)(values);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue>::ColorRGBA(const ColorRGBA<TValue>& other) noexcept
		: value(other.value)
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue>::ColorRGBA(ColorRGBA<TValue>&& other) noexcept
		: value(std::move(other.value))
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue>::ColorRGBA(const ColorRGB<TValue>& rgb, TValue a) noexcept
		: value(rgb.GetR(), rgb.GetG(), rgb.GetB(), a)
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue>::ColorRGBA(const Vector3D<TValue>& vector, TValue alpha) noexcept
		: value(vector.GetX(), vector.GetY(), vector.GetZ(), alpha)
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue>::ColorRGBA(const Vector4D<TValue>& vector) noexcept
		: value(vector.GetX(), vector.GetY(), vector.GetZ(), vector.GetW())
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue>& ColorRGBA<TValue>::operator=(const ColorRGBA<TValue>& other) noexcept
	{
		value.x = other.value.x;
		value.y = other.value.y;
		value.z = other.value.z;
		value.w = other.value.w;
		return *this;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue>& ColorRGBA<TValue>::operator=(ColorRGBA<TValue>&& other) noexcept
	{
		value.x = other.value.x;
		value.y = other.value.y;
		value.z = other.value.z;
		value.w = other.value.w;
		return *this;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue>& ColorRGBA<TValue>::operator=(const Vector4D<TValue>& other) noexcept
	{
		value.x = other.GetX();
		value.y = other.GetY();
		value.z = other.GetZ();
		value.w = other.GetW();
		return *this;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue>::operator Vector4D<TValue>() const noexcept
	{
		return Vector4D<TValue>(value.x, value.y, value.z, value.w);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue> ColorRGBA<TValue>::operator+(const ColorRGBA<TValue>& other) const noexcept
	{
		if constexpr (std::same_as<TValue, int>)
		{
			return ColorRGBA<TValue>(value.x + other.value.x, value.y + other.value.y, value.z + other.value.z, value.w + other.value.w);
		}
		else
		{
			ColorRGBA<TValue> result{};
			Store(result, DirectX::XMVectorAdd(Load(*this), Load(other)));
			return result;
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue>& ColorRGBA<TValue>::operator+=(const ColorRGBA<TValue>& other) noexcept
	{
		*this = *this + other;
		return *this;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue> ColorRGBA<TValue>::operator-(const ColorRGBA<TValue>& other) const noexcept
	{
		if constexpr (std::same_as<TValue, int>)
		{
			return ColorRGBA<TValue>(value.x - other.value.x, value.y - other.value.y, value.z - other.value.z, value.w - other.value.w);
		}
		else
		{
			ColorRGBA<TValue> result{};
			Store(result, DirectX::XMVectorSubtract(Load(*this), Load(other)));
			return result;
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue>& ColorRGBA<TValue>::operator-=(const ColorRGBA<TValue>& other) noexcept
	{
		*this = *this - other;
		return *this;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue> ColorRGBA<TValue>::operator*(const ColorRGBA<TValue>& other) const noexcept
	{
		if constexpr (std::same_as<TValue, int>)
		{
			return ColorRGBA<TValue>(value.x * other.value.x, value.y * other.value.y, value.z * other.value.z, value.w * other.value.w);
		}
		else
		{
			ColorRGBA<TValue> result{};
			Store(result, DirectX::XMVectorMultiply(Load(*this), Load(other)));
			return result;
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue> ColorRGBA<TValue>::operator*(TValue scalar) const noexcept
	{
		if constexpr (std::same_as<TValue, int>)
		{
			return ColorRGBA<TValue>(value.x * scalar, value.y * scalar, value.z * scalar, value.w * scalar);
		}
		else
		{
			ColorRGBA<TValue> result{};
			Store(result, DirectX::XMVectorScale(Load(*this), scalar));
			return result;
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue>& ColorRGBA<TValue>::operator*=(const ColorRGBA<TValue>& other) noexcept
	{
		*this = *this * other;
		return *this;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue>& ColorRGBA<TValue>::operator*=(TValue scalar) noexcept
	{
		*this = *this * scalar;
		return *this;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue> ColorRGBA<TValue>::operator/(TValue scalar) const noexcept
	{
		if constexpr (std::same_as<TValue, int>)
		{
			assert(scalar != TValue{});
			return ColorRGBA<TValue>(value.x / scalar, value.y / scalar, value.z / scalar, value.w / scalar);
		}
		else
		{
			assert(scalar != 0.0f);
			ColorRGBA<TValue> result{};
			Store(result, DirectX::XMVectorScale(Load(*this), 1.0f / scalar));
			return result;
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue>& ColorRGBA<TValue>::operator/=(TValue scalar) noexcept
	{
		*this = *this / scalar;
		return *this;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	bool ColorRGBA<TValue>::operator==(const ColorRGBA<TValue>& other) const noexcept
	{
		if constexpr (std::same_as<TValue, int>)
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

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	bool ColorRGBA<TValue>::operator!=(const ColorRGBA<TValue>& other) const noexcept
	{
		return !(*this == other);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	TValue ColorRGBA<TValue>::GetR() const noexcept
	{
		return value.x;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	void ColorRGBA<TValue>::SetR(TValue component) noexcept
	{
		value.x = component;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	TValue ColorRGBA<TValue>::GetG() const noexcept
	{
		return value.y;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	void ColorRGBA<TValue>::SetG(TValue component) noexcept
	{
		value.y = component;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	TValue ColorRGBA<TValue>::GetB() const noexcept
	{
		return value.z;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	void ColorRGBA<TValue>::SetB(TValue component) noexcept
	{
		value.z = component;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	TValue ColorRGBA<TValue>::GetA() const noexcept
	{
		return value.w;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	void ColorRGBA<TValue>::SetA(TValue component) noexcept
	{
		value.w = component;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	void ColorRGBA<TValue>::Set(TValue r, TValue g, TValue b, TValue a) noexcept
	{
		value.x = r;
		value.y = g;
		value.z = b;
		value.w = a;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	bool ColorRGBA<TValue>::IsTransparent(float epsilon) const noexcept
	{
		return value.w <= epsilon;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	bool ColorRGBA<TValue>::IsOpaque(float epsilon) const noexcept
	{
		constexpr TValue maximum = static_cast<TValue>(std::same_as<TValue, int> ? 255 : 1);
		return value.w >= maximum - epsilon;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue> ColorRGBA<TValue>::ToVector4D() const noexcept
	{
		return Vector4D<TValue>(value.x, value.y, value.z, value.w);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGB<TValue> ColorRGBA<TValue>::ToColorRGB() const noexcept
	{
		return ColorRGB<TValue>(value.x, value.y, value.z);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue> ColorRGBA<TValue>::GetBlack() noexcept
	{
		if constexpr (std::same_as<TValue, int>)
		{
			return ColorRGBA<TValue>(0, 0, 0, 255);
		}
		else
		{
			return ColorRGBA<TValue>(0.0f, 0.0f, 0.0f, 1.0f);
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue> ColorRGBA<TValue>::GetWhite() noexcept
	{
		if constexpr (std::same_as<TValue, int>)
		{
			return ColorRGBA<TValue>(255, 255, 255, 255);
		}
		else
		{
			return ColorRGBA<TValue>(1.0f, 1.0f, 1.0f, 1.0f);
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue> ColorRGBA<TValue>::GetRed() noexcept
	{
		if constexpr (std::same_as<TValue, int>)
		{
			return ColorRGBA<TValue>(255, 0, 0, 255);
		}
		else
		{
			return ColorRGBA<TValue>(1.0f, 0.0f, 0.0f, 1.0f);
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue> ColorRGBA<TValue>::GetGreen() noexcept
	{
		if constexpr (std::same_as<TValue, int>)
		{
			return ColorRGBA<TValue>(0, 255, 0, 255);
		}
		else
		{
			return ColorRGBA<TValue>(0.0f, 1.0f, 0.0f, 1.0f);
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue> ColorRGBA<TValue>::GetBlue() noexcept
	{
		if constexpr (std::same_as<TValue, int>)
		{
			return ColorRGBA<TValue>(0, 0, 255, 255);
		}
		else
		{
			return ColorRGBA<TValue>(0.0f, 0.0f, 1.0f, 1.0f);
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue> ColorRGBA<TValue>::GetYellow() noexcept
	{
		if constexpr (std::same_as<TValue, int>)
		{
			return ColorRGBA<TValue>(255, 235, 4, 255);
		}
		else
		{
			return ColorRGBA<TValue>(1.0f, 0.92f, 0.016f, 1.0f);
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue> ColorRGBA<TValue>::GetCyan() noexcept
	{
		if constexpr (std::same_as<TValue, int>)
		{
			return ColorRGBA<TValue>(0, 255, 255, 255);
		}
		else
		{
			return ColorRGBA<TValue>(0.0f, 1.0f, 1.0f, 1.0f);
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue> ColorRGBA<TValue>::GetMagenta() noexcept
	{
		if constexpr (std::same_as<TValue, int>)
		{
			return ColorRGBA<TValue>(255, 0, 255, 255);
		}
		else
		{
			return ColorRGBA<TValue>(1.0f, 0.0f, 1.0f, 1.0f);
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue> ColorRGBA<TValue>::GetClear() noexcept
	{
		if constexpr (std::same_as<TValue, int>)
		{
			return ColorRGBA<TValue>(0, 0, 0, 0);
		}
		else
		{
			return ColorRGBA<TValue>(0.0f, 0.0f, 0.0f, 0.0f);
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	DirectX::XMVECTOR ColorRGBA<TValue>::Load(const ColorRGBA<TValue>& color) noexcept
		requires std::same_as<TValue, float>
	{
		return DirectX::XMLoadFloat4(&color.value);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	void ColorRGBA<TValue>::Store(ColorRGBA<TValue>& destination, DirectX::XMVECTOR source) noexcept
		requires std::same_as<TValue, float>
	{
		DirectX::XMStoreFloat4(&destination.value, source);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	bool ColorRGBA<TValue>::IsApproximately(const ColorRGBA<TValue>& lhs, const ColorRGBA<TValue>& rhs, float epsilon) noexcept
		requires std::same_as<TValue, float>
	{
		return std::abs(lhs.value.x - rhs.value.x) <= epsilon &&
		std::abs(lhs.value.y - rhs.value.y) <= epsilon &&
		std::abs(lhs.value.z - rhs.value.z) <= epsilon &&
		std::abs(lhs.value.w - rhs.value.w) <= epsilon;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue> ColorRGBA<TValue>::Lerp(const ColorRGBA<TValue>& start, const ColorRGBA<TValue>& end, float t) noexcept
		requires std::same_as<TValue, float>
	{
		ColorRGBA<TValue> result{};
		Store(result, DirectX::XMVectorLerp(Load(start), Load(end), t));
		return result;
	}

	template class ColorRGBA<int>;
	template class ColorRGBA<float>;
}
