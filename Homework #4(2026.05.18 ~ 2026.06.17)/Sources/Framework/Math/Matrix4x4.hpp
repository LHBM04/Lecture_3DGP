#pragma once

#include <array>
#include <compare>
#include <concepts>
#include <cstddef>
#include <limits>
#include <type_traits>

#include <DirectXMath.h>

#include "Vector3D.hpp"
#include "Vector4D.hpp"

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
		explicit Matrix4x4(TValue value) noexcept;
		explicit Matrix4x4(const TValue values[16]) noexcept;
		explicit Matrix4x4(
			TValue m00, TValue m01, TValue m02, TValue m03,
			TValue m10, TValue m11, TValue m12, TValue m13,
			TValue m20, TValue m21, TValue m22, TValue m23,
			TValue m30, TValue m31, TValue m32, TValue m33) noexcept;

		Matrix4x4(const Matrix4x4& other) noexcept;
		Matrix4x4(Matrix4x4&& other) noexcept;

		explicit Matrix4x4(DirectX::XMMATRIX matrix) noexcept requires std::same_as<TValue, float>;

		Matrix4x4& operator=(const Matrix4x4& other) noexcept;
		Matrix4x4& operator=(Matrix4x4&& other) noexcept;

		[[nodiscard]] TValue operator[](size_t index) const noexcept;
		[[nodiscard]] TValue& operator[](size_t index) noexcept;

		[[nodiscard]] Matrix4x4 operator*(const Matrix4x4& other) const noexcept;
		Matrix4x4& operator*=(const Matrix4x4& other) noexcept;

		[[nodiscard]] bool operator==(const Matrix4x4& other) const noexcept;
		[[nodiscard]] bool operator!=(const Matrix4x4& other) const noexcept;
		[[nodiscard]] std::partial_ordering operator<=>(const Matrix4x4& other) const noexcept;

		[[nodiscard]] Vector4D<TValue> GetRow(std::size_t index) const noexcept;
		void SetRow(std::size_t index, const Vector4D<TValue>& row) noexcept;

		[[nodiscard]] Vector4D<TValue> GetColumn(std::size_t index) const noexcept;
		void SetColumn(std::size_t index, const Vector4D<TValue>& column) noexcept;

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

		void SetTRS(const Vector3D<float>& position, const Quaternion<TValue>& rotation, const Vector3D<float>& scale) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] Vector3D<TValue> MultiplyPoint3x4(const Vector3D<TValue>& point) const noexcept;
		[[nodiscard]] Vector3D<float> MultiplyPoint(const Vector3D<float>& point) const noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] Vector3D<TValue> MultiplyVector(const Vector3D<TValue>& vector) const noexcept;

		bool TryGetInverse(Matrix4x4& result) const noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] bool CanInverse() const noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] bool IsValidTRS() const noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] bool IsIdentity(float epsilon = std::numeric_limits<float>::epsilon()) const noexcept;

		[[nodiscard]] static Matrix4x4 GetIdentity() noexcept;
		[[nodiscard]] static Matrix4x4 GetZero() noexcept;

		[[nodiscard]] static DirectX::XMMATRIX Load(const Matrix4x4& matrix) noexcept requires std::same_as<TValue, float>;
		static void Store(Matrix4x4& destination, DirectX::XMMATRIX source) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static bool IsApproximately(const Matrix4x4& lhs, const Matrix4x4& rhs, float epsilon = std::numeric_limits<float>::epsilon()) noexcept;

		[[nodiscard]] static Matrix4x4 Translate(const Vector3D<TValue>& translation) noexcept;
		[[nodiscard]] static Matrix4x4 Rotate(const Quaternion<TValue>& rotation) noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] static Matrix4x4 Scale(const Vector3D<TValue>& scale) noexcept;
		[[nodiscard]] static Matrix4x4 TRS(const Vector3D<float>& translation, const Quaternion<TValue>& rotation, const Vector3D<float>& scale) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static Matrix4x4 LookAt(const Vector3D<float>& from, const Vector3D<float>& to, const Vector3D<float>& up) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static Matrix4x4 Perspective(float fovYDegrees, float aspect, float nearZ, float farZ) noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] static Matrix4x4 Frustum(float left, float right, float bottom, float top, float nearZ, float farZ) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static Matrix4x4 Ortho(float left, float right, float bottom, float top, float nearZ, float farZ) noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] static Matrix4x4 Ortho(float width, float height, float nearZ, float farZ) noexcept requires std::same_as<TValue, float>;

		static bool TryInverse3DAffine(const Matrix4x4& input, Matrix4x4& result) noexcept requires std::same_as<TValue, float>;

	private:
		std::conditional_t<std::same_as<TValue, int>, std::array<int, 16>, DirectX::XMFLOAT4X4> value;
	};

	extern template class Matrix4x4<int>;
	extern template class Matrix4x4<float>;
}
