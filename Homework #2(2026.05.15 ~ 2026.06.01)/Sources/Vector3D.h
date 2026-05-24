#pragma once

#include "Mathf.h"

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

	[[nodiscard]] bool IsZero(float epsilon_ = Mathf::EPSILON) const noexcept;

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

	[[nodiscard]] static bool IsApproximately(const Vector3D& lhs_, const Vector3D& rhs_, float epsilon_ = Mathf::EPSILON) noexcept;

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