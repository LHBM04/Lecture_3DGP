#include "Precompiled.h"
#include "Mathf.h"
#include "Vector2D.h"

#include "Vector3D.h"
#include "Vector4D.h"

namespace TUK::Framework
{
	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector2D<TValue>::Vector2D() noexcept
		: value(TValue{}, TValue{})
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector2D<TValue>::Vector2D(TValue scalar) noexcept
		: value(scalar, scalar)
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector2D<TValue>::Vector2D(TValue x, TValue y) noexcept
		: value(x, y)
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector2D<TValue>::Vector2D(const Vector2D& other) noexcept
		: value(other.value)
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector2D<TValue>::Vector2D(Vector2D&& other) noexcept
		: value(std::move(other.value))
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector2D<TValue>::Vector2D(const Vector3D<TValue>& vector) noexcept
		: value(TValue{}, TValue{})
	{
		Set(vector.GetX(), vector.GetY());
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector2D<TValue>::Vector2D(const Vector4D<TValue>& vector) noexcept
		: value(TValue{}, TValue{})
	{
		Set(vector.GetX(), vector.GetY());
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector2D<TValue>& Vector2D<TValue>::operator=(const Vector2D& other) noexcept
	{
		value = other.value;
		return *this;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector2D<TValue>& Vector2D<TValue>::operator=(Vector2D&& other) noexcept
	{
		value = std::move(other.value);
		return *this;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector2D<TValue>::operator Vector3D<TValue>() const noexcept
	{
		return Vector3D<TValue>(GetX(), GetY(), TValue{});
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector2D<TValue>::operator Vector4D<TValue>() const noexcept
	{
		return Vector4D<TValue>(GetX(), GetY(), TValue{}, TValue{});
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	TValue Vector2D<TValue>::operator[](std::size_t index) const noexcept
	{
		assert(index < 2);
		switch (index)
		{
		case 0:
			return value.x;
		default:
			return value.y;
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	TValue& Vector2D<TValue>::operator[](std::size_t index) noexcept
	{
		assert(index < 2);
		switch (index)
		{
		case 0:
			return value.x;
		default:
			return value.y;
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector2D<TValue> Vector2D<TValue>::operator+() const noexcept
	{
		return *this;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector2D<TValue> Vector2D<TValue>::operator+(const Vector2D& other) const noexcept
	{
		if constexpr (std::same_as<TValue, float>)
		{
			Vector2D result;
			Store(result, DirectX::XMVectorAdd(Load(*this), Load(other)));
			return result;
		}
		else
		{
			return Vector2D(GetX() + other.GetX(), GetY() + other.GetY());
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector2D<TValue>& Vector2D<TValue>::operator+=(const Vector2D& other) noexcept
	{
		*this = *this + other;
		return *this;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector2D<TValue> Vector2D<TValue>::operator-() const noexcept
	{
		return Vector2D(-GetX(), -GetY());
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector2D<TValue> Vector2D<TValue>::operator-(const Vector2D& other) const noexcept
	{
		if constexpr (std::same_as<TValue, float>)
		{
			Vector2D result;
			Store(result, DirectX::XMVectorSubtract(Load(*this), Load(other)));
			return result;
		}
		else
		{
			return Vector2D(GetX() - other.GetX(), GetY() - other.GetY());
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector2D<TValue>& Vector2D<TValue>::operator-=(const Vector2D& other) noexcept
	{
		*this = *this - other;
		return *this;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector2D<TValue> Vector2D<TValue>::operator*(TValue scalar) const noexcept
	{
		if constexpr (std::same_as<TValue, float>)
		{
			Vector2D result;
			Store(result, DirectX::XMVectorScale(Load(*this), scalar));
			return result;
		}
		else
		{
			return Vector2D(GetX() * scalar, GetY() * scalar);
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector2D<TValue>& Vector2D<TValue>::operator*=(TValue scalar) noexcept
	{
		*this = *this * scalar;
		return *this;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector2D<TValue> Vector2D<TValue>::operator/(TValue scalar) const noexcept
	{
		assert(scalar != TValue{});
		if constexpr (std::same_as<TValue, float>)
		{
			Vector2D result;
			Store(result, DirectX::XMVectorDivide(Load(*this), DirectX::XMVectorReplicate(scalar)));
			return result;
		}
		else
		{
			return Vector2D(GetX() / scalar, GetY() / scalar);
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector2D<TValue>& Vector2D<TValue>::operator/=(TValue scalar) noexcept
	{
		*this = *this / scalar;
		return *this;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	bool Vector2D<TValue>::operator==(const Vector2D& other) const noexcept
	{
		return GetX() == other.GetX() && GetY() == other.GetY();
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	bool Vector2D<TValue>::operator!=(const Vector2D& other) const noexcept
	{
		return !(*this == other);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	std::partial_ordering Vector2D<TValue>::operator<=>(const Vector2D& other) const noexcept
	{
		if (const auto order = GetX() <=> other.GetX(); order != 0)
		{
			return order;
		}
		return GetY() <=> other.GetY();
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	TValue Vector2D<TValue>::GetX() const noexcept
	{
		return value.x;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	void Vector2D<TValue>::SetX(TValue x) noexcept
	{
		value.x = x;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	TValue Vector2D<TValue>::GetY() const noexcept
	{
		return value.y;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	void Vector2D<TValue>::SetY(TValue y) noexcept
	{
		value.y = y;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	void Vector2D<TValue>::Set(TValue x, TValue y) noexcept
	{
		SetX(x);
		SetY(y);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	float Vector2D<TValue>::GetMagnitude() const noexcept
		requires std::same_as<TValue, float>
	{
		return DirectX::XMVectorGetX(DirectX::XMVector2Length(Load(*this)));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	float Vector2D<TValue>::GetSqrMagnitude() const noexcept
		requires std::same_as<TValue, float>
	{
		return Dot(*this, *this);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	float Vector2D<TValue>::GetLength() const noexcept
		requires std::same_as<TValue, float>
	{
		return GetMagnitude();
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	float Vector2D<TValue>::GetLengthSquared() const noexcept
		requires std::same_as<TValue, float>
	{
		return GetSqrMagnitude();
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector2D<TValue> Vector2D<TValue>::GetNormalized() const noexcept
		requires std::same_as<TValue, float>
	{
		return Normalize(*this);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	bool Vector2D<TValue>::IsZero(float epsilon) const noexcept
	{
		assert(epsilon >= 0.0f);
		return std::abs(static_cast<double>(GetX())) <= epsilon && std::abs(static_cast<double>(GetY())) <= epsilon;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	bool Vector2D<TValue>::IsFinite() const noexcept
	{
		return std::isfinite(static_cast<double>(GetX())) && std::isfinite(static_cast<double>(GetY()));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	void Vector2D<TValue>::Normalize() noexcept
		requires std::same_as<TValue, float>
	{
		*this = GetNormalized();
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	float Vector2D<TValue>::Dot(const Vector2D& other) const noexcept
		requires std::same_as<TValue, float>
	{
		return Dot(*this, other);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector2D<TValue> Vector2D<TValue>::GetZero() noexcept
	{
		return Vector2D<TValue>(TValue{0}, TValue{0});
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector2D<TValue> Vector2D<TValue>::GetOne() noexcept
	{
		return Vector2D<TValue>(TValue{1}, TValue{1});
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector2D<TValue> Vector2D<TValue>::GetUp() noexcept
	{
		return Vector2D<TValue>(TValue{0}, TValue{1});
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector2D<TValue> Vector2D<TValue>::GetDown() noexcept
	{
		return Vector2D<TValue>(TValue{0}, TValue{-1});
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector2D<TValue> Vector2D<TValue>::GetLeft() noexcept
	{
		return Vector2D<TValue>(TValue{-1}, TValue{0});
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector2D<TValue> Vector2D<TValue>::GetRight() noexcept
	{
		return Vector2D<TValue>(TValue{1}, TValue{0});
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector2D<TValue> Vector2D<TValue>::GetPositiveInfinity() noexcept
		requires std::same_as<TValue, float>
	{
		const float infinity{ std::numeric_limits<float>::infinity() };
			return Vector2D<TValue>(infinity, infinity);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector2D<TValue> Vector2D<TValue>::GetNegativeInfinity() noexcept
		requires std::same_as<TValue, float>
	{
		const float negativeInfinity{ -std::numeric_limits<float>::infinity() };
			return Vector2D<TValue>(negativeInfinity, negativeInfinity);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	DirectX::XMVECTOR Vector2D<TValue>::Load(const Vector2D& vector) noexcept
		requires std::same_as<TValue, float>
	{
		return DirectX::XMLoadFloat2(&vector.value);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	void Vector2D<TValue>::Store(Vector2D& destination, DirectX::XMVECTOR source) noexcept
		requires std::same_as<TValue, float>
	{
		DirectX::XMStoreFloat2(&destination.value, source);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	bool Vector2D<TValue>::IsApproximately(const Vector2D<TValue>& lhs_, const Vector2D<TValue>& rhs_, float epsilon_) noexcept
		requires std::same_as<TValue, float>
	{
		return std::abs(lhs_.GetX() - rhs_.GetX()) <= epsilon_ && std::abs(lhs_.GetY() - rhs_.GetY()) <= epsilon_;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector2D<TValue> Vector2D<TValue>::Max(const Vector2D<TValue>& a_, const Vector2D<TValue>& b_) noexcept
		requires std::same_as<TValue, float>
	{
		return Vector2D<TValue>(std::max(a_.GetX(), b_.GetX()), std::max(a_.GetY(), b_.GetY()));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector2D<TValue> Vector2D<TValue>::Min(const Vector2D<TValue>& a_, const Vector2D<TValue>& b_) noexcept
		requires std::same_as<TValue, float>
	{
		return Vector2D<TValue>(std::min(a_.GetX(), b_.GetX()), std::min(a_.GetY(), b_.GetY()));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector2D<TValue> Vector2D<TValue>::Clamp(const Vector2D<TValue>& value_, const Vector2D<TValue>& min_, const Vector2D<TValue>& max_) noexcept
		requires std::same_as<TValue, float>
	{
		return Vector2D<TValue>(
				std::clamp(value_.GetX(), min_.GetX(), max_.GetX()),
				std::clamp(value_.GetY(), min_.GetY(), max_.GetY())
			);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector2D<TValue> Vector2D<TValue>::ClampMagnitude(const Vector2D<TValue>& vector_, float maxLength_) noexcept
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
	Vector2D<TValue> Vector2D<TValue>::Scale(const Vector2D<TValue>& vector_, const Vector2D<TValue>& scale_) noexcept
		requires std::same_as<TValue, float>
	{
		return Vector2D<TValue>(vector_.GetX() * scale_.GetX(), vector_.GetY() * scale_.GetY());
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector2D<TValue> Vector2D<TValue>::Normalize(const Vector2D<TValue>& value_) noexcept
		requires std::same_as<TValue, float>
	{
		Vector2D<TValue> result;
			Store(result, DirectX::XMVector2Normalize(Load(value_)));
			return result;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	float Vector2D<TValue>::Dot(const Vector2D<TValue>& a_, const Vector2D<TValue>& b_) noexcept
		requires std::same_as<TValue, float>
	{
		return DirectX::XMVectorGetX(DirectX::XMVector2Dot(Load(a_), Load(b_)));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector2D<TValue> Vector2D<TValue>::Cross(const Vector2D<TValue>& a_, const Vector2D<TValue>& b_) noexcept
		requires std::same_as<TValue, float>
	{
		const float z{ a_.GetX() * b_.GetY() - a_.GetY() * b_.GetX() };
			return Vector2D<TValue>(0.0f, z);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector2D<TValue> Vector2D<TValue>::Perpendicular(const Vector2D<TValue>& vector_) noexcept
		requires std::same_as<TValue, float>
	{
		return Vector2D<TValue>(-vector_.GetY(), vector_.GetX());
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	float Vector2D<TValue>::Distance(const Vector2D<TValue>& a_, const Vector2D<TValue>& b_) noexcept
		requires std::same_as<TValue, float>
	{
		return (a_ - b_).GetMagnitude();
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	float Vector2D<TValue>::Angle(const Vector2D<TValue>& a_, const Vector2D<TValue>& b_) noexcept
		requires std::same_as<TValue, float>
	{
		const Vector2D<TValue> from{ Normalize(a_) };
			const Vector2D<TValue> to{ Normalize(b_) };
			const float dot{ std::clamp(Dot(from, to), -1.0f, 1.0f) };
			return std::acos(dot);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	float Vector2D<TValue>::SignedAngle(const Vector2D<TValue>& from_, const Vector2D<TValue>& to_) noexcept
		requires std::same_as<TValue, float>
	{
		const float angle{ Angle(from_, to_) };
			const float det{ from_.GetX() * to_.GetY() - from_.GetY() * to_.GetX() };
			return (det >= 0.0f) ? angle : -angle;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector2D<TValue> Vector2D<TValue>::Reflect(const Vector2D<TValue>& vector_, const Vector2D<TValue>& normal_) noexcept
		requires std::same_as<TValue, float>
	{
		Vector2D<TValue> result;
			Store(result, DirectX::XMVector2Reflect(Load(vector_), Load(normal_)));
			return result;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector2D<TValue> Vector2D<TValue>::Lerp(const Vector2D<TValue>& a_, const Vector2D<TValue>& b_, float t_) noexcept
		requires std::same_as<TValue, float>
	{
		Vector2D<TValue> result;
			Store(result, DirectX::XMVectorLerp(Load(a_), Load(b_), std::clamp(t_, 0.0f, 1.0f)));
			return result;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector2D<TValue> Vector2D<TValue>::LerpUnclamped(const Vector2D<TValue>& a_, const Vector2D<TValue>& b_, float t_) noexcept
		requires std::same_as<TValue, float>
	{
		Vector2D<TValue> result;
			Store(result, DirectX::XMVectorLerp(Load(a_), Load(b_), t_));
			return result;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector2D<TValue> Vector2D<TValue>::Slerp(const Vector2D<TValue>& a_, const Vector2D<TValue>& b_, float t_) noexcept
		requires std::same_as<TValue, float>
	{
		return SlerpUnclamped(a_, b_, std::clamp(t_, 0.0f, 1.0f));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector2D<TValue> Vector2D<TValue>::SlerpUnclamped(const Vector2D<TValue>& a_, const Vector2D<TValue>& b_, float t_) noexcept
		requires std::same_as<TValue, float>
	{
		const float aMag{ a_.GetMagnitude() };
			const float bMag{ b_.GetMagnitude() };
		
			if (aMag <= std::numeric_limits<float>::epsilon() || bMag <= std::numeric_limits<float>::epsilon())
			{
				return LerpUnclamped(a_, b_, t_);
			}
		
			const Vector2D<TValue> from{ a_ / aMag };
			const Vector2D<TValue> to{ b_ / bMag };
		
			float dot{ std::clamp(Dot(from, to), -1.0f, 1.0f) };
			const float theta{ std::acos(dot) * t_ };
		
			Vector2D<TValue> relative{ to - from * dot };
			const float relativeMag{ relative.GetMagnitude() };
			if (relativeMag <= std::numeric_limits<float>::epsilon())
			{
				return LerpUnclamped(a_, b_, t_);
			}
		
			relative /= relativeMag;
			const Vector2D<TValue> direction{ from * std::cos(theta) + relative * std::sin(theta) };
			const float magnitude{ Mathf::Lerp(aMag, bMag, t_) };
		
			return direction * magnitude;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector2D<TValue> Vector2D<TValue>::MoveTowards(const Vector2D<TValue>& current_, const Vector2D<TValue>& target_, float maxDistanceDelta_) noexcept
		requires std::same_as<TValue, float>
	{
		const Vector2D<TValue> delta{ target_ - current_ };
			const float distance{ delta.GetMagnitude() };
		
			if (distance <= maxDistanceDelta_ || distance <= std::numeric_limits<float>::epsilon())
			{
				return target_;
			}
		
			return current_ + (delta / distance) * maxDistanceDelta_;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector2D<TValue> Vector2D<TValue>::SmoothDamp(
	const Vector2D<TValue>& current_,
	const Vector2D<TValue>& target_,
	Vector2D<TValue>& currentVelocity_,
	float smoothTime_,
	float maxSpeed_,
	float deltaTime_) noexcept
		requires std::same_as<TValue, float>
	{
		smoothTime_ = std::max(0.0001f, smoothTime_);
			const float omega{ 2.0f / smoothTime_ };
			const float x{ omega * deltaTime_ };
			const float exp{ 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x) };
		
			Vector2D<TValue> change{ current_ - target_ };
			const Vector2D<TValue> originalTarget{ target_ };
		
			const float maxChange{ maxSpeed_ * smoothTime_ };
			change = ClampMagnitude(change, maxChange);
			const Vector2D<TValue> target{ current_ - change };
		
			const Vector2D<TValue> temp{ (currentVelocity_ + change * omega) * deltaTime_ };
			currentVelocity_ = (currentVelocity_ - temp * omega) * exp;
			Vector2D<TValue> output{ target + (change + temp) * exp };
		
			if (Dot(originalTarget - current_, output - originalTarget) > 0.0f)
			{
				output = originalTarget;
				currentVelocity_ = Vector2D<TValue>(0.0f, 0.0f);
			}
		
			return output;
	}

	template class Vector2D<int>;
	template class Vector2D<float>;

	Vector2D<int> operator*(int scalar, const Vector2D<int>& vector) noexcept
	{
		return vector * scalar;
	}

	Vector2D<float> operator*(float scalar, const Vector2D<float>& vector) noexcept
	{
		return vector * scalar;
	}

}
