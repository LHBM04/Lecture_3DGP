#pragma once

#include <iostream>

#include <DirectXMath.h>

#include "MathF.h"

class Vector3D;
class Vector4D;

class Vector2D : public DirectX::XMFLOAT2
{
public:
	Vector2D() noexcept;
	explicit Vector2D(float value_) noexcept;
	Vector2D(float x_, float y_) noexcept;
	explicit Vector2D(const Vector3D& vector_) noexcept;
	explicit Vector2D(const Vector4D& vector_) noexcept;
	Vector2D(Vector2D&& other_) noexcept;
	Vector2D(const Vector2D& other_) noexcept;

    [[nodiscard]] Vector2D operator+(const Vector2D& _other) const noexcept;
    Vector2D& operator+=(const Vector2D& _other) noexcept;

    [[nodiscard]] Vector2D operator-(const Vector2D& _other) const noexcept;
    Vector2D& operator-=(const Vector2D& _other) noexcept;

    [[nodiscard]] Vector2D operator*(float _scalar) const noexcept;
    Vector2D& operator*=(float _scalar) noexcept;

    [[nodiscard]] Vector2D operator/(float _scalar) const noexcept;
    Vector2D& operator/=(float _scalar) noexcept;

    Vector2D& operator=(const Vector2D& _other) noexcept;
    Vector2D& operator=(Vector2D&& _other) noexcept;

    [[nodiscard]] bool operator==(const Vector2D& other_) const noexcept;
    [[nodiscard]] bool operator!=(const Vector2D& other_) const noexcept;

    [[nodiscard]] std::partial_ordering operator<=>(const Vector2D& other_) const noexcept;

    [[nodiscard]] float operator[](size_t index) const noexcept;
    [[nodiscard]] float& operator[](size_t index) noexcept;

    [[nodiscard]] float GetMagnitude() const noexcept;
    [[nodiscard]] float GetSqrMagnitude() const noexcept;

    [[nodiscard]] Vector2D GetNormalized() const noexcept;

	void Normalize() noexcept;

	void Set(float x_, float y_) noexcept;

	[[nodiscard]] bool IsZero(float epsilon_ = Mathf::Epsilon) const noexcept;

	[[nodiscard]] bool IsFinite() const noexcept;

	[[nodiscard]] static Vector2D GetZero() noexcept;
	[[nodiscard]] static Vector2D GetOne() noexcept;
	[[nodiscard]] static Vector2D GetUp() noexcept;
	[[nodiscard]] static Vector2D GetDown() noexcept;
	[[nodiscard]] static Vector2D GetLeft() noexcept;
	[[nodiscard]] static Vector2D GetRight() noexcept;
	[[nodiscard]] static Vector2D GetPositiveInfinity() noexcept;
	[[nodiscard]] static Vector2D GetNegativeInfinity() noexcept;

    [[nodiscard]] static DirectX::XMVECTOR Load(const Vector2D& vec_) noexcept;
	static void Store(Vector2D& d_, DirectX::XMVECTOR s_) noexcept;

	[[nodiscard]] static bool IsApproximately(const Vector2D& lhs_, const Vector2D& rhs_, float epsilon_ = Mathf::Epsilon) noexcept;

    [[nodiscard]] static Vector2D Max(const Vector2D& a_, const Vector2D& b_) noexcept;
    [[nodiscard]] static Vector2D Min(const Vector2D& a_, const Vector2D& b_) noexcept;

	[[nodiscard]] static float Angle(const Vector2D& a_, const Vector2D& b_) noexcept;
    [[nodiscard]] static float SignedAngle(const Vector2D& from_, const Vector2D& to_) noexcept;
    [[nodiscard]] static Vector2D Clamp(const Vector2D& value_, const Vector2D& min_, const Vector2D& max_) noexcept;
	[[nodiscard]] static Vector2D ClampMagnitude(const Vector2D& vector_, float maxLength_) noexcept;
    [[nodiscard]] static float Distance(const Vector2D& a_, const Vector2D& b_) noexcept;
    [[nodiscard]] static Vector2D Scale(const Vector2D& vector_, const Vector2D& scale_) noexcept;
	[[nodiscard]] static Vector2D Normalize(const Vector2D& value_) noexcept;
	[[nodiscard]] static Vector2D Perpendicular(const Vector2D& vector_) noexcept;
	[[nodiscard]] static float Dot(const Vector2D& a_, const Vector2D& b_) noexcept;

    [[nodiscard]] static Vector2D Lerp(const Vector2D& a_, const Vector2D& b_, float t_) noexcept;
    [[nodiscard]] static Vector2D LerpUnclamped(const Vector2D& a_, const Vector2D& b_, float t_) noexcept;
    [[nodiscard]] static Vector2D Slerp(const Vector2D& a_, const Vector2D& b_, float t_) noexcept;
	[[nodiscard]] static Vector2D SlerpUnclamped(const Vector2D& a_, const Vector2D& b_, float t_) noexcept;

	[[nodiscard]] static Vector2D Cross(const Vector2D& a_, const Vector2D& b_) noexcept;
	[[nodiscard]] static Vector2D Reflect(const Vector2D& vector_, const Vector2D& normal_) noexcept;

    [[nodiscard]] static Vector2D MoveTowards(const Vector2D& current_, const Vector2D& target_, float maxDistanceDelta_) noexcept;

	[[nodiscard]] static Vector2D SmoothDamp(
	    const Vector2D& current_,
	    const Vector2D& target_,
	    Vector2D& currentVelocity_,
	    float smoothTime_,
	    float maxSpeed_,
	    float deltaTime_) noexcept;

	[[nodiscard]] operator Vector3D() const noexcept;
	[[nodiscard]] operator Vector4D() const noexcept;
};

