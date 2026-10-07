#pragma once

#include <compare>
#include <concepts>
#include <cstddef>
#include <limits>
#include <type_traits>

#include <DirectXMath.h>

#include "Vector3D.h"

namespace TUK::Framework
{
	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	class Quaternion
	{
	public:
		Quaternion() noexcept;
		explicit Quaternion(TValue value_) noexcept;
		Quaternion(TValue x_, TValue y_, TValue z_, TValue w_) noexcept;

		Quaternion(const Quaternion& other_) noexcept;
		Quaternion(Quaternion&& other_) noexcept;

		Quaternion& operator=(const Quaternion& other_) noexcept;
		Quaternion& operator=(Quaternion&& other_) noexcept;

		[[nodiscard]] TValue operator[](size_t index) const noexcept;
		[[nodiscard]] TValue& operator[](size_t index) noexcept;

		[[nodiscard]] Quaternion operator+(const Quaternion& _other) const noexcept;
		[[nodiscard]] Quaternion& operator+=(const Quaternion& _other) noexcept;

		[[nodiscard]] Quaternion operator-(const Quaternion& _other) const noexcept;
		[[nodiscard]] Quaternion& operator-=(const Quaternion& _other) noexcept;

		[[nodiscard]] Quaternion operator*(TValue _scalar) const noexcept;
		[[nodiscard]] Quaternion operator*(const Quaternion& _other) const noexcept;
		[[nodiscard]] Vector3D<float> operator*(const Vector3D<float>& vector_) const noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] Quaternion& operator*=(TValue _scalar) noexcept;
		[[nodiscard]] Quaternion& operator*=(const Quaternion& _other) noexcept;

		[[nodiscard]] Quaternion operator/(TValue _scalar) const noexcept;
		[[nodiscard]] Quaternion& operator/=(TValue _scalar) noexcept;

		[[nodiscard]] bool operator==(const Quaternion& other_) const noexcept;
		[[nodiscard]] bool operator!=(const Quaternion& other_) const noexcept;
		[[nodiscard]] std::partial_ordering operator<=>(const Quaternion& other_) const noexcept;

		[[nodiscard]] TValue GetX() const noexcept;
		void SetX(TValue component) noexcept;

		[[nodiscard]] TValue GetY() const noexcept;
		void SetY(TValue component) noexcept;

		[[nodiscard]] TValue GetZ() const noexcept;
		void SetZ(TValue component) noexcept;

		[[nodiscard]] TValue GetW() const noexcept;
		void SetW(TValue component) noexcept;

		void Set(TValue x_, TValue y_, TValue z_, TValue w_) noexcept;

		[[nodiscard]] Vector3D<float> GetEulerAngles() const noexcept requires std::same_as<TValue, float>;
		void SetEulerAngles(const Vector3D<float>& eulerDegrees_) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] float GetMagnitude() const noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] float GetSqrMagnitude() const noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] Quaternion GetNormalized() const noexcept requires std::same_as<TValue, float>;

		void SetFromToRotation(const Vector3D<float>& from_, const Vector3D<float>& to_) noexcept requires std::same_as<TValue, float>;
		void SetLookRotation(const Vector3D<float>& view_) noexcept requires std::same_as<TValue, float>;
		void SetLookRotation(const Vector3D<float>& view_, const Vector3D<float>& up_) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] bool IsFinite() const noexcept;

		[[nodiscard]] bool IsNormalized(float epsilon_ = std::numeric_limits<float>::epsilon()) const noexcept requires std::same_as<TValue, float>;

		void Normalize() noexcept requires std::same_as<TValue, float>;

		void ToAngleAxis(float& angleDegrees_, Vector3D<float>& axis_) const noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static Quaternion Normalize(const Quaternion& rotation_) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static Quaternion GetIdentity() noexcept;

		[[nodiscard]] static DirectX::XMVECTOR Load(const Quaternion& quat_) noexcept requires std::same_as<TValue, float>;
		static void Store(Quaternion& d_, DirectX::XMVECTOR s_) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static bool IsApproximately(const Quaternion& lhs_, const Quaternion& rhs_, float epsilon_ = std::numeric_limits<float>::epsilon()) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static float Angle(const Quaternion& a_, const Quaternion& b_) noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] static Quaternion AngleAxis(float angleDegrees_, Vector3D<float> axis_) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static Quaternion Euler(float xDegrees_, float yDegrees_, float zDegrees_) noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] static Quaternion Euler(const Vector3D<float>& eulerDegrees_) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static Quaternion Inverse(const Quaternion& rotation_) noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] static Quaternion Conjugate(const Quaternion& rotation_) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static float Dot(const Quaternion& a_, const Quaternion& b_) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static Quaternion Lerp(const Quaternion& a_, const Quaternion& b_, float t_) noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] static Quaternion LerpUnclamped(const Quaternion& a_, const Quaternion& b_, float t_) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static Quaternion Slerp(const Quaternion& a_, const Quaternion& b_, float t_) noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] static Quaternion SlerpUnclamped(const Quaternion& a_, const Quaternion& b_, float t_) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static Quaternion FromToRotation(const Vector3D<float>& fromDirection_, const Vector3D<float>& toDirection_) noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] static Quaternion LookRotation(const Vector3D<float>& forward_, const Vector3D<float>& up_) noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] static Quaternion RotateTowards(const Quaternion& from_, const Quaternion& to_, float maxDegreesDelta_) noexcept requires std::same_as<TValue, float>;

	private:
		std::conditional_t<std::same_as<TValue, int>, DirectX::XMINT4, DirectX::XMFLOAT4> value;
	};

	extern template class Quaternion<int>;
	extern template class Quaternion<float>;
}
