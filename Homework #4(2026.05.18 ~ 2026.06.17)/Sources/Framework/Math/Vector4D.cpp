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
		return Vector4D<TValue>(TValue{0}, TValue{0}, TValue{0}, TValue{0});
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue> Vector4D<TValue>::GetOne() noexcept
	{
		return Vector4D<TValue>(TValue{1}, TValue{1}, TValue{1}, TValue{1});
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue> Vector4D<TValue>::GetPositiveInfinity() noexcept
		requires std::same_as<TValue, float>
	{
		const float infinity{ std::numeric_limits<float>::infinity() };
		    return Vector4D<TValue>(infinity, infinity, infinity, infinity);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue> Vector4D<TValue>::GetNegativeInfinity() noexcept
		requires std::same_as<TValue, float>
	{
		const float negativeInfinity{ -std::numeric_limits<float>::infinity() };
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
	bool Vector4D<TValue>::IsApproximately(const Vector4D<TValue>& lhs_, const Vector4D<TValue>& rhs_, float epsilon_) noexcept
		requires std::same_as<TValue, float>
	{
		return std::abs(lhs_.GetX() - rhs_.GetX()) <= epsilon_
		        && std::abs(lhs_.GetY() - rhs_.GetY()) <= epsilon_
		        && std::abs(lhs_.GetZ() - rhs_.GetZ()) <= epsilon_
		        && std::abs(lhs_.GetW() - rhs_.GetW()) <= epsilon_;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue> Vector4D<TValue>::Max(const Vector4D<TValue>& a_, const Vector4D<TValue>& b_) noexcept
		requires std::same_as<TValue, float>
	{
		return Vector4D<TValue>(std::max(a_.GetX(), b_.GetX()), std::max(a_.GetY(), b_.GetY()), std::max(a_.GetZ(), b_.GetZ()), std::max(a_.GetW(), b_.GetW()));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue> Vector4D<TValue>::Min(const Vector4D<TValue>& a_, const Vector4D<TValue>& b_) noexcept
		requires std::same_as<TValue, float>
	{
		return Vector4D<TValue>(std::min(a_.GetX(), b_.GetX()), std::min(a_.GetY(), b_.GetY()), std::min(a_.GetZ(), b_.GetZ()), std::min(a_.GetW(), b_.GetW()));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue> Vector4D<TValue>::Clamp(const Vector4D<TValue>& value_, const Vector4D<TValue>& min_, const Vector4D<TValue>& max_) noexcept
		requires std::same_as<TValue, float>
	{
		return Vector4D<TValue>(
		        std::clamp(value_.GetX(), min_.GetX(), max_.GetX()),
		        std::clamp(value_.GetY(), min_.GetY(), max_.GetY()),
		        std::clamp(value_.GetZ(), min_.GetZ(), max_.GetZ()),
		        std::clamp(value_.GetW(), min_.GetW(), max_.GetW())
		    );
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue> Vector4D<TValue>::ClampMagnitude(const Vector4D<TValue>& vector_, float maxLength_) noexcept
		requires std::same_as<TValue, float>
	{
		const float sqrMagnitude{ vector_.GetSqrMagnitude() };
		    const float maxSqr{ maxLength_ * maxLength_ };
		    if (sqrMagnitude <= maxSqr)
		    {
		        return vector_;
		    }
		
		    return Normalize(vector_) * maxLength_;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue> Vector4D<TValue>::Scale(const Vector4D<TValue>& vector_, const Vector4D<TValue>& scale_) noexcept
		requires std::same_as<TValue, float>
	{
		return Vector4D<TValue>(vector_.GetX() * scale_.GetX(), vector_.GetY() * scale_.GetY(), vector_.GetZ() * scale_.GetZ(), vector_.GetW() * scale_.GetW());
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue> Vector4D<TValue>::Normalize(const Vector4D<TValue>& value_) noexcept
		requires std::same_as<TValue, float>
	{
		Vector4D<TValue> result;
		    Store(result, DirectX::XMVector4Normalize(Load(value_)));
		    return result;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	float Vector4D<TValue>::Dot(const Vector4D<TValue>& a_, const Vector4D<TValue>& b_) noexcept
		requires std::same_as<TValue, float>
	{
		return DirectX::XMVectorGetX(DirectX::XMVector4Dot(Load(a_), Load(b_)));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	float Vector4D<TValue>::Distance(const Vector4D<TValue>& a_, const Vector4D<TValue>& b_) noexcept
		requires std::same_as<TValue, float>
	{
		return (a_ - b_).GetMagnitude();
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	float Vector4D<TValue>::Angle(const Vector4D<TValue>& a_, const Vector4D<TValue>& b_) noexcept
		requires std::same_as<TValue, float>
	{
		const Vector4D<TValue> from{ Normalize(a_) };
		    const Vector4D<TValue> to{ Normalize(b_) };
		    const float dot{ std::clamp(Dot(from, to), -1.0f, 1.0f) };
		    return std::acos(dot);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue> Vector4D<TValue>::Project(const Vector4D<TValue>& vector_, const Vector4D<TValue>& onNormal_) noexcept
		requires std::same_as<TValue, float>
	{
		const float denominator{ Dot(onNormal_, onNormal_) };
		    if (denominator <= std::numeric_limits<float>::epsilon())
		    {
		        return GetZero();
		    }
		
		    const float scale{ Dot(vector_, onNormal_) / denominator };
		    return onNormal_ * scale;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue> Vector4D<TValue>::Reflect(const Vector4D<TValue>& vector_, const Vector4D<TValue>& normal_) noexcept
		requires std::same_as<TValue, float>
	{
		return vector_ - normal_ * (2.0f * Dot(vector_, normal_));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue> Vector4D<TValue>::Lerp(const Vector4D<TValue>& a_, const Vector4D<TValue>& b_, float t_) noexcept
		requires std::same_as<TValue, float>
	{
		Vector4D<TValue> result;
		    Store(result, DirectX::XMVectorLerp(Load(a_), Load(b_), std::clamp(t_, 0.0f, 1.0f)));
		    return result;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue> Vector4D<TValue>::LerpUnclamped(const Vector4D<TValue>& a_, const Vector4D<TValue>& b_, float t_) noexcept
		requires std::same_as<TValue, float>
	{
		Vector4D<TValue> result;
		    Store(result, DirectX::XMVectorLerp(Load(a_), Load(b_), t_));
		    return result;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue> Vector4D<TValue>::Slerp(const Vector4D<TValue>& a_, const Vector4D<TValue>& b_, float t_) noexcept
		requires std::same_as<TValue, float>
	{
		return SlerpUnclamped(a_, b_, std::clamp(t_, 0.0f, 1.0f));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue> Vector4D<TValue>::SlerpUnclamped(const Vector4D<TValue>& a_, const Vector4D<TValue>& b_, float t_) noexcept
		requires std::same_as<TValue, float>
	{
		const float aMag{ a_.GetMagnitude() };
		    const float bMag{ b_.GetMagnitude() };
		
		    if (aMag <= std::numeric_limits<float>::epsilon() || bMag <= std::numeric_limits<float>::epsilon())
		    {
		        return LerpUnclamped(a_, b_, t_);
		    }
		
		    const Vector4D<TValue> from{ a_ / aMag };
		    const Vector4D<TValue> to{ b_ / bMag };
		
		    float dot{ std::clamp(Dot(from, to), -1.0f, 1.0f) };
		    const float theta{ std::acos(dot) * t_ };
		
		    Vector4D<TValue> relative{ to - from * dot };
		    const float relativeMag{ relative.GetMagnitude() };
		    if (relativeMag <= std::numeric_limits<float>::epsilon())
		    {
		        return LerpUnclamped(a_, b_, t_);
		    }
		
		    relative /= relativeMag;
		    const Vector4D<TValue> direction{ from * std::cos(theta) + relative * std::sin(theta) };
		    const float magnitude{ Mathf::Lerp(aMag, bMag, t_) };
		
		    return direction * magnitude;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue> Vector4D<TValue>::MoveTowards(const Vector4D<TValue>& current_, const Vector4D<TValue>& target_, float maxDistanceDelta_) noexcept
		requires std::same_as<TValue, float>
	{
		const Vector4D<TValue> delta{ target_ - current_ };
		    const float distance{ delta.GetMagnitude() };
		
		    if (distance <= maxDistanceDelta_ || distance <= std::numeric_limits<float>::epsilon())
		    {
		        return target_;
		    }
		
		    return current_ + (delta / distance) * maxDistanceDelta_;
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
