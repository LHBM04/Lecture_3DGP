#include "Precompiled.hpp"
#include "Quaternion.hpp"

#include "Math.hpp"
#include "Matrix4x4.hpp"
#include "Vector3D.hpp"

Quaternion::Quaternion()
	: XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)
{
}

Quaternion::Quaternion(float value_)
	: XMFLOAT4(value_, value_, value_, value_)
{
}

Quaternion::Quaternion(float x_, float y_, float z_, float w_)
	: XMFLOAT4(x_, y_, z_, w_)
{
}

Quaternion::Quaternion(const Quaternion& other_)
	: XMFLOAT4(other_)
{
}

Quaternion::Quaternion(Quaternion&& other_)
	: XMFLOAT4(other_)
{
}

Quaternion Quaternion::operator+(const Quaternion& _other) const
{
	return Quaternion(x + _other.x, y + _other.y, z + _other.z, w + _other.w);
}

Quaternion& Quaternion::operator+=(const Quaternion& _other)
{
	x += _other.x;
	y += _other.y;
	z += _other.z;
	w += _other.w;
	return *this;
}

Quaternion Quaternion::operator-(const Quaternion& _other) const
{
	return Quaternion(x - _other.x, y - _other.y, z - _other.z, w - _other.w);
}

Quaternion& Quaternion::operator-=(const Quaternion& _other)
{
	x -= _other.x;
	y -= _other.y;
	z -= _other.z;
	w -= _other.w;
	return *this;
}

Quaternion Quaternion::operator*(float _scalar) const
{
	return Quaternion(x * _scalar, y * _scalar, z * _scalar, w * _scalar);
}

Quaternion& Quaternion::operator*=(float _scalar)
{
	x *= _scalar;
	y *= _scalar;
	z *= _scalar;
	w *= _scalar;
	return *this;
}

Quaternion Quaternion::operator*(const Quaternion& _other) const
{
	Quaternion result;
	Store(result, DirectX::XMQuaternionMultiply(Load(*this), Load(_other)));
	return result;
}

Quaternion& Quaternion::operator*=(const Quaternion& _other)
{
	Store(*this, DirectX::XMQuaternionMultiply(Load(*this), Load(_other)));
	return *this;
}

Vector3D Quaternion::operator*(const Vector3D& vector_) const
{
	Vector3D result;
	Vector3D::Store(result, DirectX::XMVector3Rotate(Vector3D::Load(vector_), Load(*this)));
	return result;
}

Quaternion Quaternion::operator/(float _scalar) const
{
	return Quaternion(x / _scalar, y / _scalar, z / _scalar, w / _scalar);
}

Quaternion& Quaternion::operator/=(float _scalar)
{
	x /= _scalar;
	y /= _scalar;
	z /= _scalar;
	w /= _scalar;
	return *this;
}

void Quaternion::Normalize()
{
	*this = GetNormalized();
}

void Quaternion::Set(float x_, float y_, float z_, float w_)
{
	x = x_;
	y = y_;
	z = z_;
	w = w_;
}

void Quaternion::SetEulerAngles(const Vector3D& eulerDegrees_)
{
	*this = Euler(eulerDegrees_);
}

Quaternion& Quaternion::operator=(const Quaternion& _other)
{
	x = _other.x;
	y = _other.y;
	z = _other.z;
	w = _other.w;
	return *this;
}

Quaternion& Quaternion::operator=(Quaternion&& _other)
{
	x = _other.x;
	y = _other.y;
	z = _other.z;
	w = _other.w;
	return *this;
}

bool Quaternion::operator==(const Quaternion& other_) const
{
	return (*this <=> other_) == std::partial_ordering::equivalent;
}

bool Quaternion::operator!=(const Quaternion& other_) const
{
	return !(*this == other_);
}

std::partial_ordering Quaternion::operator<=>(const Quaternion& other_) const
{
	if (const auto order{ x <=> other_.x }; order != 0)
	{
		return order;
	}

	if (const auto order{ y <=> other_.y }; order != 0)
	{
		return order;
	}

	if (const auto order{ z <=> other_.z }; order != 0)
	{
		return order;
	}

	return w <=> other_.w;
}

float Quaternion::operator[](size_t index) const
{
	return (&x)[index];
}

float& Quaternion::operator[](size_t index)
{
	return (&x)[index];
}

Vector3D Quaternion::GetEulerAngles() const
{
	Quaternion result;
	Store(result, Load(*this));

	const float qx{ result.x };
	const float qy{ result.y };
	const float qz{ result.z };
	const float qw{ result.w };

	Vector3D eulerAngles;

	const float sinr_cosp{ 2.0f * (qw * qx + qy * qz) };
	const float cosr_cosp{ 1.0f - 2.0f * (qx * qx + qy * qy) };
	eulerAngles.x = std::atan2(sinr_cosp, cosr_cosp);

	const float sinp{ 2.0f * (qw * qy - qz * qx) };
	eulerAngles.y = (std::abs(sinp) >= 1.0f) ? std::copysign(Mathf::PI / 2.0f, sinp) : std::asin(sinp);

	const float siny_cosp{ 2.0f * (qw * qz + qx * qy) };
	const float cosy_cosp{ 1.0f - 2.0f * (qy * qy + qz * qz) };
	eulerAngles.z = std::atan2(siny_cosp, cosy_cosp);

	return eulerAngles * Mathf::RAD2DEG;
}

float Quaternion::GetMagnitude() const
{
	return DirectX::XMVectorGetX(DirectX::XMVector4Length(Load(*this)));
}

float Quaternion::GetSqrMagnitude() const
{
	return Dot(*this, *this);
}

Quaternion Quaternion::GetNormalized() const
{
	return Normalize(*this);
}

void Quaternion::ToAngleAxis(float& angleDegrees_, Vector3D& axis_) const
{
	Vector axis;
	float angleRadians;
	DirectX::XMQuaternionToAxisAngle(&axis, &angleRadians, Load(*this));

	Vector3D::Store(axis_, axis);
	angleDegrees_ = angleRadians * Mathf::RAD2DEG;
}

bool Quaternion::IsNormalized(float epsilon_) const
{
	const float lenSqr{ GetSqrMagnitude() };
	return std::abs(lenSqr - 1.0f) <= epsilon_;
}

bool Quaternion::IsFinite() const
{
	return std::isfinite(x) && std::isfinite(y) && std::isfinite(z) && std::isfinite(w);
}

void Quaternion::SetFromToRotation(const Vector3D& from_, const Vector3D& to_)
{
	Vector3D from{ Vector3D::Normalize(from_) };
	Vector3D to{ Vector3D::Normalize(to_) };

	const float dot{ std::clamp(Vector3D::Dot(from, to), -1.0f, 1.0f) };

	if (dot > 1.0f - Mathf::EPSILON)
	{
		*this = GetIdentity();
		return;
	}

	if (dot < -1.0f + Mathf::EPSILON)
	{
		Vector3D axis{ Vector3D::Cross(Vector3D::GetRight(), from) };
		if (axis.GetSqrMagnitude() <= Mathf::EPSILON)
		{
			axis = Vector3D::Cross(Vector3D::GetUp(), from);
		}

		*this = AngleAxis(180.0f, axis);
		return;
	}

	Vector3D axis{ Vector3D::Cross(from, to) };
	const float angleDegrees{ std::acos(dot) * Mathf::RAD2DEG };
	*this = AngleAxis(angleDegrees, axis);
}

void Quaternion::SetLookRotation(const Vector3D& view_)
{
	*this = LookRotation(view_, Vector3D::GetUp());
}

void Quaternion::SetLookRotation(const Vector3D& view_, const Vector3D& up_)
{
	*this = LookRotation(view_, up_);
}

Quaternion Quaternion::GetIdentity()
{
	Quaternion result;
	Store(result, DirectX::XMQuaternionIdentity());
	return result;
}

Vector Quaternion::Load(const Quaternion& quat_)
{
	return DirectX::XMLoadFloat4(&quat_);
}

void Quaternion::Store(Quaternion& d_, Vector s_)
{
	DirectX::XMStoreFloat4(&d_, s_);
}

float Quaternion::Angle(const Quaternion& a_, const Quaternion& b_)
{
	const float d{ std::clamp(std::abs(Dot(a_, b_)), 0.0f, 1.0f) };
	return (2.0f * std::acos(d)) * Mathf::RAD2DEG;
}

Quaternion Quaternion::AngleAxis(float angleDegrees_, Vector3D axis_)
{
	Vector3D normalizedAxis{ Vector3D::Normalize(axis_) };

	Quaternion result;
	Store(result, DirectX::XMQuaternionRotationAxis(DirectX::XMLoadFloat3(&normalizedAxis), angleDegrees_ * Mathf::DEG2RAD));
	return result;
}

Quaternion Quaternion::Euler(float xDegrees_, float yDegrees_, float zDegrees_)
{
	Quaternion result;
	Store(result, DirectX::XMQuaternionRotationRollPitchYaw(xDegrees_ * Mathf::DEG2RAD, yDegrees_ * Mathf::DEG2RAD, zDegrees_ * Mathf::DEG2RAD));
	return result;
}

Quaternion Quaternion::Euler(const Vector3D& eulerDegrees_)
{
	return Euler(eulerDegrees_.x, eulerDegrees_.y, eulerDegrees_.z);
}

Quaternion Quaternion::Inverse(const Quaternion& rotation_)
{
	Quaternion result;
	Store(result, DirectX::XMQuaternionInverse(Load(rotation_)));
	return result;
}

Quaternion Quaternion::Normalize(const Quaternion& rotation_)
{
	Quaternion result;
	Store(result, DirectX::XMQuaternionNormalize(Load(rotation_)));
	return result;
}

Quaternion Quaternion::Conjugate(const Quaternion& rotation_)
{
	Quaternion result;
	Store(result, DirectX::XMQuaternionConjugate(Load(rotation_)));
	return result;
}

float Quaternion::Dot(const Quaternion& a_, const Quaternion& b_)
{
	return DirectX::XMVectorGetX(DirectX::XMVector4Dot(Load(a_), Load(b_)));
}

bool Quaternion::IsApproximately(const Quaternion& lhs_, const Quaternion& rhs_, float epsilon_)
{
	return std::abs(Dot(lhs_, rhs_)) >= (1.0f - epsilon_);
}

Quaternion Quaternion::Lerp(const Quaternion& a_, const Quaternion& b_, float t_)
{
	return LerpUnclamped(a_, b_, Mathf::Clamp(t_, 0.0f, 1.0f));
}

Quaternion Quaternion::LerpUnclamped(const Quaternion& a_, const Quaternion& b_, float t_)
{
	Quaternion result;
	Store(result, DirectX::XMQuaternionNormalize(DirectX::XMVectorLerp(Load(a_), Load(b_), t_)));
	return result;
}

Quaternion Quaternion::Slerp(const Quaternion& a_, const Quaternion& b_, float t_)
{
	return SlerpUnclamped(a_, b_, Mathf::Clamp(t_, 0.0f, 1.0f));
}

Quaternion Quaternion::SlerpUnclamped(const Quaternion& a_, const Quaternion& b_, float t_)
{
	Quaternion result;
	Store(result, DirectX::XMQuaternionSlerp(Load(a_), Load(b_), t_));
	return result;
}

Quaternion Quaternion::FromToRotation(const Vector3D& fromDirection_, const Vector3D& toDirection_)
{
	Vector3D from{ Vector3D::Normalize(fromDirection_) };
	Vector3D to{ Vector3D::Normalize(toDirection_) };

	const float dot{ std::clamp(Vector3D::Dot(from, to), -1.0f, 1.0f) };

	if (dot > 1.0f - Mathf::EPSILON)
	{
		return GetIdentity();
	}

	if (dot < -1.0f + Mathf::EPSILON)
	{
		Vector3D axis{ Vector3D::Cross(Vector3D::GetRight(), from) };
		if (axis.GetSqrMagnitude() <= Mathf::EPSILON)
		{
			axis = Vector3D::Cross(Vector3D::GetUp(), from);
		}

		return AngleAxis(180.0f, axis);
	}

	Vector3D axis{ Vector3D::Cross(from, to) };
	const float angleDegrees{ std::acos(dot) * Mathf::RAD2DEG };
	return AngleAxis(angleDegrees, axis);
}

Quaternion Quaternion::LookRotation(const Vector3D& forward_, const Vector3D& up_ = Vector3D::GetUp())
{
	Vector3D forward{ Vector3D::Normalize(forward_) };
	if (forward.GetSqrMagnitude() <= Mathf::EPSILON)
	{
		return GetIdentity();
	}

	Vector3D right{ Vector3D::Normalize(Vector3D::Cross(up_, forward)) };
	if (right.GetSqrMagnitude() <= Mathf::EPSILON)
	{
		right = Vector3D::Normalize(Vector3D::Cross(Vector3D::GetUp(), forward));
		if (right.GetSqrMagnitude() <= Mathf::EPSILON)
		{
			right = Vector3D::Normalize(Vector3D::Cross(Vector3D::GetRight(), forward));
		}
	}

	Vector3D up{ Vector3D::Cross(forward, right) };

	Matrix basis = DirectX::XMMatrixIdentity();
	basis.r[0] = Vector3D::Load(right);
	basis.r[1] = Vector3D::Load(up);
	basis.r[2] = Vector3D::Load(forward);

	Quaternion result;
	Store(result, DirectX::XMQuaternionRotationMatrix(basis));
	return result;
}

Quaternion Quaternion::RotateTowards(const Quaternion& from_, const Quaternion& to_, float maxDegreesDelta_)
{
	const float angleDegrees{ Angle(from_, to_) };
	if (angleDegrees <= Mathf::EPSILON)
	{
		return to_;
	}

	const float t{ std::min(1.0f, maxDegreesDelta_ / angleDegrees) };
	return SlerpUnclamped(from_, to_, t);
}
