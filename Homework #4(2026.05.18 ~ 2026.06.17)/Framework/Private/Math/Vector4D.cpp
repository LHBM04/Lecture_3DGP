#include "Precompiled.hpp"
#include "Vector4D.hpp"

#include "Vector2D.hpp"
#include "Vector3D.hpp"
#include "Vector4D_Impl.hpp"

namespace TUK::Framework
{
	template <IsNumeric TValue>
	Vector4D<TValue>::Vector4D() noexcept
		: values{TValue{}, TValue{}, TValue{}, TValue{}}
	{
	}

	template <IsNumeric TValue>
	Vector4D<TValue>::Vector4D(TValue scalar) noexcept
		: values{scalar, scalar, scalar, scalar}
	{
	}

	template <IsNumeric TValue>
	Vector4D<TValue>::Vector4D(TValue x, TValue y, TValue z, TValue w) noexcept
		: values{x, y, z, w}
	{
	}

	template <IsNumeric TValue>
	Vector4D<TValue>::Vector4D(const Vector4D& other) noexcept
		: values{}
	{
		for (std::size_t index = 0; index < 4; ++index) values[index] = other.values[index];
	}

	template <IsNumeric TValue>
	Vector4D<TValue>::Vector4D(Vector4D&& other) noexcept
		: values{}
	{
		for (std::size_t index = 0; index < 4; ++index) values[index] = other.values[index];
	}

	template <IsNumeric TValue>
	Vector4D<TValue>::Vector4D(const Vector2D<TValue>& vector, TValue z, TValue w) noexcept
		: values{TValue{}, TValue{}, TValue{}, TValue{}}
	{
		Set(vector.GetX(), vector.GetY(), z, w);
	}

	template <IsNumeric TValue>
	Vector4D<TValue>::Vector4D(const Vector3D<TValue>& vector, TValue w) noexcept
		: values{TValue{}, TValue{}, TValue{}, TValue{}}
	{
		Set(vector.GetX(), vector.GetY(), vector.GetZ(), w);
	}

	template <IsNumeric TValue>
	Vector4D<TValue>& Vector4D<TValue>::operator=(const Vector4D& other) noexcept
	{
		for (std::size_t index = 0; index < 4; ++index) values[index] = other.values[index];
		return *this;
	}

	template <IsNumeric TValue>
	Vector4D<TValue>& Vector4D<TValue>::operator=(Vector4D&& other) noexcept
	{
		for (std::size_t index = 0; index < 4; ++index) values[index] = other.values[index];
		return *this;
	}

	template <IsNumeric TValue>
	Vector4D<TValue>::operator Vector2D<TValue>() const noexcept
	{
		return Vector2D<TValue>(GetX(), GetY());
	}

	template <IsNumeric TValue>
	Vector4D<TValue>::operator Vector3D<TValue>() const noexcept
	{
		return Vector3D<TValue>(GetX(), GetY(), GetZ());
	}

	template <IsNumeric TValue>
	TValue Vector4D<TValue>::operator[](std::size_t index) const noexcept
	{
		assert(index < 4);
		switch (index)
		{
		case 0:
			return values[0];
		case 1:
			return values[1];
		case 2:
			return values[2];
		default:
			return values[3];
		}
	}

	template <IsNumeric TValue>
	TValue& Vector4D<TValue>::operator[](std::size_t index) noexcept
	{
		assert(index < 4);
		switch (index)
		{
		case 0:
			return values[0];
		case 1:
			return values[1];
		case 2:
			return values[2];
		default:
			return values[3];
		}
	}

	template <IsNumeric TValue>
	Vector4D<TValue> Vector4D<TValue>::operator+() const noexcept
	{
		return *this;
	}

	template <IsNumeric TValue>
	Vector4D<TValue> Vector4D<TValue>::operator+(const Vector4D& other) const noexcept
	{
		if constexpr (std::same_as<TValue, float>)
		{
			Vector4D<TValue> result;
			Store(result, DirectX::XMVectorAdd(Load(*this), Load(other)));
			return result;
		}
		else
		{
			return Vector4D(GetX() + other.GetX(), GetY() + other.GetY(), GetZ() + other.GetZ(), GetW() + other.GetW());
		}
	}

	template <IsNumeric TValue>
	Vector4D<TValue>& Vector4D<TValue>::operator+=(const Vector4D& other) noexcept
	{
		*this = *this + other;
		return *this;
	}

	template <IsNumeric TValue>
	Vector4D<TValue> Vector4D<TValue>::operator-() const noexcept
	{
		if constexpr (std::same_as<TValue, float>)
		{
			Vector4D<TValue> result;
			Store(result, DirectX::XMVectorNegate(Load(*this)));
			return result;
		}
		else
		{
			return Vector4D(-GetX(), -GetY(), -GetZ(), -GetW());
		}
	}

	template <IsNumeric TValue>
	Vector4D<TValue> Vector4D<TValue>::operator-(const Vector4D& other) const noexcept
	{
		if constexpr (std::same_as<TValue, float>)
		{
			Vector4D<TValue> result;
			Store(result, DirectX::XMVectorSubtract(Load(*this), Load(other)));
			return result;
		}
		else
		{
			return Vector4D(GetX() - other.GetX(), GetY() - other.GetY(), GetZ() - other.GetZ(), GetW() - other.GetW());
		}
	}

	template <IsNumeric TValue>
	Vector4D<TValue>& Vector4D<TValue>::operator-=(const Vector4D& other) noexcept
	{
		*this = *this - other;
		return *this;
	}

	template <IsNumeric TValue>
	Vector4D<TValue> Vector4D<TValue>::operator*(TValue scalar) const noexcept
	{
		if constexpr (std::same_as<TValue, float>)
		{
			Vector4D<TValue> result;
			Store(result, DirectX::XMVectorScale(Load(*this), scalar));
			return result;
		}
		else
		{
			return Vector4D(GetX() * scalar, GetY() * scalar, GetZ() * scalar, GetW() * scalar);
		}
	}

	template <IsNumeric TValue>
	Vector4D<TValue>& Vector4D<TValue>::operator*=(TValue scalar) noexcept
	{
		*this = *this * scalar;
		return *this;
	}

	template <IsNumeric TValue>
	Vector4D<TValue> Vector4D<TValue>::operator/(TValue scalar) const noexcept
	{
		if constexpr (std::same_as<TValue, float>)
		{
			Vector4D<TValue> result;
			Store(result, DirectX::XMVectorScale(Load(*this), 1.0f / scalar));
			return result;
		}
		else
		{
			return Vector4D(GetX() / scalar, GetY() / scalar, GetZ() / scalar, GetW() / scalar);
		}
	}

	template <IsNumeric TValue>
	Vector4D<TValue>& Vector4D<TValue>::operator/=(TValue scalar) noexcept
	{
		*this = *this / scalar;
		return *this;
	}

	template <IsNumeric TValue>
	bool Vector4D<TValue>::operator==(const Vector4D& other) const noexcept
	{
		return GetX() == other.GetX() && GetY() == other.GetY() && GetZ() == other.GetZ() && GetW() == other.GetW();
	}

	template <IsNumeric TValue>
	bool Vector4D<TValue>::operator!=(const Vector4D& other) const noexcept
	{
		return !(*this == other);
	}

	template <IsNumeric TValue>
	std::partial_ordering Vector4D<TValue>::operator<=>(const Vector4D& other) const noexcept
	{
		if (const auto order = GetX() <=> other.GetX(); order != 0)
		{
			return order;
		}
		if (const auto order = GetY() <=> other.GetY(); order != 0)
		{
			return order;
		}
		if (const auto order = GetZ() <=> other.GetZ(); order != 0)
		{
			return order;
		}
		return GetW() <=> other.GetW();
	}

	template <IsNumeric TValue>
	TValue Vector4D<TValue>::GetX() const noexcept
	{
		return values[0];
	}

	template <IsNumeric TValue>
	void Vector4D<TValue>::SetX(TValue x) noexcept
	{
		values[0] = x;
	}

	template <IsNumeric TValue>
	TValue Vector4D<TValue>::GetY() const noexcept
	{
		return values[1];
	}

	template <IsNumeric TValue>
	void Vector4D<TValue>::SetY(TValue y) noexcept
	{
		values[1] = y;
	}

	template <IsNumeric TValue>
	TValue Vector4D<TValue>::GetZ() const noexcept
	{
		return values[2];
	}

	template <IsNumeric TValue>
	void Vector4D<TValue>::SetZ(TValue z) noexcept
	{
		values[2] = z;
	}

	template <IsNumeric TValue>
	TValue Vector4D<TValue>::GetW() const noexcept
	{
		return values[3];
	}

	template <IsNumeric TValue>
	void Vector4D<TValue>::SetW(TValue w) noexcept
	{
		values[3] = w;
	}

	template <IsNumeric TValue>
	void Vector4D<TValue>::Set(TValue x, TValue y, TValue z, TValue w) noexcept
	{
		SetX(x);
		SetY(y);
		SetZ(z);
		SetW(w);
	}

	template <IsNumeric TValue>
	float Vector4D<TValue>::GetMagnitude() const noexcept
		requires std::same_as<TValue, float>
	{
		return DirectX::XMVectorGetX(DirectX::XMVector4Length(Load(*this)));
	}

	template <IsNumeric TValue>
	float Vector4D<TValue>::GetSqrMagnitude() const noexcept
		requires std::same_as<TValue, float>
	{
		return DirectX::XMVectorGetX(DirectX::XMVector4LengthSq(Load(*this)));
	}

	template <IsNumeric TValue>
	float Vector4D<TValue>::GetLength() const noexcept
		requires std::same_as<TValue, float>
	{
		return GetMagnitude();
	}

	template <IsNumeric TValue>
	float Vector4D<TValue>::GetLengthSquared() const noexcept
		requires std::same_as<TValue, float>
	{
		return GetSqrMagnitude();
	}

	template <IsNumeric TValue>
	Vector4D<TValue> Vector4D<TValue>::GetNormalized() const noexcept
		requires std::same_as<TValue, float>
	{
		return Normalize(*this);
	}

	template <IsNumeric TValue>
	bool Vector4D<TValue>::IsZero(float epsilon) const noexcept
	{
		assert(epsilon >= 0.0f);
		return std::abs(static_cast<double>(GetX())) <= epsilon && std::abs(static_cast<double>(GetY())) <= epsilon && std::abs(static_cast<double>(GetZ())) <= epsilon && std::abs(static_cast<double>(GetW())) <= epsilon;
	}

	template <IsNumeric TValue>
	bool Vector4D<TValue>::IsFinite() const noexcept
	{
		return std::isfinite(static_cast<double>(GetX())) && std::isfinite(static_cast<double>(GetY())) && std::isfinite(static_cast<double>(GetZ())) && std::isfinite(static_cast<double>(GetW()));
	}

	template <IsNumeric TValue>
	void Vector4D<TValue>::Normalize() noexcept
		requires std::same_as<TValue, float>
	{
		*this = GetNormalized();
	}

	template <IsNumeric TValue>
	float Vector4D<TValue>::Dot(const Vector4D& other) const noexcept
		requires std::same_as<TValue, float>
	{
		return Dot(*this, other);
	}

	template <IsNumeric TValue>
	Vector4D<TValue> Vector4D<TValue>::GetZero() noexcept
	{
		return Vector4D<TValue>(static_cast<TValue>(0), static_cast<TValue>(0), static_cast<TValue>(0), static_cast<TValue>(0));
	}

	template <IsNumeric TValue>
	Vector4D<TValue> Vector4D<TValue>::GetOne() noexcept
	{
		return Vector4D<TValue>(static_cast<TValue>(1), static_cast<TValue>(1), static_cast<TValue>(1), static_cast<TValue>(1));
	}

	template <IsNumeric TValue>
	Vector4D<TValue> Vector4D<TValue>::GetPositiveInfinity() noexcept
		requires std::same_as<TValue, float>
	{
		const float infinity = std::numeric_limits<float>::infinity();
			return Vector4D<TValue>(infinity, infinity, infinity, infinity);
	}

	template <IsNumeric TValue>
	Vector4D<TValue> Vector4D<TValue>::GetNegativeInfinity() noexcept
		requires std::same_as<TValue, float>
	{
		const float negativeInfinity = -std::numeric_limits<float>::infinity();
			return Vector4D<TValue>(negativeInfinity, negativeInfinity, negativeInfinity, negativeInfinity);
	}

	template <IsNumeric TValue>
	bool Vector4D<TValue>::IsApproximately(const Vector4D<TValue>& lhs, const Vector4D<TValue>& rhs, float epsilon) noexcept
		requires std::same_as<TValue, float>
	{
		return std::abs(lhs.GetX() - rhs.GetX()) <= epsilon
				&& std::abs(lhs.GetY() - rhs.GetY()) <= epsilon
				&& std::abs(lhs.GetZ() - rhs.GetZ()) <= epsilon
				&& std::abs(lhs.GetW() - rhs.GetW()) <= epsilon;
	}

	template <IsNumeric TValue>
	Vector4D<TValue> Vector4D<TValue>::Max(const Vector4D<TValue>& a, const Vector4D<TValue>& b) noexcept
		requires std::same_as<TValue, float>
	{
		Vector4D<TValue> result;
		Store(result, DirectX::XMVectorMax(Load(a), Load(b)));
		return result;
	}

	template <IsNumeric TValue>
	Vector4D<TValue> Vector4D<TValue>::Min(const Vector4D<TValue>& a, const Vector4D<TValue>& b) noexcept
		requires std::same_as<TValue, float>
	{
		Vector4D<TValue> result;
		Store(result, DirectX::XMVectorMin(Load(a), Load(b)));
		return result;
	}

	template <IsNumeric TValue>
	Vector4D<TValue> Vector4D<TValue>::Clamp(const Vector4D<TValue>& value, const Vector4D<TValue>& min, const Vector4D<TValue>& max) noexcept
		requires std::same_as<TValue, float>
	{
		Vector4D<TValue> result;
		Store(result, DirectX::XMVectorClamp(Load(value), Load(min), Load(max)));
		return result;
	}

	template <IsNumeric TValue>
	Vector4D<TValue> Vector4D<TValue>::ClampMagnitude(const Vector4D<TValue>& vector, float maxLength) noexcept
		requires std::same_as<TValue, float>
	{
		const float sqrMagnitude = vector.GetSqrMagnitude();
			const float maxSqr = maxLength * maxLength;
			if (sqrMagnitude <= maxSqr)
			{
				return vector;
			}
		
			return Normalize(vector) * maxLength;
	}

	template <IsNumeric TValue>
	Vector4D<TValue> Vector4D<TValue>::Scale(const Vector4D<TValue>& vector, const Vector4D<TValue>& scale) noexcept
		requires std::same_as<TValue, float>
	{
		Vector4D<TValue> result;
		Store(result, DirectX::XMVectorMultiply(Load(vector), Load(scale)));
		return result;
	}

	template <IsNumeric TValue>
	Vector4D<TValue> Vector4D<TValue>::Normalize(const Vector4D<TValue>& value) noexcept
		requires std::same_as<TValue, float>
	{
		Vector4D<TValue> result;
		Store(result, DirectX::XMVector4Normalize(Load(value)));
		return result;
	}

	template <IsNumeric TValue>
	float Vector4D<TValue>::Dot(const Vector4D<TValue>& a, const Vector4D<TValue>& b) noexcept
		requires std::same_as<TValue, float>
	{
		return DirectX::XMVectorGetX(DirectX::XMVector4Dot(Load(a), Load(b)));
	}

	template <IsNumeric TValue>
	float Vector4D<TValue>::Distance(const Vector4D<TValue>& a, const Vector4D<TValue>& b) noexcept
		requires std::same_as<TValue, float>
	{
		return (a - b).GetMagnitude();
	}

	template <IsNumeric TValue>
	float Vector4D<TValue>::Angle(const Vector4D<TValue>& a, const Vector4D<TValue>& b) noexcept
		requires std::same_as<TValue, float>
	{
		const Vector4D<TValue> from = Normalize(a);
			const Vector4D<TValue> to = Normalize(b);
			const float dot = std::clamp(Dot(from, to), -1.0f, 1.0f);
			return std::acos(dot);
	}

	template <IsNumeric TValue>
	Vector4D<TValue> Vector4D<TValue>::Project(const Vector4D<TValue>& vector, const Vector4D<TValue>& onNormal) noexcept
		requires std::same_as<TValue, float>
	{
		const float denominator = Dot(onNormal, onNormal);
			if (denominator <= std::numeric_limits<float>::epsilon())
			{
				return GetZero();
			}
		
			const float scale = Dot(vector, onNormal) / denominator;
			return onNormal * scale;
	}

	template <IsNumeric TValue>
	Vector4D<TValue> Vector4D<TValue>::Reflect(const Vector4D<TValue>& vector, const Vector4D<TValue>& normal) noexcept
		requires std::same_as<TValue, float>
	{
		Vector4D<TValue> result;
		Store(result, DirectX::XMVector4Reflect(Load(vector), Load(normal)));
		return result;
	}

	template <IsNumeric TValue>
	Vector4D<TValue> Vector4D<TValue>::Lerp(const Vector4D<TValue>& a, const Vector4D<TValue>& b, float t) noexcept
		requires std::same_as<TValue, float>
	{
		return LerpUnclamped(a, b, std::clamp(t, 0.0f, 1.0f));
	}

	template <IsNumeric TValue>
	Vector4D<TValue> Vector4D<TValue>::LerpUnclamped(const Vector4D<TValue>& a, const Vector4D<TValue>& b, float t) noexcept
		requires std::same_as<TValue, float>
	{
		Vector4D<TValue> result;
		Store(result, DirectX::XMVectorLerp(Load(a), Load(b), t));
		return result;
	}

	template <IsNumeric TValue>
	Vector4D<TValue> Vector4D<TValue>::Slerp(const Vector4D<TValue>& a, const Vector4D<TValue>& b, float t) noexcept
		requires std::same_as<TValue, float>
	{
		return SlerpUnclamped(a, b, std::clamp(t, 0.0f, 1.0f));
	}

	template <IsNumeric TValue>
	Vector4D<TValue> Vector4D<TValue>::SlerpUnclamped(const Vector4D<TValue>& a, const Vector4D<TValue>& b, float t) noexcept
		requires std::same_as<TValue, float>
	{
		const float aMag = a.GetMagnitude();
			const float bMag = b.GetMagnitude();
		
			if (aMag <= std::numeric_limits<float>::epsilon() || bMag <= std::numeric_limits<float>::epsilon())
			{
				return LerpUnclamped(a, b, t);
			}
		
			const Vector4D<TValue> from = a / aMag;
			const Vector4D<TValue> to = b / bMag;
		
			float dot = std::clamp(Dot(from, to), -1.0f, 1.0f);
			const float theta = std::acos(dot) * t;
		
			Vector4D<TValue> relative = to - from * dot;
			const float relativeMag = relative.GetMagnitude();
			if (relativeMag <= std::numeric_limits<float>::epsilon())
			{
				return LerpUnclamped(a, b, t);
			}
		
			relative /= relativeMag;
			const Vector4D<TValue> direction = from * std::cos(theta) + relative * std::sin(theta);
			const float magnitude = Mathf::Lerp(aMag, bMag, t);
		
			return direction * magnitude;
	}

	template <IsNumeric TValue>
	Vector4D<TValue> Vector4D<TValue>::MoveTowards(const Vector4D<TValue>& current, const Vector4D<TValue>& target, float maxDistanceDelta) noexcept
		requires std::same_as<TValue, float>
	{
		const Vector4D<TValue> delta = target - current;
			const float distance = delta.GetMagnitude();
		
			if (distance <= maxDistanceDelta || distance <= std::numeric_limits<float>::epsilon())
			{
				return target;
			}
		
			return current + (delta / distance) * maxDistanceDelta;
	}

	template class Vector4D<std::int32_t>;
	template class Vector4D<float>;

	Vector4D<std::int32_t> operator*(std::int32_t scalar, const Vector4D<std::int32_t>& vector) noexcept
	{
		return vector * scalar;
	}

	Vector4D<float> operator*(float scalar, const Vector4D<float>& vector) noexcept
	{
		return vector * scalar;
	}

}
