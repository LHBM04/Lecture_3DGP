#include "Precompiled.h"
#include "Mathf.h"
#include "Vector4D.h"

#include "Vector2D.h"
#include "Vector3D.h"

namespace TUK::Framework
{
	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue>::Vector4D() noexcept
		: value(TValue{}, TValue{}, TValue{}, TValue{})
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue>::Vector4D(TValue scalar) noexcept
		: value(scalar, scalar, scalar, scalar)
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue>::Vector4D(TValue x, TValue y, TValue z, TValue w) noexcept
		: value(x, y, z, w)
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue>::Vector4D(const Vector4D& other) noexcept
		: value(other.value)
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue>::Vector4D(Vector4D&& other) noexcept
		: value(std::move(other.value))
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue>::Vector4D(const Vector2D<TValue>& vector, TValue z, TValue w) noexcept
		: value(TValue{}, TValue{}, TValue{}, TValue{})
	{
		Set(vector.GetX(), vector.GetY(), z, w);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue>::Vector4D(const Vector3D<TValue>& vector, TValue w) noexcept
		: value(TValue{}, TValue{}, TValue{}, TValue{})
	{
		Set(vector.GetX(), vector.GetY(), vector.GetZ(), w);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue>& Vector4D<TValue>::operator=(const Vector4D& other) noexcept
	{
		value = other.value;
		return *this;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue>& Vector4D<TValue>::operator=(Vector4D&& other) noexcept
	{
		value = std::move(other.value);
		return *this;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue>::operator Vector2D<TValue>() const noexcept
	{
		return Vector2D<TValue>(GetX(), GetY());
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue>::operator Vector3D<TValue>() const noexcept
	{
		return Vector3D<TValue>(GetX(), GetY(), GetZ());
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	TValue Vector4D<TValue>::operator[](std::size_t index) const noexcept
	{
		assert(index < 4);
		switch (index)
		{
		case 0:
			return value.x;
		case 1:
			return value.y;
		case 2:
			return value.z;
		default:
			return value.w;
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	TValue& Vector4D<TValue>::operator[](std::size_t index) noexcept
	{
		assert(index < 4);
		switch (index)
		{
		case 0:
			return value.x;
		case 1:
			return value.y;
		case 2:
			return value.z;
		default:
			return value.w;
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue> Vector4D<TValue>::operator+() const noexcept
	{
		return *this;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue> Vector4D<TValue>::operator+(const Vector4D& other) const noexcept
	{
		if constexpr (std::same_as<TValue, float>)
		{
			Vector4D result;
			Store(result, DirectX::XMVectorAdd(Load(*this), Load(other)));
			return result;
		}
		else
		{
			return Vector4D(GetX() + other.GetX(), GetY() + other.GetY(), GetZ() + other.GetZ(), GetW() + other.GetW());
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue>& Vector4D<TValue>::operator+=(const Vector4D& other) noexcept
	{
		*this = *this + other;
		return *this;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue> Vector4D<TValue>::operator-() const noexcept
	{
		return Vector4D(-GetX(), -GetY(), -GetZ(), -GetW());
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue> Vector4D<TValue>::operator-(const Vector4D& other) const noexcept
	{
		if constexpr (std::same_as<TValue, float>)
		{
			Vector4D result;
			Store(result, DirectX::XMVectorSubtract(Load(*this), Load(other)));
			return result;
		}
		else
		{
			return Vector4D(GetX() - other.GetX(), GetY() - other.GetY(), GetZ() - other.GetZ(), GetW() - other.GetW());
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue>& Vector4D<TValue>::operator-=(const Vector4D& other) noexcept
	{
		*this = *this - other;
		return *this;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue> Vector4D<TValue>::operator*(TValue scalar) const noexcept
	{
		if constexpr (std::same_as<TValue, float>)
		{
			Vector4D result;
			Store(result, DirectX::XMVectorScale(Load(*this), scalar));
			return result;
		}
		else
		{
			return Vector4D(GetX() * scalar, GetY() * scalar, GetZ() * scalar, GetW() * scalar);
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue>& Vector4D<TValue>::operator*=(TValue scalar) noexcept
	{
		*this = *this * scalar;
		return *this;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue> Vector4D<TValue>::operator/(TValue scalar) const noexcept
	{
		assert(scalar != TValue{});
		if constexpr (std::same_as<TValue, float>)
		{
			Vector4D result;
			Store(result, DirectX::XMVectorDivide(Load(*this), DirectX::XMVectorReplicate(scalar)));
			return result;
		}
		else
		{
			return Vector4D(GetX() / scalar, GetY() / scalar, GetZ() / scalar, GetW() / scalar);
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue>& Vector4D<TValue>::operator/=(TValue scalar) noexcept
	{
		*this = *this / scalar;
		return *this;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	bool Vector4D<TValue>::operator==(const Vector4D& other) const noexcept
	{
		return GetX() == other.GetX() && GetY() == other.GetY() && GetZ() == other.GetZ() && GetW() == other.GetW();
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	bool Vector4D<TValue>::operator!=(const Vector4D& other) const noexcept
	{
		return !(*this == other);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
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

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	TValue Vector4D<TValue>::GetX() const noexcept
	{
		return value.x;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	void Vector4D<TValue>::SetX(TValue x) noexcept
	{
		value.x = x;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	TValue Vector4D<TValue>::GetY() const noexcept
	{
		return value.y;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	void Vector4D<TValue>::SetY(TValue y) noexcept
	{
		value.y = y;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	TValue Vector4D<TValue>::GetZ() const noexcept
	{
		return value.z;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	void Vector4D<TValue>::SetZ(TValue z) noexcept
	{
		value.z = z;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	TValue Vector4D<TValue>::GetW() const noexcept
	{
		return value.w;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	void Vector4D<TValue>::SetW(TValue w) noexcept
	{
		value.w = w;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	void Vector4D<TValue>::Set(TValue x, TValue y, TValue z, TValue w) noexcept
	{
		SetX(x);
		SetY(y);
		SetZ(z);
		SetW(w);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	float Vector4D<TValue>::GetMagnitude() const noexcept
		requires std::same_as<TValue, float>
	{
		return DirectX::XMVectorGetX(DirectX::XMVector4Length(Load(*this)));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	float Vector4D<TValue>::GetSqrMagnitude() const noexcept
		requires std::same_as<TValue, float>
	{
		return Dot(*this, *this);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	float Vector4D<TValue>::GetLength() const noexcept
		requires std::same_as<TValue, float>
	{
		return GetMagnitude();
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	float Vector4D<TValue>::GetLengthSquared() const noexcept
		requires std::same_as<TValue, float>
	{
		return GetSqrMagnitude();
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue> Vector4D<TValue>::GetNormalized() const noexcept
		requires std::same_as<TValue, float>
	{
		return Normalize(*this);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	bool Vector4D<TValue>::IsZero(float epsilon) const noexcept
	{
		assert(epsilon >= 0.0f);
		return std::abs(static_cast<double>(GetX())) <= epsilon && std::abs(static_cast<double>(GetY())) <= epsilon && std::abs(static_cast<double>(GetZ())) <= epsilon && std::abs(static_cast<double>(GetW())) <= epsilon;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	bool Vector4D<TValue>::IsFinite() const noexcept
	{
		return std::isfinite(static_cast<double>(GetX())) && std::isfinite(static_cast<double>(GetY())) && std::isfinite(static_cast<double>(GetZ())) && std::isfinite(static_cast<double>(GetW()));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	void Vector4D<TValue>::Normalize() noexcept
		requires std::same_as<TValue, float>
	{
		*this = GetNormalized();
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	float Vector4D<TValue>::Dot(const Vector4D& other) const noexcept
		requires std::same_as<TValue, float>
	{
		return Dot(*this, other);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue> Vector4D<TValue>::GetZero() noexcept
	{
		return Vector4D<TValue>(static_cast<TValue>(0), static_cast<TValue>(0), static_cast<TValue>(0), static_cast<TValue>(0));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue> Vector4D<TValue>::GetOne() noexcept
	{
		return Vector4D<TValue>(static_cast<TValue>(1), static_cast<TValue>(1), static_cast<TValue>(1), static_cast<TValue>(1));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue> Vector4D<TValue>::GetPositiveInfinity() noexcept
		requires std::same_as<TValue, float>
	{
		const float infinity = std::numeric_limits<float>::infinity();
		    return Vector4D<TValue>(infinity, infinity, infinity, infinity);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue> Vector4D<TValue>::GetNegativeInfinity() noexcept
		requires std::same_as<TValue, float>
	{
		const float negativeInfinity = -std::numeric_limits<float>::infinity();
		    return Vector4D<TValue>(negativeInfinity, negativeInfinity, negativeInfinity, negativeInfinity);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	DirectX::XMVECTOR Vector4D<TValue>::Load(const Vector4D& vector) noexcept
		requires std::same_as<TValue, float>
	{
		return DirectX::XMLoadFloat4(&vector.value);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	void Vector4D<TValue>::Store(Vector4D& destination, DirectX::XMVECTOR source) noexcept
		requires std::same_as<TValue, float>
	{
		DirectX::XMStoreFloat4(&destination.value, source);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	bool Vector4D<TValue>::IsApproximately(const Vector4D<TValue>& lhs, const Vector4D<TValue>& rhs, float epsilon) noexcept
		requires std::same_as<TValue, float>
	{
		return std::abs(lhs.GetX() - rhs.GetX()) <= epsilon
		        && std::abs(lhs.GetY() - rhs.GetY()) <= epsilon
		        && std::abs(lhs.GetZ() - rhs.GetZ()) <= epsilon
		        && std::abs(lhs.GetW() - rhs.GetW()) <= epsilon;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue> Vector4D<TValue>::Max(const Vector4D<TValue>& a, const Vector4D<TValue>& b) noexcept
		requires std::same_as<TValue, float>
	{
		return Vector4D<TValue>(std::max(a.GetX(), b.GetX()), std::max(a.GetY(), b.GetY()), std::max(a.GetZ(), b.GetZ()), std::max(a.GetW(), b.GetW()));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue> Vector4D<TValue>::Min(const Vector4D<TValue>& a, const Vector4D<TValue>& b) noexcept
		requires std::same_as<TValue, float>
	{
		return Vector4D<TValue>(std::min(a.GetX(), b.GetX()), std::min(a.GetY(), b.GetY()), std::min(a.GetZ(), b.GetZ()), std::min(a.GetW(), b.GetW()));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue> Vector4D<TValue>::Clamp(const Vector4D<TValue>& value, const Vector4D<TValue>& min, const Vector4D<TValue>& max) noexcept
		requires std::same_as<TValue, float>
	{
		return Vector4D<TValue>(
		        std::clamp(value.GetX(), min.GetX(), max.GetX()),
		        std::clamp(value.GetY(), min.GetY(), max.GetY()),
		        std::clamp(value.GetZ(), min.GetZ(), max.GetZ()),
		        std::clamp(value.GetW(), min.GetW(), max.GetW())
		    );
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
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

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue> Vector4D<TValue>::Scale(const Vector4D<TValue>& vector, const Vector4D<TValue>& scale) noexcept
		requires std::same_as<TValue, float>
	{
		return Vector4D<TValue>(vector.GetX() * scale.GetX(), vector.GetY() * scale.GetY(), vector.GetZ() * scale.GetZ(), vector.GetW() * scale.GetW());
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue> Vector4D<TValue>::Normalize(const Vector4D<TValue>& value) noexcept
		requires std::same_as<TValue, float>
	{
		Vector4D<TValue> result{};
		    Store(result, DirectX::XMVector4Normalize(Load(value)));
		    return result;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	float Vector4D<TValue>::Dot(const Vector4D<TValue>& a, const Vector4D<TValue>& b) noexcept
		requires std::same_as<TValue, float>
	{
		return DirectX::XMVectorGetX(DirectX::XMVector4Dot(Load(a), Load(b)));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	float Vector4D<TValue>::Distance(const Vector4D<TValue>& a, const Vector4D<TValue>& b) noexcept
		requires std::same_as<TValue, float>
	{
		return (a - b).GetMagnitude();
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	float Vector4D<TValue>::Angle(const Vector4D<TValue>& a, const Vector4D<TValue>& b) noexcept
		requires std::same_as<TValue, float>
	{
		const Vector4D<TValue> from = Normalize(a);
		    const Vector4D<TValue> to = Normalize(b);
		    const float dot = std::clamp(Dot(from, to), -1.0f, 1.0f);
		    return std::acos(dot);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
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

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue> Vector4D<TValue>::Reflect(const Vector4D<TValue>& vector, const Vector4D<TValue>& normal) noexcept
		requires std::same_as<TValue, float>
	{
		return vector - normal * (2.0f * Dot(vector, normal));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue> Vector4D<TValue>::Lerp(const Vector4D<TValue>& a, const Vector4D<TValue>& b, float t) noexcept
		requires std::same_as<TValue, float>
	{
		Vector4D<TValue> result{};
		    Store(result, DirectX::XMVectorLerp(Load(a), Load(b), std::clamp(t, 0.0f, 1.0f)));
		    return result;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue> Vector4D<TValue>::LerpUnclamped(const Vector4D<TValue>& a, const Vector4D<TValue>& b, float t) noexcept
		requires std::same_as<TValue, float>
	{
		Vector4D<TValue> result{};
		    Store(result, DirectX::XMVectorLerp(Load(a), Load(b), t));
		    return result;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue> Vector4D<TValue>::Slerp(const Vector4D<TValue>& a, const Vector4D<TValue>& b, float t) noexcept
		requires std::same_as<TValue, float>
	{
		return SlerpUnclamped(a, b, std::clamp(t, 0.0f, 1.0f));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
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

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
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

	template class Vector4D<int>;
	template class Vector4D<float>;

	Vector4D<int> operator*(int scalar, const Vector4D<int>& vector) noexcept
	{
		return vector * scalar;
	}

	Vector4D<float> operator*(float scalar, const Vector4D<float>& vector) noexcept
	{
		return vector * scalar;
	}

}
