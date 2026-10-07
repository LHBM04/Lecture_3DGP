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
	class Vector3D;

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	class Vector4D;

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
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

		[[nodiscard]] float GetMagnitude() const noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] float GetSqrMagnitude() const noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] float GetLength() const noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] float GetLengthSquared() const noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] Vector2D GetNormalized() const noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] bool IsZero(float epsilon = std::numeric_limits<float>::epsilon()) const noexcept;
		[[nodiscard]] bool IsFinite() const noexcept;

		void Normalize() noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] float Dot(const Vector2D& other) const noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static Vector2D GetZero() noexcept;
		[[nodiscard]] static Vector2D GetOne() noexcept;

		[[nodiscard]] static Vector2D GetUp() noexcept;
		[[nodiscard]] static Vector2D GetDown() noexcept;

		[[nodiscard]] static Vector2D GetLeft() noexcept;
		[[nodiscard]] static Vector2D GetRight() noexcept;

		[[nodiscard]] static Vector2D GetPositiveInfinity() noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] static Vector2D GetNegativeInfinity() noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static DirectX::XMVECTOR Load(const Vector2D& vec_) noexcept requires std::same_as<TValue, float>;
		static void Store(Vector2D& d_, DirectX::XMVECTOR s_) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static bool IsApproximately(const Vector2D& lhs_, const Vector2D& rhs_, float epsilon_ = std::numeric_limits<float>::epsilon()) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static Vector2D Max(const Vector2D& a_, const Vector2D& b_) noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] static Vector2D Min(const Vector2D& a_, const Vector2D& b_) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static Vector2D Clamp(const Vector2D& value_, const Vector2D& min_, const Vector2D& max_) noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] static Vector2D ClampMagnitude(const Vector2D& vector_, float maxLength_) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static Vector2D Scale(const Vector2D& vector_, const Vector2D& scale_) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static Vector2D Normalize(const Vector2D& value_) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static float Dot(const Vector2D& a_, const Vector2D& b_) noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] static Vector2D Cross(const Vector2D& a_, const Vector2D& b_) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static Vector2D Perpendicular(const Vector2D& vector_) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static float Distance(const Vector2D& a_, const Vector2D& b_) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static float Angle(const Vector2D& a_, const Vector2D& b_) noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] static float SignedAngle(const Vector2D& from_, const Vector2D& to_) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static Vector2D Reflect(const Vector2D& vector_, const Vector2D& normal_) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static Vector2D Lerp(const Vector2D& a_, const Vector2D& b_, float t_) noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] static Vector2D LerpUnclamped(const Vector2D& a_, const Vector2D& b_, float t_) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static Vector2D Slerp(const Vector2D& a_, const Vector2D& b_, float t_) noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] static Vector2D SlerpUnclamped(const Vector2D& a_, const Vector2D& b_, float t_) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static Vector2D MoveTowards(const Vector2D& current_, const Vector2D& target_, float maxDistanceDelta_) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static Vector2D SmoothDamp(
			const Vector2D& current_,
			const Vector2D& target_,
			Vector2D& currentVelocity_,
			float smoothTime_,
			float maxSpeed_,
			float deltaTime_) noexcept requires std::same_as<TValue, float>;

	private:
		std::conditional_t<std::same_as<TValue, int>, DirectX::XMINT2, DirectX::XMFLOAT2> value;
	};

	extern template class Vector2D<int>;
	extern template class Vector2D<float>;

	[[nodiscard]] Vector2D<int> operator*(int scalar, const Vector2D<int>& vector) noexcept;
	[[nodiscard]] Vector2D<float> operator*(float scalar, const Vector2D<float>& vector) noexcept;
}
