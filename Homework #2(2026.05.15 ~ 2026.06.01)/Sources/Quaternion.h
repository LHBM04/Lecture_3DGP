#pragma once

#include <DirectXMath.h>
#include <iostream>

#include "MathF.h"

class Vector3D;

class Quaternion : public DirectX::XMFLOAT4
{
public:
	Quaternion() noexcept;
	explicit Quaternion(float value_) noexcept;
	Quaternion(float x_, float y_, float z_, float w_) noexcept;
	Quaternion(const Quaternion& other_) noexcept;
	Quaternion(Quaternion&& other_) noexcept;

	[[nodiscard]] Quaternion operator+(const Quaternion& _other) const noexcept;
	[[nodiscard]] Quaternion& operator+=(const Quaternion& _other) noexcept;

	[[nodiscard]] Quaternion operator-(const Quaternion& _other) const noexcept;
	[[nodiscard]] Quaternion& operator-=(const Quaternion& _other) noexcept;

	[[nodiscard]] Quaternion operator*(float _scalar) const noexcept;
	[[nodiscard]] Quaternion& operator*=(float _scalar) noexcept;
	[[nodiscard]] Quaternion operator*(const Quaternion& _other) const noexcept;
	[[nodiscard]] Quaternion& operator*=(const Quaternion& _other) noexcept;

	[[nodiscard]] Vector3D operator*(const Vector3D& vector_) const noexcept;

	[[nodiscard]] Quaternion operator/(float _scalar) const noexcept;
	[[nodiscard]] Quaternion& operator/=(float _scalar) noexcept;

	void Normalize() noexcept;

	void Set(float x_, float y_, float z_, float w_) noexcept;

	void SetEulerAngles(const Vector3D& eulerDegrees_) noexcept;

	Quaternion& operator=(const Quaternion& other_) noexcept;
	Quaternion& operator=(Quaternion&& other_) noexcept;

	[[nodiscard]] bool operator==(const Quaternion& other_) const noexcept;
	[[nodiscard]] bool operator!=(const Quaternion& other_) const noexcept;

	[[nodiscard]] std::partial_ordering operator<=>(const Quaternion& other_) const noexcept;

	[[nodiscard]] float operator[](size_t index) const noexcept;
	[[nodiscard]] float& operator[](size_t index) noexcept;

	[[nodiscard]] Vector3D GetEulerAngles() const noexcept;
	[[nodiscard]] float GetMagnitude() const noexcept;
	[[nodiscard]] float GetSqrMagnitude() const noexcept;
	[[nodiscard]] Quaternion GetNormalized() const noexcept;

	void ToAngleAxis(float& angleDegrees_, Vector3D& axis_) const noexcept;

	[[nodiscard]] bool IsNormalized(float epsilon_ = Mathf::Epsilon) const noexcept;

	[[nodiscard]] bool IsFinite() const noexcept;

	void SetFromToRotation(const Vector3D& from_, const Vector3D& to_) noexcept;
	void SetLookRotation(const Vector3D& view_) noexcept;
	void SetLookRotation(const Vector3D& view_, const Vector3D& up_) noexcept;

	[[nodiscard]] static Quaternion GetIdentity() noexcept;

	[[nodiscard]] static DirectX::XMVECTOR Load(const Quaternion& quat_) noexcept;
	static void Store(Quaternion& d_, DirectX::XMVECTOR s_) noexcept;

	[[nodiscard]] static float Angle(const Quaternion& a_, const Quaternion& b_) noexcept;
	[[nodiscard]] static Quaternion AngleAxis(float angleDegrees_, Vector3D axis_) noexcept;
	[[nodiscard]] static Quaternion Euler(float xDegrees_, float yDegrees_, float zDegrees_) noexcept;
	[[nodiscard]] static Quaternion Euler(const Vector3D& eulerDegrees_) noexcept;
	[[nodiscard]] static Quaternion Inverse(const Quaternion& rotation_) noexcept;
	[[nodiscard]] static Quaternion Normalize(const Quaternion& rotation_) noexcept;
	[[nodiscard]] static Quaternion Conjugate(const Quaternion& rotation_) noexcept;

	[[nodiscard]] static float Dot(const Quaternion& a_, const Quaternion& b_) noexcept;

	[[nodiscard]] static bool IsApproximately(const Quaternion& lhs_, const Quaternion& rhs_, float epsilon_ = Mathf::Epsilon) noexcept;

	[[nodiscard]] static Quaternion Lerp(const Quaternion& a_, const Quaternion& b_, float t_) noexcept;
	[[nodiscard]] static Quaternion LerpUnclamped(const Quaternion& a_, const Quaternion& b_, float t_) noexcept;
	[[nodiscard]] static Quaternion Slerp(const Quaternion& a_, const Quaternion& b_, float t_) noexcept;
	[[nodiscard]] static Quaternion SlerpUnclamped(const Quaternion& a_, const Quaternion& b_, float t_) noexcept;

	[[nodiscard]] static Quaternion FromToRotation(const Vector3D& fromDirection_, const Vector3D& toDirection_) noexcept;
	[[nodiscard]] static Quaternion LookRotation(const Vector3D& forward_, const Vector3D& up_) noexcept;
	[[nodiscard]] static Quaternion RotateTowards(const Quaternion& from_, const Quaternion& to_, float maxDegreesDelta_) noexcept;
};
