#include "Precompiled.hpp"
#include "Quaternion.hpp"

#include "Matrix4x4.hpp"
#include "Quaternion_Impl.hpp"
#include "Vector3D_Impl.hpp"

namespace TUK::Framework
{
	template <IsNumeric TValue>
	Quaternion<TValue>::Quaternion() noexcept
		: values{static_cast<TValue>(0), static_cast<TValue>(0), static_cast<TValue>(0), static_cast<TValue>(1)}
	{
	}

	template <IsNumeric TValue>
	Quaternion<TValue>::Quaternion(TValue value) noexcept
		: values{value, value, value, value}
	{
	}

	template <IsNumeric TValue>
	Quaternion<TValue>::Quaternion(TValue x, TValue y, TValue z, TValue w) noexcept
		: values{x, y, z, w}
	{
	}

	template <IsNumeric TValue>
	Quaternion<TValue>::Quaternion(const Quaternion<TValue>& other) noexcept
		: values{}
	{
		for (std::size_t index = 0; index < 4; ++index) values[index] = other.values[index];
	}

	template <IsNumeric TValue>
	Quaternion<TValue>::Quaternion(Quaternion<TValue>&& other) noexcept
		: values{}
	{
		for (std::size_t index = 0; index < 4; ++index) values[index] = other.values[index];
	}

	template <IsNumeric TValue>
	Quaternion<TValue>& Quaternion<TValue>::operator=(const Quaternion<TValue>& _other) noexcept
	{
		values[0] = _other.GetX();
		values[1] = _other.GetY();
		values[2] = _other.GetZ();
		values[3] = _other.GetW();
		return *this;
	}

	template <IsNumeric TValue>
	Quaternion<TValue>& Quaternion<TValue>::operator=(Quaternion<TValue>&& _other) noexcept
	{
		values[0] = _other.GetX();
		values[1] = _other.GetY();
		values[2] = _other.GetZ();
		values[3] = _other.GetW();
		return *this;
	}

	template <IsNumeric TValue>
	TValue Quaternion<TValue>::operator[](size_t index) const noexcept
	{
		assert(index < 4);
		switch (index)
		{
		case 0:
			return values[0];
		case 1:
			return values[1];
		case 2:
			return values[2];
		default:
			return values[3];
		}
	}

	template <IsNumeric TValue>
	TValue& Quaternion<TValue>::operator[](size_t index) noexcept
	{
		assert(index < 4);
		switch (index)
		{
		case 0:
			return values[0];
		case 1:
			return values[1];
		case 2:
			return values[2];
		default:
			return values[3];
		}
	}

	template <IsNumeric TValue>
	Quaternion<TValue> Quaternion<TValue>::operator+(const Quaternion<TValue>& _other) const noexcept
	{
		if constexpr (std::same_as<TValue, float>)
		{
			Quaternion<TValue> result;
			Store(result, DirectX::XMVectorAdd(Load(*this), Load(_other)));
			return result;
		}
		else
		{
			return Quaternion<TValue>(values[0] + _other.values[0], values[1] + _other.values[1], values[2] + _other.values[2], values[3] + _other.values[3]);
		}
	}

	template <IsNumeric TValue>
	Quaternion<TValue>& Quaternion<TValue>::operator+=(const Quaternion<TValue>& _other) noexcept
	{
		*this = *this + _other;
		return *this;
	}

	template <IsNumeric TValue>
	Quaternion<TValue> Quaternion<TValue>::operator-(const Quaternion<TValue>& _other) const noexcept
	{
		if constexpr (std::same_as<TValue, float>)
		{
			Quaternion<TValue> result;
			Store(result, DirectX::XMVectorSubtract(Load(*this), Load(_other)));
			return result;
		}
		else
		{
			return Quaternion<TValue>(values[0] - _other.values[0], values[1] - _other.values[1], values[2] - _other.values[2], values[3] - _other.values[3]);
		}
	}

	template <IsNumeric TValue>
	Quaternion<TValue>& Quaternion<TValue>::operator-=(const Quaternion<TValue>& _other) noexcept
	{
		*this = *this - _other;
		return *this;
	}

	template <IsNumeric TValue>
	Quaternion<TValue> Quaternion<TValue>::operator*(TValue _scalar) const noexcept
	{
		if constexpr (std::same_as<TValue, float>)
		{
			Quaternion<TValue> result;
			Store(result, DirectX::XMVectorScale(Load(*this), _scalar));
			return result;
		}
		else
		{
			return Quaternion<TValue>(values[0] * _scalar, values[1] * _scalar, values[2] * _scalar, values[3] * _scalar);
		}
	}

	template <IsNumeric TValue>
	Quaternion<TValue> Quaternion<TValue>::operator*(const Quaternion<TValue>& _other) const noexcept
	{
		if constexpr (std::same_as<TValue, float>)
		{
			Quaternion<TValue> result;
			Store(result, DirectX::XMQuaternionMultiply(Load(*this), Load(_other)));
			return result;
		}
		else
		{
			return Quaternion<TValue>(
				_other.values[3] * values[0] + _other.values[0] * values[3] + _other.values[1] * values[2] - _other.values[2] * values[1],
				_other.values[3] * values[1] - _other.values[0] * values[2] + _other.values[1] * values[3] + _other.values[2] * values[0],
				_other.values[3] * values[2] + _other.values[0] * values[1] - _other.values[1] * values[0] + _other.values[2] * values[3],
				_other.values[3] * values[3] - _other.values[0] * values[0] - _other.values[1] * values[1] - _other.values[2] * values[2]);
		}
	}

	template <IsNumeric TValue>
	Vector3D<float> Quaternion<TValue>::operator*(const Vector3D<float>& vector) const noexcept
		requires std::same_as<TValue, float>
	{
		Vector3D<float> result;
		Store(result, DirectX::XMVector3Rotate(Load(vector), Load(*this)));
		return result;
	}

	template <IsNumeric TValue>
	Quaternion<TValue>& Quaternion<TValue>::operator*=(TValue _scalar) noexcept
	{
		*this = *this * _scalar;
		return *this;
	}

	template <IsNumeric TValue>
	Quaternion<TValue>& Quaternion<TValue>::operator*=(const Quaternion<TValue>& _other) noexcept
	{
		*this = *this * _other;
		return *this;
	}

	template <IsNumeric TValue>
	Quaternion<TValue> Quaternion<TValue>::operator/(TValue _scalar) const noexcept
	{
		assert(_scalar != TValue{});
		if constexpr (std::same_as<TValue, float>)
		{
			Quaternion<TValue> result;
			Store(result, DirectX::XMVectorScale(Load(*this), 1.0f / _scalar));
			return result;
		}
		else
		{
			return Quaternion<TValue>(values[0] / _scalar, values[1] / _scalar, values[2] / _scalar, values[3] / _scalar);
		}
	}

	template <IsNumeric TValue>
	Quaternion<TValue>& Quaternion<TValue>::operator/=(TValue _scalar) noexcept
	{
		*this = *this / _scalar;
		return *this;
	}

	template <IsNumeric TValue>
	bool Quaternion<TValue>::operator==(const Quaternion<TValue>& other) const noexcept
	{
		return (*this <=> other) == std::partial_ordering::equivalent;
	}

	template <IsNumeric TValue>
	bool Quaternion<TValue>::operator!=(const Quaternion<TValue>& other) const noexcept
	{
		return !(*this == other);
	}

	template <IsNumeric TValue>
	std::partial_ordering Quaternion<TValue>::operator<=>(const Quaternion<TValue>& other) const noexcept
	{
		if (const auto order = values[0] <=> other.GetX(); order != 0)
		{
			return order;
		}
		if (const auto order = values[1] <=> other.GetY(); order != 0)
		{
			return order;
		}
		if (const auto order = values[2] <=> other.GetZ(); order != 0)
		{
			return order;
		}

		return values[3] <=> other.GetW();
	}

	template <IsNumeric TValue>
	TValue Quaternion<TValue>::GetX() const noexcept
	{
		return values[0];
	}

	template <IsNumeric TValue>
	void Quaternion<TValue>::SetX(TValue component) noexcept
	{
		values[0] = component;
	}

	template <IsNumeric TValue>
	TValue Quaternion<TValue>::GetY() const noexcept
	{
		return values[1];
	}

	template <IsNumeric TValue>
	void Quaternion<TValue>::SetY(TValue component) noexcept
	{
		values[1] = component;
	}

	template <IsNumeric TValue>
	TValue Quaternion<TValue>::GetZ() const noexcept
	{
		return values[2];
	}

	template <IsNumeric TValue>
	void Quaternion<TValue>::SetZ(TValue component) noexcept
	{
		values[2] = component;
	}

	template <IsNumeric TValue>
	TValue Quaternion<TValue>::GetW() const noexcept
	{
		return values[3];
	}

	template <IsNumeric TValue>
	void Quaternion<TValue>::SetW(TValue component) noexcept
	{
		values[3] = component;
	}

	template <IsNumeric TValue>
	void Quaternion<TValue>::Set(TValue x, TValue y, TValue z, TValue w) noexcept
	{
		values[0] = x;
		values[1] = y;
		values[2] = z;
		values[3] = w;
	}

	template <IsNumeric TValue>
	Vector3D<float> Quaternion<TValue>::GetEulerAngles() const noexcept
		requires std::same_as<TValue, float>
	{
		const auto matrix = Matrix4x4<TValue>::Rotate(Normalize(*this));
		const float pitch = std::asin(std::clamp(-matrix[9], -1.0f, 1.0f));
		float yaw;
		float roll;
		if (std::abs(std::cos(pitch)) > 0.00001f)
		{
			yaw = std::atan2(matrix[8], matrix[10]);
			roll = std::atan2(matrix[1], matrix[5]);
		}
		else
		{
			yaw = std::atan2(-matrix[2], matrix[0]);
			roll = 0.0f;
		}
		return Vector3D<float>(pitch, yaw, roll) * (180.0f / std::numbers::pi_v<float>);
	}

	template <IsNumeric TValue>
	void Quaternion<TValue>::SetEulerAngles(const Vector3D<float>& eulerDegrees) noexcept
		requires std::same_as<TValue, float>
	{
		*this = Euler(eulerDegrees);
	}

	template <IsNumeric TValue>
	float Quaternion<TValue>::GetMagnitude() const noexcept
		requires std::same_as<TValue, float>
	{
		return DirectX::XMVectorGetX(DirectX::XMQuaternionLength(Load(*this)));
	}

	template <IsNumeric TValue>
	float Quaternion<TValue>::GetSqrMagnitude() const noexcept
		requires std::same_as<TValue, float>
	{
		return DirectX::XMVectorGetX(DirectX::XMQuaternionLengthSq(Load(*this)));
	}

	template <IsNumeric TValue>
	Quaternion<TValue> Quaternion<TValue>::GetNormalized() const noexcept
		requires std::same_as<TValue, float>
	{
		return Normalize(*this);
	}

	template <IsNumeric TValue>
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

	template <IsNumeric TValue>
	void Quaternion<TValue>::SetLookRotation(const Vector3D<float>& view) noexcept
		requires std::same_as<TValue, float>
	{
		*this = LookRotation(view, Vector3D<float>::GetUp());
	}

	template <IsNumeric TValue>
	void Quaternion<TValue>::SetLookRotation(const Vector3D<float>& view, const Vector3D<float>& up) noexcept
		requires std::same_as<TValue, float>
	{
		*this = LookRotation(view, up);
	}

	template <IsNumeric TValue>
	bool Quaternion<TValue>::IsFinite() const noexcept
	{
		return std::isfinite(values[0]) && std::isfinite(values[1]) && std::isfinite(values[2]) && std::isfinite(values[3]);
	}

	template <IsNumeric TValue>
	bool Quaternion<TValue>::IsNormalized(float epsilon) const noexcept
		requires std::same_as<TValue, float>
	{
		const float lenSqr = GetSqrMagnitude();
		return std::abs(lenSqr - 1.0f) <= epsilon;
	}

	template <IsNumeric TValue>
	void Quaternion<TValue>::Normalize() noexcept
		requires std::same_as<TValue, float>
	{
		*this = GetNormalized();
	}

	template <IsNumeric TValue>
	void Quaternion<TValue>::ToAngleAxis(float& angleDegrees, Vector3D<float>& axis) const noexcept
		requires std::same_as<TValue, float>
	{
		axis.Set(GetX(), GetY(), GetZ());
		angleDegrees = 2.0f * std::acos(std::clamp(GetW(), -1.0f, 1.0f)) * (180.0f / std::numbers::pi_v<float>);
	}

	template <IsNumeric TValue>
	Quaternion<TValue> Quaternion<TValue>::Normalize(const Quaternion<TValue>& rotation) noexcept
		requires std::same_as<TValue, float>
	{
		Quaternion<TValue> result;
		Store(result, DirectX::XMQuaternionNormalize(Load(rotation)));
		return result;
	}

	template <IsNumeric TValue>
	Quaternion<TValue> Quaternion<TValue>::GetIdentity() noexcept
	{
		return Quaternion<TValue>(TValue{}, TValue{}, TValue{}, static_cast<TValue>(1));
	}

	template <IsNumeric TValue>
	bool Quaternion<TValue>::IsApproximately(const Quaternion<TValue>& lhs, const Quaternion<TValue>& rhs, float epsilon) noexcept
		requires std::same_as<TValue, float>
	{
		return std::abs(Dot(lhs, rhs)) >= (1.0f - epsilon);
	}

	template <IsNumeric TValue>
	float Quaternion<TValue>::Angle(const Quaternion<TValue>& a, const Quaternion<TValue>& b) noexcept
		requires std::same_as<TValue, float>
	{
		const float d = std::clamp(std::abs(Dot(a, b)), 0.0f, 1.0f);
		return (2.0f * std::acos(d)) * (180.0f / std::numbers::pi_v<float>);
	}

	template <IsNumeric TValue>
	Quaternion<TValue> Quaternion<TValue>::AngleAxis(float angleDegrees, Vector3D<float> axis) noexcept
		requires std::same_as<TValue, float>
	{
		Quaternion<TValue> result;
		Store(result, DirectX::XMQuaternionRotationNormal(DirectX::XMVector3Normalize(Load(axis)), DirectX::XMConvertToRadians(angleDegrees)));
		return result;
	}

	template <IsNumeric TValue>
	Quaternion<TValue> Quaternion<TValue>::Euler(float xDegrees, float yDegrees, float zDegrees) noexcept
		requires std::same_as<TValue, float>
	{
		Quaternion<TValue> result;
		Store(result, DirectX::XMQuaternionRotationRollPitchYaw(
			DirectX::XMConvertToRadians(xDegrees),
			DirectX::XMConvertToRadians(yDegrees),
			DirectX::XMConvertToRadians(zDegrees)));
		return result;
	}

	template <IsNumeric TValue>
	Quaternion<TValue> Quaternion<TValue>::Euler(const Vector3D<float>& eulerDegrees) noexcept
		requires std::same_as<TValue, float>
	{
		return Euler(eulerDegrees.GetX(), eulerDegrees.GetY(), eulerDegrees.GetZ());
	}

	template <IsNumeric TValue>
	Quaternion<TValue> Quaternion<TValue>::Inverse(const Quaternion<TValue>& rotation) noexcept
		requires std::same_as<TValue, float>
	{
		const float lengthSquared = rotation.GetSqrMagnitude();
		if (lengthSquared <= std::numeric_limits<float>::epsilon()) return Quaternion(TValue{});
		Quaternion<TValue> result;
		Store(result, DirectX::XMQuaternionInverse(Load(rotation)));
		return result;
	}

	template <IsNumeric TValue>
	Quaternion<TValue> Quaternion<TValue>::Conjugate(const Quaternion<TValue>& rotation) noexcept
		requires std::same_as<TValue, float>
	{
		Quaternion<TValue> result;
		Store(result, DirectX::XMQuaternionConjugate(Load(rotation)));
		return result;
	}

	template <IsNumeric TValue>
	float Quaternion<TValue>::Dot(const Quaternion<TValue>& a, const Quaternion<TValue>& b) noexcept
		requires std::same_as<TValue, float>
	{
		return DirectX::XMVectorGetX(DirectX::XMQuaternionDot(Load(a), Load(b)));
	}

	template <IsNumeric TValue>
	Quaternion<TValue> Quaternion<TValue>::Lerp(const Quaternion<TValue>& a, const Quaternion<TValue>& b, float t) noexcept
		requires std::same_as<TValue, float>
	{
		return LerpUnclamped(a, b, std::clamp(t, 0.0f, 1.0f));
	}

	template <IsNumeric TValue>
	Quaternion<TValue> Quaternion<TValue>::LerpUnclamped(const Quaternion<TValue>& a, const Quaternion<TValue>& b, float t) noexcept
		requires std::same_as<TValue, float>
	{
		return Normalize(a + (b - a) * t);
	}

	template <IsNumeric TValue>
	Quaternion<TValue> Quaternion<TValue>::Slerp(const Quaternion<TValue>& a, const Quaternion<TValue>& b, float t) noexcept
		requires std::same_as<TValue, float>
	{
		return SlerpUnclamped(a, b, std::clamp(t, 0.0f, 1.0f));
	}

	template <IsNumeric TValue>
	Quaternion<TValue> Quaternion<TValue>::SlerpUnclamped(const Quaternion<TValue>& a, const Quaternion<TValue>& b, float t) noexcept
		requires std::same_as<TValue, float>
	{
		Quaternion<TValue> result;
		Store(result, DirectX::XMQuaternionSlerp(Load(a), Load(b), t));
		return result;
	}

	template <IsNumeric TValue>
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

	template <IsNumeric TValue>
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

		Matrix4x4<TValue> basis;
		basis.SetRow(0, Vector4D<TValue>(right));
		basis.SetRow(1, Vector4D<TValue>(orthogonalUp));
		basis.SetRow(2, Vector4D<TValue>(normalizedForward));
		return basis.GetRotation();
	}

	template <IsNumeric TValue>
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

	template class Quaternion<std::int32_t>;
	template class Quaternion<float>;
}
