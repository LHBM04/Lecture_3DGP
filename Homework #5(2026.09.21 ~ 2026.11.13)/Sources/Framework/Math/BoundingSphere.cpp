#include "Precompiled.hpp"
#include "BoundingSphere.hpp"

#include "../Core/Common.hpp"

#include "BoundingBox.hpp"
#include "BoundingOrientedBox.hpp"
#include "Mathf.hpp"

namespace TUK::Framework
{
	BoundingSphere::BoundingSphere() noexcept
		: value()
	{
	}

	BoundingSphere::BoundingSphere(const Vector3D<f32>& center, f32 radius) noexcept
		: value(static_cast<DirectX::XMFLOAT3>(center), radius)
	{
		ASSERT_FMT(radius >= 0.0f, "radius는 0 이상이어야 합니다.");
	}

	BoundingSphere::BoundingSphere(const DirectX::BoundingSphere& source) noexcept
		: value(source)
	{
	}

	BoundingSphere::operator DirectX::BoundingSphere() const noexcept
	{
		return value;
	}

	bool BoundingSphere::operator==(const BoundingSphere& other) const noexcept
	{
		return GetCenter() == other.GetCenter() && value.Radius == other.value.Radius;
	}

	bool BoundingSphere::operator!=(const BoundingSphere& other) const noexcept
	{
		return !(*this == other);
	}

	std::partial_ordering BoundingSphere::operator<=>(const BoundingSphere& other) const noexcept
	{
		if (const auto order = GetCenter() <=> other.GetCenter(); order != 0)
		{
			return order;
		}

		return value.Radius <=> other.value.Radius;
	}

	Vector3D<f32> BoundingSphere::GetCenter() const noexcept
	{
		return Vector3D<f32>(value.Center);
	}

	void BoundingSphere::SetCenter(const Vector3D<f32>& center) noexcept
	{
		value.Center = static_cast<DirectX::XMFLOAT3>(center);
	}

	f32 BoundingSphere::GetRadius() const noexcept
	{
		return value.Radius;
	}

	void BoundingSphere::SetRadius(f32 radius) noexcept
	{
		ASSERT_FMT(radius >= 0.0f, "radius는 0 이상이어야 합니다.");
		value.Radius = radius;
	}

	void BoundingSphere::Set(const Vector3D<f32>& center, f32 radius) noexcept
	{
		SetCenter(center);
		SetRadius(radius);
	}

	bool BoundingSphere::IsFinite() const noexcept
	{
		return GetCenter().IsFinite() && std::isfinite(value.Radius);
	}

	bool BoundingSphere::IsValid() const noexcept
	{
		return IsFinite() && value.Radius >= 0.0f;
	}

	BoundingSphere BoundingSphere::GetTransformed(const Matrix4x4<f32>& matrix) const noexcept
	{
		BoundingSphere result{};
		Transform(result, matrix);
		return result;
	}

	BoundingSphere BoundingSphere::GetTransformed(f32 scale, const Quaternion<f32>& rotation, const Vector3D<f32>& translation) const noexcept
	{
		BoundingSphere result{};
		Transform(result, scale, rotation, translation);
		return result;
	}

	void BoundingSphere::Transform(const Matrix4x4<f32>& matrix) noexcept
	{
		*this = GetTransformed(matrix);
	}

	void BoundingSphere::Transform(f32 scale, const Quaternion<f32>& rotation, const Vector3D<f32>& translation) noexcept
	{
		*this = GetTransformed(scale, rotation, translation);
	}

	void BoundingSphere::Transform(BoundingSphere& out, const Matrix4x4<f32>& matrix) const noexcept
	{
		value.Transform(out.value, Matrix4x4<f32>::Load(matrix));
	}

	void BoundingSphere::Transform(BoundingSphere& out, f32 scale, const Quaternion<f32>& rotation, const Vector3D<f32>& translation) const noexcept
	{
		const Quaternion<f32> normalizedRotation = Detail::NormalizeRotation(rotation);
		value.Transform(
			out.value,
			scale,
			Quaternion<f32>::Load(normalizedRotation),
			Vector3D<f32>::Load(translation));
	}

	ContainmentType BoundingSphere::Contains(const Vector3D<f32>& point) const noexcept
	{
		return static_cast<ContainmentType>(value.Contains(Vector3D<f32>::Load(point)));
	}

	ContainmentType BoundingSphere::Contains(const Vector3D<f32>& v0, const Vector3D<f32>& v1, const Vector3D<f32>& v2) const noexcept
	{
		return static_cast<ContainmentType>(value.Contains(
			Vector3D<f32>::Load(v0),
			Vector3D<f32>::Load(v1),
			Vector3D<f32>::Load(v2)));
	}

	ContainmentType BoundingSphere::Contains(const BoundingSphere& sphere) const noexcept
	{
		return static_cast<ContainmentType>(value.Contains(sphere.value));
	}

	ContainmentType BoundingSphere::Contains(const BoundingBox& box) const noexcept
	{
		return static_cast<ContainmentType>(value.Contains(static_cast<DirectX::BoundingBox>(box)));
	}

	ContainmentType BoundingSphere::Contains(const BoundingOrientedBox& box) const noexcept
	{
		return static_cast<ContainmentType>(value.Contains(static_cast<DirectX::BoundingOrientedBox>(box)));
	}

	ContainmentType BoundingSphere::Contains(const DirectX::BoundingFrustum& frustum) const noexcept
	{
		return static_cast<ContainmentType>(value.Contains(frustum));
	}

	bool BoundingSphere::Intersects(const BoundingSphere& sphere) const noexcept
	{
		return value.Intersects(sphere.value);
	}

	bool BoundingSphere::Intersects(const BoundingBox& box) const noexcept
	{
		return value.Intersects(static_cast<DirectX::BoundingBox>(box));
	}

	bool BoundingSphere::Intersects(const BoundingOrientedBox& box) const noexcept
	{
		return value.Intersects(static_cast<DirectX::BoundingOrientedBox>(box));
	}

	bool BoundingSphere::Intersects(const DirectX::BoundingFrustum& frustum) const noexcept
	{
		return value.Intersects(frustum);
	}

	bool BoundingSphere::Intersects(const Vector3D<f32>& v0, const Vector3D<f32>& v1, const Vector3D<f32>& v2) const noexcept
	{
		Detail::AssertNonDegenerateTriangle(v0, v1, v2);
		return value.Intersects(
			Vector3D<f32>::Load(v0),
			Vector3D<f32>::Load(v1),
			Vector3D<f32>::Load(v2));
	}

	PlaneIntersectionType BoundingSphere::Intersects(const Vector4D<f32>& plane) const noexcept
	{
		return static_cast<PlaneIntersectionType>(value.Intersects(Detail::NormalizePlane(plane)));
	}

	bool BoundingSphere::Intersects(const Vector3D<f32>& origin, const Vector3D<f32>& direction, f32& distance) const noexcept
	{
		return value.Intersects(
			Vector3D<f32>::Load(origin),
			Detail::NormalizeDirection(direction),
			distance);
	}

	ContainmentType BoundingSphere::ContainedBy(
		const Vector4D<f32>& plane0,
		const Vector4D<f32>& plane1,
		const Vector4D<f32>& plane2,
		const Vector4D<f32>& plane3,
		const Vector4D<f32>& plane4,
		const Vector4D<f32>& plane5) const noexcept
	{
		return static_cast<ContainmentType>(value.ContainedBy(
			Detail::NormalizePlane(plane0),
			Detail::NormalizePlane(plane1),
			Detail::NormalizePlane(plane2),
			Detail::NormalizePlane(plane3),
			Detail::NormalizePlane(plane4),
			Detail::NormalizePlane(plane5)));
	}

	bool BoundingSphere::IsApproximately(const BoundingSphere& lhs, const BoundingSphere& rhs, f32 epsilon) noexcept
	{
		return Vector3D<f32>::IsApproximately(lhs.GetCenter(), rhs.GetCenter(), epsilon) &&
			   std::abs(lhs.GetRadius() - rhs.GetRadius()) <= epsilon;
	}

	BoundingSphere BoundingSphere::CreateMerged(const BoundingSphere& s1, const BoundingSphere& s2) noexcept
	{
		DirectX::BoundingSphere merged{};
		DirectX::BoundingSphere::CreateMerged(merged, s1.value, s2.value);
		return BoundingSphere(merged);
	}

	BoundingSphere BoundingSphere::CreateFromBoundingBox(const BoundingBox& box) noexcept
	{
		DirectX::BoundingSphere sphere{};
		DirectX::BoundingSphere::CreateFromBoundingBox(sphere, static_cast<DirectX::BoundingBox>(box));
		return BoundingSphere(sphere);
	}

	BoundingSphere BoundingSphere::CreateFromBoundingBox(const BoundingOrientedBox& box) noexcept
	{
		DirectX::BoundingSphere sphere{};
		DirectX::BoundingSphere::CreateFromBoundingBox(sphere, static_cast<DirectX::BoundingOrientedBox>(box));
		return BoundingSphere(sphere);
	}

	BoundingSphere BoundingSphere::CreateFromFrustum(const DirectX::BoundingFrustum& frustum) noexcept
	{
		DirectX::BoundingSphere sphere{};
		DirectX::BoundingSphere::CreateFromFrustum(sphere, frustum);
		return BoundingSphere(sphere);
	}

	BoundingSphere BoundingSphere::CreateFromPoints(std::span<const Vector3D<f32>> points) noexcept
	{
		return CreateFromPoints(points.size(), Detail::AsXMFloat3Pointer(points), sizeof(Vector3D<f32>));
	}

	BoundingSphere BoundingSphere::CreateFromPoints(usize count, const DirectX::XMFLOAT3* points, usize stride) noexcept
	{
		ASSERT_FMT(count > 0, "count는 0보다 커야 합니다.");
		ASSERT_FMT(points != nullptr, "points가 nullptr입니다.");
		ASSERT_FMT(stride >= sizeof(DirectX::XMFLOAT3), "stride는 DirectX::XMFLOAT3 크기 이상이어야 합니다.");

		DirectX::BoundingSphere sphere{};
		DirectX::BoundingSphere::CreateFromPoints(sphere, count, points, stride);
		return BoundingSphere(sphere);
	}
}
