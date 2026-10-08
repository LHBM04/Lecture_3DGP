#include "Precompiled.hpp"
#include "Mathf.hpp"
#include "Vector3D.hpp"

#include "Vector2D.hpp"
#include "Vector4D.hpp"

namespace TUK::Framework
{
	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<TValue>::Vector3D() noexcept
		: value(TValue{}, TValue{}, TValue{})
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<TValue>::Vector3D(TValue scalar) noexcept
		: value(scalar, scalar, scalar)
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<TValue>::Vector3D(TValue x, TValue y, TValue z) noexcept
		: value(x, y, z)
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<TValue>::Vector3D(const Vector3D& other) noexcept
		: value(other.value)
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<TValue>::Vector3D(Vector3D&& other) noexcept
		: value(std::move(other.value))
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<TValue>::Vector3D(const Vector2D<TValue>& vector, TValue z) noexcept
		: value(TValue{}, TValue{}, TValue{})
	{
		Set(vector.GetX(), vector.GetY(), z);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<TValue>::Vector3D(const Vector4D<TValue>& vector) noexcept
		: value(TValue{}, TValue{}, TValue{})
	{
		Set(vector.GetX(), vector.GetY(), vector.GetZ());
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<TValue>::Vector3D(DirectX::XMVECTOR vector) noexcept
		requires std::same_as<TValue, float>
		: value(TValue{}, TValue{}, TValue{})
	{
		Store(*this, vector);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<TValue>& Vector3D<TValue>::operator=(const Vector3D& other) noexcept
	{
		value = other.value;
		return *this;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<TValue>& Vector3D<TValue>::operator=(Vector3D&& other) noexcept
	{
		value = std::move(other.value);
		return *this;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<TValue>::operator Vector2D<TValue>() const noexcept
	{
		return Vector2D<TValue>(GetX(), GetY());
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<TValue>::operator Vector4D<TValue>() const noexcept
	{
		return Vector4D<TValue>(GetX(), GetY(), GetZ(), TValue{});
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	TValue Vector3D<TValue>::operator[](std::size_t index) const noexcept
	{
		assert(index < 3);
		switch (index)
		{
		case 0:
			return value.x;
		case 1:
			return value.y;
		default:
			return value.z;
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	TValue& Vector3D<TValue>::operator[](std::size_t index) noexcept
	{
		assert(index < 3);
		switch (index)
		{
		case 0:
			return value.x;
		case 1:
			return value.y;
		default:
			return value.z;
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<TValue> Vector3D<TValue>::operator+() const noexcept
	{
		return *this;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<TValue> Vector3D<TValue>::operator+(const Vector3D& other) const noexcept
	{
		if constexpr (std::same_as<TValue, float>)
		{
			Vector3D result;
			Store(result, DirectX::XMVectorAdd(Load(*this), Load(other)));
			return result;
		}
		else
		{
			return Vector3D(GetX() + other.GetX(), GetY() + other.GetY(), GetZ() + other.GetZ());
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<TValue>& Vector3D<TValue>::operator+=(const Vector3D& other) noexcept
	{
		*this = *this + other;
		return *this;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<TValue> Vector3D<TValue>::operator-() const noexcept
	{
		return Vector3D(-GetX(), -GetY(), -GetZ());
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<TValue> Vector3D<TValue>::operator-(const Vector3D& other) const noexcept
	{
		if constexpr (std::same_as<TValue, float>)
		{
			Vector3D result;
			Store(result, DirectX::XMVectorSubtract(Load(*this), Load(other)));
			return result;
		}
		else
		{
			return Vector3D(GetX() - other.GetX(), GetY() - other.GetY(), GetZ() - other.GetZ());
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<TValue>& Vector3D<TValue>::operator-=(const Vector3D& other) noexcept
	{
		*this = *this - other;
		return *this;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<TValue> Vector3D<TValue>::operator*(TValue scalar) const noexcept
	{
		if constexpr (std::same_as<TValue, float>)
		{
			Vector3D result;
			Store(result, DirectX::XMVectorScale(Load(*this), scalar));
			return result;
		}
		else
		{
			return Vector3D(GetX() * scalar, GetY() * scalar, GetZ() * scalar);
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<TValue>& Vector3D<TValue>::operator*=(TValue scalar) noexcept
	{
		*this = *this * scalar;
		return *this;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<TValue> Vector3D<TValue>::operator/(TValue scalar) const noexcept
	{
		assert(scalar != TValue{});
		if constexpr (std::same_as<TValue, float>)
		{
			Vector3D result;
			Store(result, DirectX::XMVectorDivide(Load(*this), DirectX::XMVectorReplicate(scalar)));
			return result;
		}
		else
		{
			return Vector3D(GetX() / scalar, GetY() / scalar, GetZ() / scalar);
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<TValue>& Vector3D<TValue>::operator/=(TValue scalar) noexcept
	{
		*this = *this / scalar;
		return *this;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	bool Vector3D<TValue>::operator==(const Vector3D& other) const noexcept
	{
		return GetX() == other.GetX() && GetY() == other.GetY() && GetZ() == other.GetZ();
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	bool Vector3D<TValue>::operator!=(const Vector3D& other) const noexcept
	{
		return !(*this == other);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	std::partial_ordering Vector3D<TValue>::operator<=>(const Vector3D& other) const noexcept
	{
		if (const auto order = GetX() <=> other.GetX(); order != 0)
		{
			return order;
		}
		if (const auto order = GetY() <=> other.GetY(); order != 0)
		{
			return order;
		}
		return GetZ() <=> other.GetZ();
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	TValue Vector3D<TValue>::GetX() const noexcept
	{
		return value.x;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	void Vector3D<TValue>::SetX(TValue x) noexcept
	{
		value.x = x;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	TValue Vector3D<TValue>::GetY() const noexcept
	{
		return value.y;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	void Vector3D<TValue>::SetY(TValue y) noexcept
	{
		value.y = y;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	TValue Vector3D<TValue>::GetZ() const noexcept
	{
		return value.z;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	void Vector3D<TValue>::SetZ(TValue z) noexcept
	{
		value.z = z;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	void Vector3D<TValue>::Set(TValue x, TValue y, TValue z) noexcept
	{
		SetX(x);
		SetY(y);
		SetZ(z);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	float Vector3D<TValue>::GetMagnitude() const noexcept
		requires std::same_as<TValue, float>
	{
		return DirectX::XMVectorGetX(DirectX::XMVector3Length(Load(*this)));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	float Vector3D<TValue>::GetSqrMagnitude() const noexcept
		requires std::same_as<TValue, float>
	{
		return Dot(*this, *this);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	float Vector3D<TValue>::GetLength() const noexcept
		requires std::same_as<TValue, float>
	{
		return GetMagnitude();
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	float Vector3D<TValue>::GetLengthSquared() const noexcept
		requires std::same_as<TValue, float>
	{
		return GetSqrMagnitude();
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<TValue> Vector3D<TValue>::GetNormalized() const noexcept
		requires std::same_as<TValue, float>
	{
		return Normalize(*this);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	bool Vector3D<TValue>::IsZero(float epsilon) const noexcept
	{
		assert(epsilon >= 0.0f);
		return std::abs(static_cast<double>(GetX())) <= epsilon && std::abs(static_cast<double>(GetY())) <= epsilon && std::abs(static_cast<double>(GetZ())) <= epsilon;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	bool Vector3D<TValue>::IsFinite() const noexcept
	{
		return std::isfinite(static_cast<double>(GetX())) && std::isfinite(static_cast<double>(GetY())) && std::isfinite(static_cast<double>(GetZ()));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	void Vector3D<TValue>::Normalize() noexcept
		requires std::same_as<TValue, float>
	{
		*this = GetNormalized();
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	float Vector3D<TValue>::Dot(const Vector3D& other) const noexcept
		requires std::same_as<TValue, float>
	{
		return Dot(*this, other);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<TValue> Vector3D<TValue>::GetZero() noexcept
	{
		return Vector3D<TValue>(static_cast<TValue>(0), static_cast<TValue>(0), static_cast<TValue>(0));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<TValue> Vector3D<TValue>::GetOne() noexcept
	{
		return Vector3D<TValue>(static_cast<TValue>(1), static_cast<TValue>(1), static_cast<TValue>(1));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<TValue> Vector3D<TValue>::GetUp() noexcept
	{
		return Vector3D<TValue>(static_cast<TValue>(0), static_cast<TValue>(1), static_cast<TValue>(0));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<TValue> Vector3D<TValue>::GetDown() noexcept
	{
		return Vector3D<TValue>(static_cast<TValue>(0), static_cast<TValue>(-1), static_cast<TValue>(0));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<TValue> Vector3D<TValue>::GetLeft() noexcept
	{
		return Vector3D<TValue>(static_cast<TValue>(-1), static_cast<TValue>(0), static_cast<TValue>(0));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<TValue> Vector3D<TValue>::GetRight() noexcept
	{
		return Vector3D<TValue>(static_cast<TValue>(1), static_cast<TValue>(0), static_cast<TValue>(0));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<TValue> Vector3D<TValue>::GetForward() noexcept
	{
		return Vector3D<TValue>(static_cast<TValue>(0), static_cast<TValue>(0), static_cast<TValue>(1));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<TValue> Vector3D<TValue>::GetBack() noexcept
	{
		return Vector3D<TValue>(static_cast<TValue>(0), static_cast<TValue>(0), static_cast<TValue>(-1));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<TValue> Vector3D<TValue>::GetPositiveInfinity() noexcept
		requires std::same_as<TValue, float>
	{
		const float infinity = std::numeric_limits<float>::infinity();
		    return Vector3D<TValue>(infinity, infinity, infinity);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<TValue> Vector3D<TValue>::GetNegativeInfinity() noexcept
		requires std::same_as<TValue, float>
	{
		const float negativeInfinity = -std::numeric_limits<float>::infinity();
		    return Vector3D<TValue>(negativeInfinity, negativeInfinity, negativeInfinity);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	DirectX::XMVECTOR Vector3D<TValue>::Load(const Vector3D& vector) noexcept
		requires std::same_as<TValue, float>
	{
		return DirectX::XMLoadFloat3(&vector.value);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	void Vector3D<TValue>::Store(Vector3D& destination, DirectX::XMVECTOR source) noexcept
		requires std::same_as<TValue, float>
	{
		DirectX::XMStoreFloat3(&destination.value, source);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	bool Vector3D<TValue>::IsApproximately(const Vector3D<TValue>& lhs, const Vector3D<TValue>& rhs, float epsilon) noexcept
		requires std::same_as<TValue, float>
	{
		return std::abs(lhs.GetX() - rhs.GetX()) <= epsilon
		        && std::abs(lhs.GetY() - rhs.GetY()) <= epsilon
		        && std::abs(lhs.GetZ() - rhs.GetZ()) <= epsilon;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<TValue> Vector3D<TValue>::Max(const Vector3D<TValue>& a, const Vector3D<TValue>& b) noexcept
		requires std::same_as<TValue, float>
	{
		return Vector3D<TValue>(std::max(a.GetX(), b.GetX()), std::max(a.GetY(), b.GetY()), std::max(a.GetZ(), b.GetZ()));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<TValue> Vector3D<TValue>::Min(const Vector3D<TValue>& a, const Vector3D<TValue>& b) noexcept
		requires std::same_as<TValue, float>
	{
		return Vector3D<TValue>(std::min(a.GetX(), b.GetX()), std::min(a.GetY(), b.GetY()), std::min(a.GetZ(), b.GetZ()));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<TValue> Vector3D<TValue>::Clamp(const Vector3D<TValue>& value, const Vector3D<TValue>& min, const Vector3D<TValue>& max) noexcept
		requires std::same_as<TValue, float>
	{
		return Vector3D<TValue>(
		        std::clamp(value.GetX(), min.GetX(), max.GetX()),
		        std::clamp(value.GetY(), min.GetY(), max.GetY()),
		        std::clamp(value.GetZ(), min.GetZ(), max.GetZ())
		    );
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<TValue> Vector3D<TValue>::ClampMagnitude(const Vector3D<TValue>& vector, float maxLength) noexcept
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
	Vector3D<TValue> Vector3D<TValue>::Scale(const Vector3D<TValue>& vector, const Vector3D<TValue>& scale) noexcept
		requires std::same_as<TValue, float>
	{
		return Vector3D<TValue>(vector.GetX() * scale.GetX(), vector.GetY() * scale.GetY(), vector.GetZ() * scale.GetZ());
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<TValue> Vector3D<TValue>::Normalize(const Vector3D<TValue>& value) noexcept
		requires std::same_as<TValue, float>
	{
		Vector3D<TValue> res{};
		    Store(res, DirectX::XMVector3Normalize(Load(value)));
		    return res;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	void Vector3D<TValue>::OrthoNormalize(Vector3D<TValue>& normal, Vector3D<TValue>& tangent) noexcept
		requires std::same_as<TValue, float>
	{
		normal = Normalize(normal);
		    tangent = tangent - normal * Dot(normal, tangent);
		    tangent = Normalize(tangent);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	void Vector3D<TValue>::OrthoNormalize(Vector3D<TValue>& normal, Vector3D<TValue>& tangent, Vector3D<TValue>& binormal) noexcept
		requires std::same_as<TValue, float>
	{
		OrthoNormalize(normal, tangent);
		    binormal = Normalize(Cross(normal, tangent));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	float Vector3D<TValue>::Dot(const Vector3D<TValue>& a, const Vector3D<TValue>& b) noexcept
		requires std::same_as<TValue, float>
	{
		return DirectX::XMVectorGetX(DirectX::XMVector3Dot(Load(a), Load(b)));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<TValue> Vector3D<TValue>::Cross(const Vector3D<TValue>& a, const Vector3D<TValue>& b) noexcept
		requires std::same_as<TValue, float>
	{
		Vector3D<TValue> res{};
		    Store(res, DirectX::XMVector3Cross(Load(a), Load(b)));
		    return res;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	float Vector3D<TValue>::Distance(const Vector3D<TValue>& _v1, const Vector3D<TValue>& _v2) noexcept
		requires std::same_as<TValue, float>
	{
		return (_v1 - _v2).GetMagnitude();
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	float Vector3D<TValue>::Angle(const Vector3D<TValue>& a, const Vector3D<TValue>& b) noexcept
		requires std::same_as<TValue, float>
	{
		const Vector3D<TValue> from = Normalize(a);
		    const Vector3D<TValue> to = Normalize(b);
		    const float dot = std::clamp(Dot(from, to), -1.0f, 1.0f);
		    return std::acos(dot);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	float Vector3D<TValue>::SignedAngle(const Vector3D<TValue>& from, const Vector3D<TValue>& to, const Vector3D<TValue>& axis) noexcept
		requires std::same_as<TValue, float>
	{
		const Vector3D<TValue> cross = Cross(from, to);
		    const float angle = Angle(from, to);
		    const float sign = (Dot(axis, cross) >= 0.0f) ? 1.0f : -1.0f;
		    return angle * sign;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<TValue> Vector3D<TValue>::Project(const Vector3D<TValue>& vector, const Vector3D<TValue>& onNormal) noexcept
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
	Vector3D<TValue> Vector3D<TValue>::ProjectOnPlane(const Vector3D<TValue>& vector, const Vector3D<TValue>& planeNormal) noexcept
		requires std::same_as<TValue, float>
	{
		return vector - Project(vector, planeNormal);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<TValue> Vector3D<TValue>::Reflect(const Vector3D<TValue>& _vector, const Vector3D<TValue>& _normal) noexcept
		requires std::same_as<TValue, float>
	{
		Vector3D<TValue> res{};
		    Store(res, DirectX::XMVector3Reflect(Load(_vector), Load(_normal)));
		    return res;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<TValue> Vector3D<TValue>::Refract(const Vector3D<TValue>& _vector, const Vector3D<TValue>& _normal, float _eta) noexcept
		requires std::same_as<TValue, float>
	{
		Vector3D<TValue> res{};
		    Store(res, DirectX::XMVector3Refract(Load(_vector), Load(_normal), _eta));
		    return res;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<TValue> Vector3D<TValue>::Lerp(const Vector3D<TValue>& a, const Vector3D<TValue>& b, float t) noexcept
		requires std::same_as<TValue, float>
	{
		Vector3D<TValue> res{};
		    Store(res, DirectX::XMVectorLerp(Load(a), Load(b), std::clamp(t, 0.0f, 1.0f)));
		    return res;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<TValue> Vector3D<TValue>::LerpUnclamped(const Vector3D<TValue>& a, const Vector3D<TValue>& b, float t) noexcept
		requires std::same_as<TValue, float>
	{
		Vector3D<TValue> res{};
		    Store(res, DirectX::XMVectorLerp(Load(a), Load(b), t));
		    return res;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<TValue> Vector3D<TValue>::Slerp(const Vector3D<TValue>& a, const Vector3D<TValue>& b, float t) noexcept
		requires std::same_as<TValue, float>
	{
		return SlerpUnclamped(a, b, std::clamp(t, 0.0f, 1.0f));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<TValue> Vector3D<TValue>::SlerpUnclamped(const Vector3D<TValue>& a, const Vector3D<TValue>& b, float t) noexcept
		requires std::same_as<TValue, float>
	{
		const float aMag = a.GetMagnitude();
		    const float bMag = b.GetMagnitude();
		
		    if (aMag <= std::numeric_limits<float>::epsilon() || bMag <= std::numeric_limits<float>::epsilon())
		    {
		        return LerpUnclamped(a, b, t);
		    }
		
		    const Vector3D<TValue> from = a / aMag;
		    const Vector3D<TValue> to = b / bMag;
		
		    float dot = std::clamp(Dot(from, to), -1.0f, 1.0f);
		    const float theta = std::acos(dot) * t;
		
		    Vector3D<TValue> relative = to - from * dot;
		    const float relativeMag = relative.GetMagnitude();
		    if (relativeMag <= std::numeric_limits<float>::epsilon())
		    {
		        return LerpUnclamped(a, b, t);
		    }
		
		    relative /= relativeMag;
		    const Vector3D<TValue> direction = from * std::cos(theta) + relative * std::sin(theta);
		    const float magnitude = Mathf::Lerp(aMag, bMag, t);
		
		    return direction * magnitude;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<TValue> Vector3D<TValue>::MoveTowards(const Vector3D<TValue>& current, const Vector3D<TValue>& target, float maxDistanceDelta) noexcept
		requires std::same_as<TValue, float>
	{
		const Vector3D<TValue> delta = target - current;
		    const float distance = delta.GetMagnitude();
		
		    if (distance <= maxDistanceDelta || distance <= std::numeric_limits<float>::epsilon())
		    {
		        return target;
		    }
		
		    return current + (delta / distance) * maxDistanceDelta;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<TValue> Vector3D<TValue>::RotateTowards(const Vector3D<TValue>& current, const Vector3D<TValue>& target, float maxRadiansDelta, float maxMagnitudeDelta) noexcept
		requires std::same_as<TValue, float>
	{
		const float currentMag = current.GetMagnitude();
		    const float targetMag = target.GetMagnitude();
		
		    if (currentMag <= std::numeric_limits<float>::epsilon() || targetMag <= std::numeric_limits<float>::epsilon())
		    {
		        return MoveTowards(current, target, maxMagnitudeDelta);
		    }
		
		    const Vector3D<TValue> currentDir = current / currentMag;
		    const Vector3D<TValue> targetDir = target / targetMag;
		
		    const float angle = Angle(currentDir, targetDir);
		    const float t = (angle <= std::numeric_limits<float>::epsilon()) ? 1.0f : std::min(1.0f, maxRadiansDelta / angle);
		
		    const Vector3D<TValue> newDir = SlerpUnclamped(currentDir, targetDir, t).GetNormalized();
		
		    float deltaMag = targetMag - currentMag;
		    deltaMag = std::clamp(deltaMag, -maxMagnitudeDelta, maxMagnitudeDelta);
		    const float newMag = currentMag + deltaMag;
		
		    return newDir * newMag;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<TValue> Vector3D<TValue>::SmoothDamp(
	const Vector3D<TValue>& current,
	const Vector3D<TValue>& target,
	Vector3D<TValue>& currentVelocity,
	float smoothTime,
	float maxSpeed,
	float deltaTime) noexcept
		requires std::same_as<TValue, float>
	{
		smoothTime = std::max(0.0001f, smoothTime);
		    const float omega = 2.0f / smoothTime;
		    const float x = omega * deltaTime;
		    const float exp = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);
		
		    Vector3D<TValue> change = current - target;
		    const Vector3D<TValue> originalTarget = target;
		
		    const float maxChange = maxSpeed * smoothTime;
		    change = ClampMagnitude(change, maxChange);
		    const Vector3D<TValue> adjustedTarget = current - change;
		
		    const Vector3D<TValue> temp = (currentVelocity + change * omega) * deltaTime;
		    currentVelocity = (currentVelocity - temp * omega) * exp;
		    Vector3D<TValue> output = adjustedTarget + (change + temp) * exp;
		
		    if (Dot(originalTarget - current, output - originalTarget) > 0.0f)
		    {
		        output = originalTarget;
		        currentVelocity = Vector3D<TValue>(0.0f, 0.0f, 0.0f);
		    }
		
		    return output;
	}

	template class Vector3D<int>;
	template class Vector3D<float>;

	Vector3D<int> operator*(int scalar, const Vector3D<int>& vector) noexcept
	{
		return vector * scalar;
	}

	Vector3D<float> operator*(float scalar, const Vector3D<float>& vector) noexcept
	{
		return vector * scalar;
	}

}
