#include "Precompiled.h"
#include "Quaternion.h"

#include "Matrix4x4.h"

namespace TUK::Framework
{
	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Quaternion<TValue>::Quaternion() noexcept
		: value(static_cast<TValue>(0), static_cast<TValue>(0), static_cast<TValue>(0), static_cast<TValue>(1))
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Quaternion<TValue>::Quaternion(TValue value) noexcept
		: value(value, value, value, value)
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Quaternion<TValue>::Quaternion(TValue x, TValue y, TValue z, TValue w) noexcept
		: value(x, y, z, w)
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Quaternion<TValue>::Quaternion(const Quaternion<TValue>& other) noexcept
		: value(other.value)
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Quaternion<TValue>::Quaternion(Quaternion<TValue>&& other) noexcept
		: value(other.value)
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Quaternion<TValue>& Quaternion<TValue>::operator=(const Quaternion<TValue>& _other) noexcept
	{
		value.x = _other.GetX();
		value.y = _other.GetY();
		value.z = _other.GetZ();
		value.w = _other.GetW();
		return *this;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Quaternion<TValue>& Quaternion<TValue>::operator=(Quaternion<TValue>&& _other) noexcept
	{
		value.x = _other.GetX();
		value.y = _other.GetY();
		value.z = _other.GetZ();
		value.w = _other.GetW();
		return *this;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	TValue Quaternion<TValue>::operator[](size_t index) const noexcept
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
	TValue& Quaternion<TValue>::operator[](size_t index) noexcept
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
	Quaternion<TValue> Quaternion<TValue>::operator+(const Quaternion<TValue>& _other) const noexcept
	{
		if constexpr (std::same_as<TValue, int>)
		{
			return Quaternion<TValue>(value.x + _other.value.x, value.y + _other.value.y, value.z + _other.value.z, value.w + _other.value.w);
		}
		else
		{
			Quaternion<TValue> result{};
			Store(result, DirectX::XMVectorAdd(Load(*this), Load(_other)));
			return result;
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Quaternion<TValue>& Quaternion<TValue>::operator+=(const Quaternion<TValue>& _other) noexcept
	{
		*this = *this + _other;
		return *this;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Quaternion<TValue> Quaternion<TValue>::operator-(const Quaternion<TValue>& _other) const noexcept
	{
		if constexpr (std::same_as<TValue, int>)
		{
			return Quaternion<TValue>(value.x - _other.value.x, value.y - _other.value.y, value.z - _other.value.z, value.w - _other.value.w);
		}
		else
		{
			Quaternion<TValue> result{};
			Store(result, DirectX::XMVectorSubtract(Load(*this), Load(_other)));
			return result;
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Quaternion<TValue>& Quaternion<TValue>::operator-=(const Quaternion<TValue>& _other) noexcept
	{
		*this = *this - _other;
		return *this;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Quaternion<TValue> Quaternion<TValue>::operator*(TValue _scalar) const noexcept
	{
		if constexpr (std::same_as<TValue, int>)
		{
			return Quaternion<TValue>(value.x * _scalar, value.y * _scalar, value.z * _scalar, value.w * _scalar);
		}
		else
		{
			Quaternion<TValue> result{};
			Store(result, DirectX::XMVectorScale(Load(*this), _scalar));
			return result;
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Quaternion<TValue> Quaternion<TValue>::operator*(const Quaternion<TValue>& _other) const noexcept
	{
		if constexpr (std::same_as<TValue, int>)
		{
			return Quaternion<TValue>(
				_other.value.w * value.x + _other.value.x * value.w + _other.value.y * value.z - _other.value.z * value.y,
				_other.value.w * value.y - _other.value.x * value.z + _other.value.y * value.w + _other.value.z * value.x,
				_other.value.w * value.z + _other.value.x * value.y - _other.value.y * value.x + _other.value.z * value.w,
				_other.value.w * value.w - _other.value.x * value.x - _other.value.y * value.y - _other.value.z * value.z);
		}
		else
		{
			Quaternion<TValue> result{};
			Store(result, DirectX::XMQuaternionMultiply(Load(*this), Load(_other)));
			return result;
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<float> Quaternion<TValue>::operator*(const Vector3D<float>& vector) const noexcept
		requires std::same_as<TValue, float>
	{
		Vector3D<float> result{};
		Vector3D<float>::Store(result, DirectX::XMVector3Rotate(Vector3D<float>::Load(vector), Load(*this)));
		return result;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Quaternion<TValue>& Quaternion<TValue>::operator*=(TValue _scalar) noexcept
	{
		*this = *this * _scalar;
		return *this;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Quaternion<TValue>& Quaternion<TValue>::operator*=(const Quaternion<TValue>& _other) noexcept
	{
		*this = *this * _other;
		return *this;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Quaternion<TValue> Quaternion<TValue>::operator/(TValue _scalar) const noexcept
	{
		if constexpr (std::same_as<TValue, int>)
		{
			assert(_scalar != TValue{});
			return Quaternion<TValue>(value.x / _scalar, value.y / _scalar, value.z / _scalar, value.w / _scalar);
		}
		else
		{
			assert(_scalar != 0.0f);
			Quaternion<TValue> result{};
			Store(result, DirectX::XMVectorDivide(Load(*this), DirectX::XMVectorReplicate(_scalar)));
			return result;
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Quaternion<TValue>& Quaternion<TValue>::operator/=(TValue _scalar) noexcept
	{
		*this = *this / _scalar;
		return *this;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	bool Quaternion<TValue>::operator==(const Quaternion<TValue>& other) const noexcept
	{
		return (*this <=> other) == std::partial_ordering::equivalent;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	bool Quaternion<TValue>::operator!=(const Quaternion<TValue>& other) const noexcept
	{
		return !(*this == other);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	std::partial_ordering Quaternion<TValue>::operator<=>(const Quaternion<TValue>& other) const noexcept
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

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	TValue Quaternion<TValue>::GetX() const noexcept
	{
		return value.x;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	void Quaternion<TValue>::SetX(TValue component) noexcept
	{
		value.x = component;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	TValue Quaternion<TValue>::GetY() const noexcept
	{
		return value.y;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	void Quaternion<TValue>::SetY(TValue component) noexcept
	{
		value.y = component;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	TValue Quaternion<TValue>::GetZ() const noexcept
	{
		return value.z;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	void Quaternion<TValue>::SetZ(TValue component) noexcept
	{
		value.z = component;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	TValue Quaternion<TValue>::GetW() const noexcept
	{
		return value.w;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	void Quaternion<TValue>::SetW(TValue component) noexcept
	{
		value.w = component;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	void Quaternion<TValue>::Set(TValue x, TValue y, TValue z, TValue w) noexcept
	{
		value.x = x;
		value.y = y;
		value.z = z;
		value.w = w;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<float> Quaternion<TValue>::GetEulerAngles() const noexcept
		requires std::same_as<TValue, float>
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

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	void Quaternion<TValue>::SetEulerAngles(const Vector3D<float>& eulerDegrees) noexcept
		requires std::same_as<TValue, float>
	{
		*this = Euler(eulerDegrees);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	float Quaternion<TValue>::GetMagnitude() const noexcept
		requires std::same_as<TValue, float>
	{
		return DirectX::XMVectorGetX(DirectX::XMVector4Length(Load(*this)));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	float Quaternion<TValue>::GetSqrMagnitude() const noexcept
		requires std::same_as<TValue, float>
	{
		return Dot(*this, *this);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Quaternion<TValue> Quaternion<TValue>::GetNormalized() const noexcept
		requires std::same_as<TValue, float>
	{
		return Normalize(*this);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	void Quaternion<TValue>::SetFromToRotation(const Vector3D<float>& from, const Vector3D<float>& to) noexcept
		requires std::same_as<TValue, float>
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

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	void Quaternion<TValue>::SetLookRotation(const Vector3D<float>& view) noexcept
		requires std::same_as<TValue, float>
	{
		*this = LookRotation(view, Vector3D<float>::GetUp());
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	void Quaternion<TValue>::SetLookRotation(const Vector3D<float>& view, const Vector3D<float>& up) noexcept
		requires std::same_as<TValue, float>
	{
		*this = LookRotation(view, up);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	bool Quaternion<TValue>::IsFinite() const noexcept
	{
		return std::isfinite(value.x) && std::isfinite(value.y) && std::isfinite(value.z) && std::isfinite(value.w);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	bool Quaternion<TValue>::IsNormalized(float epsilon) const noexcept
		requires std::same_as<TValue, float>
	{
		const float lenSqr = GetSqrMagnitude();
		return std::abs(lenSqr - 1.0f) <= epsilon;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	void Quaternion<TValue>::Normalize() noexcept
		requires std::same_as<TValue, float>
	{
		*this = GetNormalized();
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	void Quaternion<TValue>::ToAngleAxis(float& angleDegrees, Vector3D<float>& axis) const noexcept
		requires std::same_as<TValue, float>
	{
		DirectX::XMVECTOR axisVector;
		float angleRadians;
		DirectX::XMQuaternionToAxisAngle(&axisVector, &angleRadians, Load(*this));

		Vector3D<float>::Store(axis, axisVector);
		angleDegrees = angleRadians * (180.0f / std::numbers::pi_v<float>);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Quaternion<TValue> Quaternion<TValue>::Normalize(const Quaternion<TValue>& rotation) noexcept
		requires std::same_as<TValue, float>
	{
		Quaternion<TValue> result{};
		Store(result, DirectX::XMQuaternionNormalize(Load(rotation)));
		return result;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Quaternion<TValue> Quaternion<TValue>::GetIdentity() noexcept
	{
		return Quaternion<TValue>(TValue{}, TValue{}, TValue{}, static_cast<TValue>(1));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	DirectX::XMVECTOR Quaternion<TValue>::Load(const Quaternion<TValue>& quat) noexcept
		requires std::same_as<TValue, float>
	{
		return DirectX::XMLoadFloat4(&quat.value);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	void Quaternion<TValue>::Store(Quaternion<TValue>& d, DirectX::XMVECTOR s) noexcept
		requires std::same_as<TValue, float>
	{
		DirectX::XMStoreFloat4(&d.value, s);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	bool Quaternion<TValue>::IsApproximately(const Quaternion<TValue>& lhs, const Quaternion<TValue>& rhs, float epsilon) noexcept
		requires std::same_as<TValue, float>
	{
		return std::abs(Dot(lhs, rhs)) >= (1.0f - epsilon);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	float Quaternion<TValue>::Angle(const Quaternion<TValue>& a, const Quaternion<TValue>& b) noexcept
		requires std::same_as<TValue, float>
	{
		const float d = std::clamp(std::abs(Dot(a, b)), 0.0f, 1.0f);
		return (2.0f * std::acos(d)) * (180.0f / std::numbers::pi_v<float>);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Quaternion<TValue> Quaternion<TValue>::AngleAxis(float angleDegrees, Vector3D<float> axis) noexcept
		requires std::same_as<TValue, float>
	{
		Vector3D<float> normalizedAxis = Vector3D<float>::Normalize(axis);

		Quaternion<TValue> result{};
		Store(result, DirectX::XMQuaternionRotationAxis(Vector3D<float>::Load(normalizedAxis), angleDegrees * (std::numbers::pi_v<float> / 180.0f)));
		return result;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Quaternion<TValue> Quaternion<TValue>::Euler(float xDegrees, float yDegrees, float zDegrees) noexcept
		requires std::same_as<TValue, float>
	{
		Quaternion<TValue> result{};
		Store(result, DirectX::XMQuaternionRotationRollPitchYaw(xDegrees * (std::numbers::pi_v<float> / 180.0f), yDegrees * (std::numbers::pi_v<float> / 180.0f), zDegrees * (std::numbers::pi_v<float> / 180.0f)));
		return result;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Quaternion<TValue> Quaternion<TValue>::Euler(const Vector3D<float>& eulerDegrees) noexcept
		requires std::same_as<TValue, float>
	{
		return Euler(eulerDegrees.GetX(), eulerDegrees.GetY(), eulerDegrees.GetZ());
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Quaternion<TValue> Quaternion<TValue>::Inverse(const Quaternion<TValue>& rotation) noexcept
		requires std::same_as<TValue, float>
	{
		Quaternion<TValue> result{};
		Store(result, DirectX::XMQuaternionInverse(Load(rotation)));
		return result;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Quaternion<TValue> Quaternion<TValue>::Conjugate(const Quaternion<TValue>& rotation) noexcept
		requires std::same_as<TValue, float>
	{
		Quaternion<TValue> result{};
		Store(result, DirectX::XMQuaternionConjugate(Load(rotation)));
		return result;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	float Quaternion<TValue>::Dot(const Quaternion<TValue>& a, const Quaternion<TValue>& b) noexcept
		requires std::same_as<TValue, float>
	{
		return DirectX::XMVectorGetX(DirectX::XMVector4Dot(Load(a), Load(b)));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Quaternion<TValue> Quaternion<TValue>::Lerp(const Quaternion<TValue>& a, const Quaternion<TValue>& b, float t) noexcept
		requires std::same_as<TValue, float>
	{
		return LerpUnclamped(a, b, std::clamp(t, 0.0f, 1.0f));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Quaternion<TValue> Quaternion<TValue>::LerpUnclamped(const Quaternion<TValue>& a, const Quaternion<TValue>& b, float t) noexcept
		requires std::same_as<TValue, float>
	{
		Quaternion<TValue> result{};
		Store(result, DirectX::XMQuaternionNormalize(DirectX::XMVectorLerp(Load(a), Load(b), t)));
		return result;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Quaternion<TValue> Quaternion<TValue>::Slerp(const Quaternion<TValue>& a, const Quaternion<TValue>& b, float t) noexcept
		requires std::same_as<TValue, float>
	{
		return SlerpUnclamped(a, b, std::clamp(t, 0.0f, 1.0f));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Quaternion<TValue> Quaternion<TValue>::SlerpUnclamped(const Quaternion<TValue>& a, const Quaternion<TValue>& b, float t) noexcept
		requires std::same_as<TValue, float>
	{
		Quaternion<TValue> result{};
		Store(result, DirectX::XMQuaternionSlerp(Load(a), Load(b), t));
		return result;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Quaternion<TValue> Quaternion<TValue>::FromToRotation(const Vector3D<float>& fromDirection, const Vector3D<float>& toDirection) noexcept
		requires std::same_as<TValue, float>
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

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Quaternion<TValue> Quaternion<TValue>::LookRotation(const Vector3D<float>& forward, const Vector3D<float>& up) noexcept
		requires std::same_as<TValue, float>
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

		Quaternion<TValue> result{};
		Store(result, DirectX::XMQuaternionRotationMatrix(basis));
		return result;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Quaternion<TValue> Quaternion<TValue>::RotateTowards(const Quaternion<TValue>& from, const Quaternion<TValue>& to, float maxDegreesDelta) noexcept
		requires std::same_as<TValue, float>
	{
		const float angleDegrees = Angle(from, to);
		if (angleDegrees <= std::numeric_limits<float>::epsilon())
		{
			return to;
		}

		const float t = std::min(1.0f, maxDegreesDelta / angleDegrees);
		return SlerpUnclamped(from, to, t);
	}

	template class Quaternion<int>;
	template class Quaternion<float>;
}
