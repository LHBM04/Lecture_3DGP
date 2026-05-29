#pragma once

#include "MathF.h"

struct Vector2D;
struct Vector4D;

struct Vector3D : public DirectX::XMFLOAT3
{
    Vector3D() noexcept;
    explicit Vector3D(float value_) noexcept;
    explicit Vector3D(float x_, float y_, float z_) noexcept;
	explicit Vector3D(const Vector2D& vector_, float z_ = 0.0f) noexcept;
	explicit Vector3D(const Vector4D& vector_) noexcept;

    Vector3D(const Vector3D& other_) noexcept;
    Vector3D(Vector3D&& other_) noexcept;

    [[nodiscard]] Vector3D operator+(const Vector3D& _other) const noexcept;
    Vector3D& operator+=(const Vector3D& _other) noexcept;

    [[nodiscard]] Vector3D operator-(const Vector3D& _other) const noexcept;
    Vector3D& operator-=(const Vector3D& _other) noexcept;

    [[nodiscard]] Vector3D operator*(float _scalar) const noexcept;
    Vector3D& operator*=(float _scalar) noexcept;

    [[nodiscard]] Vector3D operator/(float _scalar) const noexcept;
    Vector3D& operator/=(float _scalar) noexcept;

    Vector3D& operator=(const Vector3D& _other) noexcept;
    Vector3D& operator=(Vector3D&& _other) noexcept;

    [[nodiscard]] bool operator==(const Vector3D& other_) const noexcept;
    [[nodiscard]] bool operator!=(const Vector3D& other_) const noexcept;

    [[nodiscard]] std::partial_ordering operator<=>(const Vector3D& other_) const noexcept;

    [[nodiscard]] float operator[](size_t index) const noexcept;
    [[nodiscard]] float& operator[](size_t index) noexcept;

    [[nodiscard]] float GetMagnitude() const noexcept;
    [[nodiscard]] float GetSqrMagnitude() const noexcept;

    [[nodiscard]] Vector3D GetNormalized() const noexcept;

	void Normalize() noexcept;

	void Set(float x_, float y_, float z_) noexcept;

	[[nodiscard]] bool IsZero(float epsilon_ = Mathf::Epsilon) const noexcept;

	[[nodiscard]] bool IsFinite() const noexcept;

	[[nodiscard]] static Vector3D GetZero() noexcept;
	[[nodiscard]] static Vector3D GetOne() noexcept;
	[[nodiscard]] static Vector3D GetUp() noexcept;
	[[nodiscard]] static Vector3D GetDown() noexcept;
	[[nodiscard]] static Vector3D GetLeft() noexcept;
	[[nodiscard]] static Vector3D GetRight() noexcept;
	[[nodiscard]] static Vector3D GetForward() noexcept;
	[[nodiscard]] static Vector3D GetBack() noexcept;
	[[nodiscard]] static Vector3D GetPositiveInfinity() noexcept;
	[[nodiscard]] static Vector3D GetNegativeInfinity() noexcept;

    [[nodiscard]] static DirectX::XMVECTOR Load(const Vector3D& vec_) noexcept;
    static void Store(Vector3D& d_, DirectX::XMVECTOR s_) noexcept;

	[[nodiscard]] static bool IsApproximately(const Vector3D& lhs_, const Vector3D& rhs_, float epsilon_ = Mathf::Epsilon) noexcept;

    [[nodiscard]] static Vector3D Max(const Vector3D& a_, const Vector3D& b_) noexcept;
    [[nodiscard]] static Vector3D Min(const Vector3D& a_, const Vector3D& b_) noexcept;

    [[nodiscard]] static float Angle(const Vector3D& a_, const Vector3D& b_) noexcept;
    [[nodiscard]] static float SignedAngle(const Vector3D& from_, const Vector3D& to_, const Vector3D& axis_) noexcept;
    [[nodiscard]] static Vector3D Clamp(const Vector3D& value_, const Vector3D& min_, const Vector3D& max_) noexcept;
    [[nodiscard]] static Vector3D ClampMagnitude(const Vector3D& vector_, float maxLength_) noexcept;
    [[nodiscard]] static float Distance(const Vector3D& _v1, const Vector3D& _v2) noexcept;
    [[nodiscard]] static Vector3D Scale(const Vector3D& vector_, const Vector3D& scale_) noexcept;
    [[nodiscard]] static Vector3D Normalize(const Vector3D& value_) noexcept;

    [[nodiscard]] static Vector3D Lerp(const Vector3D& a_, const Vector3D& b_, float t_) noexcept;
    [[nodiscard]] static Vector3D LerpUnclamped(const Vector3D& a_, const Vector3D& b_, float t_) noexcept;
    [[nodiscard]] static Vector3D Slerp(const Vector3D& a_, const Vector3D& b_, float t_) noexcept;
    [[nodiscard]] static Vector3D SlerpUnclamped(const Vector3D& a_, const Vector3D& b_, float t_) noexcept;

    [[nodiscard]] static Vector3D Cross(const Vector3D& a_, const Vector3D& b_) noexcept;
    [[nodiscard]] static float Dot(const Vector3D& a_, const Vector3D& b_) noexcept;
    [[nodiscard]] static Vector3D Project(const Vector3D& vector_, const Vector3D& onNormal_) noexcept;
    [[nodiscard]] static Vector3D ProjectOnPlane(const Vector3D& vector_, const Vector3D& planeNormal_) noexcept;
    [[nodiscard]] static Vector3D Reflect(const Vector3D& _vector, const Vector3D& _normal) noexcept;
    [[nodiscard]] static Vector3D Refract(const Vector3D& _vector, const Vector3D& _normal, float _eta) noexcept;

	static void OrthoNormalize(Vector3D& normal_, Vector3D& tangent_) noexcept;

	static void OrthoNormalize(Vector3D& normal_, Vector3D& tangent_, Vector3D& binormal_) noexcept;

	[[nodiscard]] static Vector3D SmoothDamp(
	    const Vector3D& current_,
	    const Vector3D& target_,
	    Vector3D& currentVelocity_,
	    float smoothTime_,
	    float maxSpeed_,
	    float deltaTime_) noexcept;

	[[nodiscard]] operator Vector2D() const noexcept;
	[[nodiscard]] operator Vector4D() const noexcept;

    [[nodiscard]] static Vector3D MoveTowards(const Vector3D& current_, const Vector3D& target_, float maxDistanceDelta_) noexcept;
    [[nodiscard]] static Vector3D RotateTowards(const Vector3D& current_, const Vector3D& target_, float maxRadiansDelta_, float maxMagnitudeDelta_) noexcept;
};

inline Vector3D::Vector3D() noexcept
    : DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f)
{
}

inline Vector3D::Vector3D(float value_) noexcept
    : DirectX::XMFLOAT3(value_, value_, value_)
{
}

inline Vector3D::Vector3D(float x_, float y_, float z_) noexcept
    : DirectX::XMFLOAT3(x_, y_, z_)
{
}

inline Vector3D::Vector3D(const Vector3D& other_) noexcept
    : DirectX::XMFLOAT3(other_)
{
}

inline Vector3D::Vector3D(Vector3D&& other_) noexcept
    : DirectX::XMFLOAT3(other_)
{
}

inline Vector3D Vector3D::operator+(const Vector3D& _other) const noexcept
{
    Vector3D result;
    Store(result, DirectX::XMVectorAdd(Load(*this), Load(_other)));
    return result;
}

inline Vector3D& Vector3D::operator+=(const Vector3D& _other) noexcept
{
    Store(*this, DirectX::XMVectorAdd(Load(*this), Load(_other)));
    return *this;
}

inline Vector3D Vector3D::operator-(const Vector3D& _other) const noexcept
{
    Vector3D result;
    Store(result, DirectX::XMVectorSubtract(Load(*this), Load(_other)));
    return result;
}

inline Vector3D& Vector3D::operator-=(const Vector3D& _other) noexcept
{
    Store(*this, DirectX::XMVectorSubtract(Load(*this), Load(_other)));
    return *this;
}

inline Vector3D Vector3D::operator*(float _scalar) const noexcept
{
    Vector3D result;
    Store(result, DirectX::XMVectorScale(Load(*this), _scalar));
    return result;
}

inline Vector3D& Vector3D::operator*=(float _scalar) noexcept
{
    Store(*this, DirectX::XMVectorScale(Load(*this), _scalar));
    return *this;
}

inline Vector3D Vector3D::operator/(float _scalar) const noexcept
{
    Vector3D result;
    Store(result, DirectX::XMVectorScale(Load(*this), 1.0f / _scalar));
    return result;
}

inline Vector3D& Vector3D::operator/=(float _scalar) noexcept
{
    Store(*this, DirectX::XMVectorScale(Load(*this), 1.0f / _scalar));
    return *this;
}

inline Vector3D& Vector3D::operator=(const Vector3D& _other) noexcept
{
    x = _other.x;
    y = _other.y;
    z = _other.z;
    return *this;
}

inline Vector3D& Vector3D::operator=(Vector3D&& _other) noexcept
{
    x = _other.x;
    y = _other.y;
    z = _other.z;
    return *this;
}

inline bool Vector3D::operator==(const Vector3D& other_) const noexcept
{
    return (*this <=> other_) == std::partial_ordering::equivalent;
}

inline bool Vector3D::operator!=(const Vector3D& other_) const noexcept
{
    return !(*this == other_);
}

inline std::partial_ordering Vector3D::operator<=>(const Vector3D& other_) const noexcept
{
    if (const auto order{ x <=> other_.x }; order != 0)
    {
        return order;
    }

    if (const auto order{ y <=> other_.y }; order != 0)
    {
        return order;
    }

    return z <=> other_.z;
}

inline float Vector3D::operator[](size_t index) const noexcept
{
    return (&x)[index];
}

inline float& Vector3D::operator[](size_t index) noexcept
{
    return (&x)[index];
}

inline float Vector3D::GetMagnitude() const noexcept
{
    return DirectX::XMVectorGetX(DirectX::XMVector3Length(Load(*this)));
}

inline float Vector3D::GetSqrMagnitude() const noexcept
{
    return Dot(*this, *this);
}

inline Vector3D Vector3D::GetNormalized() const noexcept
{
    return Normalize(*this);
}

inline void Vector3D::Normalize() noexcept
{
    *this = GetNormalized();
}

inline void Vector3D::Set(float x_, float y_, float z_) noexcept
{
    x = x_;
    y = y_;
    z = z_;
}

inline bool Vector3D::IsZero(float epsilon_) const noexcept
{
    return std::abs(x) <= epsilon_ && std::abs(y) <= epsilon_ && std::abs(z) <= epsilon_;
}

inline bool Vector3D::IsFinite() const noexcept
{
    return std::isfinite(x) && std::isfinite(y) && std::isfinite(z);
}

inline Vector3D Vector3D::GetZero() noexcept
{
    return Vector3D(0.0f, 0.0f, 0.0f);
}

inline Vector3D Vector3D::GetOne() noexcept
{
    return Vector3D(1.0f, 1.0f, 1.0f);
}

inline Vector3D Vector3D::GetUp() noexcept
{
    return Vector3D(0.0f, 1.0f, 0.0f);
}

inline Vector3D Vector3D::GetDown() noexcept
{
    return Vector3D(0.0f, -1.0f, 0.0f);
}

inline Vector3D Vector3D::GetLeft() noexcept
{
    return Vector3D(-1.0f, 0.0f, 0.0f);
}

inline Vector3D Vector3D::GetRight() noexcept
{
    return Vector3D(1.0f, 0.0f, 0.0f);
}

inline Vector3D Vector3D::GetForward() noexcept
{
    return Vector3D(0.0f, 0.0f, 1.0f);
}

inline Vector3D Vector3D::GetBack() noexcept
{
    return Vector3D(0.0f, 0.0f, -1.0f);
}

inline Vector3D Vector3D::GetPositiveInfinity() noexcept
{
    const float infinity{ std::numeric_limits<float>::infinity() };
    return Vector3D(infinity, infinity, infinity);
}

inline Vector3D Vector3D::GetNegativeInfinity() noexcept
{
    const float negativeInfinity{ -std::numeric_limits<float>::infinity() };
    return Vector3D(negativeInfinity, negativeInfinity, negativeInfinity);
}

inline DirectX::XMVECTOR Vector3D::Load(const Vector3D& vec_) noexcept
{
    return DirectX::XMLoadFloat3(&vec_);
}

inline void Vector3D::Store(Vector3D& d_, DirectX::XMVECTOR s_) noexcept
{
    DirectX::XMStoreFloat3(&d_, s_);
}

inline bool Vector3D::IsApproximately(const Vector3D& lhs_, const Vector3D& rhs_, float epsilon_) noexcept
{
    return std::abs(lhs_.x - rhs_.x) <= epsilon_
        && std::abs(lhs_.y - rhs_.y) <= epsilon_
        && std::abs(lhs_.z - rhs_.z) <= epsilon_;
}

inline Vector3D Vector3D::Max(const Vector3D& a_, const Vector3D& b_) noexcept
{
    return Vector3D(std::max(a_.x, b_.x), std::max(a_.y, b_.y), std::max(a_.z, b_.z));
}

inline Vector3D Vector3D::Min(const Vector3D& a_, const Vector3D& b_) noexcept
{
    return Vector3D(std::min(a_.x, b_.x), std::min(a_.y, b_.y), std::min(a_.z, b_.z));
}

inline float Vector3D::Angle(const Vector3D& a_, const Vector3D& b_) noexcept
{
    const Vector3D from = Normalize(a_);
    const Vector3D to = Normalize(b_);
    const float dot = std::clamp(Dot(from, to), -1.0f, 1.0f);
    return std::acos(dot);
}

inline float Vector3D::SignedAngle(const Vector3D& from_, const Vector3D& to_, const Vector3D& axis_) noexcept
{
    const Vector3D cross = Cross(from_, to_);
    const float angle = Angle(from_, to_);
    const float sign = (Dot(axis_, cross) >= 0.0f) ? 1.0f : -1.0f;
    return angle * sign;
}

inline Vector3D Vector3D::Clamp(const Vector3D& value_, const Vector3D& min_, const Vector3D& max_) noexcept
{
    return Vector3D(
        Mathf::Clamp(value_.x, min_.x, max_.x),
        Mathf::Clamp(value_.y, min_.y, max_.y),
        Mathf::Clamp(value_.z, min_.z, max_.z)
    );
}

inline Vector3D Vector3D::ClampMagnitude(const Vector3D& vector_, float maxLength_) noexcept
{
    const float sqrMagnitude = vector_.GetSqrMagnitude();
    const float maxSqr = maxLength_ * maxLength_;
    if (sqrMagnitude <= maxSqr)
    {
        return vector_;
    }

    return Normalize(vector_) * maxLength_;
}

inline float Vector3D::Distance(const Vector3D& _v1, const Vector3D& _v2) noexcept
{
    return (_v1 - _v2).GetMagnitude();
}

inline Vector3D Vector3D::Scale(const Vector3D& vector_, const Vector3D& scale_) noexcept
{
    return Vector3D(vector_.x * scale_.x, vector_.y * scale_.y, vector_.z * scale_.z);
}

inline Vector3D Vector3D::Normalize(const Vector3D& value_) noexcept
{
    Vector3D res;
    Store(res, DirectX::XMVector3Normalize(Load(value_)));
    return res;
}

inline Vector3D Vector3D::Lerp(const Vector3D& a_, const Vector3D& b_, float t_) noexcept
{
    Vector3D res;
    Store(res, DirectX::XMVectorLerp(Load(a_), Load(b_), Mathf::Clamp(t_, 0.0f, 1.0f)));
    return res;
}

inline Vector3D Vector3D::LerpUnclamped(const Vector3D& a_, const Vector3D& b_, float t_) noexcept
{
    Vector3D res;
    Store(res, DirectX::XMVectorLerp(Load(a_), Load(b_), t_));
    return res;
}

inline Vector3D Vector3D::Slerp(const Vector3D& a_, const Vector3D& b_, float t_) noexcept
{
    return SlerpUnclamped(a_, b_, Mathf::Clamp(t_, 0.0f, 1.0f));
}

inline Vector3D Vector3D::SlerpUnclamped(const Vector3D& a_, const Vector3D& b_, float t_) noexcept
{
    const float aMag = a_.GetMagnitude();
    const float bMag = b_.GetMagnitude();

    if (aMag <= Mathf::Epsilon || bMag <= Mathf::Epsilon)
    {
        return LerpUnclamped(a_, b_, t_);
    }

    const Vector3D from = a_ / aMag;
    const Vector3D to = b_ / bMag;

    float dot = std::clamp(Dot(from, to), -1.0f, 1.0f);
    const float theta = std::acos(dot) * t_;

    Vector3D relative = to - from * dot;
    const float relativeMag = relative.GetMagnitude();
    if (relativeMag <= Mathf::Epsilon)
    {
        return LerpUnclamped(a_, b_, t_);
    }

    relative /= relativeMag;
    const Vector3D direction = from * std::cos(theta) + relative * std::sin(theta);
    const float magnitude = Mathf::Lerp(aMag, bMag, t_);

    return direction * magnitude;
}

inline Vector3D Vector3D::Cross(const Vector3D& a_, const Vector3D& b_) noexcept
{
    Vector3D res;
    Store(res, DirectX::XMVector3Cross(Load(a_), Load(b_)));
    return res;
}

inline float Vector3D::Dot(const Vector3D& a_, const Vector3D& b_) noexcept
{
    return DirectX::XMVectorGetX(DirectX::XMVector3Dot(Load(a_), Load(b_)));
}

inline Vector3D Vector3D::Project(const Vector3D& vector_, const Vector3D& onNormal_) noexcept
{
    const float denominator = Dot(onNormal_, onNormal_);
    if (denominator <= Mathf::Epsilon)
    {
        return GetZero();
    }

    const float scale = Dot(vector_, onNormal_) / denominator;
    return onNormal_ * scale;
}

inline Vector3D Vector3D::ProjectOnPlane(const Vector3D& vector_, const Vector3D& planeNormal_) noexcept
{
    return vector_ - Project(vector_, planeNormal_);
}

inline Vector3D Vector3D::Reflect(const Vector3D& _vector, const Vector3D& _normal) noexcept
{
    Vector3D res;
    Store(res, DirectX::XMVector3Reflect(Load(_vector), Load(_normal)));
    return res;
}

inline Vector3D Vector3D::Refract(const Vector3D& _vector, const Vector3D& _normal, float _eta) noexcept
{
    Vector3D res;
    Store(res, DirectX::XMVector3Refract(Load(_vector), Load(_normal), _eta));
    return res;
}

inline void Vector3D::OrthoNormalize(Vector3D& normal_, Vector3D& tangent_) noexcept
{
    normal_ = Normalize(normal_);
    tangent_ = tangent_ - normal_ * Dot(normal_, tangent_);
    tangent_ = Normalize(tangent_);
}

inline void Vector3D::OrthoNormalize(Vector3D& normal_, Vector3D& tangent_, Vector3D& binormal_) noexcept
{
    OrthoNormalize(normal_, tangent_);
    binormal_ = Normalize(Cross(normal_, tangent_));
}

inline Vector3D Vector3D::SmoothDamp(
    const Vector3D& current_,
    const Vector3D& target_,
    Vector3D& currentVelocity_,
    float smoothTime_,
    float maxSpeed_,
    float deltaTime_) noexcept
{
    smoothTime_ = std::max(0.0001f, smoothTime_);
    const float omega{ 2.0f / smoothTime_ };
    const float x{ omega * deltaTime_ };
    const float exp{ 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x) };

    Vector3D change{ current_ - target_ };
    const Vector3D originalTarget{ target_ };

    const float maxChange{ maxSpeed_ * smoothTime_ };
    change = ClampMagnitude(change, maxChange);
    const Vector3D target{ current_ - change };

    const Vector3D temp{ (currentVelocity_ + change * omega) * deltaTime_ };
    currentVelocity_ = (currentVelocity_ - temp * omega) * exp;
    Vector3D output{ target + (change + temp) * exp };

    if (Dot(originalTarget - current_, output - originalTarget) > 0.0f)
    {
        output = originalTarget;
        currentVelocity_ = Vector3D(0.0f, 0.0f, 0.0f);
    }

    return output;
}

inline Vector3D Vector3D::MoveTowards(const Vector3D& current_, const Vector3D& target_, float maxDistanceDelta_) noexcept
{
    const Vector3D delta = target_ - current_;
    const float distance = delta.GetMagnitude();

    if (distance <= maxDistanceDelta_ || distance <= Mathf::Epsilon)
    {
        return target_;
    }

    return current_ + (delta / distance) * maxDistanceDelta_;
}

inline Vector3D Vector3D::RotateTowards(const Vector3D& current_, const Vector3D& target_, float maxRadiansDelta_, float maxMagnitudeDelta_) noexcept
{
    const float currentMag = current_.GetMagnitude();
    const float targetMag = target_.GetMagnitude();

    if (currentMag <= Mathf::Epsilon || targetMag <= Mathf::Epsilon)
    {
        return MoveTowards(current_, target_, maxMagnitudeDelta_);
    }

    const Vector3D currentDir = current_ / currentMag;
    const Vector3D targetDir = target_ / targetMag;

    const float angle = Angle(currentDir, targetDir);
    const float t = (angle <= Mathf::Epsilon) ? 1.0f : std::min(1.0f, maxRadiansDelta_ / angle);

    const Vector3D newDir = SlerpUnclamped(currentDir, targetDir, t).GetNormalized();

    float deltaMag = targetMag - currentMag;
    deltaMag = std::clamp(deltaMag, -maxMagnitudeDelta_, maxMagnitudeDelta_);
    const float newMag = currentMag + deltaMag;

    return newDir * newMag;
}

#include "Vector2D.h"
#include "Vector4D.h"

inline Vector3D::Vector3D(const Vector2D& vector_, float z_) noexcept
    : DirectX::XMFLOAT3(vector_.x, vector_.y, z_)
{
}

inline Vector3D::Vector3D(const Vector4D& vector_) noexcept
    : DirectX::XMFLOAT3(vector_.x, vector_.y, vector_.z)
{
}

inline Vector3D::operator Vector2D() const noexcept
{
    return Vector2D(x, y);
}

inline Vector3D::operator Vector4D() const noexcept
{
    return Vector4D(x, y, z, 0.0f);
}
