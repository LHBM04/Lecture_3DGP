#include "Precompiled.h"
#include "Vector2D.h"

#include "MathF.h"
#include "Vector3D.h"
#include "Vector4D.h"

Vector2D::Vector2D() noexcept
	: DirectX::XMFLOAT2(0.0f, 0.0f)
{
}

Vector2D::Vector2D(float value_) noexcept
	: DirectX::XMFLOAT2(value_, value_)
{
}

Vector2D::Vector2D(float x_, float y_) noexcept
	: DirectX::XMFLOAT2(x_, y_)
{
}

Vector2D::Vector2D(Vector2D&& other_) noexcept
	: DirectX::XMFLOAT2(other_)
{
}

Vector2D::Vector2D(const Vector2D& other_) noexcept
	: DirectX::XMFLOAT2(other_)
{
}

Vector2D Vector2D::operator+(const Vector2D& _other) const noexcept
{
	Vector2D result;
	Store(result, DirectX::XMVectorAdd(Load(*this), Load(_other)));
	return result;
}

Vector2D& Vector2D::operator+=(const Vector2D& _other) noexcept
{
	Store(*this, DirectX::XMVectorAdd(Load(*this), Load(_other)));
	return *this;
}

Vector2D Vector2D::operator-(const Vector2D& _other) const noexcept
{
	Vector2D result;
	Store(result, DirectX::XMVectorSubtract(Load(*this), Load(_other)));
	return result;
}

Vector2D& Vector2D::operator-=(const Vector2D& _other) noexcept
{
	Store(*this, DirectX::XMVectorSubtract(Load(*this), Load(_other)));
	return *this;
}

Vector2D Vector2D::operator*(float _scalar) const noexcept
{
	Vector2D result;
	Store(result, DirectX::XMVectorScale(Load(*this), _scalar));
	return result;
}

Vector2D& Vector2D::operator*=(float _scalar) noexcept
{
	Store(*this, DirectX::XMVectorScale(Load(*this), _scalar));
	return *this;
}

Vector2D Vector2D::operator/(float _scalar) const noexcept
{
	Vector2D result;
	Store(result, DirectX::XMVectorScale(Load(*this), 1.0f / _scalar));
	return result;
}

Vector2D& Vector2D::operator/=(float _scalar) noexcept
{
	Store(*this, DirectX::XMVectorScale(Load(*this), 1.0f / _scalar));
	return *this;
}

Vector2D& Vector2D::operator=(const Vector2D& _other) noexcept
{
	x = _other.x;
	y = _other.y;
	return *this;
}

Vector2D& Vector2D::operator=(Vector2D&& _other) noexcept
{
	x = _other.x;
	y = _other.y;
	return *this;
}

bool Vector2D::operator==(const Vector2D& other_) const noexcept
{
	return (*this <=> other_) == std::partial_ordering::equivalent;
}

bool Vector2D::operator!=(const Vector2D& other_) const noexcept
{
	return !(*this == other_);
}

std::partial_ordering Vector2D::operator<=>(const Vector2D& other_) const noexcept
{
	if (const std::partial_ordering order{ x <=> other_.x }; order != 0)
	{
		return order;
	}
	return y <=> other_.y;
}

float Vector2D::operator[](size_t index) const noexcept
{
	return (&x)[index];
}

float& Vector2D::operator[](size_t index) noexcept
{
	return (&x)[index];
}

float Vector2D::GetMagnitude() const noexcept
{
	return DirectX::XMVectorGetX(DirectX::XMVector2Length(Load(*this)));
}

float Vector2D::GetSqrMagnitude() const noexcept
{
	return Dot(*this, *this);
}

Vector2D Vector2D::GetNormalized() const noexcept
{
	return Normalize(*this);
}

void Vector2D::Normalize() noexcept
{
	*this = GetNormalized();
}

void Vector2D::Set(float x_, float y_) noexcept
{
	x = x_;
	y = y_;
}

bool Vector2D::IsZero(float epsilon_) const noexcept
{
	return std::abs(x) <= epsilon_ && std::abs(y) <= epsilon_;
}

bool Vector2D::IsFinite() const noexcept
{
	return std::isfinite(x) && std::isfinite(y);
}

Vector2D Vector2D::GetZero() noexcept
{
	return Vector2D(0.0f, 0.0f);
}

Vector2D Vector2D::GetOne() noexcept
{
	return Vector2D(1.0f, 1.0f);
}

Vector2D Vector2D::GetUp() noexcept
{
	return Vector2D(0.0f, 1.0f);
}

Vector2D Vector2D::GetDown() noexcept
{
	return Vector2D(0.0f, -1.0f);
}

Vector2D Vector2D::GetLeft() noexcept
{
	return Vector2D(-1.0f, 0.0f);
}

Vector2D Vector2D::GetRight() noexcept
{
	return Vector2D(1.0f, 0.0f);
}

Vector2D Vector2D::GetPositiveInfinity() noexcept
{
	const float infinity{ std::numeric_limits<float>::infinity() };
	return Vector2D(infinity, infinity);
}

Vector2D Vector2D::GetNegativeInfinity() noexcept
{
	const float negativeInfinity{ -std::numeric_limits<float>::infinity() };
	return Vector2D(negativeInfinity, negativeInfinity);
}

DirectX::XMVECTOR Vector2D::Load(const Vector2D& vec_) noexcept
{
	return DirectX::XMLoadFloat2(&vec_);
}

void Vector2D::Store(Vector2D& d_, DirectX::XMVECTOR s_) noexcept
{
	DirectX::XMStoreFloat2(&d_, s_);
}

bool Vector2D::IsApproximately(const Vector2D& lhs_, const Vector2D& rhs_, float epsilon_) noexcept
{
	return std::abs(lhs_.x - rhs_.x) <= epsilon_ && std::abs(lhs_.y - rhs_.y) <= epsilon_;
}

Vector2D Vector2D::Max(const Vector2D& a_, const Vector2D& b_) noexcept
{
	return Vector2D(std::max(a_.x, b_.x), std::max(a_.y, b_.y));
}

Vector2D Vector2D::Min(const Vector2D& a_, const Vector2D& b_) noexcept
{
	return Vector2D(std::min(a_.x, b_.x), std::min(a_.y, b_.y));
}

float Vector2D::Angle(const Vector2D& a_, const Vector2D& b_) noexcept
{
	const Vector2D from{ Normalize(a_) };
	const Vector2D to{ Normalize(b_) };
	const float dot{ std::clamp(Dot(from, to), -1.0f, 1.0f) };
	return std::acos(dot);
}

float Vector2D::SignedAngle(const Vector2D& from_, const Vector2D& to_) noexcept
{
	const float angle{ Angle(from_, to_) };
	const float det{ from_.x * to_.y - from_.y * to_.x };
	return (det >= 0.0f) ? angle : -angle;
}

Vector2D Vector2D::Clamp(const Vector2D& value_, const Vector2D& min_, const Vector2D& max_) noexcept
{
	return Vector2D(
		Mathf::Clamp(value_.x, min_.x, max_.x),
		Mathf::Clamp(value_.y, min_.y, max_.y)
	);
}

Vector2D Vector2D::ClampMagnitude(const Vector2D& vector_, float maxLength_) noexcept
{
	const float sqrMagnitude{ vector_.GetSqrMagnitude() };
	const float maxSqr{ maxLength_ * maxLength_ };
	if (sqrMagnitude <= maxSqr)
	{
		return vector_;
	}

	return Normalize(vector_) * maxLength_;
}

float Vector2D::Distance(const Vector2D& a_, const Vector2D& b_) noexcept
{
	return (a_ - b_).GetMagnitude();
}

Vector2D Vector2D::Scale(const Vector2D& vector_, const Vector2D& scale_) noexcept
{
	return Vector2D(vector_.x * scale_.x, vector_.y * scale_.y);
}

Vector2D Vector2D::Normalize(const Vector2D& value_) noexcept
{
	Vector2D result;
	Store(result, DirectX::XMVector2Normalize(Load(value_)));
	return result;
}

Vector2D Vector2D::Perpendicular(const Vector2D& vector_) noexcept
{
	return Vector2D(-vector_.y, vector_.x);
}

float Vector2D::Dot(const Vector2D& a_, const Vector2D& b_) noexcept
{
	return DirectX::XMVectorGetX(DirectX::XMVector2Dot(Load(a_), Load(b_)));
}

Vector2D Vector2D::Lerp(const Vector2D& a_, const Vector2D& b_, float t_) noexcept
{
	Vector2D result;
	Store(result, DirectX::XMVectorLerp(Load(a_), Load(b_), Mathf::Clamp(t_, 0.0f, 1.0f)));
	return result;
}

Vector2D Vector2D::LerpUnclamped(const Vector2D& a_, const Vector2D& b_, float t_) noexcept
{
	Vector2D result;
	Store(result, DirectX::XMVectorLerp(Load(a_), Load(b_), t_));
	return result;
}

Vector2D Vector2D::Slerp(const Vector2D& a_, const Vector2D& b_, float t_) noexcept
{
	return SlerpUnclamped(a_, b_, Mathf::Clamp(t_, 0.0f, 1.0f));
}

Vector2D Vector2D::SlerpUnclamped(const Vector2D& a_, const Vector2D& b_, float t_) noexcept
{
	const float aMag{ a_.GetMagnitude() };
	const float bMag{ b_.GetMagnitude() };

	if (aMag <= Mathf::Epsilon || bMag <= Mathf::Epsilon)
	{
		return LerpUnclamped(a_, b_, t_);
	}

	const Vector2D from{ a_ / aMag };
	const Vector2D to{ b_ / bMag };

	float dot{ std::clamp(Dot(from, to), -1.0f, 1.0f) };
	const float theta{ std::acos(dot) * t_ };

	Vector2D relative{ to - from * dot };
	const float relativeMag{ relative.GetMagnitude() };
	if (relativeMag <= Mathf::Epsilon)
	{
		return LerpUnclamped(a_, b_, t_);
	}

	relative /= relativeMag;
	const Vector2D direction{ from * std::cos(theta) + relative * std::sin(theta) };
	const float magnitude{ Mathf::Lerp(aMag, bMag, t_) };

	return direction * magnitude;
}

Vector2D Vector2D::Cross(const Vector2D& a_, const Vector2D& b_) noexcept
{
	const float z{ a_.x * b_.y - a_.y * b_.x };
	return Vector2D(0.0f, z);
}

Vector2D Vector2D::Reflect(const Vector2D& vector_, const Vector2D& normal_) noexcept
{
	Vector2D result;
	Store(result, DirectX::XMVector2Reflect(Load(vector_), Load(normal_)));
	return result;
}

Vector2D Vector2D::MoveTowards(const Vector2D& current_, const Vector2D& target_, float maxDistanceDelta_) noexcept
{
	const Vector2D delta{ target_ - current_ };
	const float distance{ delta.GetMagnitude() };

	if (distance <= maxDistanceDelta_ || distance <= Mathf::Epsilon)
	{
		return target_;
	}

	return current_ + (delta / distance) * maxDistanceDelta_;
}

Vector2D Vector2D::SmoothDamp(
	const Vector2D& current_,
	const Vector2D& target_,
	Vector2D& currentVelocity_,
	float smoothTime_,
	float maxSpeed_,
	float deltaTime_) noexcept
{
	smoothTime_ = std::max(0.0001f, smoothTime_);
	const float omega{ 2.0f / smoothTime_ };
	const float x{ omega * deltaTime_ };
	const float exp{ 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x) };

	Vector2D change{ current_ - target_ };
	const Vector2D originalTarget{ target_ };

	const float maxChange{ maxSpeed_ * smoothTime_ };
	change = ClampMagnitude(change, maxChange);
	const Vector2D target{ current_ - change };

	const Vector2D temp{ (currentVelocity_ + change * omega) * deltaTime_ };
	currentVelocity_ = (currentVelocity_ - temp * omega) * exp;
	Vector2D output{ target + (change + temp) * exp };

	if (Dot(originalTarget - current_, output - originalTarget) > 0.0f)
	{
		output = originalTarget;
		currentVelocity_ = Vector2D(0.0f, 0.0f);
	}

	return output;
}

Vector2D::Vector2D(const Vector3D& vector_) noexcept
    : DirectX::XMFLOAT2(vector_.x, vector_.y)
{
}

Vector2D::Vector2D(const Vector4D& vector_) noexcept
    : DirectX::XMFLOAT2(vector_.x, vector_.y)
{
}

Vector2D::operator Vector3D() const noexcept
{
    return Vector3D(x, y, 0.0f);
}

Vector2D::operator Vector4D() const noexcept
{
    return Vector4D(x, y, 0.0f, 0.0f);
}
