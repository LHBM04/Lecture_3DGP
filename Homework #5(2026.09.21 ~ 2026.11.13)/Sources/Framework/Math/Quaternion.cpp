#include "Precompiled.hpp"
#include "Quaternion.hpp"

#include "Mathf.hpp"

#include "Matrix4x4.hpp"

#include "../Core/Common.hpp"

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
	TScalar Quaternion<TScalar>::operator[](size_t index) const noexcept
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
	TScalar& Quaternion<TScalar>::operator[](size_t index) noexcept
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
	Quaternion<TScalar> Quaternion<TScalar>::operator+(const Quaternion<TScalar>& _other) const noexcept
	{
		if constexpr (std::same_as<TScalar, std::int32_t>)
		{
			return Quaternion<TScalar>(value.x + _other.value.x, value.y + _other.value.y, value.z + _other.value.z, value.w + _other.value.w);
		}
		else
		{
			Quaternion<TScalar> result{};
			Store(result, DirectX::XMVectorAdd(Load(*this), Load(_other)));
			return result;
		}
	}

	template <Scalarable TScalar>
	Quaternion<TScalar>& Quaternion<TScalar>::operator+=(const Quaternion<TScalar>& _other) noexcept
	{
		*this = *this + _other;
		return *this;
	}

	template <Scalarable TScalar>
	Quaternion<TScalar> Quaternion<TScalar>::operator-(const Quaternion<TScalar>& _other) const noexcept
	{
		if constexpr (std::same_as<TScalar, std::int32_t>)
		{
			return Quaternion<TScalar>(value.x - _other.value.x, value.y - _other.value.y, value.z - _other.value.z, value.w - _other.value.w);
		}
		else
		{
			Quaternion<TScalar> result{};
			Store(result, DirectX::XMVectorSubtract(Load(*this), Load(_other)));
			return result;
		}
	}

	template <Scalarable TScalar>
	Quaternion<TScalar>& Quaternion<TScalar>::operator-=(const Quaternion<TScalar>& _other) noexcept
	{
		*this = *this - _other;
		return *this;
	}

	template <Scalarable TScalar>
	Quaternion<TScalar> Quaternion<TScalar>::operator*(TScalar _scalar) const noexcept
	{
		if constexpr (std::same_as<TScalar, std::int32_t>)
		{
			return Quaternion<TScalar>(value.x * _scalar, value.y * _scalar, value.z * _scalar, value.w * _scalar);
		}
		else
		{
			Quaternion<TScalar> result{};
			Store(result, DirectX::XMVectorScale(Load(*this), _scalar));
			return result;
		}
	}

	template <Scalarable TScalar>
	Quaternion<TScalar> Quaternion<TScalar>::operator*(const Quaternion<TScalar>& _other) const noexcept
	{
		if constexpr (std::same_as<TScalar, std::int32_t>)
		{
			return Quaternion<TScalar>(
				_other.value.w * value.x + _other.value.x * value.w + _other.value.y * value.z - _other.value.z * value.y,
				_other.value.w * value.y - _other.value.x * value.z + _other.value.y * value.w + _other.value.z * value.x,
				_other.value.w * value.z + _other.value.x * value.y - _other.value.y * value.x + _other.value.z * value.w,
				_other.value.w * value.w - _other.value.x * value.x - _other.value.y * value.y - _other.value.z * value.z);
		}
		else
		{
			Quaternion<TScalar> result{};
			Store(result, DirectX::XMQuaternionMultiply(Load(*this), Load(_other)));
			return result;
		}
	}

	template <Scalarable TScalar>
	Vector3D<float> Quaternion<TScalar>::operator*(const Vector3D<float>& vector) const noexcept
		requires std::same_as<TScalar, f32>
	{
		Vector3D<float> result{};
		Vector3D<float>::Store(result, DirectX::XMVector3Rotate(Vector3D<float>::Load(vector), Load(*this)));
		return result;
	}

	template <Scalarable TScalar>
	Quaternion<TScalar>& Quaternion<TScalar>::operator*=(TScalar _scalar) noexcept
	{
		*this = *this * _scalar;
		return *this;
	}

	template <Scalarable TScalar>
	Quaternion<TScalar>& Quaternion<TScalar>::operator*=(const Quaternion<TScalar>& _other) noexcept
	{
		*this = *this * _other;
		return *this;
	}

	template <Scalarable TScalar>
	Quaternion<TScalar> Quaternion<TScalar>::operator/(TScalar _scalar) const noexcept
	{
		if constexpr (std::same_as<TScalar, std::int32_t>)
		{
			ASSERT_FMT(_scalar != TScalar{}, "_scalar가 0입니다.");

			return Quaternion<TScalar>(value.x / _scalar, value.y / _scalar, value.z / _scalar, value.w / _scalar);
		}
		else
		{
			ASSERT_FMT(_scalar != 0.0f, "_scalar가 0입니다.");

			Quaternion<TScalar> result{};
			Store(result, DirectX::XMVectorDivide(Load(*this), DirectX::XMVectorReplicate(_scalar)));
			return result;
		}
	}

	template <Scalarable TScalar>
	Quaternion<TScalar>& Quaternion<TScalar>::operator/=(TScalar _scalar) noexcept
	{
		*this = *this / _scalar;
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
	Vector3D<float> Quaternion<TScalar>::GetEulerAngles() const noexcept
		requires std::same_as<TScalar, f32>
	{
		const auto rotation = Normalize(*this);
		DirectX::XMFLOAT4X4 matrix;
		DirectX::XMStoreFloat4x4(&matrix, DirectX::XMMatrixRotationQuaternion(Load(rotation)));
		const float pitch = std::asin(std::clamp(-matrix._32, -1.0f, 1.0f));
		float yaw;
		float roll;
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
		return Vector3D<float>(pitch, yaw, roll) * (180.0f / std::numbers::pi_v<float>);
	}

	template <Scalarable TScalar>
	void Quaternion<TScalar>::SetEulerAngles(const Vector3D<float>& eulerDegrees) noexcept
		requires std::same_as<TScalar, f32>
	{
		*this = Euler(eulerDegrees);
	}

	template <Scalarable TScalar>
	float Quaternion<TScalar>::GetMagnitude() const noexcept
		requires std::same_as<TScalar, f32>
	{
		return DirectX::XMVectorGetX(DirectX::XMVector4Length(Load(*this)));
	}

	template <Scalarable TScalar>
	float Quaternion<TScalar>::GetSqrMagnitude() const noexcept
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
	void Quaternion<TScalar>::SetFromToRotation(const Vector3D<float>& from, const Vector3D<float>& to) noexcept
		requires std::same_as<TScalar, f32>
	{
		Vector3D<float> normalizedFrom = Vector3D<float>::Normalize(from);
		Vector3D<float> normalizedTo = Vector3D<float>::Normalize(to);

		const float dot = std::clamp(Vector3D<float>::Dot(normalizedFrom, normalizedTo), -1.0f, 1.0f);

		if (dot > 1.0f - std::numeric_limits<float>::epsilon())
		{
			*this = GetIdentity();
			return;
		}

		if (dot < -1.0f + std::numeric_limits<float>::epsilon())
		{
			Vector3D<float> axis = Vector3D<float>::Cross(Vector3D<float>::GetRight(), normalizedFrom);
			if (axis.GetSqrMagnitude() <= std::numeric_limits<float>::epsilon())
			{
				axis = Vector3D<float>::Cross(Vector3D<float>::GetUp(), normalizedFrom);
			}

			*this = AngleAxis(180.0f, axis);
			return;
		}

		Vector3D<float> axis = Vector3D<float>::Cross(normalizedFrom, normalizedTo);
		const float angleDegrees = std::acos(dot) * (180.0f / std::numbers::pi_v<float>);
		*this = AngleAxis(angleDegrees, axis);
	}

	template <Scalarable TScalar>
	void Quaternion<TScalar>::SetLookRotation(const Vector3D<float>& view) noexcept
		requires std::same_as<TScalar, f32>
	{
		*this = LookRotation(view, Vector3D<float>::GetUp());
	}

	template <Scalarable TScalar>
	void Quaternion<TScalar>::SetLookRotation(const Vector3D<float>& view, const Vector3D<float>& up) noexcept
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
	bool Quaternion<TScalar>::IsNormalized(float epsilon) const noexcept
		requires std::same_as<TScalar, f32>
	{
		const float lenSqr = GetSqrMagnitude();
		return std::abs(lenSqr - 1.0f) <= epsilon;
	}

	template <Scalarable TScalar>
	void Quaternion<TScalar>::Normalize() noexcept
		requires std::same_as<TScalar, f32>
	{
		*this = GetNormalized();
	}

	template <Scalarable TScalar>
	void Quaternion<TScalar>::ToAngleAxis(float& angleDegrees, Vector3D<float>& axis) const noexcept
		requires std::same_as<TScalar, f32>
	{
		DirectX::XMVECTOR axisVector;
		float angleRadians;
		DirectX::XMQuaternionToAxisAngle(&axisVector, &angleRadians, Load(*this));

		Vector3D<float>::Store(axis, axisVector);
		angleDegrees = angleRadians * (180.0f / std::numbers::pi_v<float>);
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
	bool Quaternion<TScalar>::IsApproximately(const Quaternion<TScalar>& lhs, const Quaternion<TScalar>& rhs, float epsilon) noexcept
		requires std::same_as<TScalar, f32>
	{
		return std::abs(Dot(lhs, rhs)) >= (1.0f - epsilon);
	}

	template <Scalarable TScalar>
	float Quaternion<TScalar>::Angle(const Quaternion<TScalar>& a, const Quaternion<TScalar>& b) noexcept
		requires std::same_as<TScalar, f32>
	{
		const float d = std::clamp(std::abs(Dot(a, b)), 0.0f, 1.0f);
		return (2.0f * std::acos(d)) * (180.0f / std::numbers::pi_v<float>);
	}

	template <Scalarable TScalar>
	Quaternion<TScalar> Quaternion<TScalar>::AngleAxis(float angleDegrees, Vector3D<float> axis) noexcept
		requires std::same_as<TScalar, f32>
	{
		Vector3D<float> normalizedAxis = Vector3D<float>::Normalize(axis);

		Quaternion<TScalar> result{};
		Store(result, DirectX::XMQuaternionRotationAxis(Vector3D<float>::Load(normalizedAxis), angleDegrees * (std::numbers::pi_v<float> / 180.0f)));
		return result;
	}

	template <Scalarable TScalar>
	Quaternion<TScalar> Quaternion<TScalar>::Euler(float xDegrees, float yDegrees, float zDegrees) noexcept
		requires std::same_as<TScalar, f32>
	{
		Quaternion<TScalar> result{};
		Store(result, DirectX::XMQuaternionRotationRollPitchYaw(xDegrees * (std::numbers::pi_v<float> / 180.0f), yDegrees * (std::numbers::pi_v<float> / 180.0f), zDegrees * (std::numbers::pi_v<float> / 180.0f)));
		return result;
	}

	template <Scalarable TScalar>
	Quaternion<TScalar> Quaternion<TScalar>::Euler(const Vector3D<float>& eulerDegrees) noexcept
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
	float Quaternion<TScalar>::Dot(const Quaternion<TScalar>& a, const Quaternion<TScalar>& b) noexcept
		requires std::same_as<TScalar, f32>
	{
		return DirectX::XMVectorGetX(DirectX::XMVector4Dot(Load(a), Load(b)));
	}

	template <Scalarable TScalar>
	Quaternion<TScalar> Quaternion<TScalar>::Lerp(const Quaternion<TScalar>& a, const Quaternion<TScalar>& b, float t) noexcept
		requires std::same_as<TScalar, f32>
	{
		return LerpUnclamped(a, b, std::clamp(t, 0.0f, 1.0f));
	}

	template <Scalarable TScalar>
	Quaternion<TScalar> Quaternion<TScalar>::LerpUnclamped(const Quaternion<TScalar>& a, const Quaternion<TScalar>& b, float t) noexcept
		requires std::same_as<TScalar, f32>
	{
		Quaternion<TScalar> result{};
		Store(result, DirectX::XMQuaternionNormalize(DirectX::XMVectorLerp(Load(a), Load(b), t)));
		return result;
	}

	template <Scalarable TScalar>
	Quaternion<TScalar> Quaternion<TScalar>::Slerp(const Quaternion<TScalar>& a, const Quaternion<TScalar>& b, float t) noexcept
		requires std::same_as<TScalar, f32>
	{
		return SlerpUnclamped(a, b, std::clamp(t, 0.0f, 1.0f));
	}

	template <Scalarable TScalar>
	Quaternion<TScalar> Quaternion<TScalar>::SlerpUnclamped(const Quaternion<TScalar>& a, const Quaternion<TScalar>& b, float t) noexcept
		requires std::same_as<TScalar, f32>
	{
		Quaternion<TScalar> result{};
		Store(result, DirectX::XMQuaternionSlerp(Load(a), Load(b), t));
		return result;
	}

	template <Scalarable TScalar>
	Quaternion<TScalar> Quaternion<TScalar>::FromToRotation(const Vector3D<float>& fromDirection, const Vector3D<float>& toDirection) noexcept
		requires std::same_as<TScalar, f32>
	{
		Vector3D<float> from = Vector3D<float>::Normalize(fromDirection);
		Vector3D<float> to = Vector3D<float>::Normalize(toDirection);

		const float dot = std::clamp(Vector3D<float>::Dot(from, to), -1.0f, 1.0f);

		if (dot > 1.0f - std::numeric_limits<float>::epsilon())
		{
			return GetIdentity();
		}

		if (dot < -1.0f + std::numeric_limits<float>::epsilon())
		{
			Vector3D<float> axis = Vector3D<float>::Cross(Vector3D<float>::GetRight(), from);
			if (axis.GetSqrMagnitude() <= std::numeric_limits<float>::epsilon())
			{
				axis = Vector3D<float>::Cross(Vector3D<float>::GetUp(), from);
			}

			return AngleAxis(180.0f, axis);
		}

		Vector3D<float> axis = Vector3D<float>::Cross(from, to);
		const float angleDegrees = std::acos(dot) * (180.0f / std::numbers::pi_v<float>);
		return AngleAxis(angleDegrees, axis);
	}

	template <Scalarable TScalar>
	Quaternion<TScalar> Quaternion<TScalar>::LookRotation(const Vector3D<float>& forward, const Vector3D<float>& up) noexcept
		requires std::same_as<TScalar, f32>
	{
		Vector3D<float> normalizedForward = Vector3D<float>::Normalize(forward);
		if (normalizedForward.GetSqrMagnitude() <= std::numeric_limits<float>::epsilon())
		{
			return GetIdentity();
		}

		Vector3D<float> right = Vector3D<float>::Normalize(Vector3D<float>::Cross(up, normalizedForward));
		if (right.GetSqrMagnitude() <= std::numeric_limits<float>::epsilon())
		{
			right = Vector3D<float>::Normalize(Vector3D<float>::Cross(Vector3D<float>::GetUp(), normalizedForward));
			if (right.GetSqrMagnitude() <= std::numeric_limits<float>::epsilon())
			{
				right = Vector3D<float>::Normalize(Vector3D<float>::Cross(Vector3D<float>::GetRight(), normalizedForward));
			}
		}

		Vector3D<float> orthogonalUp = Vector3D<float>::Cross(normalizedForward, right);

		DirectX::XMMATRIX basis = DirectX::XMMatrixIdentity();
		basis.r[0] = Vector3D<float>::Load(right);
		basis.r[1] = Vector3D<float>::Load(orthogonalUp);
		basis.r[2] = Vector3D<float>::Load(normalizedForward);

		Quaternion<TScalar> result{};
		Store(result, DirectX::XMQuaternionRotationMatrix(basis));
		return result;
	}

	template <Scalarable TScalar>
	Quaternion<TScalar> Quaternion<TScalar>::RotateTowards(const Quaternion<TScalar>& from, const Quaternion<TScalar>& to, float maxDegreesDelta) noexcept
		requires std::same_as<TScalar, f32>
	{
		const float angleDegrees = Angle(from, to);
		if (angleDegrees <= std::numeric_limits<float>::epsilon())
		{
			return to;
		}

		const float t = std::min(1.0f, maxDegreesDelta / angleDegrees);
		return SlerpUnclamped(from, to, t);
	}

	template <Scalarable TScalar>
	nlohmann::json Quaternion<TScalar>::Serialize() const
	{
		const TScalar components[]{ GetX(), GetY(), GetZ(), GetW() };

		if (!IsFinite())
		{
			return nlohmann::json::value_t::discarded;
		}

		return nlohmann::json(components);
	}

	template <Scalarable TScalar>
	bool Quaternion<TScalar>::Deserialize(const nlohmann::json& value) noexcept
	{
		constexpr usize COMPONENT_COUNT = 4;
		if (!value.is_array() || value.size() != COMPONENT_COUNT)
		{
			return false;
		}

		TScalar components[COMPONENT_COUNT]{};
		for (usize index = 0; index < COMPONENT_COUNT; ++index)
		{
			const auto& component = value[index];
			const bool isNumber = std::same_as<TScalar, i32> ? component.is_number_integer() : component.is_number();
			if (!isNumber || !Mathf::TryConvertScalar(component.get<f64>(), components[index]))
			{
				return false;
			}
		}

		*this = Quaternion<TScalar>{ components[0], components[1], components[2], components[3] };
		return true;
	}

	template class Quaternion<std::int32_t>;
	template class Quaternion<float>;
}
