#pragma once

#include <compare>
#include <concepts>
#include <cstddef>
#include <limits>
#include <type_traits>

#include <directxmath/DirectXMath.h>

#include "../Core/Common.hpp"

namespace TUK::Framework
{
	template <ScalarType TScalar>
	class Vector2D;

	template <ScalarType TScalar>
	class Vector4D;

	template <ScalarType TScalar>
	class Vector3D
	{
	public:
		Vector3D() noexcept;
		explicit Vector3D(TScalar scalar) noexcept;
		Vector3D(TScalar x, TScalar y, TScalar z) noexcept;

		Vector3D(const Vector3D& other) noexcept;
		Vector3D(Vector3D&& other) noexcept;

		explicit Vector3D(const Vector2D<TScalar>& vector, TScalar z = TScalar{}) noexcept;
		explicit Vector3D(const Vector4D<TScalar>& vector) noexcept;

		explicit Vector3D(DirectX::XMVECTOR vector) noexcept requires std::same_as<TScalar, f32>;

		Vector3D& operator=(const Vector3D& other) noexcept;
		Vector3D& operator=(Vector3D&& other) noexcept;

		[[nodiscard]] operator Vector2D<TScalar>() const noexcept;
		[[nodiscard]] operator Vector4D<TScalar>() const noexcept;

		[[nodiscard]] TScalar operator[](std::size_t index) const noexcept;
		[[nodiscard]] TScalar& operator[](std::size_t index) noexcept;

		[[nodiscard]] Vector3D operator+() const noexcept;
		[[nodiscard]] Vector3D operator+(const Vector3D& other) const noexcept;
		Vector3D& operator+=(const Vector3D& other) noexcept;

		[[nodiscard]] Vector3D operator-() const noexcept;
		[[nodiscard]] Vector3D operator-(const Vector3D& other) const noexcept;
		Vector3D& operator-=(const Vector3D& other) noexcept;

		[[nodiscard]] Vector3D operator*(TScalar scalar) const noexcept;
		Vector3D& operator*=(TScalar scalar) noexcept;

		[[nodiscard]] Vector3D operator/(TScalar scalar) const noexcept;
		Vector3D& operator/=(TScalar scalar) noexcept;

		[[nodiscard]] bool operator==(const Vector3D& other) const noexcept;
		[[nodiscard]] bool operator!=(const Vector3D& other) const noexcept;
		[[nodiscard]] std::partial_ordering operator<=>(const Vector3D& other) const noexcept;

		[[nodiscard]] TScalar GetX() const noexcept;
		void SetX(TScalar x) noexcept;

		[[nodiscard]] TScalar GetY() const noexcept;
		void SetY(TScalar y) noexcept;

		[[nodiscard]] TScalar GetZ() const noexcept;
		void SetZ(TScalar z) noexcept;

		void Set(TScalar x, TScalar y, TScalar z) noexcept;

		[[nodiscard]] f32 GetMagnitude() const noexcept requires std::same_as<TScalar, f32>;
		[[nodiscard]] f32 GetSqrMagnitude() const noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] f32 GetLength() const noexcept requires std::same_as<TScalar, f32>;
		[[nodiscard]] f32 GetLengthSquared() const noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] Vector3D GetNormalized() const noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] bool IsZero(f32 epsilon = std::numeric_limits<f32>::epsilon()) const noexcept;
		[[nodiscard]] bool IsFinite() const noexcept;

		void Normalize() noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] f32 Dot(const Vector3D& other) const noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] static Vector3D GetZero() noexcept;
		[[nodiscard]] static Vector3D GetOne() noexcept;

		[[nodiscard]] static Vector3D GetUp() noexcept;
		[[nodiscard]] static Vector3D GetDown() noexcept;

		[[nodiscard]] static Vector3D GetLeft() noexcept;
		[[nodiscard]] static Vector3D GetRight() noexcept;

		[[nodiscard]] static Vector3D GetForward() noexcept;
		[[nodiscard]] static Vector3D GetBack() noexcept;

		[[nodiscard]] static Vector3D GetPositiveInfinity() noexcept requires std::same_as<TScalar, f32>;
		[[nodiscard]] static Vector3D GetNegativeInfinity() noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] static DirectX::XMVECTOR Load(const Vector3D& vec) noexcept requires std::same_as<TScalar, f32>;
		static void Store(Vector3D& d, DirectX::XMVECTOR s) noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] static bool IsApproximately(const Vector3D& lhs, const Vector3D& rhs, f32 epsilon = std::numeric_limits<f32>::epsilon()) noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] static Vector3D Max(const Vector3D& a, const Vector3D& b) noexcept requires std::same_as<TScalar, f32>;
		[[nodiscard]] static Vector3D Min(const Vector3D& a, const Vector3D& b) noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] static Vector3D Clamp(const Vector3D& value, const Vector3D& min, const Vector3D& max) noexcept requires std::same_as<TScalar, f32>;
		[[nodiscard]] static Vector3D ClampMagnitude(const Vector3D& vector, f32 maxLength) noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] static Vector3D Scale(const Vector3D& vector, const Vector3D& scale) noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] static Vector3D Normalize(const Vector3D& value) noexcept requires std::same_as<TScalar, f32>;
		static void OrthoNormalize(Vector3D& normal, Vector3D& tangent) noexcept requires std::same_as<TScalar, f32>;
		static void OrthoNormalize(Vector3D& normal, Vector3D& tangent, Vector3D& binormal) noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] static f32 Dot(const Vector3D& a, const Vector3D& b) noexcept requires std::same_as<TScalar, f32>;
		[[nodiscard]] static Vector3D Cross(const Vector3D& a, const Vector3D& b) noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] static f32 Distance(const Vector3D& _v1, const Vector3D& _v2) noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] static f32 Angle(const Vector3D& a, const Vector3D& b) noexcept requires std::same_as<TScalar, f32>;
		[[nodiscard]] static f32 SignedAngle(const Vector3D& from, const Vector3D& to, const Vector3D& axis) noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] static Vector3D Project(const Vector3D& vector, const Vector3D& onNormal) noexcept requires std::same_as<TScalar, f32>;
		[[nodiscard]] static Vector3D ProjectOnPlane(const Vector3D& vector, const Vector3D& planeNormal) noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] static Vector3D Reflect(const Vector3D& _vector, const Vector3D& _normal) noexcept requires std::same_as<TScalar, f32>;
		[[nodiscard]] static Vector3D Refract(const Vector3D& _vector, const Vector3D& _normal, f32 _eta) noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] static Vector3D Lerp(const Vector3D& a, const Vector3D& b, f32 t) noexcept requires std::same_as<TScalar, f32>;
		[[nodiscard]] static Vector3D LerpUnclamped(const Vector3D& a, const Vector3D& b, f32 t) noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] static Vector3D Slerp(const Vector3D& a, const Vector3D& b, f32 t) noexcept requires std::same_as<TScalar, f32>;
		[[nodiscard]] static Vector3D SlerpUnclamped(const Vector3D& a, const Vector3D& b, f32 t) noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] static Vector3D MoveTowards(const Vector3D& current, const Vector3D& target, f32 maxDistanceDelta) noexcept requires std::same_as<TScalar, f32>;
		[[nodiscard]] static Vector3D RotateTowards(const Vector3D& current, const Vector3D& target, f32 maxRadiansDelta, f32 maxMagnitudeDelta) noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] static Vector3D SmoothDamp(
			const Vector3D& current,
			const Vector3D& target,
			Vector3D& currentVelocity,
			f32 smoothTime,
			f32 maxSpeed,
			f32 deltaTime) noexcept requires std::same_as<TScalar, f32>;

	private:
		std::conditional_t<std::same_as<TScalar, i32>, DirectX::XMINT3, DirectX::XMFLOAT3> value;
	};

	extern template class Vector3D<i32>;
	extern template class Vector3D<f32>;

	[[nodiscard]] Vector3D<i32> operator*(i32 scalar, const Vector3D<i32>& vector) noexcept;
	[[nodiscard]] Vector3D<f32> operator*(f32 scalar, const Vector3D<f32>& vector) noexcept;
}
