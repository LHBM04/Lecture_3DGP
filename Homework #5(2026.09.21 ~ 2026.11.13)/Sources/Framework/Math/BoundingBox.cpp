#include "Precompiled.hpp"
#include "BoundingBox.hpp"

#include "../Core/Common.hpp"

#include "BoundingOrientedBox.hpp"
#include "BoundingSphere.hpp"

namespace TUK::Framework
{
	BoundingBox::BoundingBox() noexcept
		: value()
	{
	}

	BoundingBox::BoundingBox(const Vector3D<f32>& center, const Vector3D<f32>& extents) noexcept
		: value(static_cast<DirectX::XMFLOAT3>(center), static_cast<DirectX::XMFLOAT3>(extents))
	{
		ASSERT_FMT(extents.GetX() >= 0.0f &&
				   extents.GetY() >= 0.0f &&
				   extents.GetZ() >= 0.0f, "extents의 각 성분은 0 이상이어야 합니다.");
	}

	BoundingBox::BoundingBox(const DirectX::BoundingBox& source) noexcept
		: value(source)
	{
	}

	BoundingBox::operator DirectX::BoundingBox() const noexcept
	{
		return value;
	}

	bool BoundingBox::operator==(const BoundingBox& other) const noexcept
	{
		return GetCenter() == other.GetCenter() && GetExtents() == other.GetExtents();
	}

	bool BoundingBox::operator!=(const BoundingBox& other) const noexcept
	{
		return !(*this == other);
	}

	std::partial_ordering BoundingBox::operator<=>(const BoundingBox& other) const noexcept
	{
		if (const auto order = GetCenter() <=> other.GetCenter(); order != 0)
		{
			return order;
		}

		return GetExtents() <=> other.GetExtents();
	}

	Vector3D<f32> BoundingBox::GetCenter() const noexcept
	{
		return Vector3D<f32>(value.Center);
	}

	void BoundingBox::SetCenter(const Vector3D<f32>& center) noexcept
	{
		value.Center = static_cast<DirectX::XMFLOAT3>(center);
	}

	Vector3D<f32> BoundingBox::GetExtents() const noexcept
	{
		return Vector3D<f32>(value.Extents);
	}

	void BoundingBox::SetExtents(const Vector3D<f32>& extents) noexcept
	{
		ASSERT_FMT(extents.GetX() >= 0.0f &&
				   extents.GetY() >= 0.0f &&
				   extents.GetZ() >= 0.0f, "extents의 각 성분은 0 이상이어야 합니다.");
		value.Extents = static_cast<DirectX::XMFLOAT3>(extents);
	}

	void BoundingBox::Set(const Vector3D<f32>& center, const Vector3D<f32>& extents) noexcept
	{
		SetCenter(center);
		SetExtents(extents);
	}

	Vector3D<f32> BoundingBox::GetMin() const noexcept
	{
		return GetCenter() - GetExtents();
	}

	Vector3D<f32> BoundingBox::GetMax() const noexcept
	{
		return GetCenter() + GetExtents();
	}

	Vector3D<f32> BoundingBox::GetSize() const noexcept
	{
		return GetExtents() * 2.0f;
	}

	void BoundingBox::SetMinMax(const Vector3D<f32>& min, const Vector3D<f32>& max) noexcept
	{
		ASSERT_FMT(min.GetX() <= max.GetX() &&
				   min.GetY() <= max.GetY() &&
				   min.GetZ() <= max.GetZ(), "min은 max보다 작거나 같아야 합니다.");

		Set((min + max) * 0.5f, (max - min) * 0.5f);
	}

	bool BoundingBox::IsFinite() const noexcept
	{
		return GetCenter().IsFinite() && GetExtents().IsFinite();
	}

	bool BoundingBox::IsValid() const noexcept
	{
		const Vector3D<f32> extents = GetExtents();
		return IsFinite() &&
			   extents.GetX() >= 0.0f &&
			   extents.GetY() >= 0.0f &&
			   extents.GetZ() >= 0.0f;
	}

	std::array<Vector3D<f32>, BoundingBox::CORNER_COUNT> BoundingBox::GetCorners() const noexcept
	{
		return Detail::ExtractCorners(value);
	}

	void BoundingBox::GetCorners(std::span<Vector3D<f32>, CORNER_COUNT> corners) const noexcept
	{
		Detail::ExtractCorners(value, corners);
	}

	void BoundingBox::GetCorners(DirectX::XMFLOAT3* corners) const noexcept
	{
		ASSERT_FMT(corners != nullptr, "corners가 nullptr입니다.");
		value.GetCorners(corners);
	}

	BoundingBox BoundingBox::GetTransformed(const Matrix4x4<f32>& matrix) const noexcept
	{
		BoundingBox result{};
		Transform(result, matrix);
		return result;
	}

	BoundingBox BoundingBox::GetTransformed(f32 scale, const Quaternion<f32>& rotation, const Vector3D<f32>& translation) const noexcept
	{
		BoundingBox result{};
		Transform(result, scale, rotation, translation);
		return result;
	}

	void BoundingBox::Transform(const Matrix4x4<f32>& matrix) noexcept
	{
		*this = GetTransformed(matrix);
	}

	void BoundingBox::Transform(f32 scale, const Quaternion<f32>& rotation, const Vector3D<f32>& translation) noexcept
	{
		*this = GetTransformed(scale, rotation, translation);
	}

	void BoundingBox::Transform(BoundingBox& out, const Matrix4x4<f32>& matrix) const noexcept
	{
		value.Transform(out.value, Matrix4x4<f32>::Load(matrix));
	}

	void BoundingBox::Transform(BoundingBox& out, f32 scale, const Quaternion<f32>& rotation, const Vector3D<f32>& translation) const noexcept
	{
		const Quaternion<f32> normalizedRotation = Detail::NormalizeRotation(rotation);
		value.Transform(
			out.value,
			scale,
			Quaternion<f32>::Load(normalizedRotation),
			Vector3D<f32>::Load(translation));
	}

	ContainmentType BoundingBox::Contains(const Vector3D<f32>& point) const noexcept
	{
		return static_cast<ContainmentType>(value.Contains(Vector3D<f32>::Load(point)));
	}

	ContainmentType BoundingBox::Contains(const Vector3D<f32>& v0, const Vector3D<f32>& v1, const Vector3D<f32>& v2) const noexcept
	{
		return static_cast<ContainmentType>(value.Contains(
			Vector3D<f32>::Load(v0),
			Vector3D<f32>::Load(v1),
			Vector3D<f32>::Load(v2)));
	}

	ContainmentType BoundingBox::Contains(const BoundingSphere& sphere) const noexcept
	{
		return static_cast<ContainmentType>(value.Contains(static_cast<DirectX::BoundingSphere>(sphere)));
	}

	ContainmentType BoundingBox::Contains(const BoundingBox& box) const noexcept
	{
		return static_cast<ContainmentType>(value.Contains(box.value));
	}

	ContainmentType BoundingBox::Contains(const BoundingOrientedBox& box) const noexcept
	{
		return static_cast<ContainmentType>(value.Contains(static_cast<DirectX::BoundingOrientedBox>(box)));
	}

	ContainmentType BoundingBox::Contains(const DirectX::BoundingFrustum& frustum) const noexcept
	{
		return static_cast<ContainmentType>(value.Contains(frustum));
	}

	bool BoundingBox::Intersects(const BoundingSphere& sphere) const noexcept
	{
		return value.Intersects(static_cast<DirectX::BoundingSphere>(sphere));
	}

	bool BoundingBox::Intersects(const BoundingBox& box) const noexcept
	{
		return value.Intersects(box.value);
	}

	bool BoundingBox::Intersects(const BoundingOrientedBox& box) const noexcept
	{
		return value.Intersects(static_cast<DirectX::BoundingOrientedBox>(box));
	}

	bool BoundingBox::Intersects(const DirectX::BoundingFrustum& frustum) const noexcept
	{
		return value.Intersects(frustum);
	}

	bool BoundingBox::Intersects(const Vector3D<f32>& v0, const Vector3D<f32>& v1, const Vector3D<f32>& v2) const noexcept
	{
		Detail::AssertNonDegenerateTriangle(v0, v1, v2);
		return value.Intersects(
			Vector3D<f32>::Load(v0),
			Vector3D<f32>::Load(v1),
			Vector3D<f32>::Load(v2));
	}

	PlaneIntersectionType BoundingBox::Intersects(const Vector4D<f32>& plane) const noexcept
	{
		return static_cast<PlaneIntersectionType>(value.Intersects(Detail::NormalizePlane(plane)));
	}

	bool BoundingBox::Intersects(const Vector3D<f32>& origin, const Vector3D<f32>& direction, f32& distance) const noexcept
	{
		return value.Intersects(
			Vector3D<f32>::Load(origin),
			Detail::NormalizeDirection(direction),
			distance);
	}

	ContainmentType BoundingBox::ContainedBy(
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

	bool BoundingBox::IsApproximately(const BoundingBox& lhs, const BoundingBox& rhs, f32 epsilon) noexcept
	{
		return Vector3D<f32>::IsApproximately(lhs.GetCenter(), rhs.GetCenter(), epsilon) &&
			   Vector3D<f32>::IsApproximately(lhs.GetExtents(), rhs.GetExtents(), epsilon);
	}

	BoundingBox BoundingBox::CreateMerged(const BoundingBox& b1, const BoundingBox& b2) noexcept
	{
		DirectX::BoundingBox merged{};
		DirectX::BoundingBox::CreateMerged(merged, b1.value, b2.value);
		return BoundingBox(merged);
	}

	BoundingBox BoundingBox::CreateFromSphere(const BoundingSphere& sphere) noexcept
	{
		DirectX::BoundingBox box{};
		DirectX::BoundingBox::CreateFromSphere(box, static_cast<DirectX::BoundingSphere>(sphere));
		return BoundingBox(box);
	}

	BoundingBox BoundingBox::CreateFromMinMax(const Vector3D<f32>& min, const Vector3D<f32>& max) noexcept
	{
		BoundingBox box{};
		box.SetMinMax(min, max);
		return box;
	}

	BoundingBox BoundingBox::CreateFromPoints(const Vector3D<f32>& point1, const Vector3D<f32>& point2) noexcept
	{
		DirectX::BoundingBox box{};
		DirectX::BoundingBox::CreateFromPoints(box, Vector3D<f32>::Load(point1), Vector3D<f32>::Load(point2));
		return BoundingBox(box);
	}

	BoundingBox BoundingBox::CreateFromPoints(std::span<const Vector3D<f32>> points) noexcept
	{
		return CreateFromPoints(points.size(), Detail::AsXMFloat3Pointer(points), sizeof(Vector3D<f32>));
	}

	BoundingBox BoundingBox::CreateFromPoints(usize count, const DirectX::XMFLOAT3* points, usize stride) noexcept
	{
		ASSERT_FMT(count > 0, "count는 0보다 커야 합니다.");
		ASSERT_FMT(points != nullptr, "points가 nullptr입니다.");
		ASSERT_FMT(stride >= sizeof(DirectX::XMFLOAT3), "stride는 DirectX::XMFLOAT3 크기 이상이어야 합니다.");

		DirectX::BoundingBox box{};
		DirectX::BoundingBox::CreateFromPoints(box, count, points, stride);
		return BoundingBox(box);
	}
}
