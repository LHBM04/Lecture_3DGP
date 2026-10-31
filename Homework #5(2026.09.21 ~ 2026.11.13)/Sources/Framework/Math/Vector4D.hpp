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
	class Vector3D;

	template <ScalarType TScalar>
	class Vector4D
	{
	public:
		Vector4D() noexcept;
		explicit Vector4D(TScalar scalar) noexcept;
		Vector4D(TScalar x, TScalar y, TScalar z, TScalar w) noexcept;

		Vector4D(const Vector4D& other) noexcept;
		Vector4D(Vector4D&& other) noexcept;

		explicit Vector4D(const Vector2D<TScalar>& vector, TScalar z = TScalar{}, TScalar w = TScalar{}) noexcept;
		explicit Vector4D(const Vector3D<TScalar>& vector, TScalar w = TScalar{}) noexcept;

		Vector4D& operator=(const Vector4D& other) noexcept;
		Vector4D& operator=(Vector4D&& other) noexcept;

		[[nodiscard]] operator Vector2D<TScalar>() const noexcept;
		[[nodiscard]] operator Vector3D<TScalar>() const noexcept;

		[[nodiscard]] TScalar operator[](std::size_t index) const noexcept;
		[[nodiscard]] TScalar& operator[](std::size_t index) noexcept;

		[[nodiscard]] Vector4D operator+() const noexcept;
		[[nodiscard]] Vector4D operator+(const Vector4D& other) const noexcept;
		Vector4D& operator+=(const Vector4D& other) noexcept;

		[[nodiscard]] Vector4D operator-() const noexcept;
		[[nodiscard]] Vector4D operator-(const Vector4D& other) const noexcept;
		Vector4D& operator-=(const Vector4D& other) noexcept;

		[[nodiscard]] Vector4D operator*(TScalar scalar) const noexcept;
		Vector4D& operator*=(TScalar scalar) noexcept;

		[[nodiscard]] Vector4D operator/(TScalar scalar) const noexcept;
		Vector4D& operator/=(TScalar scalar) noexcept;

		[[nodiscard]] bool operator==(const Vector4D& other) const noexcept;
		[[nodiscard]] bool operator!=(const Vector4D& other) const noexcept;
		[[nodiscard]] std::partial_ordering operator<=>(const Vector4D& other) const noexcept;

		[[nodiscard]] TScalar GetX() const noexcept;
		void SetX(TScalar x) noexcept;

		[[nodiscard]] TScalar GetY() const noexcept;
		void SetY(TScalar y) noexcept;

		[[nodiscard]] TScalar GetZ() const noexcept;
		void SetZ(TScalar z) noexcept;

		[[nodiscard]] TScalar GetW() const noexcept;
		void SetW(TScalar w) noexcept;

		void Set(TScalar x, TScalar y, TScalar z, TScalar w) noexcept;

		[[nodiscard]] f32 GetMagnitude() const noexcept requires std::same_as<TScalar, f32>;
		[[nodiscard]] f32 GetSqrMagnitude() const noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] f32 GetLength() const noexcept requires std::same_as<TScalar, f32>;
		[[nodiscard]] f32 GetLengthSquared() const noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] Vector4D GetNormalized() const noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] bool IsZero(f32 epsilon = std::numeric_limits<f32>::epsilon()) const noexcept;
		[[nodiscard]] bool IsFinite() const noexcept;

		void Normalize() noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] f32 Dot(const Vector4D& other) const noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] static Vector4D GetZero() noexcept;
		[[nodiscard]] static Vector4D GetOne() noexcept;

		[[nodiscard]] static Vector4D GetPositiveInfinity() noexcept requires std::same_as<TScalar, f32>;
		[[nodiscard]] static Vector4D GetNegativeInfinity() noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] static DirectX::XMVECTOR Load(const Vector4D& vec) noexcept requires std::same_as<TScalar, f32>;
		static void Store(Vector4D& d, DirectX::XMVECTOR s) noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] static bool IsApproximately(const Vector4D& lhs, const Vector4D& rhs, f32 epsilon = std::numeric_limits<f32>::epsilon()) noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] static Vector4D Max(const Vector4D& a, const Vector4D& b) noexcept requires std::same_as<TScalar, f32>;
		[[nodiscard]] static Vector4D Min(const Vector4D& a, const Vector4D& b) noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] static Vector4D Clamp(const Vector4D& value, const Vector4D& min, const Vector4D& max) noexcept requires std::same_as<TScalar, f32>;
		[[nodiscard]] static Vector4D ClampMagnitude(const Vector4D& vector, f32 maxLength) noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] static Vector4D Scale(const Vector4D& vector, const Vector4D& scale) noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] static Vector4D Normalize(const Vector4D& value) noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] static f32 Dot(const Vector4D& a, const Vector4D& b) noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] static f32 Distance(const Vector4D& a, const Vector4D& b) noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] static f32 Angle(const Vector4D& a, const Vector4D& b) noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] static Vector4D Project(const Vector4D& vector, const Vector4D& onNormal) noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] static Vector4D Reflect(const Vector4D& vector, const Vector4D& normal) noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] static Vector4D Lerp(const Vector4D& a, const Vector4D& b, f32 t) noexcept requires std::same_as<TScalar, f32>;
		[[nodiscard]] static Vector4D LerpUnclamped(const Vector4D& a, const Vector4D& b, f32 t) noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] static Vector4D Slerp(const Vector4D& a, const Vector4D& b, f32 t) noexcept requires std::same_as<TScalar, f32>;
		[[nodiscard]] static Vector4D SlerpUnclamped(const Vector4D& a, const Vector4D& b, f32 t) noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] static Vector4D MoveTowards(const Vector4D& current, const Vector4D& target, f32 maxDistanceDelta) noexcept requires std::same_as<TScalar, f32>;

	private:
		std::conditional_t<std::same_as<TScalar, i32>, DirectX::XMINT4, DirectX::XMFLOAT4> value;
	};

	extern template class Vector4D<i32>;
	extern template class Vector4D<f32>;

	[[nodiscard]] Vector4D<i32> operator*(i32 scalar, const Vector4D<i32>& vector) noexcept;
	[[nodiscard]] Vector4D<f32> operator*(f32 scalar, const Vector4D<f32>& vector) noexcept;
}
