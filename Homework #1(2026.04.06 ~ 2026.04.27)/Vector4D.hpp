#pragma once

#include "Math.hpp"

struct Vector2D;
struct Vector3D;

struct Vector4D : public DirectX::XMFLOAT4
{
	Vector4D();
	Vector4D(float value_);
	Vector4D(float x_, float y_, float z_, float w_);
	Vector4D(const Vector2D& vector_, float z_ = 0.0f, float w_ = 0.0f);
	Vector4D(const Vector3D& vector_, float w_ = 0.0f);
	Vector4D(const Vector4D& other_);
	Vector4D(Vector4D&& other_);

	Vector4D operator+(const Vector4D& _other) const;
	Vector4D& operator+=(const Vector4D& _other);

	Vector4D operator-(const Vector4D& _other) const;
	Vector4D& operator-=(const Vector4D& _other);

	Vector4D operator*(float _scalar) const;
	Vector4D& operator*=(float _scalar);

	Vector4D operator/(float _scalar) const;
	Vector4D& operator/=(float _scalar);

	Vector4D& operator=(const Vector4D& _other);
	Vector4D& operator=(Vector4D&& _other);

	bool operator==(const Vector4D& other_) const;
	bool operator!=(const Vector4D& other_) const;

	std::partial_ordering operator<=>(const Vector4D& other_) const;

	float operator[](size_t index) const;
	float& operator[](size_t index);

	static Vector4D GetZero();
	static Vector4D GetOne();
	static Vector4D GetPositiveInfinity();
	static Vector4D GetNegativeInfinity();

	float GetMagnitude() const;
	float GetSqrMagnitude() const;
	Vector4D GetNormalized() const;

	void Normalize();

	void Set(float x_, float y_, float z_, float w_);

	bool IsZero(float epsilon_ = Mathf::EPSILON) const;

	bool IsFinite() const;

	static Vector Load(const Vector4D& vec_);
	static void Store(Vector4D& d_, Vector s_);

	static bool IsApproximately(const Vector4D& lhs_, const Vector4D& rhs_, float epsilon_ = Mathf::EPSILON);

	static Vector4D Max(const Vector4D& a_, const Vector4D& b_);
	static Vector4D Min(const Vector4D& a_, const Vector4D& b_);
	static float Angle(const Vector4D& a_, const Vector4D& b_);
	static Vector4D Clamp(const Vector4D& value_, const Vector4D& min_, const Vector4D& max_);
	static Vector4D ClampMagnitude(const Vector4D& vector_, float maxLength_);
	static float Distance(const Vector4D& a_, const Vector4D& b_);
	static Vector4D Scale(const Vector4D& vector_, const Vector4D& scale_);
	static Vector4D Normalize(const Vector4D& value_);
	static float Dot(const Vector4D& a_, const Vector4D& b_);
	static Vector4D Lerp(const Vector4D& a_, const Vector4D& b_, float t_);
	static Vector4D LerpUnclamped(const Vector4D& a_, const Vector4D& b_, float t_);
	static Vector4D Slerp(const Vector4D& a_, const Vector4D& b_, float t_);
	static Vector4D SlerpUnclamped(const Vector4D& a_, const Vector4D& b_, float t_);
	static Vector4D Project(const Vector4D& vector_, const Vector4D& onNormal_);
	static Vector4D Reflect(const Vector4D& vector_, const Vector4D& normal_);
	static Vector4D MoveTowards(const Vector4D& current_, const Vector4D& target_, float maxDistanceDelta_);

	operator Vector2D() const;
	operator Vector3D() const;
};