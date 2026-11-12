#pragma once

#include "CollisionTypes.hpp"
#include "Matrix4x4.hpp"
#include "Quaternion.hpp"
#include "Vector3D.hpp"
#include "Vector4D.hpp"

#include "../Core/Common.hpp"

namespace TUK::Framework
{
	class BoundingBox;
	class BoundingOrientedBox;

	class BoundingSphere
	{
	public:
		BoundingSphere() noexcept;
		explicit BoundingSphere(const Vector3D<f32>& center, f32 radius) noexcept;
		explicit BoundingSphere(const DirectX::BoundingSphere& source) noexcept;

		BoundingSphere(const BoundingSphere& other) noexcept = default;
		BoundingSphere& operator=(const BoundingSphere& other) noexcept = default;

		BoundingSphere(BoundingSphere&& other) noexcept = default;
		BoundingSphere& operator=(BoundingSphere&& other) noexcept = default;

		[[nodiscard]] operator DirectX::BoundingSphere() const noexcept;

		[[nodiscard]] bool operator==(const BoundingSphere& other) const noexcept;
		[[nodiscard]] bool operator!=(const BoundingSphere& other) const noexcept;
		[[nodiscard]] std::partial_ordering operator<=>(const BoundingSphere& other) const noexcept;

		[[nodiscard]] Vector3D<f32> GetCenter() const noexcept;
		void SetCenter(const Vector3D<f32>& center) noexcept;

		[[nodiscard]] f32 GetRadius() const noexcept;
		void SetRadius(f32 radius) noexcept;

		void Set(const Vector3D<f32>& center, f32 radius) noexcept;

		[[nodiscard]] bool IsFinite() const noexcept;
		[[nodiscard]] bool IsValid() const noexcept;

		[[nodiscard]] BoundingSphere GetTransformed(const Matrix4x4<f32>& matrix) const noexcept;
		[[nodiscard]] BoundingSphere GetTransformed(f32 scale, const Quaternion<f32>& rotation, const Vector3D<f32>& translation) const noexcept;

		void Transform(const Matrix4x4<f32>& matrix) noexcept;
		void Transform(f32 scale, const Quaternion<f32>& rotation, const Vector3D<f32>& translation) noexcept;
		void Transform(BoundingSphere& out, const Matrix4x4<f32>& matrix) const noexcept;
		void Transform(BoundingSphere& out, f32 scale, const Quaternion<f32>& rotation, const Vector3D<f32>& translation) const noexcept;

		[[nodiscard]] ContainmentType Contains(const Vector3D<f32>& point) const noexcept;
		[[nodiscard]] ContainmentType Contains(const Vector3D<f32>& v0, const Vector3D<f32>& v1, const Vector3D<f32>& v2) const noexcept;
		[[nodiscard]] ContainmentType Contains(const BoundingSphere& sphere) const noexcept;
		[[nodiscard]] ContainmentType Contains(const BoundingBox& box) const noexcept;
		[[nodiscard]] ContainmentType Contains(const BoundingOrientedBox& box) const noexcept;
		[[nodiscard]] ContainmentType Contains(const DirectX::BoundingFrustum& frustum) const noexcept;

		[[nodiscard]] bool Intersects(const BoundingSphere& sphere) const noexcept;
		[[nodiscard]] bool Intersects(const BoundingBox& box) const noexcept;
		[[nodiscard]] bool Intersects(const BoundingOrientedBox& box) const noexcept;
		[[nodiscard]] bool Intersects(const DirectX::BoundingFrustum& frustum) const noexcept;
		[[nodiscard]] bool Intersects(const Vector3D<f32>& v0, const Vector3D<f32>& v1, const Vector3D<f32>& v2) const noexcept;
		[[nodiscard]] PlaneIntersectionType Intersects(const Vector4D<f32>& plane) const noexcept;
		[[nodiscard]] bool Intersects(const Vector3D<f32>& origin, const Vector3D<f32>& direction, f32& distance) const noexcept;

		[[nodiscard]] ContainmentType ContainedBy(
			const Vector4D<f32>& plane0,
			const Vector4D<f32>& plane1,
			const Vector4D<f32>& plane2,
			const Vector4D<f32>& plane3,
			const Vector4D<f32>& plane4,
			const Vector4D<f32>& plane5) const noexcept;

		[[nodiscard]] static bool IsApproximately(const BoundingSphere& lhs, const BoundingSphere& rhs, f32 epsilon = std::numeric_limits<f32>::epsilon()) noexcept;

		[[nodiscard]] static BoundingSphere CreateMerged(const BoundingSphere& s1, const BoundingSphere& s2) noexcept;
		[[nodiscard]] static BoundingSphere CreateFromBoundingBox(const BoundingBox& box) noexcept;
		[[nodiscard]] static BoundingSphere CreateFromBoundingBox(const BoundingOrientedBox& box) noexcept;
		[[nodiscard]] static BoundingSphere CreateFromFrustum(const DirectX::BoundingFrustum& frustum) noexcept;
		[[nodiscard]] static BoundingSphere CreateFromPoints(std::span<const Vector3D<f32>> points) noexcept;
		[[nodiscard]] static BoundingSphere CreateFromPoints(usize count, const DirectX::XMFLOAT3* points, usize stride = sizeof(DirectX::XMFLOAT3)) noexcept;

	private:
		DirectX::BoundingSphere value;
	};
}
