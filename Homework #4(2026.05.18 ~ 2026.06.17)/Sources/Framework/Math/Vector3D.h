#pragma once

#include <compare>
#include <concepts>
#include <cstddef>
#include <limits>
#include <type_traits>

#include <DirectXMath.h>

namespace TUK::Framework
{
	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	class Vector2D;

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	class Vector4D;

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	class Vector3D
	{
	public:
		Vector3D() noexcept;
		explicit Vector3D(TValue scalar) noexcept;
		Vector3D(TValue x, TValue y, TValue z) noexcept;

		Vector3D(const Vector3D& other) noexcept;
		Vector3D(Vector3D&& other) noexcept;

		explicit Vector3D(const Vector2D<TValue>& vector, TValue z = TValue{}) noexcept;
		explicit Vector3D(const Vector4D<TValue>& vector) noexcept;

		explicit Vector3D(DirectX::XMVECTOR vector) noexcept requires std::same_as<TValue, float>;

		Vector3D& operator=(const Vector3D& other) noexcept;
		Vector3D& operator=(Vector3D&& other) noexcept;

		[[nodiscard]] operator Vector2D<TValue>() const noexcept;
		[[nodiscard]] operator Vector4D<TValue>() const noexcept;

		[[nodiscard]] TValue operator[](std::size_t index) const noexcept;
		[[nodiscard]] TValue& operator[](std::size_t index) noexcept;

		[[nodiscard]] Vector3D operator+() const noexcept;
		[[nodiscard]] Vector3D operator+(const Vector3D& other) const noexcept;
		Vector3D& operator+=(const Vector3D& other) noexcept;

		[[nodiscard]] Vector3D operator-() const noexcept;
		[[nodiscard]] Vector3D operator-(const Vector3D& other) const noexcept;
		Vector3D& operator-=(const Vector3D& other) noexcept;

		[[nodiscard]] Vector3D operator*(TValue scalar) const noexcept;
		Vector3D& operator*=(TValue scalar) noexcept;

		[[nodiscard]] Vector3D operator/(TValue scalar) const noexcept;
		Vector3D& operator/=(TValue scalar) noexcept;

		[[nodiscard]] bool operator==(const Vector3D& other) const noexcept;
		[[nodiscard]] bool operator!=(const Vector3D& other) const noexcept;
		[[nodiscard]] std::partial_ordering operator<=>(const Vector3D& other) const noexcept;

		[[nodiscard]] TValue GetX() const noexcept;
		void SetX(TValue x) noexcept;

		[[nodiscard]] TValue GetY() const noexcept;
		void SetY(TValue y) noexcept;

		[[nodiscard]] TValue GetZ() const noexcept;
		void SetZ(TValue z) noexcept;

		void Set(TValue x, TValue y, TValue z) noexcept;

		[[nodiscard]] float GetMagnitude() const noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] float GetSqrMagnitude() const noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] float GetLength() const noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] float GetLengthSquared() const noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] Vector3D GetNormalized() const noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] bool IsZero(float epsilon = std::numeric_limits<float>::epsilon()) const noexcept;
		[[nodiscard]] bool IsFinite() const noexcept;

		void Normalize() noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] float Dot(const Vector3D& other) const noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static Vector3D GetZero() noexcept;
		[[nodiscard]] static Vector3D GetOne() noexcept;

		[[nodiscard]] static Vector3D GetUp() noexcept;
		[[nodiscard]] static Vector3D GetDown() noexcept;

		[[nodiscard]] static Vector3D GetLeft() noexcept;
		[[nodiscard]] static Vector3D GetRight() noexcept;

		[[nodiscard]] static Vector3D GetForward() noexcept;
		[[nodiscard]] static Vector3D GetBack() noexcept;

		[[nodiscard]] static Vector3D GetPositiveInfinity() noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] static Vector3D GetNegativeInfinity() noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static DirectX::XMVECTOR Load(const Vector3D& vec_) noexcept requires std::same_as<TValue, float>;
		static void Store(Vector3D& d_, DirectX::XMVECTOR s_) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static bool IsApproximately(const Vector3D& lhs_, const Vector3D& rhs_, float epsilon_ = std::numeric_limits<float>::epsilon()) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static Vector3D Max(const Vector3D& a_, const Vector3D& b_) noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] static Vector3D Min(const Vector3D& a_, const Vector3D& b_) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static Vector3D Clamp(const Vector3D& value_, const Vector3D& min_, const Vector3D& max_) noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] static Vector3D ClampMagnitude(const Vector3D& vector_, float maxLength_) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static Vector3D Scale(const Vector3D& vector_, const Vector3D& scale_) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static Vector3D Normalize(const Vector3D& value_) noexcept requires std::same_as<TValue, float>;
		static void OrthoNormalize(Vector3D& normal_, Vector3D& tangent_) noexcept requires std::same_as<TValue, float>;
		static void OrthoNormalize(Vector3D& normal_, Vector3D& tangent_, Vector3D& binormal_) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static float Dot(const Vector3D& a_, const Vector3D& b_) noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] static Vector3D Cross(const Vector3D& a_, const Vector3D& b_) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static float Distance(const Vector3D& _v1, const Vector3D& _v2) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static float Angle(const Vector3D& a_, const Vector3D& b_) noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] static float SignedAngle(const Vector3D& from_, const Vector3D& to_, const Vector3D& axis_) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static Vector3D Project(const Vector3D& vector_, const Vector3D& onNormal_) noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] static Vector3D ProjectOnPlane(const Vector3D& vector_, const Vector3D& planeNormal_) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static Vector3D Reflect(const Vector3D& _vector, const Vector3D& _normal) noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] static Vector3D Refract(const Vector3D& _vector, const Vector3D& _normal, float _eta) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static Vector3D Lerp(const Vector3D& a_, const Vector3D& b_, float t_) noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] static Vector3D LerpUnclamped(const Vector3D& a_, const Vector3D& b_, float t_) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static Vector3D Slerp(const Vector3D& a_, const Vector3D& b_, float t_) noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] static Vector3D SlerpUnclamped(const Vector3D& a_, const Vector3D& b_, float t_) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static Vector3D MoveTowards(const Vector3D& current_, const Vector3D& target_, float maxDistanceDelta_) noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] static Vector3D RotateTowards(const Vector3D& current_, const Vector3D& target_, float maxRadiansDelta_, float maxMagnitudeDelta_) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static Vector3D SmoothDamp(
			const Vector3D& current_,
			const Vector3D& target_,
			Vector3D& currentVelocity_,
			float smoothTime_,
			float maxSpeed_,
			float deltaTime_) noexcept requires std::same_as<TValue, float>;

	private:
		std::conditional_t<std::same_as<TValue, int>, DirectX::XMINT3, DirectX::XMFLOAT3> value;
	};

	extern template class Vector3D<int>;
	extern template class Vector3D<float>;

	[[nodiscard]] Vector3D<int> operator*(int scalar, const Vector3D<int>& vector) noexcept;
	[[nodiscard]] Vector3D<float> operator*(float scalar, const Vector3D<float>& vector) noexcept;
}
