#pragma once

#include "CollisionTypes.hpp"
#include "Matrix4x4.hpp"
#include "Quaternion.hpp"
#include "Vector3D.hpp"
#include "Vector4D.hpp"

#include "../Core/Common.hpp"

namespace TUK::Framework
{
	class BoundingOrientedBox;
	class BoundingSphere;

	class BoundingBox
	{
	public:
		static constexpr usize CORNER_COUNT = DirectX::BoundingBox::CORNER_COUNT;

		BoundingBox() noexcept;
		BoundingBox(const Vector3D<f32>& center, const Vector3D<f32>& extents) noexcept;

		BoundingBox(const BoundingBox& other) noexcept = default;
		BoundingBox(BoundingBox&& other) noexcept = default;

		explicit BoundingBox(const DirectX::BoundingBox& source) noexcept;

		BoundingBox& operator=(const BoundingBox& other) noexcept = default;
		BoundingBox& operator=(BoundingBox&& other) noexcept = default;

		[[nodiscard]] operator DirectX::BoundingBox() const noexcept;

		[[nodiscard]] bool operator==(const BoundingBox& other) const noexcept;
		[[nodiscard]] bool operator!=(const BoundingBox& other) const noexcept;
		[[nodiscard]] std::partial_ordering operator<=>(const BoundingBox& other) const noexcept;

		[[nodiscard]] Vector3D<f32> GetCenter() const noexcept;
		void SetCenter(const Vector3D<f32>& center) noexcept;

		[[nodiscard]] Vector3D<f32> GetExtents() const noexcept;
		void SetExtents(const Vector3D<f32>& extents) noexcept;

		void Set(const Vector3D<f32>& center, const Vector3D<f32>& extents) noexcept;

		[[nodiscard]] Vector3D<f32> GetMin() const noexcept;
		[[nodiscard]] Vector3D<f32> GetMax() const noexcept;
		[[nodiscard]] Vector3D<f32> GetSize() const noexcept;
		void SetMinMax(const Vector3D<f32>& min, const Vector3D<f32>& max) noexcept;

		[[nodiscard]] bool IsFinite() const noexcept;
		[[nodiscard]] bool IsValid() const noexcept;

		[[nodiscard]] std::array<Vector3D<f32>, CORNER_COUNT> GetCorners() const noexcept;
		void GetCorners(std::span<Vector3D<f32>, CORNER_COUNT> corners) const noexcept;
		void GetCorners(DirectX::XMFLOAT3* corners) const noexcept;

		[[nodiscard]] BoundingBox GetTransformed(const Matrix4x4<f32>& matrix) const noexcept;
		[[nodiscard]] BoundingBox GetTransformed(f32 scale, const Quaternion<f32>& rotation, const Vector3D<f32>& translation) const noexcept;

		void Transform(const Matrix4x4<f32>& matrix) noexcept;
		void Transform(f32 scale, const Quaternion<f32>& rotation, const Vector3D<f32>& translation) noexcept;
		void Transform(BoundingBox& out, const Matrix4x4<f32>& matrix) const noexcept;
		void Transform(BoundingBox& out, f32 scale, const Quaternion<f32>& rotation, const Vector3D<f32>& translation) const noexcept;

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

		[[nodiscard]] static bool IsApproximately(const BoundingBox& lhs, const BoundingBox& rhs, f32 epsilon = std::numeric_limits<f32>::epsilon()) noexcept;

		[[nodiscard]] static BoundingBox CreateMerged(const BoundingBox& b1, const BoundingBox& b2) noexcept;
		[[nodiscard]] static BoundingBox CreateFromSphere(const BoundingSphere& sphere) noexcept;
		[[nodiscard]] static BoundingBox CreateFromMinMax(const Vector3D<f32>& min, const Vector3D<f32>& max) noexcept;
		[[nodiscard]] static BoundingBox CreateFromPoints(const Vector3D<f32>& point1, const Vector3D<f32>& point2) noexcept;
		[[nodiscard]] static BoundingBox CreateFromPoints(std::span<const Vector3D<f32>> points) noexcept;
		[[nodiscard]] static BoundingBox CreateFromPoints(usize count, const DirectX::XMFLOAT3* points, usize stride = sizeof(DirectX::XMFLOAT3)) noexcept;

	private:
		DirectX::BoundingBox value;
	};
}
