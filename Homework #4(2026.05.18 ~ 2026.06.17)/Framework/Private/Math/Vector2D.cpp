#include "Precompiled.hpp"
#include "Vector2D.hpp"

#include "Vector3D.hpp"
#include "Vector4D.hpp"
#include "Vector2D_Impl.hpp"

namespace TUK::Framework
{
	template <IsNumeric TValue>
	Vector2D<TValue>::Vector2D() noexcept
		: values{TValue{}, TValue{}}
	{
	}

	template <IsNumeric TValue>
	Vector2D<TValue>::Vector2D(TValue scalar) noexcept
		: values{scalar, scalar}
	{
	}

	template <IsNumeric TValue>
	Vector2D<TValue>::Vector2D(TValue x, TValue y) noexcept
		: values{x, y}
	{
	}

	template <IsNumeric TValue>
	Vector2D<TValue>::Vector2D(const Vector2D& other) noexcept
		: values{}
	{
		for (std::size_t index = 0; index < 2; ++index) values[index] = other.values[index];
	}

	template <IsNumeric TValue>
	Vector2D<TValue>::Vector2D(Vector2D&& other) noexcept
		: values{}
	{
		for (std::size_t index = 0; index < 2; ++index) values[index] = other.values[index];
	}

	template <IsNumeric TValue>
	Vector2D<TValue>::Vector2D(const Vector3D<TValue>& vector) noexcept
		: values{TValue{}, TValue{}}
	{
		Set(vector.GetX(), vector.GetY());
	}

	template <IsNumeric TValue>
	Vector2D<TValue>::Vector2D(const Vector4D<TValue>& vector) noexcept
		: values{TValue{}, TValue{}}
	{
		Set(vector.GetX(), vector.GetY());
	}

	template <IsNumeric TValue>
	Vector2D<TValue>& Vector2D<TValue>::operator=(const Vector2D& other) noexcept
	{
		for (std::size_t index = 0; index < 2; ++index) values[index] = other.values[index];
		return *this;
	}

	template <IsNumeric TValue>
	Vector2D<TValue>& Vector2D<TValue>::operator=(Vector2D&& other) noexcept
	{
		for (std::size_t index = 0; index < 2; ++index) values[index] = other.values[index];
		return *this;
	}

	template <IsNumeric TValue>
	Vector2D<TValue>::operator Vector3D<TValue>() const noexcept
	{
		return Vector3D<TValue>(GetX(), GetY(), TValue{});
	}

	template <IsNumeric TValue>
	Vector2D<TValue>::operator Vector4D<TValue>() const noexcept
	{
		return Vector4D<TValue>(GetX(), GetY(), TValue{}, TValue{});
	}

	template <IsNumeric TValue>
	TValue Vector2D<TValue>::operator[](std::size_t index) const noexcept
	{
		assert(index < 2);
		switch (index)
		{
		case 0:
			return values[0];
		default:
			return values[1];
		}
	}

	template <IsNumeric TValue>
	TValue& Vector2D<TValue>::operator[](std::size_t index) noexcept
	{
		assert(index < 2);
		switch (index)
		{
		case 0:
			return values[0];
		default:
			return values[1];
		}
	}

	template <IsNumeric TValue>
	Vector2D<TValue> Vector2D<TValue>::operator+() const noexcept
	{
		return *this;
	}

	template <IsNumeric TValue>
	Vector2D<TValue> Vector2D<TValue>::operator+(const Vector2D& other) const noexcept
	{
		if constexpr (std::same_as<TValue, float>)
		{
			Vector2D<TValue> result;
			Store(result, DirectX::XMVectorAdd(Load(*this), Load(other)));
			return result;
		}
		else
		{
			return Vector2D(GetX() + other.GetX(), GetY() + other.GetY());
		}
	}

	template <IsNumeric TValue>
	Vector2D<TValue>& Vector2D<TValue>::operator+=(const Vector2D& other) noexcept
	{
		*this = *this + other;
		return *this;
	}

	template <IsNumeric TValue>
	Vector2D<TValue> Vector2D<TValue>::operator-() const noexcept
	{
		if constexpr (std::same_as<TValue, float>)
		{
			Vector2D<TValue> result;
			Store(result, DirectX::XMVectorNegate(Load(*this)));
			return result;
		}
		else
		{
			return Vector2D(-GetX(), -GetY());
		}
	}

	template <IsNumeric TValue>
	Vector2D<TValue> Vector2D<TValue>::operator-(const Vector2D& other) const noexcept
	{
		if constexpr (std::same_as<TValue, float>)
		{
			Vector2D<TValue> result;
			Store(result, DirectX::XMVectorSubtract(Load(*this), Load(other)));
			return result;
		}
		else
		{
			return Vector2D(GetX() - other.GetX(), GetY() - other.GetY());
		}
	}

	template <IsNumeric TValue>
	Vector2D<TValue>& Vector2D<TValue>::operator-=(const Vector2D& other) noexcept
	{
		*this = *this - other;
		return *this;
	}

	template <IsNumeric TValue>
	Vector2D<TValue> Vector2D<TValue>::operator*(TValue scalar) const noexcept
	{
		if constexpr (std::same_as<TValue, float>)
		{
			Vector2D<TValue> result;
			Store(result, DirectX::XMVectorScale(Load(*this), scalar));
			return result;
		}
		else
		{
			return Vector2D(GetX() * scalar, GetY() * scalar);
		}
	}

	template <IsNumeric TValue>
	Vector2D<TValue>& Vector2D<TValue>::operator*=(TValue scalar) noexcept
	{
		*this = *this * scalar;
		return *this;
	}

	template <IsNumeric TValue>
	Vector2D<TValue> Vector2D<TValue>::operator/(TValue scalar) const noexcept
	{
		if constexpr (std::same_as<TValue, float>)
		{
			Vector2D<TValue> result;
			Store(result, DirectX::XMVectorScale(Load(*this), 1.0f / scalar));
			return result;
		}
		else
		{
			return Vector2D(GetX() / scalar, GetY() / scalar);
		}
	}

	template <IsNumeric TValue>
	Vector2D<TValue>& Vector2D<TValue>::operator/=(TValue scalar) noexcept
	{
		*this = *this / scalar;
		return *this;
	}

	template <IsNumeric TValue>
	bool Vector2D<TValue>::operator==(const Vector2D& other) const noexcept
	{
		return GetX() == other.GetX() && GetY() == other.GetY();
	}

	template <IsNumeric TValue>
	bool Vector2D<TValue>::operator!=(const Vector2D& other) const noexcept
	{
		return !(*this == other);
	}

	template <IsNumeric TValue>
	std::partial_ordering Vector2D<TValue>::operator<=>(const Vector2D& other) const noexcept
	{
		if (const auto order = GetX() <=> other.GetX(); order != 0)
		{
			return order;
		}
		return GetY() <=> other.GetY();
	}

	template <IsNumeric TValue>
	TValue Vector2D<TValue>::GetX() const noexcept
	{
		return values[0];
	}

	template <IsNumeric TValue>
	void Vector2D<TValue>::SetX(TValue x) noexcept
	{
		values[0] = x;
	}

	template <IsNumeric TValue>
	TValue Vector2D<TValue>::GetY() const noexcept
	{
		return values[1];
	}

	template <IsNumeric TValue>
	void Vector2D<TValue>::SetY(TValue y) noexcept
	{
		values[1] = y;
	}

	template <IsNumeric TValue>
	void Vector2D<TValue>::Set(TValue x, TValue y) noexcept
	{
		SetX(x);
		SetY(y);
	}

	template <IsNumeric TValue>
	float Vector2D<TValue>::GetMagnitude() const noexcept
		requires std::same_as<TValue, float>
	{
		return DirectX::XMVectorGetX(DirectX::XMVector2Length(Load(*this)));
	}

	template <IsNumeric TValue>
	float Vector2D<TValue>::GetSqrMagnitude() const noexcept
		requires std::same_as<TValue, float>
	{
		return DirectX::XMVectorGetX(DirectX::XMVector2LengthSq(Load(*this)));
	}

	template <IsNumeric TValue>
	float Vector2D<TValue>::GetLength() const noexcept
		requires std::same_as<TValue, float>
	{
		return GetMagnitude();
	}

	template <IsNumeric TValue>
	float Vector2D<TValue>::GetLengthSquared() const noexcept
		requires std::same_as<TValue, float>
	{
		return GetSqrMagnitude();
	}

	template <IsNumeric TValue>
	Vector2D<TValue> Vector2D<TValue>::GetNormalized() const noexcept
		requires std::same_as<TValue, float>
	{
		return Normalize(*this);
	}

	template <IsNumeric TValue>
	bool Vector2D<TValue>::IsZero(float epsilon) const noexcept
	{
		assert(epsilon >= 0.0f);
		return std::abs(static_cast<double>(GetX())) <= epsilon && std::abs(static_cast<double>(GetY())) <= epsilon;
	}

	template <IsNumeric TValue>
	bool Vector2D<TValue>::IsFinite() const noexcept
	{
		return std::isfinite(static_cast<double>(GetX())) && std::isfinite(static_cast<double>(GetY()));
	}

	template <IsNumeric TValue>
	void Vector2D<TValue>::Normalize() noexcept
		requires std::same_as<TValue, float>
	{
		*this = GetNormalized();
	}

	template <IsNumeric TValue>
	float Vector2D<TValue>::Dot(const Vector2D& other) const noexcept
		requires std::same_as<TValue, float>
	{
		return Dot(*this, other);
	}

	template <IsNumeric TValue>
	Vector2D<TValue> Vector2D<TValue>::GetZero() noexcept
	{
		return Vector2D<TValue>(static_cast<TValue>(0), static_cast<TValue>(0));
	}

	template <IsNumeric TValue>
	Vector2D<TValue> Vector2D<TValue>::GetOne() noexcept
	{
		return Vector2D<TValue>(static_cast<TValue>(1), static_cast<TValue>(1));
	}

	template <IsNumeric TValue>
	Vector2D<TValue> Vector2D<TValue>::GetUp() noexcept
	{
		return Vector2D<TValue>(static_cast<TValue>(0), static_cast<TValue>(1));
	}

	template <IsNumeric TValue>
	Vector2D<TValue> Vector2D<TValue>::GetDown() noexcept
	{
		return Vector2D<TValue>(static_cast<TValue>(0), static_cast<TValue>(-1));
	}

	template <IsNumeric TValue>
	Vector2D<TValue> Vector2D<TValue>::GetLeft() noexcept
	{
		return Vector2D<TValue>(static_cast<TValue>(-1), static_cast<TValue>(0));
	}

	template <IsNumeric TValue>
	Vector2D<TValue> Vector2D<TValue>::GetRight() noexcept
	{
		return Vector2D<TValue>(static_cast<TValue>(1), static_cast<TValue>(0));
	}

	template <IsNumeric TValue>
	Vector2D<TValue> Vector2D<TValue>::GetPositiveInfinity() noexcept
		requires std::same_as<TValue, float>
	{
		const float infinity = std::numeric_limits<float>::infinity();
			return Vector2D<TValue>(infinity, infinity);
	}

	template <IsNumeric TValue>
	Vector2D<TValue> Vector2D<TValue>::GetNegativeInfinity() noexcept
		requires std::same_as<TValue, float>
	{
		const float negativeInfinity = -std::numeric_limits<float>::infinity();
			return Vector2D<TValue>(negativeInfinity, negativeInfinity);
	}

	template <IsNumeric TValue>
	bool Vector2D<TValue>::IsApproximately(const Vector2D<TValue>& lhs, const Vector2D<TValue>& rhs, float epsilon) noexcept
		requires std::same_as<TValue, float>
	{
		return std::abs(lhs.GetX() - rhs.GetX()) <= epsilon && std::abs(lhs.GetY() - rhs.GetY()) <= epsilon;
	}

	template <IsNumeric TValue>
	Vector2D<TValue> Vector2D<TValue>::Max(const Vector2D<TValue>& a, const Vector2D<TValue>& b) noexcept
		requires std::same_as<TValue, float>
	{
		Vector2D<TValue> result;
		Store(result, DirectX::XMVectorMax(Load(a), Load(b)));
		return result;
	}

	template <IsNumeric TValue>
	Vector2D<TValue> Vector2D<TValue>::Min(const Vector2D<TValue>& a, const Vector2D<TValue>& b) noexcept
		requires std::same_as<TValue, float>
	{
		Vector2D<TValue> result;
		Store(result, DirectX::XMVectorMin(Load(a), Load(b)));
		return result;
	}

	template <IsNumeric TValue>
	Vector2D<TValue> Vector2D<TValue>::Clamp(const Vector2D<TValue>& value, const Vector2D<TValue>& min, const Vector2D<TValue>& max) noexcept
		requires std::same_as<TValue, float>
	{
		Vector2D<TValue> result;
		Store(result, DirectX::XMVectorClamp(Load(value), Load(min), Load(max)));
		return result;
	}

	template <IsNumeric TValue>
	Vector2D<TValue> Vector2D<TValue>::ClampMagnitude(const Vector2D<TValue>& vector, float maxLength) noexcept
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
	Vector2D<TValue> Vector2D<TValue>::Scale(const Vector2D<TValue>& vector, const Vector2D<TValue>& scale) noexcept
		requires std::same_as<TValue, float>
	{
		Vector2D<TValue> result;
		Store(result, DirectX::XMVectorMultiply(Load(vector), Load(scale)));
		return result;
	}

	template <IsNumeric TValue>
	Vector2D<TValue> Vector2D<TValue>::Normalize(const Vector2D<TValue>& value) noexcept
		requires std::same_as<TValue, float>
	{
		Vector2D<TValue> result;
		Store(result, DirectX::XMVector2Normalize(Load(value)));
		return result;
	}

	template <IsNumeric TValue>
	float Vector2D<TValue>::Dot(const Vector2D<TValue>& a, const Vector2D<TValue>& b) noexcept
		requires std::same_as<TValue, float>
	{
		return DirectX::XMVectorGetX(DirectX::XMVector2Dot(Load(a), Load(b)));
	}

	template <IsNumeric TValue>
	Vector2D<TValue> Vector2D<TValue>::Cross(const Vector2D<TValue>& a, const Vector2D<TValue>& b) noexcept
		requires std::same_as<TValue, float>
	{
		const float z = DirectX::XMVectorGetX(DirectX::XMVector2Cross(Load(a), Load(b)));
		return Vector2D<TValue>(0.0f, z);
	}

	template <IsNumeric TValue>
	Vector2D<TValue> Vector2D<TValue>::Perpendicular(const Vector2D<TValue>& vector) noexcept
		requires std::same_as<TValue, float>
	{
		return Vector2D<TValue>(-vector.GetY(), vector.GetX());
	}

	template <IsNumeric TValue>
	float Vector2D<TValue>::Distance(const Vector2D<TValue>& a, const Vector2D<TValue>& b) noexcept
		requires std::same_as<TValue, float>
	{
		return (a - b).GetMagnitude();
	}

	template <IsNumeric TValue>
	float Vector2D<TValue>::Angle(const Vector2D<TValue>& a, const Vector2D<TValue>& b) noexcept
		requires std::same_as<TValue, float>
	{
		const Vector2D<TValue> from = Normalize(a);
			const Vector2D<TValue> to = Normalize(b);
			const float dot = std::clamp(Dot(from, to), -1.0f, 1.0f);
			return std::acos(dot);
	}

	template <IsNumeric TValue>
	float Vector2D<TValue>::SignedAngle(const Vector2D<TValue>& from, const Vector2D<TValue>& to) noexcept
		requires std::same_as<TValue, float>
	{
		const float angle = Angle(from, to);
			const float det = from.GetX() * to.GetY() - from.GetY() * to.GetX();
			return (det >= 0.0f) ? angle : -angle;
	}

	template <IsNumeric TValue>
	Vector2D<TValue> Vector2D<TValue>::Reflect(const Vector2D<TValue>& vector, const Vector2D<TValue>& normal) noexcept
		requires std::same_as<TValue, float>
	{
		Vector2D<TValue> result;
		Store(result, DirectX::XMVector2Reflect(Load(vector), Load(normal)));
		return result;
	}

	template <IsNumeric TValue>
	Vector2D<TValue> Vector2D<TValue>::Lerp(const Vector2D<TValue>& a, const Vector2D<TValue>& b, float t) noexcept
		requires std::same_as<TValue, float>
	{
		return LerpUnclamped(a, b, std::clamp(t, 0.0f, 1.0f));
	}

	template <IsNumeric TValue>
	Vector2D<TValue> Vector2D<TValue>::LerpUnclamped(const Vector2D<TValue>& a, const Vector2D<TValue>& b, float t) noexcept
		requires std::same_as<TValue, float>
	{
		Vector2D<TValue> result;
		Store(result, DirectX::XMVectorLerp(Load(a), Load(b), t));
		return result;
	}

	template <IsNumeric TValue>
	Vector2D<TValue> Vector2D<TValue>::Slerp(const Vector2D<TValue>& a, const Vector2D<TValue>& b, float t) noexcept
		requires std::same_as<TValue, float>
	{
		return SlerpUnclamped(a, b, std::clamp(t, 0.0f, 1.0f));
	}

	template <IsNumeric TValue>
	Vector2D<TValue> Vector2D<TValue>::SlerpUnclamped(const Vector2D<TValue>& a, const Vector2D<TValue>& b, float t) noexcept
		requires std::same_as<TValue, float>
	{
		const float aMag = a.GetMagnitude();
			const float bMag = b.GetMagnitude();
		
			if (aMag <= std::numeric_limits<float>::epsilon() || bMag <= std::numeric_limits<float>::epsilon())
			{
				return LerpUnclamped(a, b, t);
			}
		
			const Vector2D<TValue> from = a / aMag;
			const Vector2D<TValue> to = b / bMag;
		
			float dot = std::clamp(Dot(from, to), -1.0f, 1.0f);
			const float theta = std::acos(dot) * t;
		
			Vector2D<TValue> relative = to - from * dot;
			const float relativeMag = relative.GetMagnitude();
			if (relativeMag <= std::numeric_limits<float>::epsilon())
			{
				return LerpUnclamped(a, b, t);
			}
		
			relative /= relativeMag;
			const Vector2D<TValue> direction = from * std::cos(theta) + relative * std::sin(theta);
			const float magnitude = Mathf::Lerp(aMag, bMag, t);
		
			return direction * magnitude;
	}

	template <IsNumeric TValue>
	Vector2D<TValue> Vector2D<TValue>::MoveTowards(const Vector2D<TValue>& current, const Vector2D<TValue>& target, float maxDistanceDelta) noexcept
		requires std::same_as<TValue, float>
	{
		const Vector2D<TValue> delta = target - current;
			const float distance = delta.GetMagnitude();
		
			if (distance <= maxDistanceDelta || distance <= std::numeric_limits<float>::epsilon())
			{
				return target;
			}
		
			return current + (delta / distance) * maxDistanceDelta;
	}

	template <IsNumeric TValue>
	Vector2D<TValue> Vector2D<TValue>::SmoothDamp(
	const Vector2D<TValue>& current,
	const Vector2D<TValue>& target,
	Vector2D<TValue>& currentVelocity,
	float smoothTime,
	float maxSpeed,
	float deltaTime) noexcept
		requires std::same_as<TValue, float>
	{
		smoothTime = std::max(0.0001f, smoothTime);
			const float omega = 2.0f / smoothTime;
			const float x = omega * deltaTime;
			const float exp = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);
		
			Vector2D<TValue> change = current - target;
			const Vector2D<TValue> originalTarget = target;
		
			const float maxChange = maxSpeed * smoothTime;
			change = ClampMagnitude(change, maxChange);
			const Vector2D<TValue> adjustedTarget = current - change;
		
			const Vector2D<TValue> temp = (currentVelocity + change * omega) * deltaTime;
			currentVelocity = (currentVelocity - temp * omega) * exp;
			Vector2D<TValue> output = adjustedTarget + (change + temp) * exp;
		
			if (Dot(originalTarget - current, output - originalTarget) > 0.0f)
			{
				output = originalTarget;
				currentVelocity = Vector2D<TValue>(0.0f, 0.0f);
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
