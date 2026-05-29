#include "Precompiled.h"
#include "Vector4D.h"

#include "MathF.h"
#include "Vector2D.h"
#include "Vector3D.h"

Vector4D::Vector4D() noexcept
    : DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f)
{
}

Vector4D::Vector4D(float value_) noexcept
    : DirectX::XMFLOAT4(value_, value_, value_, value_)
{
}

Vector4D::Vector4D(float x_, float y_, float z_, float w_) noexcept
    : DirectX::XMFLOAT4(x_, y_, z_, w_)
{
}

Vector4D::Vector4D(const Vector4D& other_) noexcept
    : DirectX::XMFLOAT4(other_)
{
}

Vector4D::Vector4D(Vector4D&& other_) noexcept
    : DirectX::XMFLOAT4(other_)
{
}

Vector4D Vector4D::operator+(const Vector4D& _other) const noexcept
{
    Vector4D result;
    Store(result, DirectX::XMVectorAdd(Load(*this), Load(_other)));
    return result;
}

Vector4D& Vector4D::operator+=(const Vector4D& _other) noexcept
{
    Store(*this, DirectX::XMVectorAdd(Load(*this), Load(_other)));
    return *this;
}

Vector4D Vector4D::operator-(const Vector4D& _other) const noexcept
{
    Vector4D result;
    Store(result, DirectX::XMVectorSubtract(Load(*this), Load(_other)));
    return result;
}

Vector4D& Vector4D::operator-=(const Vector4D& _other) noexcept
{
    Store(*this, DirectX::XMVectorSubtract(Load(*this), Load(_other)));
    return *this;
}

Vector4D Vector4D::operator*(float _scalar) const noexcept
{
    Vector4D result;
    Store(result, DirectX::XMVectorScale(Load(*this), _scalar));
    return result;
}

Vector4D& Vector4D::operator*=(float _scalar) noexcept
{
    Store(*this, DirectX::XMVectorScale(Load(*this), _scalar));
    return *this;
}

Vector4D Vector4D::operator/(float _scalar) const noexcept
{
    Vector4D result;
    Store(result, DirectX::XMVectorScale(Load(*this), 1.0f / _scalar));
    return result;
}

Vector4D& Vector4D::operator/=(float _scalar) noexcept
{
    Store(*this, DirectX::XMVectorScale(Load(*this), 1.0f / _scalar));
    return *this;
}

Vector4D& Vector4D::operator=(const Vector4D& _other) noexcept
{
    x = _other.x;
    y = _other.y;
    z = _other.z;
    w = _other.w;
    return *this;
}

Vector4D& Vector4D::operator=(Vector4D&& _other) noexcept
{
    x = _other.x;
    y = _other.y;
    z = _other.z;
    w = _other.w;
    return *this;
}

bool Vector4D::operator==(const Vector4D& other_) const noexcept
{
    return (*this <=> other_) == std::partial_ordering::equivalent;
}

bool Vector4D::operator!=(const Vector4D& other_) const noexcept
{
    return !(*this == other_);
}

std::partial_ordering Vector4D::operator<=>(const Vector4D& other_) const noexcept
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

float Vector4D::operator[](size_t index) const noexcept
{
    return (&x)[index];
}

float& Vector4D::operator[](size_t index) noexcept
{
    return (&x)[index];
}

Vector4D Vector4D::GetZero() noexcept
{
    return Vector4D(0.0f, 0.0f, 0.0f, 0.0f);
}

Vector4D Vector4D::GetOne() noexcept
{
    return Vector4D(1.0f, 1.0f, 1.0f, 1.0f);
}

Vector4D Vector4D::GetPositiveInfinity() noexcept
{
    const float infinity{ std::numeric_limits<float>::infinity() };
    return Vector4D(infinity, infinity, infinity, infinity);
}

Vector4D Vector4D::GetNegativeInfinity() noexcept
{
    const float negativeInfinity{ -std::numeric_limits<float>::infinity() };
    return Vector4D(negativeInfinity, negativeInfinity, negativeInfinity, negativeInfinity);
}

float Vector4D::GetMagnitude() const noexcept
{
    return DirectX::XMVectorGetX(DirectX::XMVector4Length(Load(*this)));
}

float Vector4D::GetSqrMagnitude() const noexcept
{
    return Dot(*this, *this);
}

Vector4D Vector4D::GetNormalized() const noexcept
{
    return Normalize(*this);
}

void Vector4D::Normalize() noexcept
{
    *this = GetNormalized();
}

void Vector4D::Set(float x_, float y_, float z_, float w_) noexcept
{
    x = x_;
    y = y_;
    z = z_;
    w = w_;
}

bool Vector4D::IsZero(float epsilon_) const noexcept
{
    return std::abs(x) <= epsilon_
        && std::abs(y) <= epsilon_
        && std::abs(z) <= epsilon_
        && std::abs(w) <= epsilon_;
}

bool Vector4D::IsFinite() const noexcept
{
    return std::isfinite(x) && std::isfinite(y) && std::isfinite(z) && std::isfinite(w);
}

DirectX::XMVECTOR Vector4D::Load(const Vector4D& vec_) noexcept
{
    return DirectX::XMLoadFloat4(&vec_);
}

void Vector4D::Store(Vector4D& d_, DirectX::XMVECTOR s_) noexcept
{
    DirectX::XMStoreFloat4(&d_, s_);
}

bool Vector4D::IsApproximately(const Vector4D& lhs_, const Vector4D& rhs_, float epsilon_) noexcept
{
    return std::abs(lhs_.x - rhs_.x) <= epsilon_
        && std::abs(lhs_.y - rhs_.y) <= epsilon_
        && std::abs(lhs_.z - rhs_.z) <= epsilon_
        && std::abs(lhs_.w - rhs_.w) <= epsilon_;
}

Vector4D Vector4D::Max(const Vector4D& a_, const Vector4D& b_) noexcept
{
    return Vector4D(std::max(a_.x, b_.x), std::max(a_.y, b_.y), std::max(a_.z, b_.z), std::max(a_.w, b_.w));
}

Vector4D Vector4D::Min(const Vector4D& a_, const Vector4D& b_) noexcept
{
    return Vector4D(std::min(a_.x, b_.x), std::min(a_.y, b_.y), std::min(a_.z, b_.z), std::min(a_.w, b_.w));
}

float Vector4D::Angle(const Vector4D& a_, const Vector4D& b_) noexcept
{
    const Vector4D from = Normalize(a_);
    const Vector4D to = Normalize(b_);
    const float dot = std::clamp(Dot(from, to), -1.0f, 1.0f);
    return std::acos(dot);
}

Vector4D Vector4D::Clamp(const Vector4D& value_, const Vector4D& min_, const Vector4D& max_) noexcept
{
    return Vector4D(
        Mathf::Clamp(value_.x, min_.x, max_.x),
        Mathf::Clamp(value_.y, min_.y, max_.y),
        Mathf::Clamp(value_.z, min_.z, max_.z),
        Mathf::Clamp(value_.w, min_.w, max_.w)
    );
}

Vector4D Vector4D::ClampMagnitude(const Vector4D& vector_, float maxLength_) noexcept
{
    const float sqrMagnitude = vector_.GetSqrMagnitude();
    const float maxSqr = maxLength_ * maxLength_;
    if (sqrMagnitude <= maxSqr)
    {
        return vector_;
    }

    return Normalize(vector_) * maxLength_;
}

float Vector4D::Distance(const Vector4D& a_, const Vector4D& b_) noexcept
{
    return (a_ - b_).GetMagnitude();
}

Vector4D Vector4D::Scale(const Vector4D& vector_, const Vector4D& scale_) noexcept
{
    return Vector4D(vector_.x * scale_.x, vector_.y * scale_.y, vector_.z * scale_.z, vector_.w * scale_.w);
}

Vector4D Vector4D::Normalize(const Vector4D& value_) noexcept
{
    Vector4D result;
    Store(result, DirectX::XMVector4Normalize(Load(value_)));
    return result;
}

float Vector4D::Dot(const Vector4D& a_, const Vector4D& b_) noexcept
{
    return DirectX::XMVectorGetX(DirectX::XMVector4Dot(Load(a_), Load(b_)));
}

Vector4D Vector4D::Lerp(const Vector4D& a_, const Vector4D& b_, float t_) noexcept
{
    Vector4D result;
    Store(result, DirectX::XMVectorLerp(Load(a_), Load(b_), Mathf::Clamp(t_, 0.0f, 1.0f)));
    return result;
}

Vector4D Vector4D::LerpUnclamped(const Vector4D& a_, const Vector4D& b_, float t_) noexcept
{
    Vector4D result;
    Store(result, DirectX::XMVectorLerp(Load(a_), Load(b_), t_));
    return result;
}

Vector4D Vector4D::Slerp(const Vector4D& a_, const Vector4D& b_, float t_) noexcept
{
    return SlerpUnclamped(a_, b_, Mathf::Clamp(t_, 0.0f, 1.0f));
}

Vector4D Vector4D::SlerpUnclamped(const Vector4D& a_, const Vector4D& b_, float t_) noexcept
{
    const float aMag = a_.GetMagnitude();
    const float bMag = b_.GetMagnitude();

    if (aMag <= Mathf::Epsilon || bMag <= Mathf::Epsilon)
    {
        return LerpUnclamped(a_, b_, t_);
    }

    const Vector4D from = a_ / aMag;
    const Vector4D to = b_ / bMag;

    float dot = std::clamp(Dot(from, to), -1.0f, 1.0f);
    const float theta = std::acos(dot) * t_;

    Vector4D relative = to - from * dot;
    const float relativeMag = relative.GetMagnitude();
    if (relativeMag <= Mathf::Epsilon)
    {
        return LerpUnclamped(a_, b_, t_);
    }

    relative /= relativeMag;
    const Vector4D direction = from * std::cos(theta) + relative * std::sin(theta);
    const float magnitude = Mathf::Lerp(aMag, bMag, t_);

    return direction * magnitude;
}

Vector4D Vector4D::Project(const Vector4D& vector_, const Vector4D& onNormal_) noexcept
{
    const float denominator = Dot(onNormal_, onNormal_);
    if (denominator <= Mathf::Epsilon)
    {
        return GetZero();
    }

    const float scale = Dot(vector_, onNormal_) / denominator;
    return onNormal_ * scale;
}

Vector4D Vector4D::Reflect(const Vector4D& vector_, const Vector4D& normal_) noexcept
{
    return vector_ - normal_ * (2.0f * Dot(vector_, normal_));
}

Vector4D Vector4D::MoveTowards(const Vector4D& current_, const Vector4D& target_, float maxDistanceDelta_) noexcept
{
    const Vector4D delta = target_ - current_;
    const float distance = delta.GetMagnitude();

    if (distance <= maxDistanceDelta_ || distance <= Mathf::Epsilon)
    {
        return target_;
    }

    return current_ + (delta / distance) * maxDistanceDelta_;
}

Vector4D::Vector4D(const Vector2D& vector_, float z_, float w_) noexcept
    : DirectX::XMFLOAT4(vector_.x, vector_.y, z_, w_)
{
}

Vector4D::Vector4D(const Vector3D& vector_, float w_) noexcept
    : DirectX::XMFLOAT4(vector_.x, vector_.y, vector_.z, w_)
{
}

Vector4D::operator Vector2D() const noexcept
{
    return Vector2D(x, y);
}

Vector4D::operator Vector3D() const noexcept
{
    return Vector3D(x, y, z);
}
