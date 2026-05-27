#pragma once

#include "Math.hpp"

struct Vector2D;
struct Vector4D;

struct Vector3D : public DirectX::XMFLOAT3
{
    Vector3D();
    explicit Vector3D(float value_);
    explicit Vector3D(float x_, float y_, float z_);
	explicit Vector3D(const Vector2D& vector_, float z_ = 0.0f);
	explicit Vector3D(const Vector4D& vector_);

    Vector3D(const Vector3D& other_);
    Vector3D(Vector3D&& other_);

    Vector3D operator+(const Vector3D& _other) const;
    Vector3D& operator+=(const Vector3D& _other);

    Vector3D operator-(const Vector3D& _other) const;
    Vector3D& operator-=(const Vector3D& _other);

    Vector3D operator*(float _scalar) const;
    Vector3D& operator*=(float _scalar);

    Vector3D operator/(float _scalar) const;
    Vector3D& operator/=(float _scalar);

    Vector3D& operator=(const Vector3D& _other);
    Vector3D& operator=(Vector3D&& _other);

    bool operator==(const Vector3D& other_) const;
    bool operator!=(const Vector3D& other_) const;

    std::partial_ordering operator<=>(const Vector3D& other_) const;

    float operator[](size_t index) const;
    float& operator[](size_t index);

    float GetMagnitude() const;
    float GetSqrMagnitude() const;

    Vector3D GetNormalized() const;

	void Normalize();

	void Set(float x_, float y_, float z_);

	bool IsZero(float epsilon_ = Mathf::EPSILON) const;

	bool IsFinite() const;

	static Vector3D GetZero();
	static Vector3D GetOne();
	static Vector3D GetUp();
	static Vector3D GetDown();
	static Vector3D GetLeft();
	static Vector3D GetRight();
	static Vector3D GetForward();
	static Vector3D GetBack();
	static Vector3D GetPositiveInfinity();
	static Vector3D GetNegativeInfinity();

    static Vector Load(const Vector3D& vec_);
    static void Store(Vector3D& d_, Vector s_);

	static bool IsApproximately(const Vector3D& lhs_, const Vector3D& rhs_, float epsilon_ = Mathf::EPSILON);

    static Vector3D Max(const Vector3D& a_, const Vector3D& b_);
    static Vector3D Min(const Vector3D& a_, const Vector3D& b_);

    static float Angle(const Vector3D& a_, const Vector3D& b_);
    static float SignedAngle(const Vector3D& from_, const Vector3D& to_, const Vector3D& axis_);
    static Vector3D Clamp(const Vector3D& value_, const Vector3D& min_, const Vector3D& max_);
    static Vector3D ClampMagnitude(const Vector3D& vector_, float maxLength_);
    static float Distance(const Vector3D& _v1, const Vector3D& _v2);
    static Vector3D Scale(const Vector3D& vector_, const Vector3D& scale_);
    static Vector3D Normalize(const Vector3D& value_);

    static Vector3D Lerp(const Vector3D& a_, const Vector3D& b_, float t_);
    static Vector3D LerpUnclamped(const Vector3D& a_, const Vector3D& b_, float t_);
    static Vector3D Slerp(const Vector3D& a_, const Vector3D& b_, float t_);
    static Vector3D SlerpUnclamped(const Vector3D& a_, const Vector3D& b_, float t_);

    static Vector3D Cross(const Vector3D& a_, const Vector3D& b_);
    static float Dot(const Vector3D& a_, const Vector3D& b_);
    static Vector3D Project(const Vector3D& vector_, const Vector3D& onNormal_);
    static Vector3D ProjectOnPlane(const Vector3D& vector_, const Vector3D& planeNormal_);
    static Vector3D Reflect(const Vector3D& _vector, const Vector3D& _normal);
    static Vector3D Refract(const Vector3D& _vector, const Vector3D& _normal, float _eta);

	static void OrthoNormalize(Vector3D& normal_, Vector3D& tangent_);

	static void OrthoNormalize(Vector3D& normal_, Vector3D& tangent_, Vector3D& binormal_);

	static Vector3D SmoothDamp(
	    const Vector3D& current_,
	    const Vector3D& target_,
	    Vector3D& currentVelocity_,
	    float smoothTime_,
	    float maxSpeed_,
	    float deltaTime_);

	operator Vector2D() const;
	operator Vector4D() const;

    static Vector3D MoveTowards(const Vector3D& current_, const Vector3D& target_, float maxDistanceDelta_);
    static Vector3D RotateTowards(const Vector3D& current_, const Vector3D& target_, float maxRadiansDelta_, float maxMagnitudeDelta_);
};