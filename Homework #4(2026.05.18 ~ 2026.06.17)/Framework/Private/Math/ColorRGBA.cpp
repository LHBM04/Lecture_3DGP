#include "Precompiled.hpp"
#include "ColorRGBA.hpp"

#include "ColorRGB.hpp"
#include "ColorRGBA_Impl.hpp"

namespace TUK::Framework
{
	template <IsNumeric TValue>
	ColorRGBA<TValue>::ColorRGBA() noexcept
		: values{static_cast<TValue>(0), static_cast<TValue>(0), static_cast<TValue>(0), static_cast<TValue>(std::same_as<TValue, int> ? 255 : 1)}
	{
	}

	template <IsNumeric TValue>
	ColorRGBA<TValue>::ColorRGBA(TValue value) noexcept
		: values{value, value, value, value}
	{
	}

	template <IsNumeric TValue>
	ColorRGBA<TValue>::ColorRGBA(TValue r, TValue g, TValue b, TValue a) noexcept
		: values{r, g, b, a}
	{
	}

	template <IsNumeric TValue>
	ColorRGBA<TValue>::ColorRGBA(const TValue* values) noexcept
		: values{static_cast<TValue>(0), static_cast<TValue>(0), static_cast<TValue>(0), static_cast<TValue>(std::same_as<TValue, int> ? 255 : 1)}
	{
		assert(values);
		std::copy_n(values, 4, this->values);
	}

	template <IsNumeric TValue>
	ColorRGBA<TValue>::ColorRGBA(const ColorRGBA<TValue>& other) noexcept
		: values{}
	{
		for (std::size_t index = 0; index < 4; ++index) values[index] = other.values[index];
	}

	template <IsNumeric TValue>
	ColorRGBA<TValue>::ColorRGBA(ColorRGBA<TValue>&& other) noexcept
		: values{}
	{
		for (std::size_t index = 0; index < 4; ++index) values[index] = other.values[index];
	}

	template <IsNumeric TValue>
	ColorRGBA<TValue>::ColorRGBA(const ColorRGB<TValue>& rgb, TValue a) noexcept
		: values{rgb.GetR(), rgb.GetG(), rgb.GetB(), a}
	{
	}

	template <IsNumeric TValue>
	ColorRGBA<TValue>::ColorRGBA(const Vector3D<TValue>& vector, TValue alpha) noexcept
		: values{vector.GetX(), vector.GetY(), vector.GetZ(), alpha}
	{
	}

	template <IsNumeric TValue>
	ColorRGBA<TValue>::ColorRGBA(const Vector4D<TValue>& vector) noexcept
		: values{vector.GetX(), vector.GetY(), vector.GetZ(), vector.GetW()}
	{
	}

	template <IsNumeric TValue>
	ColorRGBA<TValue>& ColorRGBA<TValue>::operator=(const ColorRGBA<TValue>& other) noexcept
	{
		values[0] = other.values[0];
		values[1] = other.values[1];
		values[2] = other.values[2];
		values[3] = other.values[3];
		return *this;
	}

	template <IsNumeric TValue>
	ColorRGBA<TValue>& ColorRGBA<TValue>::operator=(ColorRGBA<TValue>&& other) noexcept
	{
		values[0] = other.values[0];
		values[1] = other.values[1];
		values[2] = other.values[2];
		values[3] = other.values[3];
		return *this;
	}

	template <IsNumeric TValue>
	ColorRGBA<TValue>& ColorRGBA<TValue>::operator=(const Vector4D<TValue>& other) noexcept
	{
		values[0] = other.GetX();
		values[1] = other.GetY();
		values[2] = other.GetZ();
		values[3] = other.GetW();
		return *this;
	}

	template <IsNumeric TValue>
	ColorRGBA<TValue>::operator Vector4D<TValue>() const noexcept
	{
		return Vector4D<TValue>(values[0], values[1], values[2], values[3]);
	}

	template <IsNumeric TValue>
	ColorRGBA<TValue> ColorRGBA<TValue>::operator+(const ColorRGBA<TValue>& other) const noexcept
	{
		if constexpr (std::same_as<TValue, int>)
		{
			return ColorRGBA<TValue>(values[0] + other.values[0], values[1] + other.values[1], values[2] + other.values[2], values[3] + other.values[3]);
		}
		else
		{
			ColorRGBA<TValue> result{};
			Store(result, DirectX::XMVectorAdd(Load(*this), Load(other)));
			return result;
		}
	}

	template <IsNumeric TValue>
	ColorRGBA<TValue>& ColorRGBA<TValue>::operator+=(const ColorRGBA<TValue>& other) noexcept
	{
		*this = *this + other;
		return *this;
	}

	template <IsNumeric TValue>
	ColorRGBA<TValue> ColorRGBA<TValue>::operator-(const ColorRGBA<TValue>& other) const noexcept
	{
		if constexpr (std::same_as<TValue, int>)
		{
			return ColorRGBA<TValue>(values[0] - other.values[0], values[1] - other.values[1], values[2] - other.values[2], values[3] - other.values[3]);
		}
		else
		{
			ColorRGBA<TValue> result{};
			Store(result, DirectX::XMVectorSubtract(Load(*this), Load(other)));
			return result;
		}
	}

	template <IsNumeric TValue>
	ColorRGBA<TValue>& ColorRGBA<TValue>::operator-=(const ColorRGBA<TValue>& other) noexcept
	{
		*this = *this - other;
		return *this;
	}

	template <IsNumeric TValue>
	ColorRGBA<TValue> ColorRGBA<TValue>::operator*(const ColorRGBA<TValue>& other) const noexcept
	{
		if constexpr (std::same_as<TValue, int>)
		{
			return ColorRGBA<TValue>(values[0] * other.values[0], values[1] * other.values[1], values[2] * other.values[2], values[3] * other.values[3]);
		}
		else
		{
			ColorRGBA<TValue> result{};
			Store(result, DirectX::XMVectorMultiply(Load(*this), Load(other)));
			return result;
		}
	}

	template <IsNumeric TValue>
	ColorRGBA<TValue> ColorRGBA<TValue>::operator*(TValue scalar) const noexcept
	{
		if constexpr (std::same_as<TValue, int>)
		{
			return ColorRGBA<TValue>(values[0] * scalar, values[1] * scalar, values[2] * scalar, values[3] * scalar);
		}
		else
		{
			ColorRGBA<TValue> result{};
			Store(result, DirectX::XMVectorScale(Load(*this), scalar));
			return result;
		}
	}

	template <IsNumeric TValue>
	ColorRGBA<TValue>& ColorRGBA<TValue>::operator*=(const ColorRGBA<TValue>& other) noexcept
	{
		*this = *this * other;
		return *this;
	}

	template <IsNumeric TValue>
	ColorRGBA<TValue>& ColorRGBA<TValue>::operator*=(TValue scalar) noexcept
	{
		*this = *this * scalar;
		return *this;
	}

	template <IsNumeric TValue>
	ColorRGBA<TValue> ColorRGBA<TValue>::operator/(TValue scalar) const noexcept
	{
		assert(scalar != TValue{});
		if constexpr (std::same_as<TValue, int>)
		{
			return ColorRGBA<TValue>(values[0] / scalar, values[1] / scalar, values[2] / scalar, values[3] / scalar);
		}
		else
		{
			ColorRGBA<TValue> result{};
			Store(result, DirectX::XMVectorScale(Load(*this), 1.0f / scalar));
			return result;
		}
	}

	template <IsNumeric TValue>
	ColorRGBA<TValue>& ColorRGBA<TValue>::operator/=(TValue scalar) noexcept
	{
		*this = *this / scalar;
		return *this;
	}

	template <IsNumeric TValue>
	bool ColorRGBA<TValue>::operator==(const ColorRGBA<TValue>& other) const noexcept
	{
		if constexpr (std::same_as<TValue, int>)
		{
			return values[0] == other.values[0] && values[1] == other.values[1] && values[2] == other.values[2] && values[3] == other.values[3];
		}
		else
		{
			return std::abs(values[0] - other.values[0]) < std::numeric_limits<float>::epsilon() &&
			std::abs(values[1] - other.values[1]) < std::numeric_limits<float>::epsilon() &&
			std::abs(values[2] - other.values[2]) < std::numeric_limits<float>::epsilon() &&
			std::abs(values[3] - other.values[3]) < std::numeric_limits<float>::epsilon();
		}
	}

	template <IsNumeric TValue>
	bool ColorRGBA<TValue>::operator!=(const ColorRGBA<TValue>& other) const noexcept
	{
		return !(*this == other);
	}

	template <IsNumeric TValue>
	TValue ColorRGBA<TValue>::GetR() const noexcept
	{
		return values[0];
	}

	template <IsNumeric TValue>
	void ColorRGBA<TValue>::SetR(TValue component) noexcept
	{
		values[0] = component;
	}

	template <IsNumeric TValue>
	TValue ColorRGBA<TValue>::GetG() const noexcept
	{
		return values[1];
	}

	template <IsNumeric TValue>
	void ColorRGBA<TValue>::SetG(TValue component) noexcept
	{
		values[1] = component;
	}

	template <IsNumeric TValue>
	TValue ColorRGBA<TValue>::GetB() const noexcept
	{
		return values[2];
	}

	template <IsNumeric TValue>
	void ColorRGBA<TValue>::SetB(TValue component) noexcept
	{
		values[2] = component;
	}

	template <IsNumeric TValue>
	TValue ColorRGBA<TValue>::GetA() const noexcept
	{
		return values[3];
	}

	template <IsNumeric TValue>
	void ColorRGBA<TValue>::SetA(TValue component) noexcept
	{
		values[3] = component;
	}

	template <IsNumeric TValue>
	void ColorRGBA<TValue>::Set(TValue r, TValue g, TValue b, TValue a) noexcept
	{
		values[0] = r;
		values[1] = g;
		values[2] = b;
		values[3] = a;
	}

	template <IsNumeric TValue>
	bool ColorRGBA<TValue>::IsTransparent(float epsilon) const noexcept
	{
		return values[3] <= epsilon;
	}

	template <IsNumeric TValue>
	bool ColorRGBA<TValue>::IsOpaque(float epsilon) const noexcept
	{
		constexpr TValue maximum = static_cast<TValue>(std::same_as<TValue, int> ? 255 : 1);
		return values[3] >= maximum - epsilon;
	}

	template <IsNumeric TValue>
	Vector4D<TValue> ColorRGBA<TValue>::ToVector4D() const noexcept
	{
		return Vector4D<TValue>(values[0], values[1], values[2], values[3]);
	}

	template <IsNumeric TValue>
	ColorRGB<TValue> ColorRGBA<TValue>::ToColorRGB() const noexcept
	{
		return ColorRGB<TValue>(values[0], values[1], values[2]);
	}

	template <IsNumeric TValue>
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

	template <IsNumeric TValue>
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

	template <IsNumeric TValue>
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

	template <IsNumeric TValue>
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

	template <IsNumeric TValue>
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

	template <IsNumeric TValue>
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

	template <IsNumeric TValue>
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

	template <IsNumeric TValue>
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

	template <IsNumeric TValue>
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

	template <IsNumeric TValue>
	bool ColorRGBA<TValue>::IsApproximately(const ColorRGBA<TValue>& lhs, const ColorRGBA<TValue>& rhs, float epsilon) noexcept
		requires std::same_as<TValue, float>
	{
		return std::abs(lhs.values[0] - rhs.values[0]) <= epsilon &&
		std::abs(lhs.values[1] - rhs.values[1]) <= epsilon &&
		std::abs(lhs.values[2] - rhs.values[2]) <= epsilon &&
		std::abs(lhs.values[3] - rhs.values[3]) <= epsilon;
	}

	template <IsNumeric TValue>
	ColorRGBA<TValue> ColorRGBA<TValue>::Lerp(const ColorRGBA<TValue>& start, const ColorRGBA<TValue>& end, float t) noexcept
		requires std::same_as<TValue, float>
	{
		ColorRGBA<TValue> result;
		Store(result, DirectX::XMVectorLerp(Load(start), Load(end), t));
		return result;
	}

	template class ColorRGBA<int>;
	template class ColorRGBA<float>;
}
