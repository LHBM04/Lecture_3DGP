#include "Precompiled.h"
#include "Quaternion.h"

#include "MathF.h"
#include "Matrix4x4.h"
#include "Vector3D.h"

Quaternion::Quaternion() noexcept
	: DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)
{
}

Quaternion::Quaternion(float value_) noexcept
	: DirectX::XMFLOAT4(value_, value_, value_, value_)
{
}

Quaternion::Quaternion(float x_, float y_, float z_, float w_) noexcept
	: DirectX::XMFLOAT4(x_, y_, z_, w_)
{
}

Quaternion::Quaternion(const Quaternion& other_) noexcept
	: DirectX::XMFLOAT4(other_)
{
}

Quaternion::Quaternion(Quaternion&& other_) noexcept
	: DirectX::XMFLOAT4(other_)
{
}

Quaternion Quaternion::operator+(const Quaternion& _other) const noexcept
{
	return Quaternion(x + _other.x, y + _other.y, z + _other.z, w + _other.w);
}

Quaternion& Quaternion::operator+=(const Quaternion& _other) noexcept
{
	x += _other.x;
	y += _other.y;
	z += _other.z;
	w += _other.w;
	return *this;
}

Quaternion Quaternion::operator-(const Quaternion& _other) const noexcept
{
	return Quaternion(x - _other.x, y - _other.y, z - _other.z, w - _other.w);
}

Quaternion& Quaternion::operator-=(const Quaternion& _other) noexcept
{
	x -= _other.x;
	y -= _other.y;
	z -= _other.z;
	w -= _other.w;
	return *this;
}

Quaternion Quaternion::operator*(float _scalar) const noexcept
{
	return Quaternion(x * _scalar, y * _scalar, z * _scalar, w * _scalar);
}

Quaternion& Quaternion::operator*=(float _scalar) noexcept
{
	x *= _scalar;
	y *= _scalar;
	z *= _scalar;
	w *= _scalar;
	return *this;
}

Quaternion Quaternion::operator*(const Quaternion& _other) const noexcept
{
	Quaternion result;
	Store(result, DirectX::XMQuaternionMultiply(Load(*this), Load(_other)));
	return result;
}

Quaternion& Quaternion::operator*=(const Quaternion& _other) noexcept
{
	Store(*this, DirectX::XMQuaternionMultiply(Load(*this), Load(_other)));
	return *this;
}

Vector3D Quaternion::operator*(const Vector3D& vector_) const noexcept
{
	Vector3D result;
	Vector3D::Store(result, DirectX::XMVector3Rotate(Vector3D::Load(vector_), Load(*this)));
	return result;
}

Quaternion Quaternion::operator/(float _scalar) const noexcept
{
	return Quaternion(x / _scalar, y / _scalar, z / _scalar, w / _scalar);
}

Quaternion& Quaternion::operator/=(float _scalar) noexcept
{
	x /= _scalar;
	y /= _scalar;
	z /= _scalar;
	w /= _scalar;
	return *this;
}

void Quaternion::Normalize() noexcept
{
	*this = GetNormalized();
}

void Quaternion::Set(float x_, float y_, float z_, float w_) noexcept
{
	x = x_;
	y = y_;
	z = z_;
	w = w_;
}

void Quaternion::SetEulerAngles(const Vector3D& eulerDegrees_) noexcept
{
	*this = Euler(eulerDegrees_);
}

Quaternion& Quaternion::operator=(const Quaternion& _other) noexcept
{
	x = _other.x;
	y = _other.y;
	z = _other.z;
	w = _other.w;
	return *this;
}

Quaternion& Quaternion::operator=(Quaternion&& _other) noexcept
{
	x = _other.x;
	y = _other.y;
	z = _other.z;
	w = _other.w;
	return *this;
}

bool Quaternion::operator==(const Quaternion& other_) const noexcept
{
	return (*this <=> other_) == std::partial_ordering::equivalent;
}

bool Quaternion::operator!=(const Quaternion& other_) const noexcept
{
	return !(*this == other_);
}

std::partial_ordering Quaternion::operator<=>(const Quaternion& other_) const noexcept
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

float Quaternion::operator[](size_t index) const noexcept
{
	return (&x)[index];
}

float& Quaternion::operator[](size_t index) noexcept
{
	return (&x)[index];
}

Vector3D Quaternion::GetEulerAngles() const noexcept
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
	eulerAngles.y = (std::abs(sinp) >= 1.0f) ? std::copysign(Mathf::Pi / 2.0f, sinp) : std::asin(sinp);

	const float siny_cosp{ 2.0f * (qw * qz + qx * qy) };
	const float cosy_cosp{ 1.0f - 2.0f * (qy * qy + qz * qz) };
	eulerAngles.z = std::atan2(siny_cosp, cosy_cosp);

	return eulerAngles * Mathf::Rad2Deg;
}

float Quaternion::GetMagnitude() const noexcept
{
	return DirectX::XMVectorGetX(DirectX::XMVector4Length(Load(*this)));
}

float Quaternion::GetSqrMagnitude() const noexcept
{
	return Dot(*this, *this);
}

Quaternion Quaternion::GetNormalized() const noexcept
{
	return Normalize(*this);
}

void Quaternion::ToAngleAxis(float& angleDegrees_, Vector3D& axis_) const noexcept
{
	DirectX::XMVECTOR axis;
	float angleRadians;
	DirectX::XMQuaternionToAxisAngle(&axis, &angleRadians, Load(*this));

	Vector3D::Store(axis_, axis);
	angleDegrees_ = angleRadians * Mathf::Rad2Deg;
}

bool Quaternion::IsNormalized(float epsilon_) const noexcept
{
	const float lenSqr{ GetSqrMagnitude() };
	return std::abs(lenSqr - 1.0f) <= epsilon_;
}

bool Quaternion::IsFinite() const noexcept
{
	return std::isfinite(x) && std::isfinite(y) && std::isfinite(z) && std::isfinite(w);
}

void Quaternion::SetFromToRotation(const Vector3D& from_, const Vector3D& to_) noexcept
{
	Vector3D from{ Vector3D::Normalize(from_) };
	Vector3D to{ Vector3D::Normalize(to_) };

	const float dot{ std::clamp(Vector3D::Dot(from, to), -1.0f, 1.0f) };

	if (dot > 1.0f - Mathf::Epsilon)
	{
		*this = GetIdentity();
		return;
	}

	if (dot < -1.0f + Mathf::Epsilon)
	{
		Vector3D axis{ Vector3D::Cross(Vector3D::GetRight(), from) };
		if (axis.GetSqrMagnitude() <= Mathf::Epsilon)
		{
			axis = Vector3D::Cross(Vector3D::GetUp(), from);
		}

		*this = AngleAxis(180.0f, axis);
		return;
	}

	Vector3D axis{ Vector3D::Cross(from, to) };
	const float angleDegrees{ std::acos(dot) * Mathf::Rad2Deg };
	*this = AngleAxis(angleDegrees, axis);
}

void Quaternion::SetLookRotation(const Vector3D& view_) noexcept
{
	*this = LookRotation(view_, Vector3D::GetUp());
}

void Quaternion::SetLookRotation(const Vector3D& view_, const Vector3D& up_) noexcept
{
	*this = LookRotation(view_, up_);
}

Quaternion Quaternion::GetIdentity() noexcept
{
	Quaternion result;
	Store(result, DirectX::XMQuaternionIdentity());
	return result;
}

DirectX::XMVECTOR Quaternion::Load(const Quaternion& quat_) noexcept
{
	return DirectX::XMLoadFloat4(&quat_);
}

void Quaternion::Store(Quaternion& d_, DirectX::XMVECTOR s_) noexcept
{
	DirectX::XMStoreFloat4(&d_, s_);
}

float Quaternion::Angle(const Quaternion& a_, const Quaternion& b_) noexcept
{
	const float d{ std::clamp(std::abs(Dot(a_, b_)), 0.0f, 1.0f) };
	return (2.0f * std::acos(d)) * Mathf::Rad2Deg;
}

Quaternion Quaternion::AngleAxis(float angleDegrees_, Vector3D axis_) noexcept
{
	Vector3D normalizedAxis{ Vector3D::Normalize(axis_) };

	Quaternion result;
	Store(result, DirectX::XMQuaternionRotationAxis(DirectX::XMLoadFloat3(&normalizedAxis), angleDegrees_ * Mathf::Deg2Rad));
	return result;
}

Quaternion Quaternion::Euler(float xDegrees_, float yDegrees_, float zDegrees_) noexcept
{
	Quaternion result;
	Store(result, DirectX::XMQuaternionRotationRollPitchYaw(xDegrees_ * Mathf::Deg2Rad, yDegrees_ * Mathf::Deg2Rad, zDegrees_ * Mathf::Deg2Rad));
	return result;
}

Quaternion Quaternion::Euler(const Vector3D& eulerDegrees_) noexcept
{
	return Euler(eulerDegrees_.x, eulerDegrees_.y, eulerDegrees_.z);
}

Quaternion Quaternion::Inverse(const Quaternion& rotation_) noexcept
{
	Quaternion result;
	Store(result, DirectX::XMQuaternionInverse(Load(rotation_)));
	return result;
}

Quaternion Quaternion::Normalize(const Quaternion& rotation_) noexcept
{
	Quaternion result;
	Store(result, DirectX::XMQuaternionNormalize(Load(rotation_)));
	return result;
}

Quaternion Quaternion::Conjugate(const Quaternion& rotation_) noexcept
{
	Quaternion result;
	Store(result, DirectX::XMQuaternionConjugate(Load(rotation_)));
	return result;
}

float Quaternion::Dot(const Quaternion& a_, const Quaternion& b_) noexcept
{
	return DirectX::XMVectorGetX(DirectX::XMVector4Dot(Load(a_), Load(b_)));
}

bool Quaternion::IsApproximately(const Quaternion& lhs_, const Quaternion& rhs_, float epsilon_) noexcept
{
	return std::abs(Dot(lhs_, rhs_)) >= (1.0f - epsilon_);
}

Quaternion Quaternion::Lerp(const Quaternion& a_, const Quaternion& b_, float t_) noexcept
{
	return LerpUnclamped(a_, b_, Mathf::Clamp(t_, 0.0f, 1.0f));
}

Quaternion Quaternion::LerpUnclamped(const Quaternion& a_, const Quaternion& b_, float t_) noexcept
{
	Quaternion result;
	Store(result, DirectX::XMQuaternionNormalize(DirectX::XMVectorLerp(Load(a_), Load(b_), t_)));
	return result;
}

Quaternion Quaternion::Slerp(const Quaternion& a_, const Quaternion& b_, float t_) noexcept
{
	return SlerpUnclamped(a_, b_, Mathf::Clamp(t_, 0.0f, 1.0f));
}

Quaternion Quaternion::SlerpUnclamped(const Quaternion& a_, const Quaternion& b_, float t_) noexcept
{
	Quaternion result;
	Store(result, DirectX::XMQuaternionSlerp(Load(a_), Load(b_), t_));
	return result;
}

Quaternion Quaternion::FromToRotation(const Vector3D& fromDirection_, const Vector3D& toDirection_) noexcept
{
	Vector3D from{ Vector3D::Normalize(fromDirection_) };
	Vector3D to{ Vector3D::Normalize(toDirection_) };

	const float dot{ std::clamp(Vector3D::Dot(from, to), -1.0f, 1.0f) };

	if (dot > 1.0f - Mathf::Epsilon)
	{
		return GetIdentity();
	}

	if (dot < -1.0f + Mathf::Epsilon)
	{
		Vector3D axis{ Vector3D::Cross(Vector3D::GetRight(), from) };
		if (axis.GetSqrMagnitude() <= Mathf::Epsilon)
		{
			axis = Vector3D::Cross(Vector3D::GetUp(), from);
		}

		return AngleAxis(180.0f, axis);
	}

	Vector3D axis{ Vector3D::Cross(from, to) };
	const float angleDegrees{ std::acos(dot) * Mathf::Rad2Deg };
	return AngleAxis(angleDegrees, axis);
}

Quaternion Quaternion::LookRotation(const Vector3D& forward_, const Vector3D& up_) noexcept
{
	Vector3D forward{ Vector3D::Normalize(forward_) };
	if (forward.GetSqrMagnitude() <= Mathf::Epsilon)
	{
		return GetIdentity();
	}

	Vector3D right{ Vector3D::Normalize(Vector3D::Cross(up_, forward)) };
	if (right.GetSqrMagnitude() <= Mathf::Epsilon)
	{
		right = Vector3D::Normalize(Vector3D::Cross(Vector3D::GetUp(), forward));
		if (right.GetSqrMagnitude() <= Mathf::Epsilon)
		{
			right = Vector3D::Normalize(Vector3D::Cross(Vector3D::GetRight(), forward));
		}
	}

	Vector3D up{ Vector3D::Cross(forward, right) };

	DirectX::XMMATRIX basis = DirectX::XMMatrixIdentity();
	basis.r[0] = Vector3D::Load(right);
	basis.r[1] = Vector3D::Load(up);
	basis.r[2] = Vector3D::Load(forward);

	Quaternion result;
	Store(result, DirectX::XMQuaternionRotationMatrix(basis));
	return result;
}

Quaternion Quaternion::RotateTowards(const Quaternion& from_, const Quaternion& to_, float maxDegreesDelta_) noexcept
{
	const float angleDegrees{ Angle(from_, to_) };
	if (angleDegrees <= Mathf::Epsilon)
	{
		return to_;
	}

	const float t{ std::min(1.0f, maxDegreesDelta_ / angleDegrees) };
	return SlerpUnclamped(from_, to_, t);
}
