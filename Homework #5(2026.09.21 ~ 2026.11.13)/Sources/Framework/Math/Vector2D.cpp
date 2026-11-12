#include "Precompiled.hpp"
#include "Vector2D.hpp"

#include "../Core/Common.hpp"

#include "Mathf.hpp"
#include "Vector3D.hpp"
#include "Vector4D.hpp"

namespace TUK::Framework
{
	template <Scalarable TScalar>
	Vector2D<TScalar>::Vector2D() noexcept
		: value(TScalar{}, TScalar{})
	{
	}

	template <Scalarable TScalar>
	Vector2D<TScalar>::Vector2D(TScalar scalar) noexcept
		: value(scalar, scalar)
	{
	}

	template <Scalarable TScalar>
	Vector2D<TScalar>::Vector2D(TScalar x, TScalar y) noexcept
		: value(x, y)
	{
	}

	template <Scalarable TScalar>
	Vector2D<TScalar>::Vector2D(const Vector3D<TScalar>& vector) noexcept
		: value(TScalar{}, TScalar{})
	{
		Set(vector.GetX(), vector.GetY());
	}

	template <Scalarable TScalar>
	Vector2D<TScalar>::Vector2D(const Vector4D<TScalar>& vector) noexcept
		: value(TScalar{}, TScalar{})
	{
		Set(vector.GetX(), vector.GetY());
	}

	template <Scalarable TScalar>
	Vector2D<TScalar>::Vector2D(DirectX::XMVECTOR vector) noexcept
		requires std::same_as<TScalar, f32>
		: value(TScalar{}, TScalar{})
	{
		Store(*this, vector);
	}

	template <Scalarable TScalar>
	Vector2D<TScalar>::Vector2D(const DirectX::XMFLOAT2& source) noexcept
		requires std::same_as<TScalar, f32>
		: value(source)
	{
	}

	template <Scalarable TScalar>
	Vector2D<TScalar>::Vector2D(const DirectX::XMINT2& source) noexcept
		requires std::same_as<TScalar, i32>
		: value(source)
	{
	}

	template <Scalarable TScalar>
	Vector2D<TScalar>::operator Vector3D<TScalar>() const noexcept
	{
		return Vector3D<TScalar>(GetX(), GetY(), TScalar{});
	}

	template <Scalarable TScalar>
	Vector2D<TScalar>::operator Vector4D<TScalar>() const noexcept
	{
		return Vector4D<TScalar>(GetX(), GetY(), TScalar{}, TScalar{});
	}

	template <Scalarable TScalar>
	Vector2D<TScalar>::operator DirectX::XMVECTOR() const noexcept
		requires std::same_as<TScalar, f32>
	{
		return Load(*this);
	}

	template <Scalarable TScalar>
	Vector2D<TScalar>::operator DirectX::XMFLOAT2() const noexcept
		requires std::same_as<TScalar, f32>
	{
		return value;
	}

	template <Scalarable TScalar>
	Vector2D<TScalar>::operator DirectX::XMINT2() const noexcept
		requires std::same_as<TScalar, i32>
	{
		return value;
	}

	template <Scalarable TScalar>
	TScalar Vector2D<TScalar>::operator[](usize index) const noexcept
	{
		ASSERT_FMT(index < 2, "index가 유효 범위(0~1)를 벗어났습니다.");

		switch (index)
		{
			case 0:
				return value.x;
			default:
				return value.y;
		}
	}

	template <Scalarable TScalar>
	TScalar& Vector2D<TScalar>::operator[](usize index) noexcept
	{
		ASSERT_FMT(index < 2, "index가 유효 범위(0~1)를 벗어났습니다.");

		switch (index)
		{
			case 0:
				return value.x;
			default:
				return value.y;
		}
	}

	template <Scalarable TScalar>
	Vector2D<TScalar> Vector2D<TScalar>::operator+() const noexcept
	{
		return *this;
	}

	template <Scalarable TScalar>
	Vector2D<TScalar> Vector2D<TScalar>::operator+(const Vector2D& other) const noexcept
	{
		if constexpr (std::same_as<TScalar, f32>)
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

	template <Scalarable TScalar>
	Vector2D<TScalar>& Vector2D<TScalar>::operator+=(const Vector2D& other) noexcept
	{
		*this = *this + other;
		return *this;
	}

	template <Scalarable TScalar>
	Vector2D<TScalar> Vector2D<TScalar>::operator-() const noexcept
	{
		return Vector2D(-GetX(), -GetY());
	}

	template <Scalarable TScalar>
	Vector2D<TScalar> Vector2D<TScalar>::operator-(const Vector2D& other) const noexcept
	{
		if constexpr (std::same_as<TScalar, f32>)
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

	template <Scalarable TScalar>
	Vector2D<TScalar>& Vector2D<TScalar>::operator-=(const Vector2D& other) noexcept
	{
		*this = *this - other;
		return *this;
	}

	template <Scalarable TScalar>
	Vector2D<TScalar> Vector2D<TScalar>::operator*(TScalar scalar) const noexcept
	{
		if constexpr (std::same_as<TScalar, f32>)
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

	template <Scalarable TScalar>
	Vector2D<TScalar>& Vector2D<TScalar>::operator*=(TScalar scalar) noexcept
	{
		*this = *this * scalar;
		return *this;
	}

	template <Scalarable TScalar>
	Vector2D<TScalar> Vector2D<TScalar>::operator/(TScalar scalar) const noexcept
	{
		ASSERT_FMT(scalar != TScalar{}, "scalar가 0입니다.");

		if constexpr (std::same_as<TScalar, f32>)
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

	template <Scalarable TScalar>
	Vector2D<TScalar>& Vector2D<TScalar>::operator/=(TScalar scalar) noexcept
	{
		*this = *this / scalar;
		return *this;
	}

	template <Scalarable TScalar>
	bool Vector2D<TScalar>::operator==(const Vector2D& other) const noexcept
	{
		return GetX() == other.GetX() && GetY() == other.GetY();
	}

	template <Scalarable TScalar>
	bool Vector2D<TScalar>::operator!=(const Vector2D& other) const noexcept
	{
		return !(*this == other);
	}

	template <Scalarable TScalar>
	std::partial_ordering Vector2D<TScalar>::operator<=>(const Vector2D& other) const noexcept
	{
		if (const auto order = GetX() <=> other.GetX(); order != 0)
		{
			return order;
		}
		return GetY() <=> other.GetY();
	}

	template <Scalarable TScalar>
	TScalar Vector2D<TScalar>::GetX() const noexcept
	{
		return value.x;
	}

	template <Scalarable TScalar>
	void Vector2D<TScalar>::SetX(TScalar x) noexcept
	{
		value.x = x;
	}

	template <Scalarable TScalar>
	TScalar Vector2D<TScalar>::GetY() const noexcept
	{
		return value.y;
	}

	template <Scalarable TScalar>
	void Vector2D<TScalar>::SetY(TScalar y) noexcept
	{
		value.y = y;
	}

	template <Scalarable TScalar>
	void Vector2D<TScalar>::Set(TScalar x, TScalar y) noexcept
	{
		SetX(x);
		SetY(y);
	}

	template <Scalarable TScalar>
	f32 Vector2D<TScalar>::GetMagnitude() const noexcept
		requires std::same_as<TScalar, f32>
	{
		return DirectX::XMVectorGetX(DirectX::XMVector2Length(Load(*this)));
	}

	template <Scalarable TScalar>
	f32 Vector2D<TScalar>::GetSqrMagnitude() const noexcept
		requires std::same_as<TScalar, f32>
	{
		return Dot(*this, *this);
	}

	template <Scalarable TScalar>
	f32 Vector2D<TScalar>::GetLength() const noexcept
		requires std::same_as<TScalar, f32>
	{
		return GetMagnitude();
	}

	template <Scalarable TScalar>
	f32 Vector2D<TScalar>::GetLengthSquared() const noexcept
		requires std::same_as<TScalar, f32>
	{
		return GetSqrMagnitude();
	}

	template <Scalarable TScalar>
	Vector2D<TScalar> Vector2D<TScalar>::GetNormalized() const noexcept
		requires std::same_as<TScalar, f32>
	{
		return Normalize(*this);
	}

	template <Scalarable TScalar>
	bool Vector2D<TScalar>::IsZero(f32 epsilon) const noexcept
	{
		ASSERT_FMT(epsilon >= 0.0f, "epsilon이 0 이상이어야 합니다.");

		return std::abs(static_cast<f64>(GetX())) <= epsilon && std::abs(static_cast<f64>(GetY())) <= epsilon;
	}

	template <Scalarable TScalar>
	bool Vector2D<TScalar>::IsFinite() const noexcept
	{
		return std::isfinite(static_cast<f64>(GetX())) && std::isfinite(static_cast<f64>(GetY()));
	}

	template <Scalarable TScalar>
	void Vector2D<TScalar>::Normalize() noexcept
		requires std::same_as<TScalar, f32>
	{
		*this = GetNormalized();
	}

	template <Scalarable TScalar>
	f32 Vector2D<TScalar>::Dot(const Vector2D& other) const noexcept
		requires std::same_as<TScalar, f32>
	{
		return Dot(*this, other);
	}

	template <Scalarable TScalar>
	Vector2D<TScalar> Vector2D<TScalar>::GetZero() noexcept
	{
		return Vector2D<TScalar>(static_cast<TScalar>(0), static_cast<TScalar>(0));
	}

	template <Scalarable TScalar>
	Vector2D<TScalar> Vector2D<TScalar>::GetOne() noexcept
	{
		return Vector2D<TScalar>(static_cast<TScalar>(1), static_cast<TScalar>(1));
	}

	template <Scalarable TScalar>
	Vector2D<TScalar> Vector2D<TScalar>::GetUp() noexcept
	{
		return Vector2D<TScalar>(static_cast<TScalar>(0), static_cast<TScalar>(1));
	}

	template <Scalarable TScalar>
	Vector2D<TScalar> Vector2D<TScalar>::GetDown() noexcept
	{
		return Vector2D<TScalar>(static_cast<TScalar>(0), static_cast<TScalar>(-1));
	}

	template <Scalarable TScalar>
	Vector2D<TScalar> Vector2D<TScalar>::GetLeft() noexcept
	{
		return Vector2D<TScalar>(static_cast<TScalar>(-1), static_cast<TScalar>(0));
	}

	template <Scalarable TScalar>
	Vector2D<TScalar> Vector2D<TScalar>::GetRight() noexcept
	{
		return Vector2D<TScalar>(static_cast<TScalar>(1), static_cast<TScalar>(0));
	}

	template <Scalarable TScalar>
	Vector2D<TScalar> Vector2D<TScalar>::GetPositiveInfinity() noexcept
		requires std::same_as<TScalar, f32>
	{
		const f32 infinity = std::numeric_limits<f32>::infinity();
			return Vector2D<TScalar>(infinity, infinity);
	}

	template <Scalarable TScalar>
	Vector2D<TScalar> Vector2D<TScalar>::GetNegativeInfinity() noexcept
		requires std::same_as<TScalar, f32>
	{
		const f32 negativeInfinity = -std::numeric_limits<f32>::infinity();
			return Vector2D<TScalar>(negativeInfinity, negativeInfinity);
	}

	template <Scalarable TScalar>
	DirectX::XMVECTOR Vector2D<TScalar>::Load(const Vector2D& vector) noexcept
		requires std::same_as<TScalar, f32>
	{
		return DirectX::XMLoadFloat2(&vector.value);
	}

	template <Scalarable TScalar>
	void Vector2D<TScalar>::Store(Vector2D& destination, DirectX::XMVECTOR source) noexcept
		requires std::same_as<TScalar, f32>
	{
		DirectX::XMStoreFloat2(&destination.value, source);
	}

	template <Scalarable TScalar>
	bool Vector2D<TScalar>::IsApproximately(const Vector2D<TScalar>& lhs, const Vector2D<TScalar>& rhs, f32 epsilon) noexcept
		requires std::same_as<TScalar, f32>
	{
		return std::abs(lhs.GetX() - rhs.GetX()) <= epsilon && std::abs(lhs.GetY() - rhs.GetY()) <= epsilon;
	}

	template <Scalarable TScalar>
	Vector2D<TScalar> Vector2D<TScalar>::Max(const Vector2D<TScalar>& a, const Vector2D<TScalar>& b) noexcept
		requires std::same_as<TScalar, f32>
	{
		return Vector2D<TScalar>(std::max(a.GetX(), b.GetX()), std::max(a.GetY(), b.GetY()));
	}

	template <Scalarable TScalar>
	Vector2D<TScalar> Vector2D<TScalar>::Min(const Vector2D<TScalar>& a, const Vector2D<TScalar>& b) noexcept
		requires std::same_as<TScalar, f32>
	{
		return Vector2D<TScalar>(std::min(a.GetX(), b.GetX()), std::min(a.GetY(), b.GetY()));
	}

	template <Scalarable TScalar>
	Vector2D<TScalar> Vector2D<TScalar>::Clamp(const Vector2D<TScalar>& value, const Vector2D<TScalar>& min, const Vector2D<TScalar>& max) noexcept
		requires std::same_as<TScalar, f32>
	{
		return Vector2D<TScalar>(
				std::clamp(value.GetX(), min.GetX(), max.GetX()),
				std::clamp(value.GetY(), min.GetY(), max.GetY())
			);
	}

	template <Scalarable TScalar>
	Vector2D<TScalar> Vector2D<TScalar>::ClampMagnitude(const Vector2D<TScalar>& vector, f32 maxLength) noexcept
		requires std::same_as<TScalar, f32>
	{
		const f32 sqrMagnitude = vector.GetSqrMagnitude();
			const f32 maxSqr = maxLength * maxLength;
			if (sqrMagnitude <= maxSqr)
			{
				return vector;
			}

			return Normalize(vector) * maxLength;
	}

	template <Scalarable TScalar>
	Vector2D<TScalar> Vector2D<TScalar>::Scale(const Vector2D<TScalar>& vector, const Vector2D<TScalar>& scale) noexcept
		requires std::same_as<TScalar, f32>
	{
		return Vector2D<TScalar>(vector.GetX() * scale.GetX(), vector.GetY() * scale.GetY());
	}

	template <Scalarable TScalar>
	Vector2D<TScalar> Vector2D<TScalar>::Normalize(const Vector2D<TScalar>& value) noexcept
		requires std::same_as<TScalar, f32>
	{
		Vector2D<TScalar> result{};
			Store(result, DirectX::XMVector2Normalize(Load(value)));
			return result;
	}

	template <Scalarable TScalar>
	f32 Vector2D<TScalar>::Dot(const Vector2D<TScalar>& a, const Vector2D<TScalar>& b) noexcept
		requires std::same_as<TScalar, f32>
	{
		return DirectX::XMVectorGetX(DirectX::XMVector2Dot(Load(a), Load(b)));
	}

	template <Scalarable TScalar>
	Vector2D<TScalar> Vector2D<TScalar>::Cross(const Vector2D<TScalar>& a, const Vector2D<TScalar>& b) noexcept
		requires std::same_as<TScalar, f32>
	{
		const f32 z = a.GetX() * b.GetY() - a.GetY() * b.GetX();
			return Vector2D<TScalar>(0.0f, z);
	}

	template <Scalarable TScalar>
	Vector2D<TScalar> Vector2D<TScalar>::Perpendicular(const Vector2D<TScalar>& vector) noexcept
		requires std::same_as<TScalar, f32>
	{
		return Vector2D<TScalar>(-vector.GetY(), vector.GetX());
	}

	template <Scalarable TScalar>
	f32 Vector2D<TScalar>::Distance(const Vector2D<TScalar>& a, const Vector2D<TScalar>& b) noexcept
		requires std::same_as<TScalar, f32>
	{
		return (a - b).GetMagnitude();
	}

	template <Scalarable TScalar>
	f32 Vector2D<TScalar>::Angle(const Vector2D<TScalar>& a, const Vector2D<TScalar>& b) noexcept
		requires std::same_as<TScalar, f32>
	{
		const Vector2D<TScalar> from = Normalize(a);
			const Vector2D<TScalar> to = Normalize(b);
			const f32 dot = std::clamp(Dot(from, to), -1.0f, 1.0f);
			return std::acos(dot);
	}

	template <Scalarable TScalar>
	f32 Vector2D<TScalar>::SignedAngle(const Vector2D<TScalar>& from, const Vector2D<TScalar>& to) noexcept
		requires std::same_as<TScalar, f32>
	{
		const f32 angle = Angle(from, to);
			const f32 det = from.GetX() * to.GetY() - from.GetY() * to.GetX();
			return (det >= 0.0f) ? angle : -angle;
	}

	template <Scalarable TScalar>
	Vector2D<TScalar> Vector2D<TScalar>::Reflect(const Vector2D<TScalar>& vector, const Vector2D<TScalar>& normal) noexcept
		requires std::same_as<TScalar, f32>
	{
		Vector2D<TScalar> result{};
			Store(result, DirectX::XMVector2Reflect(Load(vector), Load(normal)));
			return result;
	}

	template <Scalarable TScalar>
	Vector2D<TScalar> Vector2D<TScalar>::Lerp(const Vector2D<TScalar>& a, const Vector2D<TScalar>& b, f32 t) noexcept
		requires std::same_as<TScalar, f32>
	{
		Vector2D<TScalar> result{};
			Store(result, DirectX::XMVectorLerp(Load(a), Load(b), std::clamp(t, 0.0f, 1.0f)));
			return result;
	}

	template <Scalarable TScalar>
	Vector2D<TScalar> Vector2D<TScalar>::LerpUnclamped(const Vector2D<TScalar>& a, const Vector2D<TScalar>& b, f32 t) noexcept
		requires std::same_as<TScalar, f32>
	{
		Vector2D<TScalar> result{};
			Store(result, DirectX::XMVectorLerp(Load(a), Load(b), t));
			return result;
	}

	template <Scalarable TScalar>
	Vector2D<TScalar> Vector2D<TScalar>::Slerp(const Vector2D<TScalar>& a, const Vector2D<TScalar>& b, f32 t) noexcept
		requires std::same_as<TScalar, f32>
	{
		return SlerpUnclamped(a, b, std::clamp(t, 0.0f, 1.0f));
	}

	template <Scalarable TScalar>
	Vector2D<TScalar> Vector2D<TScalar>::SlerpUnclamped(const Vector2D<TScalar>& a, const Vector2D<TScalar>& b, f32 t) noexcept
		requires std::same_as<TScalar, f32>
	{
		const f32 aMag = a.GetMagnitude();
			const f32 bMag = b.GetMagnitude();

			if (aMag <= std::numeric_limits<f32>::epsilon() || bMag <= std::numeric_limits<f32>::epsilon())
			{
				return LerpUnclamped(a, b, t);
			}

			const Vector2D<TScalar> from = a / aMag;
			const Vector2D<TScalar> to = b / bMag;

			f32 dot = std::clamp(Dot(from, to), -1.0f, 1.0f);
			const f32 theta = std::acos(dot) * t;

			Vector2D<TScalar> relative = to - from * dot;
			const f32 relativeMag = relative.GetMagnitude();
			if (relativeMag <= std::numeric_limits<f32>::epsilon())
			{
				return LerpUnclamped(a, b, t);
			}

			relative /= relativeMag;
			const Vector2D<TScalar> direction = from * std::cos(theta) + relative * std::sin(theta);
			const f32 magnitude = Mathf::Lerp(aMag, bMag, t);

			return direction * magnitude;
	}

	template <Scalarable TScalar>
	Vector2D<TScalar> Vector2D<TScalar>::MoveTowards(const Vector2D<TScalar>& current, const Vector2D<TScalar>& target, f32 maxDistanceDelta) noexcept
		requires std::same_as<TScalar, f32>
	{
		const Vector2D<TScalar> delta = target - current;
			const f32 distance = delta.GetMagnitude();

			if (distance <= maxDistanceDelta || distance <= std::numeric_limits<f32>::epsilon())
			{
				return target;
			}

			return current + (delta / distance) * maxDistanceDelta;
	}

	template <Scalarable TScalar>
	Vector2D<TScalar> Vector2D<TScalar>::SmoothDamp(
	const Vector2D<TScalar>& current,
	const Vector2D<TScalar>& target,
	Vector2D<TScalar>& currentVelocity,
	f32 smoothTime,
	f32 maxSpeed,
	f32 deltaTime) noexcept
		requires std::same_as<TScalar, f32>
	{
		smoothTime = std::max(0.0001f, smoothTime);
			const f32 omega = 2.0f / smoothTime;
			const f32 x = omega * deltaTime;
			const f32 exp = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);

			Vector2D<TScalar> change = current - target;
			const Vector2D<TScalar> originalTarget = target;

			const f32 maxChange = maxSpeed * smoothTime;
			change = ClampMagnitude(change, maxChange);
			const Vector2D<TScalar> adjustedTarget = current - change;

			const Vector2D<TScalar> temp = (currentVelocity + change * omega) * deltaTime;
			currentVelocity = (currentVelocity - temp * omega) * exp;
			Vector2D<TScalar> output = adjustedTarget + (change + temp) * exp;

			if (Dot(originalTarget - current, output - originalTarget) > 0.0f)
			{
				output = originalTarget;
				currentVelocity = Vector2D<TScalar>(0.0f, 0.0f);
			}

			return output;
	}

	template class Vector2D<i32>;
	template class Vector2D<f32>;

	Vector2D<i32> operator*(i32 scalar, const Vector2D<i32>& vector) noexcept
	{
		return vector * scalar;
	}

	Vector2D<f32> operator*(f32 scalar, const Vector2D<f32>& vector) noexcept
	{
		return vector * scalar;
	}
}
