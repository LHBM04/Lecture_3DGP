#include "Precompiled.hpp"
#include "Vector4D.hpp"

#include "Mathf.hpp"
#include "Vector2D.hpp"
#include "Vector3D.hpp"

#include "../Core/Common.hpp"

namespace TUK::Framework
{
	template <Scalarable TScalar>
	Vector4D<TScalar>::Vector4D() noexcept
		: value(TScalar{}, TScalar{}, TScalar{}, TScalar{})
	{
	}

	template <Scalarable TScalar>
	Vector4D<TScalar>::Vector4D(TScalar scalar) noexcept
		: value(scalar, scalar, scalar, scalar)
	{
	}

	template <Scalarable TScalar>
	Vector4D<TScalar>::Vector4D(TScalar x, TScalar y, TScalar z, TScalar w) noexcept
		: value(x, y, z, w)
	{
	}

	template <Scalarable TScalar>
	Vector4D<TScalar>::Vector4D(const Vector2D<TScalar>& vector, TScalar z, TScalar w) noexcept
		: value(TScalar{}, TScalar{}, TScalar{}, TScalar{})
	{
		Set(vector.GetX(), vector.GetY(), z, w);
	}

	template <Scalarable TScalar>
	Vector4D<TScalar>::Vector4D(const Vector3D<TScalar>& vector, TScalar w) noexcept
		: value(TScalar{}, TScalar{}, TScalar{}, TScalar{})
	{
		Set(vector.GetX(), vector.GetY(), vector.GetZ(), w);
	}

	template <Scalarable TScalar>
	Vector4D<TScalar>::Vector4D(DirectX::XMVECTOR vector) noexcept
		requires std::same_as<TScalar, f32>
		: value(TScalar{}, TScalar{}, TScalar{}, TScalar{})
	{
		Store(*this, vector);
	}

	template <Scalarable TScalar>
	Vector4D<TScalar>::Vector4D(const DirectX::XMFLOAT4& source) noexcept
		requires std::same_as<TScalar, f32>
		: value(source)
	{
	}

	template <Scalarable TScalar>
	Vector4D<TScalar>::Vector4D(const DirectX::XMINT4& source) noexcept
		requires std::same_as<TScalar, i32>
		: value(source)
	{
	}

	template <Scalarable TScalar>
	Vector4D<TScalar>::operator Vector2D<TScalar>() const noexcept
	{
		return Vector2D<TScalar>(GetX(), GetY());
	}

	template <Scalarable TScalar>
	Vector4D<TScalar>::operator Vector3D<TScalar>() const noexcept
	{
		return Vector3D<TScalar>(GetX(), GetY(), GetZ());
	}

	template <Scalarable TScalar>
	Vector4D<TScalar>::operator DirectX::XMVECTOR() const noexcept
		requires std::same_as<TScalar, f32>
	{
		return Load(*this);
	}

	template <Scalarable TScalar>
	Vector4D<TScalar>::operator DirectX::XMFLOAT4() const noexcept
		requires std::same_as<TScalar, f32>
	{
		return value;
	}

	template <Scalarable TScalar>
	Vector4D<TScalar>::operator DirectX::XMINT4() const noexcept
		requires std::same_as<TScalar, i32>
	{
		return value;
	}

	template <Scalarable TScalar>
	TScalar Vector4D<TScalar>::operator[](std::size_t index) const noexcept
	{
		ASSERT_FMT(index < 4, "index가 유효 범위(0~3)를 벗어났습니다.");

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

	template <Scalarable TScalar>
	TScalar& Vector4D<TScalar>::operator[](std::size_t index) noexcept
	{
		ASSERT_FMT(index < 4, "index가 유효 범위(0~3)를 벗어났습니다.");

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

	template <Scalarable TScalar>
	Vector4D<TScalar> Vector4D<TScalar>::operator+() const noexcept
	{
		return *this;
	}

	template <Scalarable TScalar>
	Vector4D<TScalar> Vector4D<TScalar>::operator+(const Vector4D& other) const noexcept
	{
		if constexpr (std::same_as<TScalar, float>)
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

	template <Scalarable TScalar>
	Vector4D<TScalar>& Vector4D<TScalar>::operator+=(const Vector4D& other) noexcept
	{
		*this = *this + other;
		return *this;
	}

	template <Scalarable TScalar>
	Vector4D<TScalar> Vector4D<TScalar>::operator-() const noexcept
	{
		return Vector4D(-GetX(), -GetY(), -GetZ(), -GetW());
	}

	template <Scalarable TScalar>
	Vector4D<TScalar> Vector4D<TScalar>::operator-(const Vector4D& other) const noexcept
	{
		if constexpr (std::same_as<TScalar, float>)
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

	template <Scalarable TScalar>
	Vector4D<TScalar>& Vector4D<TScalar>::operator-=(const Vector4D& other) noexcept
	{
		*this = *this - other;
		return *this;
	}

	template <Scalarable TScalar>
	Vector4D<TScalar> Vector4D<TScalar>::operator*(TScalar scalar) const noexcept
	{
		if constexpr (std::same_as<TScalar, float>)
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

	template <Scalarable TScalar>
	Vector4D<TScalar>& Vector4D<TScalar>::operator*=(TScalar scalar) noexcept
	{
		*this = *this * scalar;
		return *this;
	}

	template <Scalarable TScalar>
	Vector4D<TScalar> Vector4D<TScalar>::operator/(TScalar scalar) const noexcept
	{
		ASSERT_FMT(scalar != TScalar{}, "scalar가 0입니다.");

		if constexpr (std::same_as<TScalar, float>)
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

	template <Scalarable TScalar>
	Vector4D<TScalar>& Vector4D<TScalar>::operator/=(TScalar scalar) noexcept
	{
		*this = *this / scalar;
		return *this;
	}

	template <Scalarable TScalar>
	bool Vector4D<TScalar>::operator==(const Vector4D& other) const noexcept
	{
		return GetX() == other.GetX() && GetY() == other.GetY() && GetZ() == other.GetZ() && GetW() == other.GetW();
	}

	template <Scalarable TScalar>
	bool Vector4D<TScalar>::operator!=(const Vector4D& other) const noexcept
	{
		return !(*this == other);
	}

	template <Scalarable TScalar>
	std::partial_ordering Vector4D<TScalar>::operator<=>(const Vector4D& other) const noexcept
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

	template <Scalarable TScalar>
	TScalar Vector4D<TScalar>::GetX() const noexcept
	{
		return value.x;
	}

	template <Scalarable TScalar>
	void Vector4D<TScalar>::SetX(TScalar x) noexcept
	{
		value.x = x;
	}

	template <Scalarable TScalar>
	TScalar Vector4D<TScalar>::GetY() const noexcept
	{
		return value.y;
	}

	template <Scalarable TScalar>
	void Vector4D<TScalar>::SetY(TScalar y) noexcept
	{
		value.y = y;
	}

	template <Scalarable TScalar>
	TScalar Vector4D<TScalar>::GetZ() const noexcept
	{
		return value.z;
	}

	template <Scalarable TScalar>
	void Vector4D<TScalar>::SetZ(TScalar z) noexcept
	{
		value.z = z;
	}

	template <Scalarable TScalar>
	TScalar Vector4D<TScalar>::GetW() const noexcept
	{
		return value.w;
	}

	template <Scalarable TScalar>
	void Vector4D<TScalar>::SetW(TScalar w) noexcept
	{
		value.w = w;
	}

	template <Scalarable TScalar>
	void Vector4D<TScalar>::Set(TScalar x, TScalar y, TScalar z, TScalar w) noexcept
	{
		SetX(x);
		SetY(y);
		SetZ(z);
		SetW(w);
	}

	template <Scalarable TScalar>
	float Vector4D<TScalar>::GetMagnitude() const noexcept
		requires std::same_as<TScalar, f32>
	{
		return DirectX::XMVectorGetX(DirectX::XMVector4Length(Load(*this)));
	}

	template <Scalarable TScalar>
	float Vector4D<TScalar>::GetSqrMagnitude() const noexcept
		requires std::same_as<TScalar, f32>
	{
		return Dot(*this, *this);
	}

	template <Scalarable TScalar>
	float Vector4D<TScalar>::GetLength() const noexcept
		requires std::same_as<TScalar, f32>
	{
		return GetMagnitude();
	}

	template <Scalarable TScalar>
	float Vector4D<TScalar>::GetLengthSquared() const noexcept
		requires std::same_as<TScalar, f32>
	{
		return GetSqrMagnitude();
	}

	template <Scalarable TScalar>
	Vector4D<TScalar> Vector4D<TScalar>::GetNormalized() const noexcept
		requires std::same_as<TScalar, f32>
	{
		return Normalize(*this);
	}

	template <Scalarable TScalar>
	bool Vector4D<TScalar>::IsZero(float epsilon) const noexcept
	{
		ASSERT_FMT(epsilon >= 0.0f, "epsilon이 0 이상이어야 합니다.");

		return std::abs(static_cast<double>(GetX())) <= epsilon && std::abs(static_cast<double>(GetY())) <= epsilon && std::abs(static_cast<double>(GetZ())) <= epsilon && std::abs(static_cast<double>(GetW())) <= epsilon;
	}

	template <Scalarable TScalar>
	bool Vector4D<TScalar>::IsFinite() const noexcept
	{
		return std::isfinite(static_cast<double>(GetX())) && std::isfinite(static_cast<double>(GetY())) && std::isfinite(static_cast<double>(GetZ())) && std::isfinite(static_cast<double>(GetW()));
	}

	template <Scalarable TScalar>
	void Vector4D<TScalar>::Normalize() noexcept
		requires std::same_as<TScalar, f32>
	{
		*this = GetNormalized();
	}

	template <Scalarable TScalar>
	float Vector4D<TScalar>::Dot(const Vector4D& other) const noexcept
		requires std::same_as<TScalar, f32>
	{
		return Dot(*this, other);
	}

	template <Scalarable TScalar>
	Vector4D<TScalar> Vector4D<TScalar>::GetZero() noexcept
	{
		return Vector4D<TScalar>(static_cast<TScalar>(0), static_cast<TScalar>(0), static_cast<TScalar>(0), static_cast<TScalar>(0));
	}

	template <Scalarable TScalar>
	Vector4D<TScalar> Vector4D<TScalar>::GetOne() noexcept
	{
		return Vector4D<TScalar>(static_cast<TScalar>(1), static_cast<TScalar>(1), static_cast<TScalar>(1), static_cast<TScalar>(1));
	}

	template <Scalarable TScalar>
	Vector4D<TScalar> Vector4D<TScalar>::GetPositiveInfinity() noexcept
		requires std::same_as<TScalar, f32>
	{
		const float infinity = std::numeric_limits<float>::infinity();
		    return Vector4D<TScalar>(infinity, infinity, infinity, infinity);
	}

	template <Scalarable TScalar>
	Vector4D<TScalar> Vector4D<TScalar>::GetNegativeInfinity() noexcept
		requires std::same_as<TScalar, f32>
	{
		const float negativeInfinity = -std::numeric_limits<float>::infinity();
		    return Vector4D<TScalar>(negativeInfinity, negativeInfinity, negativeInfinity, negativeInfinity);
	}

	template <Scalarable TScalar>
	DirectX::XMVECTOR Vector4D<TScalar>::Load(const Vector4D& vector) noexcept
		requires std::same_as<TScalar, f32>
	{
		return DirectX::XMLoadFloat4(&vector.value);
	}

	template <Scalarable TScalar>
	void Vector4D<TScalar>::Store(Vector4D& destination, DirectX::XMVECTOR source) noexcept
		requires std::same_as<TScalar, f32>
	{
		DirectX::XMStoreFloat4(&destination.value, source);
	}

	template <Scalarable TScalar>
	bool Vector4D<TScalar>::IsApproximately(const Vector4D<TScalar>& lhs, const Vector4D<TScalar>& rhs, float epsilon) noexcept
		requires std::same_as<TScalar, f32>
	{
		return std::abs(lhs.GetX() - rhs.GetX()) <= epsilon
		        && std::abs(lhs.GetY() - rhs.GetY()) <= epsilon
		        && std::abs(lhs.GetZ() - rhs.GetZ()) <= epsilon
		        && std::abs(lhs.GetW() - rhs.GetW()) <= epsilon;
	}

	template <Scalarable TScalar>
	Vector4D<TScalar> Vector4D<TScalar>::Max(const Vector4D<TScalar>& a, const Vector4D<TScalar>& b) noexcept
		requires std::same_as<TScalar, f32>
	{
		return Vector4D<TScalar>(std::max(a.GetX(), b.GetX()), std::max(a.GetY(), b.GetY()), std::max(a.GetZ(), b.GetZ()), std::max(a.GetW(), b.GetW()));
	}

	template <Scalarable TScalar>
	Vector4D<TScalar> Vector4D<TScalar>::Min(const Vector4D<TScalar>& a, const Vector4D<TScalar>& b) noexcept
		requires std::same_as<TScalar, f32>
	{
		return Vector4D<TScalar>(std::min(a.GetX(), b.GetX()), std::min(a.GetY(), b.GetY()), std::min(a.GetZ(), b.GetZ()), std::min(a.GetW(), b.GetW()));
	}

	template <Scalarable TScalar>
	Vector4D<TScalar> Vector4D<TScalar>::Clamp(const Vector4D<TScalar>& value, const Vector4D<TScalar>& min, const Vector4D<TScalar>& max) noexcept
		requires std::same_as<TScalar, f32>
	{
		return Vector4D<TScalar>(
		        std::clamp(value.GetX(), min.GetX(), max.GetX()),
		        std::clamp(value.GetY(), min.GetY(), max.GetY()),
		        std::clamp(value.GetZ(), min.GetZ(), max.GetZ()),
		        std::clamp(value.GetW(), min.GetW(), max.GetW())
		    );
	}

	template <Scalarable TScalar>
	Vector4D<TScalar> Vector4D<TScalar>::ClampMagnitude(const Vector4D<TScalar>& vector, float maxLength) noexcept
		requires std::same_as<TScalar, f32>
	{
		const float sqrMagnitude = vector.GetSqrMagnitude();
		    const float maxSqr = maxLength * maxLength;
		    if (sqrMagnitude <= maxSqr)
		    {
		        return vector;
		    }
		
		    return Normalize(vector) * maxLength;
	}

	template <Scalarable TScalar>
	Vector4D<TScalar> Vector4D<TScalar>::Scale(const Vector4D<TScalar>& vector, const Vector4D<TScalar>& scale) noexcept
		requires std::same_as<TScalar, f32>
	{
		return Vector4D<TScalar>(vector.GetX() * scale.GetX(), vector.GetY() * scale.GetY(), vector.GetZ() * scale.GetZ(), vector.GetW() * scale.GetW());
	}

	template <Scalarable TScalar>
	Vector4D<TScalar> Vector4D<TScalar>::Normalize(const Vector4D<TScalar>& value) noexcept
		requires std::same_as<TScalar, f32>
	{
		Vector4D<TScalar> result{};
		    Store(result, DirectX::XMVector4Normalize(Load(value)));
		    return result;
	}

	template <Scalarable TScalar>
	float Vector4D<TScalar>::Dot(const Vector4D<TScalar>& a, const Vector4D<TScalar>& b) noexcept
		requires std::same_as<TScalar, f32>
	{
		return DirectX::XMVectorGetX(DirectX::XMVector4Dot(Load(a), Load(b)));
	}

	template <Scalarable TScalar>
	float Vector4D<TScalar>::Distance(const Vector4D<TScalar>& a, const Vector4D<TScalar>& b) noexcept
		requires std::same_as<TScalar, f32>
	{
		return (a - b).GetMagnitude();
	}

	template <Scalarable TScalar>
	float Vector4D<TScalar>::Angle(const Vector4D<TScalar>& a, const Vector4D<TScalar>& b) noexcept
		requires std::same_as<TScalar, f32>
	{
		const Vector4D<TScalar> from = Normalize(a);
		    const Vector4D<TScalar> to = Normalize(b);
		    const float dot = std::clamp(Dot(from, to), -1.0f, 1.0f);
		    return std::acos(dot);
	}

	template <Scalarable TScalar>
	Vector4D<TScalar> Vector4D<TScalar>::Project(const Vector4D<TScalar>& vector, const Vector4D<TScalar>& onNormal) noexcept
		requires std::same_as<TScalar, f32>
	{
		const float denominator = Dot(onNormal, onNormal);
		    if (denominator <= std::numeric_limits<float>::epsilon())
		    {
		        return GetZero();
		    }
		
		    const float scale = Dot(vector, onNormal) / denominator;
		    return onNormal * scale;
	}

	template <Scalarable TScalar>
	Vector4D<TScalar> Vector4D<TScalar>::Reflect(const Vector4D<TScalar>& vector, const Vector4D<TScalar>& normal) noexcept
		requires std::same_as<TScalar, f32>
	{
		return vector - normal * (2.0f * Dot(vector, normal));
	}

	template <Scalarable TScalar>
	Vector4D<TScalar> Vector4D<TScalar>::Lerp(const Vector4D<TScalar>& a, const Vector4D<TScalar>& b, float t) noexcept
		requires std::same_as<TScalar, f32>
	{
		Vector4D<TScalar> result{};
		    Store(result, DirectX::XMVectorLerp(Load(a), Load(b), std::clamp(t, 0.0f, 1.0f)));
		    return result;
	}

	template <Scalarable TScalar>
	Vector4D<TScalar> Vector4D<TScalar>::LerpUnclamped(const Vector4D<TScalar>& a, const Vector4D<TScalar>& b, float t) noexcept
		requires std::same_as<TScalar, f32>
	{
		Vector4D<TScalar> result{};
		    Store(result, DirectX::XMVectorLerp(Load(a), Load(b), t));
		    return result;
	}

	template <Scalarable TScalar>
	Vector4D<TScalar> Vector4D<TScalar>::Slerp(const Vector4D<TScalar>& a, const Vector4D<TScalar>& b, float t) noexcept
		requires std::same_as<TScalar, f32>
	{
		return SlerpUnclamped(a, b, std::clamp(t, 0.0f, 1.0f));
	}

	template <Scalarable TScalar>
	Vector4D<TScalar> Vector4D<TScalar>::SlerpUnclamped(const Vector4D<TScalar>& a, const Vector4D<TScalar>& b, float t) noexcept
		requires std::same_as<TScalar, f32>
	{
		const float aMag = a.GetMagnitude();
		    const float bMag = b.GetMagnitude();
		
		    if (aMag <= std::numeric_limits<float>::epsilon() || bMag <= std::numeric_limits<float>::epsilon())
		    {
		        return LerpUnclamped(a, b, t);
		    }
		
		    const Vector4D<TScalar> from = a / aMag;
		    const Vector4D<TScalar> to = b / bMag;
		
		    float dot = std::clamp(Dot(from, to), -1.0f, 1.0f);
		    const float theta = std::acos(dot) * t;
		
		    Vector4D<TScalar> relative = to - from * dot;
		    const float relativeMag = relative.GetMagnitude();
		    if (relativeMag <= std::numeric_limits<float>::epsilon())
		    {
		        return LerpUnclamped(a, b, t);
		    }
		
		    relative /= relativeMag;
		    const Vector4D<TScalar> direction = from * std::cos(theta) + relative * std::sin(theta);
		    const float magnitude = Mathf::Lerp(aMag, bMag, t);
		
		    return direction * magnitude;
	}

	template <Scalarable TScalar>
	Vector4D<TScalar> Vector4D<TScalar>::MoveTowards(const Vector4D<TScalar>& current, const Vector4D<TScalar>& target, float maxDistanceDelta) noexcept
		requires std::same_as<TScalar, f32>
	{
		const Vector4D<TScalar> delta = target - current;
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
