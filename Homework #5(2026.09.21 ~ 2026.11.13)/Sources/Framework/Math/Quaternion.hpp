#pragma once

#include <compare>
#include <concepts>
#include <cstddef>
#include <limits>
#include <type_traits>

#include <directxmath/DirectXMath.h>

#include "Vector3D.hpp"

#include "../Core/Common.hpp"

namespace TUK::Framework
{
	template <ScalarType TScalar>
	class Quaternion
	{
	public:
		Quaternion() noexcept;
		explicit Quaternion(TScalar value) noexcept;
		Quaternion(TScalar x, TScalar y, TScalar z, TScalar w) noexcept;

		Quaternion(const Quaternion& other) noexcept;
		Quaternion(Quaternion&& other) noexcept;

		Quaternion& operator=(const Quaternion& other) noexcept;
		Quaternion& operator=(Quaternion&& other) noexcept;

		[[nodiscard]] TScalar operator[](size_t index) const noexcept;
		[[nodiscard]] TScalar& operator[](size_t index) noexcept;

		[[nodiscard]] Quaternion operator+(const Quaternion& _other) const noexcept;
		[[nodiscard]] Quaternion& operator+=(const Quaternion& _other) noexcept;

		[[nodiscard]] Quaternion operator-(const Quaternion& _other) const noexcept;
		[[nodiscard]] Quaternion& operator-=(const Quaternion& _other) noexcept;

		[[nodiscard]] Quaternion operator*(TScalar _scalar) const noexcept;
		[[nodiscard]] Quaternion operator*(const Quaternion& _other) const noexcept;
		[[nodiscard]] Vector3D<f32> operator*(const Vector3D<f32>& vector) const noexcept requires std::same_as<TScalar, f32>;
		[[nodiscard]] Quaternion& operator*=(TScalar _scalar) noexcept;
		[[nodiscard]] Quaternion& operator*=(const Quaternion& _other) noexcept;

		[[nodiscard]] Quaternion operator/(TScalar _scalar) const noexcept;
		[[nodiscard]] Quaternion& operator/=(TScalar _scalar) noexcept;

		[[nodiscard]] bool operator==(const Quaternion& other) const noexcept;
		[[nodiscard]] bool operator!=(const Quaternion& other) const noexcept;
		[[nodiscard]] std::partial_ordering operator<=>(const Quaternion& other) const noexcept;

		[[nodiscard]] TScalar GetX() const noexcept;
		void SetX(TScalar component) noexcept;

		[[nodiscard]] TScalar GetY() const noexcept;
		void SetY(TScalar component) noexcept;

		[[nodiscard]] TScalar GetZ() const noexcept;
		void SetZ(TScalar component) noexcept;

		[[nodiscard]] TScalar GetW() const noexcept;
		void SetW(TScalar component) noexcept;

		void Set(TScalar x, TScalar y, TScalar z, TScalar w) noexcept;

		[[nodiscard]] Vector3D<f32> GetEulerAngles() const noexcept requires std::same_as<TScalar, f32>;
		void SetEulerAngles(const Vector3D<f32>& eulerDegrees) noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] f32 GetMagnitude() const noexcept requires std::same_as<TScalar, f32>;
		[[nodiscard]] f32 GetSqrMagnitude() const noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] Quaternion GetNormalized() const noexcept requires std::same_as<TScalar, f32>;

		void SetFromToRotation(const Vector3D<f32>& from, const Vector3D<f32>& to) noexcept requires std::same_as<TScalar, f32>;
		void SetLookRotation(const Vector3D<f32>& view) noexcept requires std::same_as<TScalar, f32>;
		void SetLookRotation(const Vector3D<f32>& view, const Vector3D<f32>& up) noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] bool IsFinite() const noexcept;

		[[nodiscard]] bool IsNormalized(f32 epsilon = std::numeric_limits<f32>::epsilon()) const noexcept requires std::same_as<TScalar, f32>;

		void Normalize() noexcept requires std::same_as<TScalar, f32>;

		void ToAngleAxis(f32& angleDegrees, Vector3D<f32>& axis) const noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] static Quaternion Normalize(const Quaternion& rotation) noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] static Quaternion GetIdentity() noexcept;

		[[nodiscard]] static DirectX::XMVECTOR Load(const Quaternion& quat) noexcept requires std::same_as<TScalar, f32>;
		static void Store(Quaternion& d, DirectX::XMVECTOR s) noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] static bool IsApproximately(const Quaternion& lhs, const Quaternion& rhs, f32 epsilon = std::numeric_limits<f32>::epsilon()) noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] static f32 Angle(const Quaternion& a, const Quaternion& b) noexcept requires std::same_as<TScalar, f32>;
		[[nodiscard]] static Quaternion AngleAxis(f32 angleDegrees, Vector3D<f32> axis) noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] static Quaternion Euler(f32 xDegrees, f32 yDegrees, f32 zDegrees) noexcept requires std::same_as<TScalar, f32>;
		[[nodiscard]] static Quaternion Euler(const Vector3D<f32>& eulerDegrees) noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] static Quaternion Inverse(const Quaternion& rotation) noexcept requires std::same_as<TScalar, f32>;
		[[nodiscard]] static Quaternion Conjugate(const Quaternion& rotation) noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] static f32 Dot(const Quaternion& a, const Quaternion& b) noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] static Quaternion Lerp(const Quaternion& a, const Quaternion& b, f32 t) noexcept requires std::same_as<TScalar, f32>;
		[[nodiscard]] static Quaternion LerpUnclamped(const Quaternion& a, const Quaternion& b, f32 t) noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] static Quaternion Slerp(const Quaternion& a, const Quaternion& b, f32 t) noexcept requires std::same_as<TScalar, f32>;
		[[nodiscard]] static Quaternion SlerpUnclamped(const Quaternion& a, const Quaternion& b, f32 t) noexcept requires std::same_as<TScalar, f32>;

		[[nodiscard]] static Quaternion FromToRotation(const Vector3D<f32>& fromDirection, const Vector3D<f32>& toDirection) noexcept requires std::same_as<TScalar, f32>;
		[[nodiscard]] static Quaternion LookRotation(const Vector3D<f32>& forward, const Vector3D<f32>& up) noexcept requires std::same_as<TScalar, f32>;
		[[nodiscard]] static Quaternion RotateTowards(const Quaternion& from, const Quaternion& to, f32 maxDegreesDelta) noexcept requires std::same_as<TScalar, f32>;

	private:
		std::conditional_t<std::same_as<TScalar, i32>, DirectX::XMINT4, DirectX::XMFLOAT4> value;
	};

	extern template class Quaternion<i32>;
	extern template class Quaternion<f32>;
}
