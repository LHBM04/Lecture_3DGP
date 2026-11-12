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
	class BoundingSphere;

	class BoundingOrientedBox
	{
	public:
		static constexpr usize CORNER_COUNT = DirectX::BoundingOrientedBox::CORNER_COUNT;

		BoundingOrientedBox() noexcept;
		BoundingOrientedBox(const Vector3D<f32>& center, const Vector3D<f32>& extents, const Quaternion<f32>& orientation) noexcept;

		BoundingOrientedBox(const BoundingOrientedBox& other) noexcept = default;
		BoundingOrientedBox(BoundingOrientedBox&& other) noexcept = default;

		explicit BoundingOrientedBox(const DirectX::BoundingOrientedBox& source) noexcept;

		BoundingOrientedBox& operator=(const BoundingOrientedBox& other) noexcept = default;
		BoundingOrientedBox& operator=(BoundingOrientedBox&& other) noexcept = default;

		[[nodiscard]] operator DirectX::BoundingOrientedBox() const noexcept;

		[[nodiscard]] bool operator==(const BoundingOrientedBox& other) const noexcept;
		[[nodiscard]] bool operator!=(const BoundingOrientedBox& other) const noexcept;
		[[nodiscard]] std::partial_ordering operator<=>(const BoundingOrientedBox& other) const noexcept;

		[[nodiscard]] Vector3D<f32> GetCenter() const noexcept;
		void SetCenter(const Vector3D<f32>& center) noexcept;

		[[nodiscard]] Vector3D<f32> GetExtents() const noexcept;
		void SetExtents(const Vector3D<f32>& extents) noexcept;

		[[nodiscard]] Quaternion<f32> GetOrientation() const noexcept;
		void SetOrientation(const Quaternion<f32>& orientation) noexcept;

		void Set(const Vector3D<f32>& center, const Vector3D<f32>& extents, const Quaternion<f32>& orientation) noexcept;

		[[nodiscard]] Vector3D<f32> GetSize() const noexcept;

		[[nodiscard]] bool IsFinite() const noexcept;
		[[nodiscard]] bool IsValid() const noexcept;

		[[nodiscard]] std::array<Vector3D<f32>, CORNER_COUNT> GetCorners() const noexcept;
		void GetCorners(std::span<Vector3D<f32>, CORNER_COUNT> corners) const noexcept;
		void GetCorners(DirectX::XMFLOAT3* corners) const noexcept;

		[[nodiscard]] BoundingOrientedBox GetTransformed(const Matrix4x4<f32>& matrix) const noexcept;
		[[nodiscard]] BoundingOrientedBox GetTransformed(f32 scale, const Quaternion<f32>& rotation, const Vector3D<f32>& translation) const noexcept;

		void Transform(const Matrix4x4<f32>& matrix) noexcept;
		void Transform(f32 scale, const Quaternion<f32>& rotation, const Vector3D<f32>& translation) noexcept;
		void Transform(BoundingOrientedBox& out, const Matrix4x4<f32>& matrix) const noexcept;
		void Transform(BoundingOrientedBox& out, f32 scale, const Quaternion<f32>& rotation, const Vector3D<f32>& translation) const noexcept;

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

		[[nodiscard]] static bool IsApproximately(const BoundingOrientedBox& lhs, const BoundingOrientedBox& rhs, f32 epsilon = std::numeric_limits<f32>::epsilon()) noexcept;

		[[nodiscard]] static BoundingOrientedBox CreateFromBoundingBox(const BoundingBox& box) noexcept;
		[[nodiscard]] static BoundingOrientedBox CreateFromPoints(std::span<const Vector3D<f32>> points) noexcept;
		[[nodiscard]] static BoundingOrientedBox CreateFromPoints(usize count, const DirectX::XMFLOAT3* points, usize stride = sizeof(DirectX::XMFLOAT3)) noexcept;

	private:
		DirectX::BoundingOrientedBox value;
	};
}
