#pragma once

#include "Math.hpp"

struct Vector3D;
struct Vector4D;

struct Vector2D : public DirectX::XMFLOAT2
{
	Vector2D();
	Vector2D(float value_);
	Vector2D(float x_, float y_);
	Vector2D(const Vector3D& vector_);
	Vector2D(const Vector4D& vector_);
	Vector2D(Vector2D&& other_);
	Vector2D(const Vector2D& other_);

    Vector2D operator+(const Vector2D& _other) const;
    Vector2D& operator+=(const Vector2D& _other);

    Vector2D operator-(const Vector2D& _other) const;
    Vector2D& operator-=(const Vector2D& _other);

    Vector2D operator*(float _scalar) const;
    Vector2D& operator*=(float _scalar);

    Vector2D operator/(float _scalar) const;
    Vector2D& operator/=(float _scalar);

    Vector2D& operator=(const Vector2D& _other);
    Vector2D& operator=(Vector2D&& _other);

    bool operator==(const Vector2D& other_) const;
    bool operator!=(const Vector2D& other_) const;

    std::partial_ordering operator<=>(const Vector2D& other_) const;

    float operator[](size_t index) const;
    float& operator[](size_t index);

    float GetMagnitude() const;
    float GetSqrMagnitude() const;

    Vector2D GetNormalized() const;

	void Normalize();

	void Set(float x_, float y_);

	bool IsZero(float epsilon_ = Mathf::EPSILON) const;

	bool IsFinite() const;

	static Vector2D GetZero();
	static Vector2D GetOne();
	static Vector2D GetUp();
	static Vector2D GetDown();
	static Vector2D GetLeft();
	static Vector2D GetRight();
	static Vector2D GetPositiveInfinity();
	static Vector2D GetNegativeInfinity();

    static Vector Load(const Vector2D& vec_);
	static void Store(Vector2D& d_, Vector s_);

	static bool IsApproximately(const Vector2D& lhs_, const Vector2D& rhs_, float epsilon_ = Mathf::EPSILON);

    static Vector2D Max(const Vector2D& a_, const Vector2D& b_);
    static Vector2D Min(const Vector2D& a_, const Vector2D& b_);

	static float Angle(const Vector2D& a_, const Vector2D& b_);
    static float SignedAngle(const Vector2D& from_, const Vector2D& to_);
    static Vector2D Clamp(const Vector2D& value_, const Vector2D& min_, const Vector2D& max_);
	static Vector2D ClampMagnitude(const Vector2D& vector_, float maxLength_);
    static float Distance(const Vector2D& a_, const Vector2D& b_);
    static Vector2D Scale(const Vector2D& vector_, const Vector2D& scale_);
	static Vector2D Normalize(const Vector2D& value_);
	static Vector2D Perpendicular(const Vector2D& vector_);
	static float Dot(const Vector2D& a_, const Vector2D& b_);

    static Vector2D Lerp(const Vector2D& a_, const Vector2D& b_, float t_);
    static Vector2D LerpUnclamped(const Vector2D& a_, const Vector2D& b_, float t_);
    static Vector2D Slerp(const Vector2D& a_, const Vector2D& b_, float t_);
	static Vector2D SlerpUnclamped(const Vector2D& a_, const Vector2D& b_, float t_);

	static Vector2D Cross(const Vector2D& a_, const Vector2D& b_);
	static Vector2D Reflect(const Vector2D& vector_, const Vector2D& normal_);

    static Vector2D MoveTowards(const Vector2D& current_, const Vector2D& target_, float maxDistanceDelta_);

	static Vector2D SmoothDamp(
	    const Vector2D& current_,
	    const Vector2D& target_,
	    Vector2D& currentVelocity_,
	    float smoothTime_,
	    float maxSpeed_,
	    float deltaTime_);

	operator Vector3D() const;
	operator Vector4D() const;
};
