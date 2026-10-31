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
	template <class TValue>
		requires ScalarType<TValue>
	class Vector3D;

	template <class TValue>
		requires ScalarType<TValue>
	class Vector4D;

	template <class TValue>
		requires ScalarType<TValue>
	class Vector2D
	{
	public:
		Vector2D() noexcept;
		explicit Vector2D(TValue scalar) noexcept;
		Vector2D(TValue x, TValue y) noexcept;

		Vector2D(const Vector2D& other) noexcept;
		Vector2D(Vector2D&& other) noexcept;

		explicit Vector2D(const Vector3D<TValue>& vector) noexcept;
		explicit Vector2D(const Vector4D<TValue>& vector) noexcept;

		Vector2D& operator=(const Vector2D& other) noexcept;
		Vector2D& operator=(Vector2D&& other) noexcept;

		[[nodiscard]] operator Vector3D<TValue>() const noexcept;
		[[nodiscard]] operator Vector4D<TValue>() const noexcept;

		[[nodiscard]] TValue operator[](std::size_t index) const noexcept;
		[[nodiscard]] TValue& operator[](std::size_t index) noexcept;

		[[nodiscard]] Vector2D operator+() const noexcept;
		[[nodiscard]] Vector2D operator+(const Vector2D& other) const noexcept;
		Vector2D& operator+=(const Vector2D& other) noexcept;

		[[nodiscard]] Vector2D operator-() const noexcept;
		[[nodiscard]] Vector2D operator-(const Vector2D& other) const noexcept;
		Vector2D& operator-=(const Vector2D& other) noexcept;

		[[nodiscard]] Vector2D operator*(TValue scalar) const noexcept;
		Vector2D& operator*=(TValue scalar) noexcept;

		[[nodiscard]] Vector2D operator/(TValue scalar) const noexcept;
		Vector2D& operator/=(TValue scalar) noexcept;

		[[nodiscard]] bool operator==(const Vector2D& other) const noexcept;
		[[nodiscard]] bool operator!=(const Vector2D& other) const noexcept;
		[[nodiscard]] std::partial_ordering operator<=>(const Vector2D& other) const noexcept;

		[[nodiscard]] TValue GetX() const noexcept;
		void SetX(TValue x) noexcept;

		[[nodiscard]] TValue GetY() const noexcept;
		void SetY(TValue y) noexcept;

		void Set(TValue x, TValue y) noexcept;

		[[nodiscard]] f32 GetMagnitude() const noexcept requires std::same_as<TValue, f32>;
		[[nodiscard]] f32 GetSqrMagnitude() const noexcept requires std::same_as<TValue, f32>;

		[[nodiscard]] f32 GetLength() const noexcept requires std::same_as<TValue, f32>;
		[[nodiscard]] f32 GetLengthSquared() const noexcept requires std::same_as<TValue, f32>;

		[[nodiscard]] Vector2D GetNormalized() const noexcept requires std::same_as<TValue, f32>;

		[[nodiscard]] bool IsZero(f32 epsilon = std::numeric_limits<f32>::epsilon()) const noexcept;
		[[nodiscard]] bool IsFinite() const noexcept;

		void Normalize() noexcept requires std::same_as<TValue, f32>;

		[[nodiscard]] f32 Dot(const Vector2D& other) const noexcept requires std::same_as<TValue, f32>;

		[[nodiscard]] static Vector2D GetZero() noexcept;
		[[nodiscard]] static Vector2D GetOne() noexcept;

		[[nodiscard]] static Vector2D GetUp() noexcept;
		[[nodiscard]] static Vector2D GetDown() noexcept;

		[[nodiscard]] static Vector2D GetLeft() noexcept;
		[[nodiscard]] static Vector2D GetRight() noexcept;

		[[nodiscard]] static Vector2D GetPositiveInfinity() noexcept requires std::same_as<TValue, f32>;
		[[nodiscard]] static Vector2D GetNegativeInfinity() noexcept requires std::same_as<TValue, f32>;

		[[nodiscard]] static DirectX::XMVECTOR Load(const Vector2D& vec) noexcept requires std::same_as<TValue, f32>;
		static void Store(Vector2D& d, DirectX::XMVECTOR s) noexcept requires std::same_as<TValue, f32>;

		[[nodiscard]] static bool IsApproximately(const Vector2D& lhs, const Vector2D& rhs, f32 epsilon = std::numeric_limits<f32>::epsilon()) noexcept requires std::same_as<TValue, f32>;

		[[nodiscard]] static Vector2D Max(const Vector2D& a, const Vector2D& b) noexcept requires std::same_as<TValue, f32>;
		[[nodiscard]] static Vector2D Min(const Vector2D& a, const Vector2D& b) noexcept requires std::same_as<TValue, f32>;

		[[nodiscard]] static Vector2D Clamp(const Vector2D& value, const Vector2D& min, const Vector2D& max) noexcept requires std::same_as<TValue, f32>;
		[[nodiscard]] static Vector2D ClampMagnitude(const Vector2D& vector, f32 maxLength) noexcept requires std::same_as<TValue, f32>;

		[[nodiscard]] static Vector2D Scale(const Vector2D& vector, const Vector2D& scale) noexcept requires std::same_as<TValue, f32>;

		[[nodiscard]] static Vector2D Normalize(const Vector2D& value) noexcept requires std::same_as<TValue, f32>;

		[[nodiscard]] static f32 Dot(const Vector2D& a, const Vector2D& b) noexcept requires std::same_as<TValue, f32>;
		[[nodiscard]] static Vector2D Cross(const Vector2D& a, const Vector2D& b) noexcept requires std::same_as<TValue, f32>;

		[[nodiscard]] static Vector2D Perpendicular(const Vector2D& vector) noexcept requires std::same_as<TValue, f32>;

		[[nodiscard]] static f32 Distance(const Vector2D& a, const Vector2D& b) noexcept requires std::same_as<TValue, f32>;

		[[nodiscard]] static f32 Angle(const Vector2D& a, const Vector2D& b) noexcept requires std::same_as<TValue, f32>;
		[[nodiscard]] static f32 SignedAngle(const Vector2D& from, const Vector2D& to) noexcept requires std::same_as<TValue, f32>;

		[[nodiscard]] static Vector2D Reflect(const Vector2D& vector, const Vector2D& normal) noexcept requires std::same_as<TValue, f32>;

		[[nodiscard]] static Vector2D Lerp(const Vector2D& a, const Vector2D& b, f32 t) noexcept requires std::same_as<TValue, f32>;
		[[nodiscard]] static Vector2D LerpUnclamped(const Vector2D& a, const Vector2D& b, f32 t) noexcept requires std::same_as<TValue, f32>;

		[[nodiscard]] static Vector2D Slerp(const Vector2D& a, const Vector2D& b, f32 t) noexcept requires std::same_as<TValue, f32>;
		[[nodiscard]] static Vector2D SlerpUnclamped(const Vector2D& a, const Vector2D& b, f32 t) noexcept requires std::same_as<TValue, f32>;

		[[nodiscard]] static Vector2D MoveTowards(const Vector2D& current, const Vector2D& target, f32 maxDistanceDelta) noexcept requires std::same_as<TValue, f32>;

		[[nodiscard]] static Vector2D SmoothDamp(
			const Vector2D& current,
			const Vector2D& target,
			Vector2D& currentVelocity,
			f32 smoothTime,
			f32 maxSpeed,
			f32 deltaTime) noexcept requires std::same_as<TValue, f32>;

	private:
		std::conditional_t<std::same_as<TValue, i32>, DirectX::XMINT2, DirectX::XMFLOAT2> value;
	};

	extern template class Vector2D<i32>;
	extern template class Vector2D<f32>;

	[[nodiscard]] Vector2D<i32> operator*(i32 scalar, const Vector2D<i32>& vector) noexcept;
	[[nodiscard]] Vector2D<f32> operator*(f32 scalar, const Vector2D<f32>& vector) noexcept;
}
