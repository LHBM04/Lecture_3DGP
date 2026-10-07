#include "Precompiled.h"
#include "ColorRGBA.h"

#include "ColorRGB.h"

namespace TUK::Framework
{
	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue>::ColorRGBA() noexcept
		: value(TValue{0}, TValue{0}, TValue{0}, static_cast<TValue>(std::same_as<TValue, int> ? 255 : 1))
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue>::ColorRGBA(TValue value_) noexcept
		: value(value_, value_, value_, value_)
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue>::ColorRGBA(TValue r_, TValue g_, TValue b_, TValue a_) noexcept
		: value(r_, g_, b_, a_)
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue>::ColorRGBA(const TValue* values_) noexcept
		: value(TValue{0}, TValue{0}, TValue{0}, static_cast<TValue>(std::same_as<TValue, int> ? 255 : 1))
	{
		assert(values_);
		value = decltype(value)(values_);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue>::ColorRGBA(const ColorRGBA<TValue>& other_) noexcept
		: value(other_.value)
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue>::ColorRGBA(ColorRGBA<TValue>&& other_) noexcept
		: value(std::move(other_.value))
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue>::ColorRGBA(const ColorRGB<TValue>& rgb_, TValue a_) noexcept
		: value(rgb_.GetR(), rgb_.GetG(), rgb_.GetB(), a_)
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue>::ColorRGBA(const Vector3D<TValue>& vector_, TValue alpha_) noexcept
		: value(vector_.GetX(), vector_.GetY(), vector_.GetZ(), alpha_)
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue>::ColorRGBA(const Vector4D<TValue>& vector_) noexcept
		: value(vector_.GetX(), vector_.GetY(), vector_.GetZ(), vector_.GetW())
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue>& ColorRGBA<TValue>::operator=(const ColorRGBA<TValue>& other_) noexcept
	{
		value.x = other_.value.x;
		value.y = other_.value.y;
		value.z = other_.value.z;
		value.w = other_.value.w;
		return *this;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue>& ColorRGBA<TValue>::operator=(ColorRGBA<TValue>&& other_) noexcept
	{
		value.x = other_.value.x;
		value.y = other_.value.y;
		value.z = other_.value.z;
		value.w = other_.value.w;
		return *this;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue>& ColorRGBA<TValue>::operator=(const Vector4D<TValue>& other_) noexcept
	{
		value.x = other_.GetX();
		value.y = other_.GetY();
		value.z = other_.GetZ();
		value.w = other_.GetW();
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
	ColorRGBA<TValue> ColorRGBA<TValue>::operator+(const ColorRGBA<TValue>& other_) const noexcept
	{
		if constexpr (std::same_as<TValue, int>)
		{
			return ColorRGBA<TValue>(value.x + other_.value.x, value.y + other_.value.y, value.z + other_.value.z, value.w + other_.value.w);
		}
		else
		{
			ColorRGBA<TValue> result;
			Store(result, DirectX::XMVectorAdd(Load(*this), Load(other_)));
			return result;
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue>& ColorRGBA<TValue>::operator+=(const ColorRGBA<TValue>& other_) noexcept
	{
		*this = *this + other_;
		return *this;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue> ColorRGBA<TValue>::operator-(const ColorRGBA<TValue>& other_) const noexcept
	{
		if constexpr (std::same_as<TValue, int>)
		{
			return ColorRGBA<TValue>(value.x - other_.value.x, value.y - other_.value.y, value.z - other_.value.z, value.w - other_.value.w);
		}
		else
		{
			ColorRGBA<TValue> result;
			Store(result, DirectX::XMVectorSubtract(Load(*this), Load(other_)));
			return result;
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue>& ColorRGBA<TValue>::operator-=(const ColorRGBA<TValue>& other_) noexcept
	{
		*this = *this - other_;
		return *this;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue> ColorRGBA<TValue>::operator*(const ColorRGBA<TValue>& other_) const noexcept
	{
		if constexpr (std::same_as<TValue, int>)
		{
			return ColorRGBA<TValue>(value.x * other_.value.x, value.y * other_.value.y, value.z * other_.value.z, value.w * other_.value.w);
		}
		else
		{
			ColorRGBA<TValue> result;
			Store(result, DirectX::XMVectorMultiply(Load(*this), Load(other_)));
			return result;
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue> ColorRGBA<TValue>::operator*(TValue scalar_) const noexcept
	{
		if constexpr (std::same_as<TValue, int>)
		{
			return ColorRGBA<TValue>(value.x * scalar_, value.y * scalar_, value.z * scalar_, value.w * scalar_);
		}
		else
		{
			ColorRGBA<TValue> result;
			Store(result, DirectX::XMVectorScale(Load(*this), scalar_));
			return result;
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue>& ColorRGBA<TValue>::operator*=(const ColorRGBA<TValue>& other_) noexcept
	{
		*this = *this * other_;
		return *this;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue>& ColorRGBA<TValue>::operator*=(TValue scalar_) noexcept
	{
		*this = *this * scalar_;
		return *this;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue> ColorRGBA<TValue>::operator/(TValue scalar_) const noexcept
	{
		if constexpr (std::same_as<TValue, int>)
		{
			assert(scalar_ != TValue{});
			return ColorRGBA<TValue>(value.x / scalar_, value.y / scalar_, value.z / scalar_, value.w / scalar_);
		}
		else
		{
			assert(scalar_ != 0.0f);
			ColorRGBA<TValue> result;
			Store(result, DirectX::XMVectorScale(Load(*this), 1.0f / scalar_));
			return result;
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue>& ColorRGBA<TValue>::operator/=(TValue scalar_) noexcept
	{
		*this = *this / scalar_;
		return *this;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	bool ColorRGBA<TValue>::operator==(const ColorRGBA<TValue>& other_) const noexcept
	{
		if constexpr (std::same_as<TValue, int>)
		{
			return value.x == other_.value.x && value.y == other_.value.y && value.z == other_.value.z && value.w == other_.value.w;
		}
		else
		{
			return std::abs(value.x - other_.value.x) < std::numeric_limits<float>::epsilon() &&
			std::abs(value.y - other_.value.y) < std::numeric_limits<float>::epsilon() &&
			std::abs(value.z - other_.value.z) < std::numeric_limits<float>::epsilon() &&
			std::abs(value.w - other_.value.w) < std::numeric_limits<float>::epsilon();
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	bool ColorRGBA<TValue>::operator!=(const ColorRGBA<TValue>& other_) const noexcept
	{
		return !(*this == other_);
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
	bool ColorRGBA<TValue>::IsTransparent(float epsilon_) const noexcept
	{
		return value.w <= epsilon_;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	bool ColorRGBA<TValue>::IsOpaque(float epsilon_) const noexcept
	{
		constexpr TValue maximum = static_cast<TValue>(std::same_as<TValue, int> ? 255 : 1);
		return value.w >= maximum - epsilon_;
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
	DirectX::XMVECTOR ColorRGBA<TValue>::Load(const ColorRGBA<TValue>& color_) noexcept
		requires std::same_as<TValue, float>
	{
		return DirectX::XMLoadFloat4(&color_.value);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	void ColorRGBA<TValue>::Store(ColorRGBA<TValue>& destination_, DirectX::XMVECTOR source_) noexcept
		requires std::same_as<TValue, float>
	{
		DirectX::XMStoreFloat4(&destination_.value, source_);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	bool ColorRGBA<TValue>::IsApproximately(const ColorRGBA<TValue>& lhs_, const ColorRGBA<TValue>& rhs_, float epsilon_) noexcept
		requires std::same_as<TValue, float>
	{
		return std::abs(lhs_.value.x - rhs_.value.x) <= epsilon_ &&
		std::abs(lhs_.value.y - rhs_.value.y) <= epsilon_ &&
		std::abs(lhs_.value.z - rhs_.value.z) <= epsilon_ &&
		std::abs(lhs_.value.w - rhs_.value.w) <= epsilon_;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue> ColorRGBA<TValue>::Lerp(const ColorRGBA<TValue>& start_, const ColorRGBA<TValue>& end_, float t_) noexcept
		requires std::same_as<TValue, float>
	{
		ColorRGBA<TValue> result;
		Store(result, DirectX::XMVectorLerp(Load(start_), Load(end_), t_));
		return result;
	}

	template class ColorRGBA<int>;
	template class ColorRGBA<float>;
}
