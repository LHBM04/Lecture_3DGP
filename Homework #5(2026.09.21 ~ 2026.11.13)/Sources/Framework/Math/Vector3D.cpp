#include "Precompiled.hpp"
#include "Vector3D.hpp"

#include "../Core/Common.hpp"

#include "Mathf.hpp"
#include "Vector2D.hpp"
#include "Vector4D.hpp"

namespace TUK::Framework
{
	template <Scalarable TScalar>
	Vector3D<TScalar>::Vector3D() noexcept
		: value(TScalar{}, TScalar{}, TScalar{})
	{
	}

	template <Scalarable TScalar>
	Vector3D<TScalar>::Vector3D(TScalar scalar) noexcept
		: value(scalar, scalar, scalar)
	{
	}

	template <Scalarable TScalar>
	Vector3D<TScalar>::Vector3D(TScalar x, TScalar y, TScalar z) noexcept
		: value(x, y, z)
	{
	}

	template <Scalarable TScalar>
	Vector3D<TScalar>::Vector3D(const Vector2D<TScalar>& vector, TScalar z) noexcept
		: value(TScalar{}, TScalar{}, TScalar{})
	{
		Set(vector.GetX(), vector.GetY(), z);
	}

	template <Scalarable TScalar>
	Vector3D<TScalar>::Vector3D(const Vector4D<TScalar>& vector) noexcept
		: value(TScalar{}, TScalar{}, TScalar{})
	{
		Set(vector.GetX(), vector.GetY(), vector.GetZ());
	}

	template <Scalarable TScalar>
	Vector3D<TScalar>::Vector3D(DirectX::XMVECTOR vector) noexcept
		requires std::same_as<TScalar, f32>
		: value(TScalar{}, TScalar{}, TScalar{})
	{
		Store(*this, vector);
	}

	template <Scalarable TScalar>
	Vector3D<TScalar>::Vector3D(const DirectX::XMFLOAT3& source) noexcept
		requires std::same_as<TScalar, f32>
		: value(source)
	{
	}

	template <Scalarable TScalar>
	Vector3D<TScalar>::Vector3D(const DirectX::XMINT3& source) noexcept
		requires std::same_as<TScalar, i32>
		: value(source)
	{
	}

	template <Scalarable TScalar>
	Vector3D<TScalar>::operator Vector2D<TScalar>() const noexcept
	{
		return Vector2D<TScalar>(GetX(), GetY());
	}

	template <Scalarable TScalar>
	Vector3D<TScalar>::operator Vector4D<TScalar>() const noexcept
	{
		return Vector4D<TScalar>(GetX(), GetY(), GetZ(), TScalar{});
	}

	template <Scalarable TScalar>
	Vector3D<TScalar>::operator DirectX::XMVECTOR() const noexcept
		requires std::same_as<TScalar, f32>
	{
		return Load(*this);
	}

	template <Scalarable TScalar>
	Vector3D<TScalar>::operator DirectX::XMFLOAT3() const noexcept
		requires std::same_as<TScalar, f32>
	{
		return value;
	}

	template <Scalarable TScalar>
	Vector3D<TScalar>::operator DirectX::XMINT3() const noexcept
		requires std::same_as<TScalar, i32>
	{
		return value;
	}

	template <Scalarable TScalar>
	TScalar Vector3D<TScalar>::operator[](usize index) const noexcept
	{
		ASSERT_FMT(index < 3, "index가 유효 범위(0~2)를 벗어났습니다.");

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

	template <Scalarable TScalar>
	TScalar& Vector3D<TScalar>::operator[](usize index) noexcept
	{
		ASSERT_FMT(index < 3, "index가 유효 범위(0~2)를 벗어났습니다.");

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

	template <Scalarable TScalar>
	Vector3D<TScalar> Vector3D<TScalar>::operator+() const noexcept
	{
		return *this;
	}

	template <Scalarable TScalar>
	Vector3D<TScalar> Vector3D<TScalar>::operator+(const Vector3D& other) const noexcept
	{
		if constexpr (std::same_as<TScalar, f32>)
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

	template <Scalarable TScalar>
	Vector3D<TScalar>& Vector3D<TScalar>::operator+=(const Vector3D& other) noexcept
	{
		*this = *this + other;
		return *this;
	}

	template <Scalarable TScalar>
	Vector3D<TScalar> Vector3D<TScalar>::operator-() const noexcept
	{
		return Vector3D(-GetX(), -GetY(), -GetZ());
	}

	template <Scalarable TScalar>
	Vector3D<TScalar> Vector3D<TScalar>::operator-(const Vector3D& other) const noexcept
	{
		if constexpr (std::same_as<TScalar, f32>)
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

	template <Scalarable TScalar>
	Vector3D<TScalar>& Vector3D<TScalar>::operator-=(const Vector3D& other) noexcept
	{
		*this = *this - other;
		return *this;
	}

	template <Scalarable TScalar>
	Vector3D<TScalar> Vector3D<TScalar>::operator*(TScalar scalar) const noexcept
	{
		if constexpr (std::same_as<TScalar, f32>)
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

	template <Scalarable TScalar>
	Vector3D<TScalar>& Vector3D<TScalar>::operator*=(TScalar scalar) noexcept
	{
		*this = *this * scalar;
		return *this;
	}

	template <Scalarable TScalar>
	Vector3D<TScalar> Vector3D<TScalar>::operator/(TScalar scalar) const noexcept
	{
		ASSERT_FMT(scalar != TScalar{}, "scalar가 0입니다.");

		if constexpr (std::same_as<TScalar, f32>)
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

	template <Scalarable TScalar>
	Vector3D<TScalar>& Vector3D<TScalar>::operator/=(TScalar scalar) noexcept
	{
		*this = *this / scalar;
		return *this;
	}

	template <Scalarable TScalar>
	bool Vector3D<TScalar>::operator==(const Vector3D& other) const noexcept
	{
		return GetX() == other.GetX() && GetY() == other.GetY() && GetZ() == other.GetZ();
	}

	template <Scalarable TScalar>
	bool Vector3D<TScalar>::operator!=(const Vector3D& other) const noexcept
	{
		return !(*this == other);
	}

	template <Scalarable TScalar>
	std::partial_ordering Vector3D<TScalar>::operator<=>(const Vector3D& other) const noexcept
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

	template <Scalarable TScalar>
	TScalar Vector3D<TScalar>::GetX() const noexcept
	{
		return value.x;
	}

	template <Scalarable TScalar>
	void Vector3D<TScalar>::SetX(TScalar x) noexcept
	{
		value.x = x;
	}

	template <Scalarable TScalar>
	TScalar Vector3D<TScalar>::GetY() const noexcept
	{
		return value.y;
	}

	template <Scalarable TScalar>
	void Vector3D<TScalar>::SetY(TScalar y) noexcept
	{
		value.y = y;
	}

	template <Scalarable TScalar>
	TScalar Vector3D<TScalar>::GetZ() const noexcept
	{
		return value.z;
	}

	template <Scalarable TScalar>
	void Vector3D<TScalar>::SetZ(TScalar z) noexcept
	{
		value.z = z;
	}

	template <Scalarable TScalar>
	void Vector3D<TScalar>::Set(TScalar x, TScalar y, TScalar z) noexcept
	{
		SetX(x);
		SetY(y);
		SetZ(z);
	}

	template <Scalarable TScalar>
	f32 Vector3D<TScalar>::GetMagnitude() const noexcept
		requires std::same_as<TScalar, f32>
	{
		return DirectX::XMVectorGetX(DirectX::XMVector3Length(Load(*this)));
	}

	template <Scalarable TScalar>
	f32 Vector3D<TScalar>::GetSqrMagnitude() const noexcept
		requires std::same_as<TScalar, f32>
	{
		return Dot(*this, *this);
	}

	template <Scalarable TScalar>
	f32 Vector3D<TScalar>::GetLength() const noexcept
		requires std::same_as<TScalar, f32>
	{
		return GetMagnitude();
	}

	template <Scalarable TScalar>
	f32 Vector3D<TScalar>::GetLengthSquared() const noexcept
		requires std::same_as<TScalar, f32>
	{
		return GetSqrMagnitude();
	}

	template <Scalarable TScalar>
	Vector3D<TScalar> Vector3D<TScalar>::GetNormalized() const noexcept
		requires std::same_as<TScalar, f32>
	{
		return Normalize(*this);
	}

	template <Scalarable TScalar>
	bool Vector3D<TScalar>::IsZero(f32 epsilon) const noexcept
	{
		ASSERT_FMT(epsilon >= 0.0f, "epsilon이 0 이상이어야 합니다.");

		return std::abs(static_cast<f64>(GetX())) <= epsilon && std::abs(static_cast<f64>(GetY())) <= epsilon && std::abs(static_cast<f64>(GetZ())) <= epsilon;
	}

	template <Scalarable TScalar>
	bool Vector3D<TScalar>::IsFinite() const noexcept
	{
		return std::isfinite(static_cast<f64>(GetX())) && std::isfinite(static_cast<f64>(GetY())) && std::isfinite(static_cast<f64>(GetZ()));
	}

	template <Scalarable TScalar>
	void Vector3D<TScalar>::Normalize() noexcept
		requires std::same_as<TScalar, f32>
	{
		*this = GetNormalized();
	}

	template <Scalarable TScalar>
	f32 Vector3D<TScalar>::Dot(const Vector3D& other) const noexcept
		requires std::same_as<TScalar, f32>
	{
		return Dot(*this, other);
	}

	template <Scalarable TScalar>
	Vector3D<TScalar> Vector3D<TScalar>::GetZero() noexcept
	{
		return Vector3D<TScalar>(static_cast<TScalar>(0), static_cast<TScalar>(0), static_cast<TScalar>(0));
	}

	template <Scalarable TScalar>
	Vector3D<TScalar> Vector3D<TScalar>::GetOne() noexcept
	{
		return Vector3D<TScalar>(static_cast<TScalar>(1), static_cast<TScalar>(1), static_cast<TScalar>(1));
	}

	template <Scalarable TScalar>
	Vector3D<TScalar> Vector3D<TScalar>::GetUp() noexcept
	{
		return Vector3D<TScalar>(static_cast<TScalar>(0), static_cast<TScalar>(1), static_cast<TScalar>(0));
	}

	template <Scalarable TScalar>
	Vector3D<TScalar> Vector3D<TScalar>::GetDown() noexcept
	{
		return Vector3D<TScalar>(static_cast<TScalar>(0), static_cast<TScalar>(-1), static_cast<TScalar>(0));
	}

	template <Scalarable TScalar>
	Vector3D<TScalar> Vector3D<TScalar>::GetLeft() noexcept
	{
		return Vector3D<TScalar>(static_cast<TScalar>(-1), static_cast<TScalar>(0), static_cast<TScalar>(0));
	}

	template <Scalarable TScalar>
	Vector3D<TScalar> Vector3D<TScalar>::GetRight() noexcept
	{
		return Vector3D<TScalar>(static_cast<TScalar>(1), static_cast<TScalar>(0), static_cast<TScalar>(0));
	}

	template <Scalarable TScalar>
	Vector3D<TScalar> Vector3D<TScalar>::GetForward() noexcept
	{
		return Vector3D<TScalar>(static_cast<TScalar>(0), static_cast<TScalar>(0), static_cast<TScalar>(1));
	}

	template <Scalarable TScalar>
	Vector3D<TScalar> Vector3D<TScalar>::GetBack() noexcept
	{
		return Vector3D<TScalar>(static_cast<TScalar>(0), static_cast<TScalar>(0), static_cast<TScalar>(-1));
	}

	template <Scalarable TScalar>
	Vector3D<TScalar> Vector3D<TScalar>::GetPositiveInfinity() noexcept
		requires std::same_as<TScalar, f32>
	{
		const f32 infinity = std::numeric_limits<f32>::infinity();
		return Vector3D<TScalar>(infinity, infinity, infinity);
	}

	template <Scalarable TScalar>
	Vector3D<TScalar> Vector3D<TScalar>::GetNegativeInfinity() noexcept
		requires std::same_as<TScalar, f32>
	{
		const f32 negativeInfinity = -std::numeric_limits<f32>::infinity();
		return Vector3D<TScalar>(negativeInfinity, negativeInfinity, negativeInfinity);
	}

	template <Scalarable TScalar>
	DirectX::XMVECTOR Vector3D<TScalar>::Load(const Vector3D& vector) noexcept
		requires std::same_as<TScalar, f32>
	{
		return DirectX::XMLoadFloat3(&vector.value);
	}

	template <Scalarable TScalar>
	void Vector3D<TScalar>::Store(Vector3D& destination, DirectX::XMVECTOR source) noexcept
		requires std::same_as<TScalar, f32>
	{
		DirectX::XMStoreFloat3(&destination.value, source);
	}

	template <Scalarable TScalar>
	bool Vector3D<TScalar>::IsApproximately(const Vector3D<TScalar>& lhs, const Vector3D<TScalar>& rhs, f32 epsilon) noexcept
		requires std::same_as<TScalar, f32>
	{
		return std::abs(lhs.GetX() - rhs.GetX()) <= epsilon &&
			   std::abs(lhs.GetY() - rhs.GetY()) <= epsilon &&
			   std::abs(lhs.GetZ() - rhs.GetZ()) <= epsilon;
	}

	template <Scalarable TScalar>
	Vector3D<TScalar> Vector3D<TScalar>::Max(const Vector3D<TScalar>& a, const Vector3D<TScalar>& b) noexcept
		requires std::same_as<TScalar, f32>
	{
		return Vector3D<TScalar>(std::max(a.GetX(), b.GetX()), std::max(a.GetY(), b.GetY()), std::max(a.GetZ(), b.GetZ()));
	}

	template <Scalarable TScalar>
	Vector3D<TScalar> Vector3D<TScalar>::Min(const Vector3D<TScalar>& a, const Vector3D<TScalar>& b) noexcept
		requires std::same_as<TScalar, f32>
	{
		return Vector3D<TScalar>(std::min(a.GetX(), b.GetX()), std::min(a.GetY(), b.GetY()), std::min(a.GetZ(), b.GetZ()));
	}

	template <Scalarable TScalar>
	Vector3D<TScalar> Vector3D<TScalar>::Clamp(const Vector3D<TScalar>& value, const Vector3D<TScalar>& min, const Vector3D<TScalar>& max) noexcept
		requires std::same_as<TScalar, f32>
	{
		return Vector3D<TScalar>(
			std::clamp(value.GetX(), min.GetX(), max.GetX()),
			std::clamp(value.GetY(), min.GetY(), max.GetY()),
			std::clamp(value.GetZ(), min.GetZ(), max.GetZ())
		);
	}

	template <Scalarable TScalar>
	Vector3D<TScalar> Vector3D<TScalar>::ClampMagnitude(const Vector3D<TScalar>& vector, f32 maxLength) noexcept
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
	Vector3D<TScalar> Vector3D<TScalar>::Scale(const Vector3D<TScalar>& vector, const Vector3D<TScalar>& scale) noexcept
		requires std::same_as<TScalar, f32>
	{
		return Vector3D<TScalar>(vector.GetX() * scale.GetX(), vector.GetY() * scale.GetY(), vector.GetZ() * scale.GetZ());
	}

	template <Scalarable TScalar>
	Vector3D<TScalar> Vector3D<TScalar>::Normalize(const Vector3D<TScalar>& value) noexcept
		requires std::same_as<TScalar, f32>
	{
		Vector3D<TScalar> res{};
		Store(res, DirectX::XMVector3Normalize(Load(value)));
		return res;
	}

	template <Scalarable TScalar>
	void Vector3D<TScalar>::OrthoNormalize(Vector3D<TScalar>& normal, Vector3D<TScalar>& tangent) noexcept
		requires std::same_as<TScalar, f32>
	{
		normal = Normalize(normal);
		tangent = tangent - normal * Dot(normal, tangent);
		tangent = Normalize(tangent);
	}

	template <Scalarable TScalar>
	void Vector3D<TScalar>::OrthoNormalize(Vector3D<TScalar>& normal, Vector3D<TScalar>& tangent, Vector3D<TScalar>& binormal) noexcept
		requires std::same_as<TScalar, f32>
	{
		OrthoNormalize(normal, tangent);
		binormal = Normalize(Cross(normal, tangent));
	}

	template <Scalarable TScalar>
	f32 Vector3D<TScalar>::Dot(const Vector3D<TScalar>& a, const Vector3D<TScalar>& b) noexcept
		requires std::same_as<TScalar, f32>
	{
		return DirectX::XMVectorGetX(DirectX::XMVector3Dot(Load(a), Load(b)));
	}

	template <Scalarable TScalar>
	Vector3D<TScalar> Vector3D<TScalar>::Cross(const Vector3D<TScalar>& a, const Vector3D<TScalar>& b) noexcept
		requires std::same_as<TScalar, f32>
	{
		Vector3D<TScalar> res{};
		Store(res, DirectX::XMVector3Cross(Load(a), Load(b)));
		return res;
	}

	template <Scalarable TScalar>
	f32 Vector3D<TScalar>::Distance(const Vector3D<TScalar>& a, const Vector3D<TScalar>& b) noexcept
		requires std::same_as<TScalar, f32>
	{
		return (a - b).GetMagnitude();
	}

	template <Scalarable TScalar>
	f32 Vector3D<TScalar>::Angle(const Vector3D<TScalar>& a, const Vector3D<TScalar>& b) noexcept
		requires std::same_as<TScalar, f32>
	{
		const Vector3D<TScalar> from = Normalize(a);
		const Vector3D<TScalar> to = Normalize(b);
		const f32 dot = std::clamp(Dot(from, to), -1.0f, 1.0f);
		return std::acos(dot);
	}

	template <Scalarable TScalar>
	f32 Vector3D<TScalar>::SignedAngle(const Vector3D<TScalar>& from, const Vector3D<TScalar>& to, const Vector3D<TScalar>& axis) noexcept
		requires std::same_as<TScalar, f32>
	{
		const Vector3D<TScalar> cross = Cross(from, to);
		const f32 angle = Angle(from, to);
		const f32 sign = (Dot(axis, cross) >= 0.0f) ? 1.0f : -1.0f;
		return angle * sign;
	}

	template <Scalarable TScalar>
	Vector3D<TScalar> Vector3D<TScalar>::Project(const Vector3D<TScalar>& vector, const Vector3D<TScalar>& onNormal) noexcept
		requires std::same_as<TScalar, f32>
	{
		const f32 denominator = Dot(onNormal, onNormal);
		if (denominator <= std::numeric_limits<f32>::epsilon())
		{
			return GetZero();
		}

		const f32 scale = Dot(vector, onNormal) / denominator;
		return onNormal * scale;
	}

	template <Scalarable TScalar>
	Vector3D<TScalar> Vector3D<TScalar>::ProjectOnPlane(const Vector3D<TScalar>& vector, const Vector3D<TScalar>& planeNormal) noexcept
		requires std::same_as<TScalar, f32>
	{
		return vector - Project(vector, planeNormal);
	}

	template <Scalarable TScalar>
	Vector3D<TScalar> Vector3D<TScalar>::Reflect(const Vector3D<TScalar>& vector, const Vector3D<TScalar>& normal) noexcept
		requires std::same_as<TScalar, f32>
	{
		Vector3D<TScalar> res{};
		Store(res, DirectX::XMVector3Reflect(Load(vector), Load(normal)));
		return res;
	}

	template <Scalarable TScalar>
	Vector3D<TScalar> Vector3D<TScalar>::Refract(const Vector3D<TScalar>& vector, const Vector3D<TScalar>& normal, f32 eta) noexcept
		requires std::same_as<TScalar, f32>
	{
		Vector3D<TScalar> res{};
		Store(res, DirectX::XMVector3Refract(Load(vector), Load(normal), eta));
		return res;
	}

	template <Scalarable TScalar>
	Vector3D<TScalar> Vector3D<TScalar>::Lerp(const Vector3D<TScalar>& a, const Vector3D<TScalar>& b, f32 t) noexcept
		requires std::same_as<TScalar, f32>
	{
		Vector3D<TScalar> res{};
		Store(res, DirectX::XMVectorLerp(Load(a), Load(b), std::clamp(t, 0.0f, 1.0f)));
		return res;
	}

	template <Scalarable TScalar>
	Vector3D<TScalar> Vector3D<TScalar>::LerpUnclamped(const Vector3D<TScalar>& a, const Vector3D<TScalar>& b, f32 t) noexcept
		requires std::same_as<TScalar, f32>
	{
		Vector3D<TScalar> res{};
		Store(res, DirectX::XMVectorLerp(Load(a), Load(b), t));
		return res;
	}

	template <Scalarable TScalar>
	Vector3D<TScalar> Vector3D<TScalar>::Slerp(const Vector3D<TScalar>& a, const Vector3D<TScalar>& b, f32 t) noexcept
		requires std::same_as<TScalar, f32>
	{
		return SlerpUnclamped(a, b, std::clamp(t, 0.0f, 1.0f));
	}

	template <Scalarable TScalar>
	Vector3D<TScalar> Vector3D<TScalar>::SlerpUnclamped(const Vector3D<TScalar>& a, const Vector3D<TScalar>& b, f32 t) noexcept
		requires std::same_as<TScalar, f32>
	{
		const f32 aMag = a.GetMagnitude();
		const f32 bMag = b.GetMagnitude();

		if (aMag <= std::numeric_limits<f32>::epsilon() || bMag <= std::numeric_limits<f32>::epsilon())
		{
			return LerpUnclamped(a, b, t);
		}

		const Vector3D<TScalar> from = a / aMag;
		const Vector3D<TScalar> to = b / bMag;

		f32 dot = std::clamp(Dot(from, to), -1.0f, 1.0f);
		const f32 theta = std::acos(dot) * t;

		Vector3D<TScalar> relative = to - from * dot;
		const f32 relativeMag = relative.GetMagnitude();
		if (relativeMag <= std::numeric_limits<f32>::epsilon())
		{
			return LerpUnclamped(a, b, t);
		}

		relative /= relativeMag;
		const Vector3D<TScalar> direction = from * std::cos(theta) + relative * std::sin(theta);
		const f32 magnitude = Mathf::Lerp(aMag, bMag, t);

		return direction * magnitude;
	}

	template <Scalarable TScalar>
	Vector3D<TScalar> Vector3D<TScalar>::MoveTowards(const Vector3D<TScalar>& current, const Vector3D<TScalar>& target, f32 maxDistanceDelta) noexcept
		requires std::same_as<TScalar, f32>
	{
		const Vector3D<TScalar> delta = target - current;
		const f32 distance = delta.GetMagnitude();

		if (distance <= maxDistanceDelta || distance <= std::numeric_limits<f32>::epsilon())
		{
			return target;
		}

		return current + (delta / distance) * maxDistanceDelta;
	}

	template <Scalarable TScalar>
	Vector3D<TScalar> Vector3D<TScalar>::RotateTowards(const Vector3D<TScalar>& current, const Vector3D<TScalar>& target, f32 maxRadiansDelta, f32 maxMagnitudeDelta) noexcept
		requires std::same_as<TScalar, f32>
	{
		const f32 currentMag = current.GetMagnitude();
		const f32 targetMag = target.GetMagnitude();

		if (currentMag <= std::numeric_limits<f32>::epsilon() || targetMag <= std::numeric_limits<f32>::epsilon())
		{
			return MoveTowards(current, target, maxMagnitudeDelta);
		}

		const Vector3D<TScalar> currentDir = current / currentMag;
		const Vector3D<TScalar> targetDir = target / targetMag;

		const f32 angle = Angle(currentDir, targetDir);
		const f32 t = (angle <= std::numeric_limits<f32>::epsilon()) ? 1.0f : std::min(1.0f, maxRadiansDelta / angle);

		const Vector3D<TScalar> newDir = SlerpUnclamped(currentDir, targetDir, t).GetNormalized();

		f32 deltaMag = targetMag - currentMag;
		deltaMag = std::clamp(deltaMag, -maxMagnitudeDelta, maxMagnitudeDelta);
		const f32 newMag = currentMag + deltaMag;

		return newDir * newMag;
	}

	template <Scalarable TScalar>
	Vector3D<TScalar> Vector3D<TScalar>::SmoothDamp(
	const Vector3D<TScalar>& current,
	const Vector3D<TScalar>& target,
	Vector3D<TScalar>& currentVelocity,
	f32 smoothTime,
	f32 maxSpeed,
	f32 deltaTime) noexcept
		requires std::same_as<TScalar, f32>
	{
		smoothTime = std::max(0.0001f, smoothTime);
		const f32 omega = 2.0f / smoothTime;
		const f32 x = omega * deltaTime;
		const f32 exp = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);

		Vector3D<TScalar> change = current - target;
		const Vector3D<TScalar> originalTarget = target;

		const f32 maxChange = maxSpeed * smoothTime;
		change = ClampMagnitude(change, maxChange);
		const Vector3D<TScalar> adjustedTarget = current - change;

		const Vector3D<TScalar> temp = (currentVelocity + change * omega) * deltaTime;
		currentVelocity = (currentVelocity - temp * omega) * exp;
		Vector3D<TScalar> output = adjustedTarget + (change + temp) * exp;

		if (Dot(originalTarget - current, output - originalTarget) > 0.0f)
		{
			output = originalTarget;
			currentVelocity = Vector3D<TScalar>(0.0f, 0.0f, 0.0f);
		}

		return output;
	}

	template class Vector3D<i32>;
	template class Vector3D<f32>;

	Vector3D<i32> operator*(i32 scalar, const Vector3D<i32>& vector) noexcept
	{
		return vector * scalar;
	}

	Vector3D<f32> operator*(f32 scalar, const Vector3D<f32>& vector) noexcept
	{
		return vector * scalar;
	}
}
