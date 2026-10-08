#pragma once

#include <compare>
#include <concepts>
#include <cstddef>
#include <limits>
#include <type_traits>

#include <DirectXMath.h>

#include "Vector3D.hpp"

namespace TUK::Framework
{
	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	class Quaternion
	{
	public:
		Quaternion() noexcept;
		explicit Quaternion(TValue value) noexcept;
		Quaternion(TValue x, TValue y, TValue z, TValue w) noexcept;

		Quaternion(const Quaternion& other) noexcept;
		Quaternion(Quaternion&& other) noexcept;

		Quaternion& operator=(const Quaternion& other) noexcept;
		Quaternion& operator=(Quaternion&& other) noexcept;

		[[nodiscard]] TValue operator[](size_t index) const noexcept;
		[[nodiscard]] TValue& operator[](size_t index) noexcept;

		[[nodiscard]] Quaternion operator+(const Quaternion& _other) const noexcept;
		[[nodiscard]] Quaternion& operator+=(const Quaternion& _other) noexcept;

		[[nodiscard]] Quaternion operator-(const Quaternion& _other) const noexcept;
		[[nodiscard]] Quaternion& operator-=(const Quaternion& _other) noexcept;

		[[nodiscard]] Quaternion operator*(TValue _scalar) const noexcept;
		[[nodiscard]] Quaternion operator*(const Quaternion& _other) const noexcept;
		[[nodiscard]] Vector3D<float> operator*(const Vector3D<float>& vector) const noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] Quaternion& operator*=(TValue _scalar) noexcept;
		[[nodiscard]] Quaternion& operator*=(const Quaternion& _other) noexcept;

		[[nodiscard]] Quaternion operator/(TValue _scalar) const noexcept;
		[[nodiscard]] Quaternion& operator/=(TValue _scalar) noexcept;

		[[nodiscard]] bool operator==(const Quaternion& other) const noexcept;
		[[nodiscard]] bool operator!=(const Quaternion& other) const noexcept;
		[[nodiscard]] std::partial_ordering operator<=>(const Quaternion& other) const noexcept;

		[[nodiscard]] TValue GetX() const noexcept;
		void SetX(TValue component) noexcept;

		[[nodiscard]] TValue GetY() const noexcept;
		void SetY(TValue component) noexcept;

		[[nodiscard]] TValue GetZ() const noexcept;
		void SetZ(TValue component) noexcept;

		[[nodiscard]] TValue GetW() const noexcept;
		void SetW(TValue component) noexcept;

		void Set(TValue x, TValue y, TValue z, TValue w) noexcept;

		[[nodiscard]] Vector3D<float> GetEulerAngles() const noexcept requires std::same_as<TValue, float>;
		void SetEulerAngles(const Vector3D<float>& eulerDegrees) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] float GetMagnitude() const noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] float GetSqrMagnitude() const noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] Quaternion GetNormalized() const noexcept requires std::same_as<TValue, float>;

		void SetFromToRotation(const Vector3D<float>& from, const Vector3D<float>& to) noexcept requires std::same_as<TValue, float>;
		void SetLookRotation(const Vector3D<float>& view) noexcept requires std::same_as<TValue, float>;
		void SetLookRotation(const Vector3D<float>& view, const Vector3D<float>& up) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] bool IsFinite() const noexcept;

		[[nodiscard]] bool IsNormalized(float epsilon = std::numeric_limits<float>::epsilon()) const noexcept requires std::same_as<TValue, float>;

		void Normalize() noexcept requires std::same_as<TValue, float>;

		void ToAngleAxis(float& angleDegrees, Vector3D<float>& axis) const noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static Quaternion Normalize(const Quaternion& rotation) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static Quaternion GetIdentity() noexcept;

		[[nodiscard]] static DirectX::XMVECTOR Load(const Quaternion& quat) noexcept requires std::same_as<TValue, float>;
		static void Store(Quaternion& d, DirectX::XMVECTOR s) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static bool IsApproximately(const Quaternion& lhs, const Quaternion& rhs, float epsilon = std::numeric_limits<float>::epsilon()) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static float Angle(const Quaternion& a, const Quaternion& b) noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] static Quaternion AngleAxis(float angleDegrees, Vector3D<float> axis) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static Quaternion Euler(float xDegrees, float yDegrees, float zDegrees) noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] static Quaternion Euler(const Vector3D<float>& eulerDegrees) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static Quaternion Inverse(const Quaternion& rotation) noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] static Quaternion Conjugate(const Quaternion& rotation) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static float Dot(const Quaternion& a, const Quaternion& b) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static Quaternion Lerp(const Quaternion& a, const Quaternion& b, float t) noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] static Quaternion LerpUnclamped(const Quaternion& a, const Quaternion& b, float t) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static Quaternion Slerp(const Quaternion& a, const Quaternion& b, float t) noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] static Quaternion SlerpUnclamped(const Quaternion& a, const Quaternion& b, float t) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static Quaternion FromToRotation(const Vector3D<float>& fromDirection, const Vector3D<float>& toDirection) noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] static Quaternion LookRotation(const Vector3D<float>& forward, const Vector3D<float>& up) noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] static Quaternion RotateTowards(const Quaternion& from, const Quaternion& to, float maxDegreesDelta) noexcept requires std::same_as<TValue, float>;

	private:
		std::conditional_t<std::same_as<TValue, int>, DirectX::XMINT4, DirectX::XMFLOAT4> value;
	};

	extern template class Quaternion<int>;
	extern template class Quaternion<float>;
}
