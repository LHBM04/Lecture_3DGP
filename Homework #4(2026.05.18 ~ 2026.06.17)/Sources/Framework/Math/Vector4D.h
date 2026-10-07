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
	class Vector3D;

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	class Vector4D
	{
	public:
		Vector4D() noexcept;
		explicit Vector4D(TValue scalar) noexcept;
		Vector4D(TValue x, TValue y, TValue z, TValue w) noexcept;

		Vector4D(const Vector4D& other) noexcept;
		Vector4D(Vector4D&& other) noexcept;

		explicit Vector4D(const Vector2D<TValue>& vector, TValue z = TValue{}, TValue w = TValue{}) noexcept;
		explicit Vector4D(const Vector3D<TValue>& vector, TValue w = TValue{}) noexcept;

		Vector4D& operator=(const Vector4D& other) noexcept;
		Vector4D& operator=(Vector4D&& other) noexcept;

		[[nodiscard]] operator Vector2D<TValue>() const noexcept;
		[[nodiscard]] operator Vector3D<TValue>() const noexcept;

		[[nodiscard]] TValue operator[](std::size_t index) const noexcept;
		[[nodiscard]] TValue& operator[](std::size_t index) noexcept;

		[[nodiscard]] Vector4D operator+() const noexcept;
		[[nodiscard]] Vector4D operator+(const Vector4D& other) const noexcept;
		Vector4D& operator+=(const Vector4D& other) noexcept;

		[[nodiscard]] Vector4D operator-() const noexcept;
		[[nodiscard]] Vector4D operator-(const Vector4D& other) const noexcept;
		Vector4D& operator-=(const Vector4D& other) noexcept;

		[[nodiscard]] Vector4D operator*(TValue scalar) const noexcept;
		Vector4D& operator*=(TValue scalar) noexcept;

		[[nodiscard]] Vector4D operator/(TValue scalar) const noexcept;
		Vector4D& operator/=(TValue scalar) noexcept;

		[[nodiscard]] bool operator==(const Vector4D& other) const noexcept;
		[[nodiscard]] bool operator!=(const Vector4D& other) const noexcept;
		[[nodiscard]] std::partial_ordering operator<=>(const Vector4D& other) const noexcept;

		[[nodiscard]] TValue GetX() const noexcept;
		void SetX(TValue x) noexcept;

		[[nodiscard]] TValue GetY() const noexcept;
		void SetY(TValue y) noexcept;

		[[nodiscard]] TValue GetZ() const noexcept;
		void SetZ(TValue z) noexcept;

		[[nodiscard]] TValue GetW() const noexcept;
		void SetW(TValue w) noexcept;

		void Set(TValue x, TValue y, TValue z, TValue w) noexcept;

		[[nodiscard]] float GetMagnitude() const noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] float GetSqrMagnitude() const noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] float GetLength() const noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] float GetLengthSquared() const noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] Vector4D GetNormalized() const noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] bool IsZero(float epsilon = std::numeric_limits<float>::epsilon()) const noexcept;
		[[nodiscard]] bool IsFinite() const noexcept;

		void Normalize() noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] float Dot(const Vector4D& other) const noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static Vector4D GetZero() noexcept;
		[[nodiscard]] static Vector4D GetOne() noexcept;

		[[nodiscard]] static Vector4D GetPositiveInfinity() noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] static Vector4D GetNegativeInfinity() noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static DirectX::XMVECTOR Load(const Vector4D& vec_) noexcept requires std::same_as<TValue, float>;
		static void Store(Vector4D& d_, DirectX::XMVECTOR s_) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static bool IsApproximately(const Vector4D& lhs_, const Vector4D& rhs_, float epsilon_ = std::numeric_limits<float>::epsilon()) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static Vector4D Max(const Vector4D& a_, const Vector4D& b_) noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] static Vector4D Min(const Vector4D& a_, const Vector4D& b_) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static Vector4D Clamp(const Vector4D& value_, const Vector4D& min_, const Vector4D& max_) noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] static Vector4D ClampMagnitude(const Vector4D& vector_, float maxLength_) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static Vector4D Scale(const Vector4D& vector_, const Vector4D& scale_) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static Vector4D Normalize(const Vector4D& value_) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static float Dot(const Vector4D& a_, const Vector4D& b_) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static float Distance(const Vector4D& a_, const Vector4D& b_) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static float Angle(const Vector4D& a_, const Vector4D& b_) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static Vector4D Project(const Vector4D& vector_, const Vector4D& onNormal_) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static Vector4D Reflect(const Vector4D& vector_, const Vector4D& normal_) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static Vector4D Lerp(const Vector4D& a_, const Vector4D& b_, float t_) noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] static Vector4D LerpUnclamped(const Vector4D& a_, const Vector4D& b_, float t_) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static Vector4D Slerp(const Vector4D& a_, const Vector4D& b_, float t_) noexcept requires std::same_as<TValue, float>;
		[[nodiscard]] static Vector4D SlerpUnclamped(const Vector4D& a_, const Vector4D& b_, float t_) noexcept requires std::same_as<TValue, float>;

		[[nodiscard]] static Vector4D MoveTowards(const Vector4D& current_, const Vector4D& target_, float maxDistanceDelta_) noexcept requires std::same_as<TValue, float>;

	private:
		std::conditional_t<std::same_as<TValue, int>, DirectX::XMINT4, DirectX::XMFLOAT4> value;
	};

	extern template class Vector4D<int>;
	extern template class Vector4D<float>;

	[[nodiscard]] Vector4D<int> operator*(int scalar, const Vector4D<int>& vector) noexcept;
	[[nodiscard]] Vector4D<float> operator*(float scalar, const Vector4D<float>& vector) noexcept;
}
