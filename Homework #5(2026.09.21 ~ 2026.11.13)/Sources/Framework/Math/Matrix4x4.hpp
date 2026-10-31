#pragma once

#include <array>
#include <compare>
#include <concepts>
#include <cstddef>
#include <limits>
#include <type_traits>

#include <directxmath/DirectXMath.h>

#include "Vector3D.hpp"
#include "Vector4D.hpp"

#include "../Core/Common.hpp"

namespace TUK::Framework
{
	template <ScalarType TScalar>
	class Quaternion;

	template <ScalarType TScalar>
	class Matrix4x4
	{
	public:
		Matrix4x4() noexcept;
		explicit Matrix4x4(TScalar value) noexcept;
		explicit Matrix4x4(const TScalar values[16]) noexcept;
		explicit Matrix4x4(
			TScalar m00, TScalar m01, TScalar m02, TScalar m03,
			TScalar m10, TScalar m11, TScalar m12, TScalar m13,
			TScalar m20, TScalar m21, TScalar m22, TScalar m23,
			TScalar m30, TScalar m31, TScalar m32, TScalar m33) noexcept;

		Matrix4x4(const Matrix4x4& other) noexcept;
		Matrix4x4(Matrix4x4&& other) noexcept;

		explicit Matrix4x4(DirectX::XMMATRIX matrix) noexcept requires std::same_as<TScalar, f32>;

		Matrix4x4& operator=(const Matrix4x4& other) noexcept;
		Matrix4x4& operator=(Matrix4x4&& other) noexcept;

		[[nodiscard]] TScalar operator[](size_t index) const noexcept;
		[[nodiscard]] TScalar& operator[](size_t index) noexcept;

		[[nodiscard]] Matrix4x4 operator*(const Matrix4x4& other) const noexcept;
		Matrix4x4& operator*=(const Matrix4x4& other) noexcept;

		[[nodiscard]] bool operator==(const Matrix4x4& other) const noexcept;
		[[nodiscard]] bool operator!=(const Matrix4x4& other) const noexcept;
		[[nodiscard]] std::partial_ordering operator<=>(const Matrix4x4& other) const noexcept;

		[[nodiscard]] Vector4D<TScalar> GetRow(std::size_t index) const noexcept;
		void SetRow(std::size_t index, const Vector4D<TScalar>& row) noexcept;

		[[nodiscard]] Vector4D<TScalar> GetColumn(std::size_t index) const noexcept;
		void SetColumn(std::size_t index, const Vector4D<TScalar>& column) noexcept;

		[[nodiscard]] f32 GetDeterminant() const noexcept requires std::same_as<TScalar, f32>;
		[[nodiscard]] Matrix4x4 GetTranspose() const noexcept;
		[[nodiscard]] Matrix4x4 GetInverse() const noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] Vector3D<TScalar> GetWorldPosition() const noexcept;
		[[nodiscard]] Quaternion<TScalar> GetRotation() const noexcept requires std::same_as<TScalar, f32>;
		[[nodiscard]] Vector3D<f32> GetScale() const noexcept requires std::same_as<TScalar, f32>;
		[[nodiscard]] Vector3D<f32> GetLossyScale() const noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] Vector3D<f32> GetForward() const noexcept requires std::same_as<TScalar, f32>;
		[[nodiscard]] Vector3D<f32> GetUp() const noexcept requires std::same_as<TScalar, f32>;
		[[nodiscard]] Vector3D<f32> GetRight() const noexcept requires std::same_as<TScalar, f32>;

		void SetTRS(const Vector3D<f32>& position, const Quaternion<TScalar>& rotation, const Vector3D<f32>& scale) noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] Vector3D<TScalar> MultiplyPoint3x4(const Vector3D<TScalar>& point) const noexcept;
		[[nodiscard]] Vector3D<f32> MultiplyPoint(const Vector3D<f32>& point) const noexcept requires std::same_as<TScalar, f32>;
		[[nodiscard]] Vector3D<TScalar> MultiplyVector(const Vector3D<TScalar>& vector) const noexcept;

		[[nodiscard]] bool TryDecompose(Vector3D<f32>& position, Quaternion<TScalar>& rotation, Vector3D<f32>& scale) const noexcept requires std::same_as<TScalar, f32>;

		bool TryGetInverse(Matrix4x4& result) const noexcept requires std::same_as<TScalar, f32>;
		[[nodiscard]] bool CanInverse() const noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] bool IsValidTRS() const noexcept requires std::same_as<TScalar, f32>;
		[[nodiscard]] bool IsIdentity(f32 epsilon = std::numeric_limits<f32>::epsilon()) const noexcept;

		[[nodiscard]] static Matrix4x4 GetIdentity() noexcept;
		[[nodiscard]] static Matrix4x4 GetZero() noexcept;

		[[nodiscard]] static DirectX::XMMATRIX Load(const Matrix4x4& matrix) noexcept requires std::same_as<TScalar, f32>;
		static void Store(Matrix4x4& destination, DirectX::XMMATRIX source) noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] static bool IsApproximately(const Matrix4x4& lhs, const Matrix4x4& rhs, f32 epsilon = std::numeric_limits<f32>::epsilon()) noexcept;

		[[nodiscard]] static Matrix4x4 Translate(const Vector3D<TScalar>& translation) noexcept;
		[[nodiscard]] static Matrix4x4 Rotate(const Quaternion<TScalar>& rotation) noexcept requires std::same_as<TScalar, f32>;
		[[nodiscard]] static Matrix4x4 Scale(const Vector3D<TScalar>& scale) noexcept;
		[[nodiscard]] static Matrix4x4 TRS(const Vector3D<f32>& translation, const Quaternion<TScalar>& rotation, const Vector3D<f32>& scale) noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] static Matrix4x4 LookAt(const Vector3D<f32>& from, const Vector3D<f32>& to, const Vector3D<f32>& up) noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] static Matrix4x4 Perspective(f32 fovYDegrees, f32 aspect, f32 nearZ, f32 farZ) noexcept requires std::same_as<TScalar, f32>;
		[[nodiscard]] static Matrix4x4 Frustum(f32 left, f32 right, f32 bottom, f32 top, f32 nearZ, f32 farZ) noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] static Matrix4x4 Ortho(f32 left, f32 right, f32 bottom, f32 top, f32 nearZ, f32 farZ) noexcept requires std::same_as<TScalar, f32>;
		[[nodiscard]] static Matrix4x4 Ortho(f32 width, f32 height, f32 nearZ, f32 farZ) noexcept requires std::same_as<TScalar, f32>;

		static bool TryInverse3DAffine(const Matrix4x4& input, Matrix4x4& result) noexcept requires std::same_as<TScalar, f32>;

	private:
		std::conditional_t<std::same_as<TScalar, i32>, std::array<i32, 16>, DirectX::XMFLOAT4X4> value;
	};

	extern template class Matrix4x4<i32>;
	extern template class Matrix4x4<f32>;
}
