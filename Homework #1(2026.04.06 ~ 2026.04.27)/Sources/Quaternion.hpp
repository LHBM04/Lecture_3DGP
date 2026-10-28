#pragma once

#include "Math.hpp"

struct Vector3D;

struct Quaternion : public DirectX::XMFLOAT4
{
	Quaternion();
	Quaternion(float value_);
	Quaternion(float x_, float y_, float z_, float w_);
	Quaternion(const Quaternion& other_);
	Quaternion(Quaternion&& other_);

	Quaternion operator+(const Quaternion& _other) const;
	Quaternion& operator+=(const Quaternion& _other);

	Quaternion operator-(const Quaternion& _other) const;
	Quaternion& operator-=(const Quaternion& _other);

	Quaternion operator*(float _scalar) const;
	Quaternion& operator*=(float _scalar);
	Quaternion operator*(const Quaternion& _other) const;
	Quaternion& operator*=(const Quaternion& _other);

	Vector3D operator*(const Vector3D& vector_) const;

	Quaternion operator/(float _scalar) const;
	Quaternion& operator/=(float _scalar);

	void Normalize();

	void Set(float x_, float y_, float z_, float w_);

	void SetEulerAngles(const Vector3D& eulerDegrees_);

	Quaternion& operator=(const Quaternion& other_);
	Quaternion& operator=(Quaternion&& other_);

	bool operator==(const Quaternion& other_) const;
	bool operator!=(const Quaternion& other_) const;

	std::partial_ordering operator<=>(const Quaternion& other_) const;

	float operator[](size_t index) const;
	float& operator[](size_t index);

	Vector3D GetEulerAngles() const;
	float GetMagnitude() const;
	float GetSqrMagnitude() const;
	Quaternion GetNormalized() const;

	void ToAngleAxis(float& angleDegrees_, Vector3D& axis_) const;

	bool IsNormalized(float epsilon_ = Mathf::EPSILON) const;

	bool IsFinite() const;

	void SetFromToRotation(const Vector3D& from_, const Vector3D& to_);
	void SetLookRotation(const Vector3D& view_);
	void SetLookRotation(const Vector3D& view_, const Vector3D& up_);

	static Quaternion GetIdentity();

	static Vector Load(const Quaternion& quat_);
	static void Store(Quaternion& d_, Vector s_);

	static float Angle(const Quaternion& a_, const Quaternion& b_);
	static Quaternion AngleAxis(float angleDegrees_, Vector3D axis_);
	static Quaternion Euler(float xDegrees_, float yDegrees_, float zDegrees_);
	static Quaternion Euler(const Vector3D& eulerDegrees_);
	static Quaternion Inverse(const Quaternion& rotation_);
	static Quaternion Normalize(const Quaternion& rotation_);
	static Quaternion Conjugate(const Quaternion& rotation_);

	static float Dot(const Quaternion& a_, const Quaternion& b_);

	static bool IsApproximately(const Quaternion& lhs_, const Quaternion& rhs_, float epsilon_ = Mathf::EPSILON);

	static Quaternion Lerp(const Quaternion& a_, const Quaternion& b_, float t_);
	static Quaternion LerpUnclamped(const Quaternion& a_, const Quaternion& b_, float t_);
	static Quaternion Slerp(const Quaternion& a_, const Quaternion& b_, float t_);
	static Quaternion SlerpUnclamped(const Quaternion& a_, const Quaternion& b_, float t_);

	static Quaternion FromToRotation(const Vector3D& fromDirection_, const Vector3D& toDirection_);
	static Quaternion LookRotation(const Vector3D& forward_, const Vector3D& up_);
	static Quaternion RotateTowards(const Quaternion& from_, const Quaternion& to_, float maxDegreesDelta_);
};
