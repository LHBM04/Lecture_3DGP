#pragma once

#include <array>
#include <compare>
#include <concepts>
#include <cstddef>
#include <limits>
#include <type_traits>

#include <DirectXMath.h>

#include "Vector3D.h"
#include "Vector4D.h"

namespace TUK::Framework
{
	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	class Quaternion;

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	class Matrix4x4
	{
	public:
		Matrix4x4() noexcept;
		explicit Matrix4x4(TValue value_) noexcept;
		explicit Matrix4x4(const TValue values_[16]) noexcept;
		explicit Matrix4x4(
			TValue m00_, TValue m01_, TValue m02_, TValue m03_,
			TValue m10_, TValue m11_, TValue m12_, TValue m13_,
			TValue m20_, TValue m21_, TValue m22_, TValue m23_,
			TValue m30_, TValue m31_, TValue m32_, TValue m33_) noexcept;

		Matrix4x4(const Matrix4x4& other_) noexcept;
		Matrix4x4(Matrix4x4&& other_) noexcept;

		explicit Matrix4x4(DirectX::XMMATRIX matrix_) noexcept requires std::same_as<TValue, float>;

		Matrix4x4& operator=(const Matrix4x4& other_) noexcept;
		Matrix4x4& operator=(Matrix4x4&& other_) noexcept;

		[[nodiscard]] TValue operator[](size_t index_) const noexcept;
		[[nodiscard]] TValue& operator[](size_t index_) noexcept;

		[[nodiscard]] Matrix4x4 operator*(const Matrix4x4& other_) const noexcept;
		Matrix4x4& operator*=(const Matrix4x4& other_) noexcept;

		[[nodiscard]] bool operator==(const Matrix4x4& other_) const noexcept;
		[[nodiscard]] bool operator!=(const Matrix4x4& other_) const noexcept;
		[[nodiscard]] std::partial_ordering operator<=>(const Matrix4x4& other_) const noexcept;

		[[nodiscard]] Vector4D<TValue> GetRow(std::size_t index_) const noexcept;
		void SetRow(std::size_t index_, const Vector4D<TValue>& row_) noexcept;

		[[nodiscard]] Vector4D<TValue> GetColumn(std::size_t index_) const noexcept;
		void SetColumn(std::size_t index_, const Vector4D<TValue>& column_) noexcept;

		[[nodiscard]] float GetDeterminant() const noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] Matrix4x4 GetTranspose() const noexcept;
		[[nodiscard]] Matrix4x4 GetInverse() const noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] Vector3D<TValue> GetWorldPosition() const noexcept;
		[[nodiscard]] Quaternion<TValue> GetRotation() const noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] Vector3D<float> GetScale() const noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] Vector3D<float> GetLossyScale() const noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] Vector3D<float> GetForward() const noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] Vector3D<float> GetUp() const noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] Vector3D<float> GetRight() const noexcept requires std::same_as<TValue, float>;

		void SetTRS(const Vector3D<float>& position_, const Quaternion<TValue>& rotation_, const Vector3D<float>& scale_) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] Vector3D<TValue> MultiplyPoint3x4(const Vector3D<TValue>& point_) const noexcept;
		[[nodiscard]] Vector3D<float> MultiplyPoint(const Vector3D<float>& point_) const noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] Vector3D<TValue> MultiplyVector(const Vector3D<TValue>& vector_) const noexcept;

		bool TryGetInverse(Matrix4x4& result_) const noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] bool CanInverse() const noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] bool IsValidTRS() const noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] bool IsIdentity(float epsilon_ = std::numeric_limits<float>::epsilon()) const noexcept;

		[[nodiscard]] static Matrix4x4 GetIdentity() noexcept;
		[[nodiscard]] static Matrix4x4 GetZero() noexcept;

		[[nodiscard]] static DirectX::XMMATRIX Load(const Matrix4x4& matrix_) noexcept requires std::same_as<TValue, float>;
		static void Store(Matrix4x4& destination_, DirectX::XMMATRIX source_) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static bool IsApproximately(const Matrix4x4& lhs_, const Matrix4x4& rhs_, float epsilon_ = std::numeric_limits<float>::epsilon()) noexcept;

		[[nodiscard]] static Matrix4x4 Translate(const Vector3D<TValue>& translation_) noexcept;
		[[nodiscard]] static Matrix4x4 Rotate(const Quaternion<TValue>& rotation_) noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] static Matrix4x4 Scale(const Vector3D<TValue>& scale_) noexcept;
		[[nodiscard]] static Matrix4x4 TRS(const Vector3D<float>& translation_, const Quaternion<TValue>& rotation_, const Vector3D<float>& scale_) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static Matrix4x4 LookAt(const Vector3D<float>& from_, const Vector3D<float>& to_, const Vector3D<float>& up_) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static Matrix4x4 Perspective(float fovYDegrees_, float aspect_, float nearZ_, float farZ_) noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] static Matrix4x4 Frustum(float left_, float right_, float bottom_, float top_, float nearZ_, float farZ_) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static Matrix4x4 Ortho(float left_, float right_, float bottom_, float top_, float nearZ_, float farZ_) noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] static Matrix4x4 Ortho(float width_, float height_, float nearZ_, float farZ_) noexcept requires std::same_as<TValue, float>;

		static bool TryInverse3DAffine(const Matrix4x4& input_, Matrix4x4& result_) noexcept requires std::same_as<TValue, float>;

	private:
		std::conditional_t<std::same_as<TValue, int>, std::array<int, 16>, DirectX::XMFLOAT4X4> value;
	};

	extern template class Matrix4x4<int>;
	extern template class Matrix4x4<float>;
}
