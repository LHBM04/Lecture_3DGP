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

inline Matrix4x4::Matrix4x4() noexcept
{
	Store(*this, DirectX::XMMatrixIdentity());
}

inline Matrix4x4::Matrix4x4(float value_) noexcept
	: DirectX::XMFLOAT4X4(
		value_, 0.0f, 0.0f, 0.0f,
		0.0f, value_, 0.0f, 0.0f,
		0.0f, 0.0f, value_, 0.0f,
		0.0f, 0.0f, 0.0f, value_)
{
}

inline Matrix4x4::Matrix4x4(const float values_[16]) noexcept
	: DirectX::XMFLOAT4X4(
		values_[0], values_[1], values_[2], values_[3],
		values_[4], values_[5], values_[6], values_[7],
		values_[8], values_[9], values_[10], values_[11],
		values_[12], values_[13], values_[14], values_[15])
{
}

inline Matrix4x4::Matrix4x4(
	float m00_, float m01_, float m02_, float m03_,
	float m10_, float m11_, float m12_, float m13_,
	float m20_, float m21_, float m22_, float m23_,
	float m30_, float m31_, float m32_, float m33_) noexcept
	: DirectX::XMFLOAT4X4(
		m00_, m01_, m02_, m03_,
		m10_, m11_, m12_, m13_,
		m20_, m21_, m22_, m23_,
		m30_, m31_, m32_, m33_)
{
}

inline Matrix4x4::Matrix4x4(DirectX::XMMATRIX matrix_) noexcept
{
	Store(*this, matrix_);
}

inline Matrix4x4::Matrix4x4(const Matrix4x4& other_) noexcept
	: DirectX::XMFLOAT4X4(other_)
{
}

inline Matrix4x4::Matrix4x4(Matrix4x4&& other_) noexcept
	: DirectX::XMFLOAT4X4(other_)
{
}

inline Matrix4x4 Matrix4x4::operator*(const Matrix4x4& other_) const noexcept
{
	return Matrix4x4(DirectX::XMMatrixMultiply(Load(*this), Load(other_)));
}

inline Matrix4x4& Matrix4x4::operator*=(const Matrix4x4& other_) noexcept
{
	Store(*this, DirectX::XMMatrixMultiply(Load(*this), Load(other_)));
	return *this;
}

inline Matrix4x4& Matrix4x4::operator=(const Matrix4x4& other_) noexcept
{
	const float* source{ &other_._11 };
	float* destination{ &_11 };
	for (size_t index{ 0 }; index < 16; ++index)
	{
		destination[index] = source[index];
	}

	return *this;
}

inline Matrix4x4& Matrix4x4::operator=(Matrix4x4&& other_) noexcept
{
	return *this = other_;
}

inline bool Matrix4x4::operator==(const Matrix4x4& other_) const noexcept
{
	return (*this <=> other_) == std::partial_ordering::equivalent;
}

inline bool Matrix4x4::operator!=(const Matrix4x4& other_) const noexcept
{
	return !(*this == other_);
}

inline std::partial_ordering Matrix4x4::operator<=>(const Matrix4x4& other_) const noexcept
{
	const float* lhs{ &_11 };
	const float* rhs{ &other_._11 };
	for (size_t index{ 0 }; index < 16; ++index)
	{
		if (const auto order{ lhs[index] <=> rhs[index] }; order != 0)
		{
			return order;
		}
	}

	return std::partial_ordering::equivalent;
}

inline float Matrix4x4::operator[](size_t index_) const noexcept
{
	return (&_11)[index_];
}

inline float& Matrix4x4::operator[](size_t index_) noexcept
{
	return (&_11)[index_];
}

inline float Matrix4x4::GetDeterminant() const noexcept
{
	return DirectX::XMVectorGetX(DirectX::XMMatrixDeterminant(Load(*this)));
}

inline Matrix4x4 Matrix4x4::GetTranspose() const noexcept
{
	return Matrix4x4(DirectX::XMMatrixTranspose(Load(*this)));
}

inline Matrix4x4 Matrix4x4::GetInverse() const noexcept
{
	return Matrix4x4(DirectX::XMMatrixInverse(nullptr, Load(*this)));
}

inline bool Matrix4x4::TryGetInverse(Matrix4x4& result_) const noexcept
{
	DirectX::XMVECTOR determinant;
	const DirectX::XMMATRIX inverse{ DirectX::XMMatrixInverse(&determinant, Load(*this)) };
	if (DirectX::XMVector4Equal(determinant, DirectX::XMVectorZero()))
	{
		return false;
	}

	Store(result_, inverse);
	return true;
}

inline bool Matrix4x4::CanInverse() const noexcept
{
	return std::abs(GetDeterminant()) > Mathf::Epsilon;
}

inline bool Matrix4x4::IsValidTRS() const noexcept
{
	const bool affine = std::abs(_14) <= Mathf::Epsilon
		&& std::abs(_24) <= Mathf::Epsilon
		&& std::abs(_34) <= Mathf::Epsilon
		&& std::abs(_44 - 1.0f) <= Mathf::Epsilon;

	const float* m{ &_11 };
	for (std::size_t i{ 0 }; i < 16; ++i)
	{
		if (!std::isfinite(m[i]))
		{
			return false;
		}
	}

	return affine;
}

inline bool Matrix4x4::IsIdentity(float epsilon_) const noexcept
{
	return IsApproximately(*this, GetIdentity(), epsilon_);
}

inline bool Matrix4x4::IsApproximately(const Matrix4x4& lhs_, const Matrix4x4& rhs_, float epsilon_) noexcept
{
	const float* a{ &lhs_._11 };
	const float* b{ &rhs_._11 };
	for (std::size_t i{ 0 }; i < 16; ++i)
	{
		if (std::abs(a[i] - b[i]) > epsilon_)
		{
			return false;
		}
	}
	return true;
}

inline Matrix4x4 Matrix4x4::GetIdentity() noexcept
{
	return Matrix4x4(DirectX::XMMatrixIdentity());
}

inline Matrix4x4 Matrix4x4::GetZero() noexcept
{
	return Matrix4x4(0.0f);
}

inline bool Matrix4x4::TryInverse3DAffine(const Matrix4x4& input_, Matrix4x4& result_) noexcept
{
	DirectX::XMVECTOR determinant;
	const DirectX::XMMATRIX inverse{ DirectX::XMMatrixInverse(&determinant, Load(input_)) };
	if (DirectX::XMVector4Equal(determinant, DirectX::XMVectorZero()))
	{
		return false;
	}

	Store(result_, inverse);
	return true;
}

inline DirectX::XMMATRIX Matrix4x4::Load(const Matrix4x4& matrix_) noexcept
{
	return DirectX::XMLoadFloat4x4(&matrix_);
}

inline void Matrix4x4::Store(Matrix4x4& destination_, DirectX::XMMATRIX source_) noexcept
{
	DirectX::XMStoreFloat4x4(&destination_, source_);
}

#include "Vector3D.h"
#include "Vector4D.h"
#include "Quaternion.h"

inline Vector3D Matrix4x4::MultiplyPoint3x4(const Vector3D& point_) const noexcept
{
	Vector3D result;
	Vector3D::Store(result, DirectX::XMVector3Transform(Vector3D::Load(point_), Load(*this)));
	return result;
}

inline Vector3D Matrix4x4::MultiplyPoint(const Vector3D& point_) const noexcept
{
	Vector3D result;
	Vector3D::Store(result, DirectX::XMVector3TransformCoord(Vector3D::Load(point_), Load(*this)));
	return result;
}

inline Vector3D Matrix4x4::MultiplyVector(const Vector3D& vector_) const noexcept
{
	Vector3D result;
	Vector3D::Store(result, DirectX::XMVector3TransformNormal(Vector3D::Load(vector_), Load(*this)));
	return result;
}

inline Vector3D Matrix4x4::GetWorldPosition() const noexcept
{
	return Vector3D(_41, _42, _43);
}

inline Vector3D Matrix4x4::GetScale() const noexcept
{
	const Vector3D xAxis(_11, _12, _13);
	const Vector3D yAxis(_21, _22, _23);
	const Vector3D zAxis(_31, _32, _33);
	return Vector3D(xAxis.GetMagnitude(), yAxis.GetMagnitude(), zAxis.GetMagnitude());
}

inline Vector3D Matrix4x4::GetLossyScale() const noexcept
{
	return GetScale();
}

inline Vector4D Matrix4x4::GetRow(std::size_t index_) const noexcept
{
	const float* base{ &_11 };
	return Vector4D(base[index_ * 4 + 0], base[index_ * 4 + 1], base[index_ * 4 + 2], base[index_ * 4 + 3]);
}

inline Vector4D Matrix4x4::GetColumn(std::size_t index_) const noexcept
{
	switch (index_)
	{
	case 0: return Vector4D(_11, _21, _31, _41);
	case 1: return Vector4D(_12, _22, _32, _42);
	case 2: return Vector4D(_13, _23, _33, _43);
	default: return Vector4D(_14, _24, _34, _44);
	}
}

inline void Matrix4x4::SetRow(std::size_t index_, const Vector4D& row_) noexcept
{
	float* base{ &_11 };
	base[index_ * 4 + 0] = row_.x;
	base[index_ * 4 + 1] = row_.y;
	base[index_ * 4 + 2] = row_.z;
	base[index_ * 4 + 3] = row_.w;
}

inline void Matrix4x4::SetColumn(std::size_t index_, const Vector4D& column_) noexcept
{
	switch (index_)
	{
	case 0: _11 = column_.x; _21 = column_.y; _31 = column_.z; _41 = column_.w; return;
	case 1: _12 = column_.x; _22 = column_.y; _32 = column_.z; _42 = column_.w; return;
	case 2: _13 = column_.x; _23 = column_.y; _33 = column_.z; _43 = column_.w; return;
	default: _14 = column_.x; _24 = column_.y; _34 = column_.z; _44 = column_.w; return;
	}
}

inline Quaternion Matrix4x4::GetRotation() const noexcept
{
	const Vector3D scale{ GetScale() };
	if (scale.x <= Mathf::Epsilon || scale.y <= Mathf::Epsilon || scale.z <= Mathf::Epsilon)
	{
		return Quaternion::GetIdentity();
	}

	DirectX::XMMATRIX rotation{ Load(*this) };
	rotation.r[0] = DirectX::XMVectorScale(rotation.r[0], 1.0f / scale.x);
	rotation.r[1] = DirectX::XMVectorScale(rotation.r[1], 1.0f / scale.y);
	rotation.r[2] = DirectX::XMVectorScale(rotation.r[2], 1.0f / scale.z);
	rotation.r[3] = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	Quaternion result;
	Quaternion::Store(result, DirectX::XMQuaternionRotationMatrix(rotation));
	return result;
}

inline void Matrix4x4::SetTRS(const Vector3D& position_, const Quaternion& rotation_, const Vector3D& scale_) noexcept
{
	*this = TRS(position_, rotation_, scale_);
}

inline Matrix4x4 Matrix4x4::Translate(const Vector3D& translation_) noexcept
{
	return Matrix4x4(DirectX::XMMatrixTranslation(translation_.x, translation_.y, translation_.z));
}

inline Matrix4x4 Matrix4x4::Rotate(const Quaternion& rotation_) noexcept
{
	return Matrix4x4(DirectX::XMMatrixRotationQuaternion(Quaternion::Load(rotation_)));
}

inline Matrix4x4 Matrix4x4::Scale(const Vector3D& scale_) noexcept
{
	return Matrix4x4(DirectX::XMMatrixScaling(scale_.x, scale_.y, scale_.z));
}

inline Matrix4x4 Matrix4x4::TRS(const Vector3D& translation_, const Quaternion& rotation_, const Vector3D& scale_) noexcept
{
	return Scale(scale_) * Rotate(rotation_) * Translate(translation_);
}

inline Matrix4x4 Matrix4x4::LookAt(const Vector3D& from_, const Vector3D& to_, const Vector3D& up_) noexcept
{
	return Matrix4x4(DirectX::XMMatrixLookAtLH(Vector3D::Load(from_), Vector3D::Load(to_), Vector3D::Load(up_)));
}

inline Matrix4x4 Matrix4x4::Perspective(float fovYDegrees_, float aspect_, float nearZ_, float farZ_) noexcept
{
	return Matrix4x4(DirectX::XMMatrixPerspectiveFovLH(fovYDegrees_ * Mathf::Deg2Rad, aspect_, nearZ_, farZ_));
}

inline Matrix4x4 Matrix4x4::Ortho(float left_, float right_, float bottom_, float top_, float nearZ_, float farZ_) noexcept
{
	return Matrix4x4(DirectX::XMMatrixOrthographicOffCenterLH(left_, right_, bottom_, top_, nearZ_, farZ_));
}

inline Matrix4x4 Matrix4x4::Ortho(float width_, float height_, float nearZ_, float farZ_) noexcept
{
	return Matrix4x4(DirectX::XMMatrixOrthographicLH(width_, height_, nearZ_, farZ_));
}

inline Matrix4x4 Matrix4x4::Frustum(float left_, float right_, float bottom_, float top_, float nearZ_, float farZ_) noexcept
{
	return Matrix4x4(DirectX::XMMatrixPerspectiveOffCenterLH(left_, right_, bottom_, top_, nearZ_, farZ_));
}
