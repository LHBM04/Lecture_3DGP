#include "Precompiled.hpp"
#include "Quaternion.hpp"

#include "../Core/Common.hpp"

#include "Mathf.hpp"
#include "Matrix4x4.hpp"

namespace TUK::Framework
{
	template <Scalarable TScalar>
	Quaternion<TScalar>::Quaternion() noexcept
		: value(static_cast<TScalar>(0), static_cast<TScalar>(0), static_cast<TScalar>(0), static_cast<TScalar>(1))
	{
	}

	template <Scalarable TScalar>
	Quaternion<TScalar>::Quaternion(TScalar value) noexcept
		: value(value, value, value, value)
	{
	}

	template <Scalarable TScalar>
	Quaternion<TScalar>::Quaternion(TScalar x, TScalar y, TScalar z, TScalar w) noexcept
		: value(x, y, z, w)
	{
	}

	template <Scalarable TScalar>
	Quaternion<TScalar>::Quaternion(DirectX::XMVECTOR quaternion) noexcept
		requires std::same_as<TScalar, f32>
		: value(TScalar{}, TScalar{}, TScalar{}, TScalar{})
	{
		Store(*this, quaternion);
	}

	template <Scalarable TScalar>
	Quaternion<TScalar>::Quaternion(const DirectX::XMFLOAT4& source) noexcept
		requires std::same_as<TScalar, f32>
		: value(source)
	{
	}

	template <Scalarable TScalar>
	Quaternion<TScalar>::Quaternion(const DirectX::XMINT4& source) noexcept
		requires std::same_as<TScalar, i32>
		: value(source)
	{
	}

	template <Scalarable TScalar>
	Quaternion<TScalar>::operator DirectX::XMVECTOR() const noexcept
		requires std::same_as<TScalar, f32>
	{
		return Load(*this);
	}

	template <Scalarable TScalar>
	Quaternion<TScalar>::operator DirectX::XMFLOAT4() const noexcept
		requires std::same_as<TScalar, f32>
	{
		return value;
	}

	template <Scalarable TScalar>
	Quaternion<TScalar>::operator DirectX::XMINT4() const noexcept
		requires std::same_as<TScalar, i32>
	{
		return value;
	}

	template <Scalarable TScalar>
	TScalar Quaternion<TScalar>::operator[](usize index) const noexcept
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
	TScalar& Quaternion<TScalar>::operator[](usize index) noexcept
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
	Quaternion<TScalar> Quaternion<TScalar>::operator+(const Quaternion<TScalar>& other) const noexcept
	{
		if constexpr (std::same_as<TScalar, i32>)
		{
			return Quaternion<TScalar>(value.x + other.value.x, value.y + other.value.y, value.z + other.value.z, value.w + other.value.w);
		}
		else
		{
			Quaternion<TScalar> result{};
			Store(result, DirectX::XMVectorAdd(Load(*this), Load(other)));
			return result;
		}
	}

	template <Scalarable TScalar>
	Quaternion<TScalar>& Quaternion<TScalar>::operator+=(const Quaternion<TScalar>& other) noexcept
	{
		*this = *this + other;
		return *this;
	}

	template <Scalarable TScalar>
	Quaternion<TScalar> Quaternion<TScalar>::operator-(const Quaternion<TScalar>& other) const noexcept
	{
		if constexpr (std::same_as<TScalar, i32>)
		{
			return Quaternion<TScalar>(value.x - other.value.x, value.y - other.value.y, value.z - other.value.z, value.w - other.value.w);
		}
		else
		{
			Quaternion<TScalar> result{};
			Store(result, DirectX::XMVectorSubtract(Load(*this), Load(other)));
			return result;
		}
	}

	template <Scalarable TScalar>
	Quaternion<TScalar>& Quaternion<TScalar>::operator-=(const Quaternion<TScalar>& other) noexcept
	{
		*this = *this - other;
		return *this;
	}

	template <Scalarable TScalar>
	Quaternion<TScalar> Quaternion<TScalar>::operator*(TScalar scalar) const noexcept
	{
		if constexpr (std::same_as<TScalar, i32>)
		{
			return Quaternion<TScalar>(value.x * scalar, value.y * scalar, value.z * scalar, value.w * scalar);
		}
		else
		{
			Quaternion<TScalar> result{};
			Store(result, DirectX::XMVectorScale(Load(*this), scalar));
			return result;
		}
	}

	template <Scalarable TScalar>
	Quaternion<TScalar> Quaternion<TScalar>::operator*(const Quaternion<TScalar>& other) const noexcept
	{
		if constexpr (std::same_as<TScalar, i32>)
		{
			return Quaternion<TScalar>(
				other.value.w * value.x + other.value.x * value.w + other.value.y * value.z - other.value.z * value.y,
				other.value.w * value.y - other.value.x * value.z + other.value.y * value.w + other.value.z * value.x,
				other.value.w * value.z + other.value.x * value.y - other.value.y * value.x + other.value.z * value.w,
				other.value.w * value.w - other.value.x * value.x - other.value.y * value.y - other.value.z * value.z);
		}
		else
		{
			Quaternion<TScalar> result{};
			Store(result, DirectX::XMQuaternionMultiply(Load(*this), Load(other)));
			return result;
		}
	}

	template <Scalarable TScalar>
	Vector3D<f32> Quaternion<TScalar>::operator*(const Vector3D<f32>& vector) const noexcept
		requires std::same_as<TScalar, f32>
	{
		Vector3D<f32> result{};
		Vector3D<f32>::Store(result, DirectX::XMVector3Rotate(Vector3D<f32>::Load(vector), Load(*this)));
		return result;
	}

	template <Scalarable TScalar>
	Quaternion<TScalar>& Quaternion<TScalar>::operator*=(TScalar scalar) noexcept
	{
		*this = *this * scalar;
		return *this;
	}

	template <Scalarable TScalar>
	Quaternion<TScalar>& Quaternion<TScalar>::operator*=(const Quaternion<TScalar>& other) noexcept
	{
		*this = *this * other;
		return *this;
	}

	template <Scalarable TScalar>
	Quaternion<TScalar> Quaternion<TScalar>::operator/(TScalar scalar) const noexcept
	{
		if constexpr (std::same_as<TScalar, i32>)
		{
			ASSERT_FMT(scalar != TScalar{}, "scalar가 0입니다.");

			return Quaternion<TScalar>(value.x / scalar, value.y / scalar, value.z / scalar, value.w / scalar);
		}
		else
		{
			ASSERT_FMT(scalar != 0.0f, "scalar가 0입니다.");

			Quaternion<TScalar> result{};
			Store(result, DirectX::XMVectorDivide(Load(*this), DirectX::XMVectorReplicate(scalar)));
			return result;
		}
	}

	template <Scalarable TScalar>
	Quaternion<TScalar>& Quaternion<TScalar>::operator/=(TScalar scalar) noexcept
	{
		*this = *this / scalar;
		return *this;
	}

	template <Scalarable TScalar>
	bool Quaternion<TScalar>::operator==(const Quaternion<TScalar>& other) const noexcept
	{
		return (*this <=> other) == std::partial_ordering::equivalent;
	}

	template <Scalarable TScalar>
	bool Quaternion<TScalar>::operator!=(const Quaternion<TScalar>& other) const noexcept
	{
		return !(*this == other);
	}

	template <Scalarable TScalar>
	std::partial_ordering Quaternion<TScalar>::operator<=>(const Quaternion<TScalar>& other) const noexcept
	{
		if (const auto order = value.x <=> other.GetX(); order != 0)
		{
			return order;
		}
		if (const auto order = value.y <=> other.GetY(); order != 0)
		{
			return order;
		}
		if (const auto order = value.z <=> other.GetZ(); order != 0)
		{
			return order;
		}

		return value.w <=> other.GetW();
	}

	template <Scalarable TScalar>
	TScalar Quaternion<TScalar>::GetX() const noexcept
	{
		return value.x;
	}

	template <Scalarable TScalar>
	void Quaternion<TScalar>::SetX(TScalar component) noexcept
	{
		value.x = component;
	}

	template <Scalarable TScalar>
	TScalar Quaternion<TScalar>::GetY() const noexcept
	{
		return value.y;
	}

	template <Scalarable TScalar>
	void Quaternion<TScalar>::SetY(TScalar component) noexcept
	{
		value.y = component;
	}

	template <Scalarable TScalar>
	TScalar Quaternion<TScalar>::GetZ() const noexcept
	{
		return value.z;
	}

	template <Scalarable TScalar>
	void Quaternion<TScalar>::SetZ(TScalar component) noexcept
	{
		value.z = component;
	}

	template <Scalarable TScalar>
	TScalar Quaternion<TScalar>::GetW() const noexcept
	{
		return value.w;
	}

	template <Scalarable TScalar>
	void Quaternion<TScalar>::SetW(TScalar component) noexcept
	{
		value.w = component;
	}

	template <Scalarable TScalar>
	void Quaternion<TScalar>::Set(TScalar x, TScalar y, TScalar z, TScalar w) noexcept
	{
		value.x = x;
		value.y = y;
		value.z = z;
		value.w = w;
	}

	template <Scalarable TScalar>
	Vector3D<f32> Quaternion<TScalar>::GetEulerAngles() const noexcept
		requires std::same_as<TScalar, f32>
	{
		const auto rotation = Normalize(*this);
		DirectX::XMFLOAT4X4 matrix;
		DirectX::XMStoreFloat4x4(&matrix, DirectX::XMMatrixRotationQuaternion(Load(rotation)));
		const f32 pitch = std::asin(std::clamp(-matrix._32, -1.0f, 1.0f));
		f32 yaw;
		f32 roll;
		if (std::abs(std::cos(pitch)) > 0.00001f)
		{
			yaw = std::atan2(matrix._31, matrix._33);
			roll = std::atan2(matrix._12, matrix._22);
		}
		else
		{
			yaw = std::atan2(-matrix._13, matrix._11);
			roll = 0.0f;
		}
		return Vector3D<f32>(pitch, yaw, roll) * (180.0f / std::numbers::pi_v<f32>);
	}

	template <Scalarable TScalar>
	void Quaternion<TScalar>::SetEulerAngles(const Vector3D<f32>& eulerDegrees) noexcept
		requires std::same_as<TScalar, f32>
	{
		*this = Euler(eulerDegrees);
	}

	template <Scalarable TScalar>
	f32 Quaternion<TScalar>::GetMagnitude() const noexcept
		requires std::same_as<TScalar, f32>
	{
		return DirectX::XMVectorGetX(DirectX::XMVector4Length(Load(*this)));
	}

	template <Scalarable TScalar>
	f32 Quaternion<TScalar>::GetSqrMagnitude() const noexcept
		requires std::same_as<TScalar, f32>
	{
		return Dot(*this, *this);
	}

	template <Scalarable TScalar>
	Quaternion<TScalar> Quaternion<TScalar>::GetNormalized() const noexcept
		requires std::same_as<TScalar, f32>
	{
		return Normalize(*this);
	}

	template <Scalarable TScalar>
	void Quaternion<TScalar>::SetFromToRotation(const Vector3D<f32>& from, const Vector3D<f32>& to) noexcept
		requires std::same_as<TScalar, f32>
	{
		Vector3D<f32> normalizedFrom = Vector3D<f32>::Normalize(from);
		Vector3D<f32> normalizedTo = Vector3D<f32>::Normalize(to);

		const f32 dot = std::clamp(Vector3D<f32>::Dot(normalizedFrom, normalizedTo), -1.0f, 1.0f);

		if (dot > 1.0f - std::numeric_limits<f32>::epsilon())
		{
			*this = GetIdentity();
			return;
		}

		if (dot < -1.0f + std::numeric_limits<f32>::epsilon())
		{
			Vector3D<f32> axis = Vector3D<f32>::Cross(Vector3D<f32>::GetRight(), normalizedFrom);
			if (axis.GetSqrMagnitude() <= std::numeric_limits<f32>::epsilon())
			{
				axis = Vector3D<f32>::Cross(Vector3D<f32>::GetUp(), normalizedFrom);
			}

			*this = AngleAxis(180.0f, axis);
			return;
		}

		Vector3D<f32> axis = Vector3D<f32>::Cross(normalizedFrom, normalizedTo);
		const f32 angleDegrees = std::acos(dot) * (180.0f / std::numbers::pi_v<f32>);
		*this = AngleAxis(angleDegrees, axis);
	}

	template <Scalarable TScalar>
	void Quaternion<TScalar>::SetLookRotation(const Vector3D<f32>& view) noexcept
		requires std::same_as<TScalar, f32>
	{
		*this = LookRotation(view, Vector3D<f32>::GetUp());
	}

	template <Scalarable TScalar>
	void Quaternion<TScalar>::SetLookRotation(const Vector3D<f32>& view, const Vector3D<f32>& up) noexcept
		requires std::same_as<TScalar, f32>
	{
		*this = LookRotation(view, up);
	}

	template <Scalarable TScalar>
	bool Quaternion<TScalar>::IsFinite() const noexcept
	{
		return std::isfinite(value.x) && std::isfinite(value.y) && std::isfinite(value.z) && std::isfinite(value.w);
	}

	template <Scalarable TScalar>
	bool Quaternion<TScalar>::IsNormalized(f32 epsilon) const noexcept
		requires std::same_as<TScalar, f32>
	{
		const f32 lenSqr = GetSqrMagnitude();
		return std::abs(lenSqr - 1.0f) <= epsilon;
	}

	template <Scalarable TScalar>
	void Quaternion<TScalar>::Normalize() noexcept
		requires std::same_as<TScalar, f32>
	{
		*this = GetNormalized();
	}

	template <Scalarable TScalar>
	void Quaternion<TScalar>::ToAngleAxis(f32& angleDegrees, Vector3D<f32>& axis) const noexcept
		requires std::same_as<TScalar, f32>
	{
		DirectX::XMVECTOR axisVector;
		f32 angleRadians;
		DirectX::XMQuaternionToAxisAngle(&axisVector, &angleRadians, Load(*this));

		Vector3D<f32>::Store(axis, axisVector);
		angleDegrees = angleRadians * (180.0f / std::numbers::pi_v<f32>);
	}

	template <Scalarable TScalar>
	Quaternion<TScalar> Quaternion<TScalar>::Normalize(const Quaternion<TScalar>& rotation) noexcept
		requires std::same_as<TScalar, f32>
	{
		Quaternion<TScalar> result{};
		Store(result, DirectX::XMQuaternionNormalize(Load(rotation)));
		return result;
	}

	template <Scalarable TScalar>
	Quaternion<TScalar> Quaternion<TScalar>::GetIdentity() noexcept
	{
		return Quaternion<TScalar>(TScalar{}, TScalar{}, TScalar{}, static_cast<TScalar>(1));
	}

	template <Scalarable TScalar>
	DirectX::XMVECTOR Quaternion<TScalar>::Load(const Quaternion<TScalar>& quat) noexcept
		requires std::same_as<TScalar, f32>
	{
		return DirectX::XMLoadFloat4(&quat.value);
	}

	template <Scalarable TScalar>
	void Quaternion<TScalar>::Store(Quaternion<TScalar>& d, DirectX::XMVECTOR s) noexcept
		requires std::same_as<TScalar, f32>
	{
		DirectX::XMStoreFloat4(&d.value, s);
	}

	template <Scalarable TScalar>
	bool Quaternion<TScalar>::IsApproximately(const Quaternion<TScalar>& lhs, const Quaternion<TScalar>& rhs, f32 epsilon) noexcept
		requires std::same_as<TScalar, f32>
	{
		return std::abs(Dot(lhs, rhs)) >= (1.0f - epsilon);
	}

	template <Scalarable TScalar>
	f32 Quaternion<TScalar>::Angle(const Quaternion<TScalar>& a, const Quaternion<TScalar>& b) noexcept
		requires std::same_as<TScalar, f32>
	{
		const f32 d = std::clamp(std::abs(Dot(a, b)), 0.0f, 1.0f);
		return (2.0f * std::acos(d)) * (180.0f / std::numbers::pi_v<f32>);
	}

	template <Scalarable TScalar>
	Quaternion<TScalar> Quaternion<TScalar>::AngleAxis(f32 angleDegrees, Vector3D<f32> axis) noexcept
		requires std::same_as<TScalar, f32>
	{
		Vector3D<f32> normalizedAxis = Vector3D<f32>::Normalize(axis);

		Quaternion<TScalar> result{};
		Store(result, DirectX::XMQuaternionRotationAxis(Vector3D<f32>::Load(normalizedAxis), angleDegrees * (std::numbers::pi_v<f32> / 180.0f)));
		return result;
	}

	template <Scalarable TScalar>
	Quaternion<TScalar> Quaternion<TScalar>::Euler(f32 xDegrees, f32 yDegrees, f32 zDegrees) noexcept
		requires std::same_as<TScalar, f32>
	{
		Quaternion<TScalar> result{};
		Store(result, DirectX::XMQuaternionRotationRollPitchYaw(xDegrees * (std::numbers::pi_v<f32> / 180.0f), yDegrees * (std::numbers::pi_v<f32> / 180.0f), zDegrees * (std::numbers::pi_v<f32> / 180.0f)));
		return result;
	}

	template <Scalarable TScalar>
	Quaternion<TScalar> Quaternion<TScalar>::Euler(const Vector3D<f32>& eulerDegrees) noexcept
		requires std::same_as<TScalar, f32>
	{
		return Euler(eulerDegrees.GetX(), eulerDegrees.GetY(), eulerDegrees.GetZ());
	}

	template <Scalarable TScalar>
	Quaternion<TScalar> Quaternion<TScalar>::Inverse(const Quaternion<TScalar>& rotation) noexcept
		requires std::same_as<TScalar, f32>
	{
		Quaternion<TScalar> result{};
		Store(result, DirectX::XMQuaternionInverse(Load(rotation)));
		return result;
	}

	template <Scalarable TScalar>
	Quaternion<TScalar> Quaternion<TScalar>::Conjugate(const Quaternion<TScalar>& rotation) noexcept
		requires std::same_as<TScalar, f32>
	{
		Quaternion<TScalar> result{};
		Store(result, DirectX::XMQuaternionConjugate(Load(rotation)));
		return result;
	}

	template <Scalarable TScalar>
	f32 Quaternion<TScalar>::Dot(const Quaternion<TScalar>& a, const Quaternion<TScalar>& b) noexcept
		requires std::same_as<TScalar, f32>
	{
		return DirectX::XMVectorGetX(DirectX::XMVector4Dot(Load(a), Load(b)));
	}

	template <Scalarable TScalar>
	Quaternion<TScalar> Quaternion<TScalar>::Lerp(const Quaternion<TScalar>& a, const Quaternion<TScalar>& b, f32 t) noexcept
		requires std::same_as<TScalar, f32>
	{
		return LerpUnclamped(a, b, std::clamp(t, 0.0f, 1.0f));
	}

	template <Scalarable TScalar>
	Quaternion<TScalar> Quaternion<TScalar>::LerpUnclamped(const Quaternion<TScalar>& a, const Quaternion<TScalar>& b, f32 t) noexcept
		requires std::same_as<TScalar, f32>
	{
		Quaternion<TScalar> result{};
		Store(result, DirectX::XMQuaternionNormalize(DirectX::XMVectorLerp(Load(a), Load(b), t)));
		return result;
	}

	template <Scalarable TScalar>
	Quaternion<TScalar> Quaternion<TScalar>::Slerp(const Quaternion<TScalar>& a, const Quaternion<TScalar>& b, f32 t) noexcept
		requires std::same_as<TScalar, f32>
	{
		return SlerpUnclamped(a, b, std::clamp(t, 0.0f, 1.0f));
	}

	template <Scalarable TScalar>
	Quaternion<TScalar> Quaternion<TScalar>::SlerpUnclamped(const Quaternion<TScalar>& a, const Quaternion<TScalar>& b, f32 t) noexcept
		requires std::same_as<TScalar, f32>
	{
		Quaternion<TScalar> result{};
		Store(result, DirectX::XMQuaternionSlerp(Load(a), Load(b), t));
		return result;
	}

	template <Scalarable TScalar>
	Quaternion<TScalar> Quaternion<TScalar>::FromToRotation(const Vector3D<f32>& fromDirection, const Vector3D<f32>& toDirection) noexcept
		requires std::same_as<TScalar, f32>
	{
		Vector3D<f32> from = Vector3D<f32>::Normalize(fromDirection);
		Vector3D<f32> to = Vector3D<f32>::Normalize(toDirection);

		const f32 dot = std::clamp(Vector3D<f32>::Dot(from, to), -1.0f, 1.0f);

		if (dot > 1.0f - std::numeric_limits<f32>::epsilon())
		{
			return GetIdentity();
		}

		if (dot < -1.0f + std::numeric_limits<f32>::epsilon())
		{
			Vector3D<f32> axis = Vector3D<f32>::Cross(Vector3D<f32>::GetRight(), from);
			if (axis.GetSqrMagnitude() <= std::numeric_limits<f32>::epsilon())
			{
				axis = Vector3D<f32>::Cross(Vector3D<f32>::GetUp(), from);
			}

			return AngleAxis(180.0f, axis);
		}

		Vector3D<f32> axis = Vector3D<f32>::Cross(from, to);
		const f32 angleDegrees = std::acos(dot) * (180.0f / std::numbers::pi_v<f32>);
		return AngleAxis(angleDegrees, axis);
	}

	template <Scalarable TScalar>
	Quaternion<TScalar> Quaternion<TScalar>::LookRotation(const Vector3D<f32>& forward, const Vector3D<f32>& up) noexcept
		requires std::same_as<TScalar, f32>
	{
		Vector3D<f32> normalizedForward = Vector3D<f32>::Normalize(forward);
		if (normalizedForward.GetSqrMagnitude() <= std::numeric_limits<f32>::epsilon())
		{
			return GetIdentity();
		}

		Vector3D<f32> right = Vector3D<f32>::Normalize(Vector3D<f32>::Cross(up, normalizedForward));
		if (right.GetSqrMagnitude() <= std::numeric_limits<f32>::epsilon())
		{
			right = Vector3D<f32>::Normalize(Vector3D<f32>::Cross(Vector3D<f32>::GetUp(), normalizedForward));
			if (right.GetSqrMagnitude() <= std::numeric_limits<f32>::epsilon())
			{
				right = Vector3D<f32>::Normalize(Vector3D<f32>::Cross(Vector3D<f32>::GetRight(), normalizedForward));
			}
		}

		Vector3D<f32> orthogonalUp = Vector3D<f32>::Cross(normalizedForward, right);

		DirectX::XMMATRIX basis = DirectX::XMMatrixIdentity();
		basis.r[0] = Vector3D<f32>::Load(right);
		basis.r[1] = Vector3D<f32>::Load(orthogonalUp);
		basis.r[2] = Vector3D<f32>::Load(normalizedForward);

		Quaternion<TScalar> result{};
		Store(result, DirectX::XMQuaternionRotationMatrix(basis));
		return result;
	}

	template <Scalarable TScalar>
	Quaternion<TScalar> Quaternion<TScalar>::RotateTowards(const Quaternion<TScalar>& from, const Quaternion<TScalar>& to, f32 maxDegreesDelta) noexcept
		requires std::same_as<TScalar, f32>
	{
		const f32 angleDegrees = Angle(from, to);
		if (angleDegrees <= std::numeric_limits<f32>::epsilon())
		{
			return to;
		}

		const f32 t = std::min(1.0f, maxDegreesDelta / angleDegrees);
		return SlerpUnclamped(from, to, t);
	}

	template class Quaternion<i32>;
	template class Quaternion<f32>;
}
