#include "Precompiled.h"
#include "Quaternion.h"

#include "Matrix4x4.h"

namespace TUK::Framework
{
	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Quaternion<TValue>::Quaternion() noexcept
		: value(TValue{0}, TValue{0}, TValue{0}, TValue{1})
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Quaternion<TValue>::Quaternion(TValue value_) noexcept
		: value(value_, value_, value_, value_)
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Quaternion<TValue>::Quaternion(TValue x_, TValue y_, TValue z_, TValue w_) noexcept
		: value(x_, y_, z_, w_)
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Quaternion<TValue>::Quaternion(const Quaternion<TValue>& other_) noexcept
		: value(other_.value)
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Quaternion<TValue>::Quaternion(Quaternion<TValue>&& other_) noexcept
		: value(other_.value)
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
			Quaternion<TValue> result;
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
			Quaternion<TValue> result;
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
			Quaternion<TValue> result;
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
			Quaternion<TValue> result;
			Store(result, DirectX::XMQuaternionMultiply(Load(*this), Load(_other)));
			return result;
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<float> Quaternion<TValue>::operator*(const Vector3D<float>& vector_) const noexcept
		requires std::same_as<TValue, float>
	{
		Vector3D<float> result;
		Vector3D<float>::Store(result, DirectX::XMVector3Rotate(Vector3D<float>::Load(vector_), Load(*this)));
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
			Quaternion<TValue> result;
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
	bool Quaternion<TValue>::operator==(const Quaternion<TValue>& other_) const noexcept
	{
		return (*this <=> other_) == std::partial_ordering::equivalent;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	bool Quaternion<TValue>::operator!=(const Quaternion<TValue>& other_) const noexcept
	{
		return !(*this == other_);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	std::partial_ordering Quaternion<TValue>::operator<=>(const Quaternion<TValue>& other_) const noexcept
	{
		if (const auto order = value.x <=> other_.GetX(); order != 0)
		{
			return order;
		}
		if (const auto order = value.y <=> other_.GetY(); order != 0)
		{
			return order;
		}
		if (const auto order = value.z <=> other_.GetZ(); order != 0)
		{
			return order;
		}

		return value.w <=> other_.GetW();
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
	void Quaternion<TValue>::Set(TValue x_, TValue y_, TValue z_, TValue w_) noexcept
	{
		value.x = x_;
		value.y = y_;
		value.z = z_;
		value.w = w_;
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
	void Quaternion<TValue>::SetEulerAngles(const Vector3D<float>& eulerDegrees_) noexcept
		requires std::same_as<TValue, float>
	{
		*this = Euler(eulerDegrees_);
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
	void Quaternion<TValue>::SetFromToRotation(const Vector3D<float>& from_, const Vector3D<float>& to_) noexcept
		requires std::same_as<TValue, float>
	{
		Vector3D<float> from{ Vector3D<float>::Normalize(from_) };
		Vector3D<float> to{ Vector3D<float>::Normalize(to_) };

		const float dot{ std::clamp(Vector3D<float>::Dot(from, to), -1.0f, 1.0f) };

		if (dot > 1.0f - std::numeric_limits<float>::epsilon())
		{
			*this = GetIdentity();
			return;
		}

		if (dot < -1.0f + std::numeric_limits<float>::epsilon())
		{
			Vector3D<float> axis{ Vector3D<float>::Cross(Vector3D<float>::GetRight(), from) };
			if (axis.GetSqrMagnitude() <= std::numeric_limits<float>::epsilon())
			{
				axis = Vector3D<float>::Cross(Vector3D<float>::GetUp(), from);
			}

			*this = AngleAxis(180.0f, axis);
			return;
		}

		Vector3D<float> axis{ Vector3D<float>::Cross(from, to) };
		const float angleDegrees{ std::acos(dot) * (180.0f / std::numbers::pi_v<float>) };
		*this = AngleAxis(angleDegrees, axis);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	void Quaternion<TValue>::SetLookRotation(const Vector3D<float>& view_) noexcept
		requires std::same_as<TValue, float>
	{
		*this = LookRotation(view_, Vector3D<float>::GetUp());
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	void Quaternion<TValue>::SetLookRotation(const Vector3D<float>& view_, const Vector3D<float>& up_) noexcept
		requires std::same_as<TValue, float>
	{
		*this = LookRotation(view_, up_);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	bool Quaternion<TValue>::IsFinite() const noexcept
	{
		return std::isfinite(value.x) && std::isfinite(value.y) && std::isfinite(value.z) && std::isfinite(value.w);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	bool Quaternion<TValue>::IsNormalized(float epsilon_) const noexcept
		requires std::same_as<TValue, float>
	{
		const float lenSqr{ GetSqrMagnitude() };
		return std::abs(lenSqr - 1.0f) <= epsilon_;
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
	void Quaternion<TValue>::ToAngleAxis(float& angleDegrees_, Vector3D<float>& axis_) const noexcept
		requires std::same_as<TValue, float>
	{
		DirectX::XMVECTOR axis;
		float angleRadians;
		DirectX::XMQuaternionToAxisAngle(&axis, &angleRadians, Load(*this));

		Vector3D<float>::Store(axis_, axis);
		angleDegrees_ = angleRadians * (180.0f / std::numbers::pi_v<float>);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Quaternion<TValue> Quaternion<TValue>::Normalize(const Quaternion<TValue>& rotation_) noexcept
		requires std::same_as<TValue, float>
	{
		Quaternion<TValue> result;
		Store(result, DirectX::XMQuaternionNormalize(Load(rotation_)));
		return result;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Quaternion<TValue> Quaternion<TValue>::GetIdentity() noexcept
	{
		return Quaternion<TValue>(TValue{}, TValue{}, TValue{}, TValue{1});
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	DirectX::XMVECTOR Quaternion<TValue>::Load(const Quaternion<TValue>& quat_) noexcept
		requires std::same_as<TValue, float>
	{
		return DirectX::XMLoadFloat4(&quat_.value);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	void Quaternion<TValue>::Store(Quaternion<TValue>& d_, DirectX::XMVECTOR s_) noexcept
		requires std::same_as<TValue, float>
	{
		DirectX::XMStoreFloat4(&d_.value, s_);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	bool Quaternion<TValue>::IsApproximately(const Quaternion<TValue>& lhs_, const Quaternion<TValue>& rhs_, float epsilon_) noexcept
		requires std::same_as<TValue, float>
	{
		return std::abs(Dot(lhs_, rhs_)) >= (1.0f - epsilon_);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	float Quaternion<TValue>::Angle(const Quaternion<TValue>& a_, const Quaternion<TValue>& b_) noexcept
		requires std::same_as<TValue, float>
	{
		const float d{ std::clamp(std::abs(Dot(a_, b_)), 0.0f, 1.0f) };
		return (2.0f * std::acos(d)) * (180.0f / std::numbers::pi_v<float>);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Quaternion<TValue> Quaternion<TValue>::AngleAxis(float angleDegrees_, Vector3D<float> axis_) noexcept
		requires std::same_as<TValue, float>
	{
		Vector3D<float> normalizedAxis{ Vector3D<float>::Normalize(axis_) };

		Quaternion<TValue> result;
		Store(result, DirectX::XMQuaternionRotationAxis(Vector3D<float>::Load(normalizedAxis), angleDegrees_ * (std::numbers::pi_v<float> / 180.0f)));
		return result;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Quaternion<TValue> Quaternion<TValue>::Euler(float xDegrees_, float yDegrees_, float zDegrees_) noexcept
		requires std::same_as<TValue, float>
	{
		Quaternion<TValue> result;
		Store(result, DirectX::XMQuaternionRotationRollPitchYaw(xDegrees_ * (std::numbers::pi_v<float> / 180.0f), yDegrees_ * (std::numbers::pi_v<float> / 180.0f), zDegrees_ * (std::numbers::pi_v<float> / 180.0f)));
		return result;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Quaternion<TValue> Quaternion<TValue>::Euler(const Vector3D<float>& eulerDegrees_) noexcept
		requires std::same_as<TValue, float>
	{
		return Euler(eulerDegrees_.GetX(), eulerDegrees_.GetY(), eulerDegrees_.GetZ());
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Quaternion<TValue> Quaternion<TValue>::Inverse(const Quaternion<TValue>& rotation_) noexcept
		requires std::same_as<TValue, float>
	{
		Quaternion<TValue> result;
		Store(result, DirectX::XMQuaternionInverse(Load(rotation_)));
		return result;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Quaternion<TValue> Quaternion<TValue>::Conjugate(const Quaternion<TValue>& rotation_) noexcept
		requires std::same_as<TValue, float>
	{
		Quaternion<TValue> result;
		Store(result, DirectX::XMQuaternionConjugate(Load(rotation_)));
		return result;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	float Quaternion<TValue>::Dot(const Quaternion<TValue>& a_, const Quaternion<TValue>& b_) noexcept
		requires std::same_as<TValue, float>
	{
		return DirectX::XMVectorGetX(DirectX::XMVector4Dot(Load(a_), Load(b_)));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Quaternion<TValue> Quaternion<TValue>::Lerp(const Quaternion<TValue>& a_, const Quaternion<TValue>& b_, float t_) noexcept
		requires std::same_as<TValue, float>
	{
		return LerpUnclamped(a_, b_, std::clamp(t_, 0.0f, 1.0f));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Quaternion<TValue> Quaternion<TValue>::LerpUnclamped(const Quaternion<TValue>& a_, const Quaternion<TValue>& b_, float t_) noexcept
		requires std::same_as<TValue, float>
	{
		Quaternion<TValue> result;
		Store(result, DirectX::XMQuaternionNormalize(DirectX::XMVectorLerp(Load(a_), Load(b_), t_)));
		return result;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Quaternion<TValue> Quaternion<TValue>::Slerp(const Quaternion<TValue>& a_, const Quaternion<TValue>& b_, float t_) noexcept
		requires std::same_as<TValue, float>
	{
		return SlerpUnclamped(a_, b_, std::clamp(t_, 0.0f, 1.0f));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Quaternion<TValue> Quaternion<TValue>::SlerpUnclamped(const Quaternion<TValue>& a_, const Quaternion<TValue>& b_, float t_) noexcept
		requires std::same_as<TValue, float>
	{
		Quaternion<TValue> result;
		Store(result, DirectX::XMQuaternionSlerp(Load(a_), Load(b_), t_));
		return result;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Quaternion<TValue> Quaternion<TValue>::FromToRotation(const Vector3D<float>& fromDirection_, const Vector3D<float>& toDirection_) noexcept
		requires std::same_as<TValue, float>
	{
		Vector3D<float> from{ Vector3D<float>::Normalize(fromDirection_) };
		Vector3D<float> to{ Vector3D<float>::Normalize(toDirection_) };

		const float dot{ std::clamp(Vector3D<float>::Dot(from, to), -1.0f, 1.0f) };

		if (dot > 1.0f - std::numeric_limits<float>::epsilon())
		{
			return GetIdentity();
		}

		if (dot < -1.0f + std::numeric_limits<float>::epsilon())
		{
			Vector3D<float> axis{ Vector3D<float>::Cross(Vector3D<float>::GetRight(), from) };
			if (axis.GetSqrMagnitude() <= std::numeric_limits<float>::epsilon())
			{
				axis = Vector3D<float>::Cross(Vector3D<float>::GetUp(), from);
			}

			return AngleAxis(180.0f, axis);
		}

		Vector3D<float> axis{ Vector3D<float>::Cross(from, to) };
		const float angleDegrees{ std::acos(dot) * (180.0f / std::numbers::pi_v<float>) };
		return AngleAxis(angleDegrees, axis);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Quaternion<TValue> Quaternion<TValue>::LookRotation(const Vector3D<float>& forward_, const Vector3D<float>& up_) noexcept
		requires std::same_as<TValue, float>
	{
		Vector3D<float> forward{ Vector3D<float>::Normalize(forward_) };
		if (forward.GetSqrMagnitude() <= std::numeric_limits<float>::epsilon())
		{
			return GetIdentity();
		}

		Vector3D<float> right{ Vector3D<float>::Normalize(Vector3D<float>::Cross(up_, forward)) };
		if (right.GetSqrMagnitude() <= std::numeric_limits<float>::epsilon())
		{
			right = Vector3D<float>::Normalize(Vector3D<float>::Cross(Vector3D<float>::GetUp(), forward));
			if (right.GetSqrMagnitude() <= std::numeric_limits<float>::epsilon())
			{
				right = Vector3D<float>::Normalize(Vector3D<float>::Cross(Vector3D<float>::GetRight(), forward));
			}
		}

		Vector3D<float> up{ Vector3D<float>::Cross(forward, right) };

		DirectX::XMMATRIX basis{ DirectX::XMMatrixIdentity() };
		basis.r[0] = Vector3D<float>::Load(right);
		basis.r[1] = Vector3D<float>::Load(up);
		basis.r[2] = Vector3D<float>::Load(forward);

		Quaternion<TValue> result;
		Store(result, DirectX::XMQuaternionRotationMatrix(basis));
		return result;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Quaternion<TValue> Quaternion<TValue>::RotateTowards(const Quaternion<TValue>& from_, const Quaternion<TValue>& to_, float maxDegreesDelta_) noexcept
		requires std::same_as<TValue, float>
	{
		const float angleDegrees{ Angle(from_, to_) };
		if (angleDegrees <= std::numeric_limits<float>::epsilon())
		{
			return to_;
		}

		const float t{ std::min(1.0f, maxDegreesDelta_ / angleDegrees) };
		return SlerpUnclamped(from_, to_, t);
	}

	template class Quaternion<int>;
	template class Quaternion<float>;
}
