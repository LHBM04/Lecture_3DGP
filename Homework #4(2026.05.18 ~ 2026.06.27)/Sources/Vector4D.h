#pragma once

#include <iostream>

#include <DirectXMath.h>

#include "MathF.h"

class Vector2D;
class Vector3D;

class Vector4D : public DirectX::XMFLOAT4
{
public:
	Vector4D() noexcept;
	explicit Vector4D(float value_) noexcept;
	Vector4D(float x_, float y_, float z_, float w_) noexcept;
	explicit Vector4D(const Vector2D& vector_, float z_ = 0.0f, float w_ = 0.0f) noexcept;
	explicit Vector4D(const Vector3D& vector_, float w_ = 0.0f) noexcept;
	Vector4D(const Vector4D& other_) noexcept;
	Vector4D(Vector4D&& other_) noexcept;

	[[nodiscard]] Vector4D operator+(const Vector4D& _other) const noexcept;
	Vector4D& operator+=(const Vector4D& _other) noexcept;

	[[nodiscard]] Vector4D operator-(const Vector4D& _other) const noexcept;
	Vector4D& operator-=(const Vector4D& _other) noexcept;

	[[nodiscard]] Vector4D operator*(float _scalar) const noexcept;
	Vector4D& operator*=(float _scalar) noexcept;

	[[nodiscard]] Vector4D operator/(float _scalar) const noexcept;
	Vector4D& operator/=(float _scalar) noexcept;

	Vector4D& operator=(const Vector4D& _other) noexcept;
	Vector4D& operator=(Vector4D&& _other) noexcept;

	[[nodiscard]] bool operator==(const Vector4D& other_) const noexcept;
	[[nodiscard]] bool operator!=(const Vector4D& other_) const noexcept;

	[[nodiscard]] std::partial_ordering operator<=>(const Vector4D& other_) const noexcept;

	[[nodiscard]] float operator[](size_t index) const noexcept;
	[[nodiscard]] float& operator[](size_t index) noexcept;

	[[nodiscard]] static Vector4D GetZero() noexcept;
	[[nodiscard]] static Vector4D GetOne() noexcept;
	[[nodiscard]] static Vector4D GetPositiveInfinity() noexcept;
	[[nodiscard]] static Vector4D GetNegativeInfinity() noexcept;

	[[nodiscard]] float GetMagnitude() const noexcept;
	[[nodiscard]] float GetSqrMagnitude() const noexcept;
	[[nodiscard]] Vector4D GetNormalized() const noexcept;

	void Normalize() noexcept;

	void Set(float x_, float y_, float z_, float w_) noexcept;

	[[nodiscard]] bool IsZero(float epsilon_ = Mathf::Epsilon) const noexcept;

	[[nodiscard]] bool IsFinite() const noexcept;

	[[nodiscard]] static DirectX::XMVECTOR Load(const Vector4D& vec_) noexcept;
	static void Store(Vector4D& d_, DirectX::XMVECTOR s_) noexcept;

	[[nodiscard]] static bool IsApproximately(const Vector4D& lhs_, const Vector4D& rhs_, float epsilon_ = Mathf::Epsilon) noexcept;

	[[nodiscard]] static Vector4D Max(const Vector4D& a_, const Vector4D& b_) noexcept;
	[[nodiscard]] static Vector4D Min(const Vector4D& a_, const Vector4D& b_) noexcept;
	[[nodiscard]] static float Angle(const Vector4D& a_, const Vector4D& b_) noexcept;
	[[nodiscard]] static Vector4D Clamp(const Vector4D& value_, const Vector4D& min_, const Vector4D& max_) noexcept;
	[[nodiscard]] static Vector4D ClampMagnitude(const Vector4D& vector_, float maxLength_) noexcept;
	[[nodiscard]] static float Distance(const Vector4D& a_, const Vector4D& b_) noexcept;
	[[nodiscard]] static Vector4D Scale(const Vector4D& vector_, const Vector4D& scale_) noexcept;
	[[nodiscard]] static Vector4D Normalize(const Vector4D& value_) noexcept;
	[[nodiscard]] static float Dot(const Vector4D& a_, const Vector4D& b_) noexcept;
	[[nodiscard]] static Vector4D Lerp(const Vector4D& a_, const Vector4D& b_, float t_) noexcept;
	[[nodiscard]] static Vector4D LerpUnclamped(const Vector4D& a_, const Vector4D& b_, float t_) noexcept;
	[[nodiscard]] static Vector4D Slerp(const Vector4D& a_, const Vector4D& b_, float t_) noexcept;
	[[nodiscard]] static Vector4D SlerpUnclamped(const Vector4D& a_, const Vector4D& b_, float t_) noexcept;
	[[nodiscard]] static Vector4D Project(const Vector4D& vector_, const Vector4D& onNormal_) noexcept;
	[[nodiscard]] static Vector4D Reflect(const Vector4D& vector_, const Vector4D& normal_) noexcept;
	[[nodiscard]] static Vector4D MoveTowards(const Vector4D& current_, const Vector4D& target_, float maxDistanceDelta_) noexcept;

	[[nodiscard]] operator Vector2D() const noexcept;
	[[nodiscard]] operator Vector3D() const noexcept;
};
