#pragma once

#include "Math.hpp"

struct Quaternion;
struct Vector3D;
struct Vector4D;
struct Matrix4x4 : public DirectX::XMFLOAT4X4
{
    Matrix4x4();
    explicit Matrix4x4(float value_);
    explicit Matrix4x4(const float values_[16]);
    explicit Matrix4x4(
        float m00_, float m01_, float m02_, float m03_,
        float m10_, float m11_, float m12_, float m13_,
        float m20_, float m21_, float m22_, float m23_,
        float m30_, float m31_, float m32_, float m33_);
    explicit Matrix4x4(Matrix matrix_);

    Matrix4x4(const Matrix4x4& other_);
    Matrix4x4(Matrix4x4&& other_) noexcept;

    Matrix4x4 operator*(const Matrix4x4& other_) const;
    Matrix4x4& operator*=(const Matrix4x4& other_);

    Matrix4x4& operator=(const Matrix4x4& other_);
    Matrix4x4& operator=(Matrix4x4&& other_) noexcept;

    bool operator==(const Matrix4x4& other_) const;
    bool operator!=(const Matrix4x4& other_) const;

    std::partial_ordering operator<=>(const Matrix4x4& other_) const;

    float operator[](size_t index_) const;
    float& operator[](size_t index_);

    float GetDeterminant() const;
    Matrix4x4 GetTranspose() const;
    Matrix4x4 GetInverse() const;

    bool TryGetInverse(Matrix4x4& result_) const;

    bool CanInverse() const;

    Vector3D MultiplyPoint3x4(const Vector3D& point_) const;
    Vector3D MultiplyPoint(const Vector3D& point_) const;
    Vector3D MultiplyVector(const Vector3D& vector_) const;

    Vector3D GetWorldPosition() const;
    Vector3D GetScale() const;
    Vector3D GetLossyScale() const;

    Vector4D GetRow(std::size_t index_) const;
    Vector4D GetColumn(std::size_t index_) const;

    void SetRow(std::size_t index_, const Vector4D& row_);
    void SetColumn(std::size_t index_, const Vector4D& column_);

    Quaternion GetRotation() const;

    bool IsValidTRS() const;

    bool IsIdentity(float epsilon_ = Mathf::EPSILON) const;

    static bool IsApproximately(const Matrix4x4& lhs_, const Matrix4x4& rhs_, float epsilon_ = Mathf::EPSILON);

    void SetTRS(const Vector3D& position_, const Quaternion& rotation_, const Vector3D& scale_);

    static Matrix4x4 GetIdentity();
    static Matrix4x4 GetZero();
    static Matrix4x4 Translate(const Vector3D& translation_);
    static Matrix4x4 Rotate(const Quaternion& rotation_);
    static Matrix4x4 Scale(const Vector3D& scale_);
    static Matrix4x4 TRS(const Vector3D& translation_, const Quaternion& rotation_, const Vector3D& scale_);
    static Matrix4x4 LookAt(const Vector3D& from_, const Vector3D& to_, const Vector3D& up_);
    static Matrix4x4 Perspective(float fovYDegrees_, float aspect_, float nearZ_, float farZ_);
    static Matrix4x4 Ortho(float left_, float right_, float bottom_, float top_, float nearZ_, float farZ_);
    static Matrix4x4 Ortho(float width_, float height_, float nearZ_, float farZ_);
    static Matrix4x4 Frustum(float left_, float right_, float bottom_, float top_, float nearZ_, float farZ_);

    static bool TryInverse3DAffine(const Matrix4x4& input_, Matrix4x4& result_);

    static Matrix Load(const Matrix4x4& matrix_);
    static void Store(Matrix4x4& destination_, Matrix source_);
};
