#pragma once

#include "MathF.h"

struct Quaternion;
struct Vector3D;
struct Vector4D;
struct Matrix4x4 : public DirectX::XMFLOAT4X4
{
    Matrix4x4() noexcept;
    explicit Matrix4x4(float value_) noexcept;
    explicit Matrix4x4(const float values_[16]) noexcept;
    explicit Matrix4x4(
        float m00_, float m01_, float m02_, float m03_,
        float m10_, float m11_, float m12_, float m13_,
        float m20_, float m21_, float m22_, float m23_,
        float m30_, float m31_, float m32_, float m33_) noexcept;
    explicit Matrix4x4(DirectX::XMMATRIX matrix_) noexcept;

    Matrix4x4(const Matrix4x4& other_) noexcept;
    Matrix4x4(Matrix4x4&& other_) noexcept;

    [[nodiscard]] Matrix4x4 operator*(const Matrix4x4& other_) const noexcept;
    Matrix4x4& operator*=(const Matrix4x4& other_) noexcept;

    Matrix4x4& operator=(const Matrix4x4& other_) noexcept;
    Matrix4x4& operator=(Matrix4x4&& other_) noexcept;

    [[nodiscard]] bool operator==(const Matrix4x4& other_) const noexcept;
    [[nodiscard]] bool operator!=(const Matrix4x4& other_) const noexcept;

    [[nodiscard]] std::partial_ordering operator<=>(const Matrix4x4& other_) const noexcept;

    [[nodiscard]] float operator[](size_t index_) const noexcept;
    [[nodiscard]] float& operator[](size_t index_) noexcept;

    [[nodiscard]] float GetDeterminant() const noexcept;
    [[nodiscard]] Matrix4x4 GetTranspose() const noexcept;
    [[nodiscard]] Matrix4x4 GetInverse() const noexcept;

    bool TryGetInverse(Matrix4x4& result_) const noexcept;

    [[nodiscard]] bool CanInverse() const noexcept;

    [[nodiscard]] Vector3D MultiplyPoint3x4(const Vector3D& point_) const noexcept;
    [[nodiscard]] Vector3D MultiplyPoint(const Vector3D& point_) const noexcept;
    [[nodiscard]] Vector3D MultiplyVector(const Vector3D& vector_) const noexcept;

    [[nodiscard]] Vector3D GetWorldPosition() const noexcept;
    [[nodiscard]] Vector3D GetScale() const noexcept;
    [[nodiscard]] Vector3D GetLossyScale() const noexcept;

    [[nodiscard]] Vector4D GetRow(std::size_t index_) const noexcept;
    [[nodiscard]] Vector4D GetColumn(std::size_t index_) const noexcept;

    void SetRow(std::size_t index_, const Vector4D& row_) noexcept;
    void SetColumn(std::size_t index_, const Vector4D& column_) noexcept;

    [[nodiscard]] Quaternion GetRotation() const noexcept;

    [[nodiscard]] bool IsValidTRS() const noexcept;

    [[nodiscard]] bool IsIdentity(float epsilon_ = Mathf::Epsilon) const noexcept;

    [[nodiscard]] static bool IsApproximately(const Matrix4x4& lhs_, const Matrix4x4& rhs_, float epsilon_ = Mathf::Epsilon) noexcept;

    void SetTRS(const Vector3D& position_, const Quaternion& rotation_, const Vector3D& scale_) noexcept;

    [[nodiscard]] static Matrix4x4 GetIdentity() noexcept;
    [[nodiscard]] static Matrix4x4 GetZero() noexcept;
    [[nodiscard]] static Matrix4x4 Translate(const Vector3D& translation_) noexcept;
    [[nodiscard]] static Matrix4x4 Rotate(const Quaternion& rotation_) noexcept;
    [[nodiscard]] static Matrix4x4 Scale(const Vector3D& scale_) noexcept;
    [[nodiscard]] static Matrix4x4 TRS(const Vector3D& translation_, const Quaternion& rotation_, const Vector3D& scale_) noexcept;
    [[nodiscard]] static Matrix4x4 LookAt(const Vector3D& from_, const Vector3D& to_, const Vector3D& up_) noexcept;
    [[nodiscard]] static Matrix4x4 Perspective(float fovYDegrees_, float aspect_, float nearZ_, float farZ_) noexcept;
    [[nodiscard]] static Matrix4x4 Ortho(float left_, float right_, float bottom_, float top_, float nearZ_, float farZ_) noexcept;
    [[nodiscard]] static Matrix4x4 Ortho(float width_, float height_, float nearZ_, float farZ_) noexcept;
    [[nodiscard]] static Matrix4x4 Frustum(float left_, float right_, float bottom_, float top_, float nearZ_, float farZ_) noexcept;

    static bool TryInverse3DAffine(const Matrix4x4& input_, Matrix4x4& result_) noexcept;

    [[nodiscard]] static DirectX::XMMATRIX Load(const Matrix4x4& matrix_) noexcept;
    static void Store(Matrix4x4& destination_, DirectX::XMMATRIX source_) noexcept;
};
