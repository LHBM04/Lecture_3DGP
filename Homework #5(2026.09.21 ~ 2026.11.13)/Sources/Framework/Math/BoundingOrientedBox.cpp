#include "Precompiled.hpp"
#include "BoundingOrientedBox.hpp"

#include "../Core/Common.hpp"

#include "BoundingBox.hpp"
#include "BoundingSphere.hpp"
#include "Mathf.hpp"

namespace TUK::Framework
{
	namespace
	{
		constexpr f32 AXIS_LENGTH_SQUARED_EPSILON = 1.0e-12f;

		[[nodiscard]] bool IsAffineMatrix(const Matrix4x4<f32>& matrix) noexcept
		{
			return std::abs(matrix[3]) <= Mathf::Epsilon &&
				   std::abs(matrix[7]) <= Mathf::Epsilon &&
				   std::abs(matrix[11]) <= Mathf::Epsilon &&
				   std::abs(matrix[15] - 1.0f) <= Mathf::Epsilon;
		}

		[[nodiscard]] Quaternion<f32> BuildOrientationFromBasis(
			const Vector3D<f32>& axis0,
			const Vector3D<f32>& axis1,
			const Vector3D<f32>& axis2) noexcept
		{
			DirectX::XMMATRIX basis = DirectX::XMMatrixIdentity();
			basis.r[0] = Vector3D<f32>::Load(axis0);
			basis.r[1] = Vector3D<f32>::Load(axis1);
			basis.r[2] = Vector3D<f32>::Load(axis2);

			Quaternion<f32> result{};
			Quaternion<f32>::Store(result, DirectX::XMQuaternionNormalize(DirectX::XMQuaternionRotationMatrix(basis)));
			return result;
		}

		[[nodiscard]] std::array<Vector3D<f32>, 3> CreateBasisFromSingleAxis(
			const Vector3D<f32>& axis,
			usize activeIndex) noexcept
		{
			const Vector3D<f32> reference = (std::abs(Vector3D<f32>::Dot(axis, Vector3D<f32>::GetUp())) < 0.99f)
				? Vector3D<f32>::GetUp()
				: Vector3D<f32>::GetRight();
			const Vector3D<f32> ortho1 = Vector3D<f32>::Normalize(Vector3D<f32>::Cross(reference, axis));
			const Vector3D<f32> ortho2 = Vector3D<f32>::Normalize(Vector3D<f32>::Cross(axis, ortho1));

			std::array<Vector3D<f32>, 3> basis{};
			basis[activeIndex] = axis;
			basis[(activeIndex + 1) % 3] = ortho1;
			basis[(activeIndex + 2) % 3] = ortho2;
			return basis;
		}

		[[nodiscard]] std::array<Vector3D<f32>, 3> CreateBasisFromTwoAxes(
			const Vector3D<f32>& axisA,
			usize indexA,
			const Vector3D<f32>& axisB,
			usize indexB) noexcept
		{
			const usize missingIndex = 3 - indexA - indexB;
			const Vector3D<f32> missingAxis = (indexB == (indexA + 1) % 3)
				? Vector3D<f32>::Normalize(Vector3D<f32>::Cross(axisA, axisB))
				: Vector3D<f32>::Normalize(Vector3D<f32>::Cross(axisB, axisA));

			std::array<Vector3D<f32>, 3> basis{};
			basis[indexA] = axisA;
			basis[indexB] = axisB;
			basis[missingIndex] = missingAxis;

			const usize nextIndex = (indexA + 1) % 3;
			const usize prevIndex = (indexA + 2) % 3;
			basis[nextIndex] = Vector3D<f32>::Normalize(Vector3D<f32>::Cross(basis[prevIndex], basis[indexA]));
			return basis;
		}

		[[nodiscard]] std::array<Vector3D<f32>, BoundingOrientedBox::CORNER_COUNT> TransformCornersWithAffineAxes(
			const Vector3D<f32>& center,
			const std::array<Vector3D<f32>, 3>& axes) noexcept
		{
			std::array<Vector3D<f32>, BoundingOrientedBox::CORNER_COUNT> corners{};
			for (usize index = 0; index < BoundingOrientedBox::CORNER_COUNT; ++index)
			{
				const f32 sx = (index & 1) ? 1.0f : -1.0f;
				const f32 sy = (index & 2) ? 1.0f : -1.0f;
				const f32 sz = (index & 4) ? 1.0f : -1.0f;
				corners[index] = center + axes[0] * sx + axes[1] * sy + axes[2] * sz;
			}
			return corners;
		}
	}
	BoundingOrientedBox::BoundingOrientedBox() noexcept
		: value()
	{
	}

	BoundingOrientedBox::BoundingOrientedBox(const Vector3D<f32>& center, const Vector3D<f32>& extents, const Quaternion<f32>& orientation) noexcept
		: value(
			static_cast<DirectX::XMFLOAT3>(center),
			static_cast<DirectX::XMFLOAT3>(extents),
			static_cast<DirectX::XMFLOAT4>(Detail::NormalizeRotation(orientation)))
	{
		ASSERT_FMT(extents.GetX() >= 0.0f &&
			       extents.GetY() >= 0.0f &&
			       extents.GetZ() >= 0.0f, "extents의 각 성분은 0 이상이어야 합니다.");
	}

	BoundingOrientedBox::BoundingOrientedBox(const DirectX::BoundingOrientedBox& source) noexcept
		: value(source)
	{
	}

	BoundingOrientedBox::operator DirectX::BoundingOrientedBox() const noexcept
	{
		return value;
	}

	bool BoundingOrientedBox::operator==(const BoundingOrientedBox& other) const noexcept
	{
		return GetCenter() == other.GetCenter() &&
			   GetExtents() == other.GetExtents() &&
			   GetOrientation() == other.GetOrientation();
	}

	bool BoundingOrientedBox::operator!=(const BoundingOrientedBox& other) const noexcept
	{
		return !(*this == other);
	}

	std::partial_ordering BoundingOrientedBox::operator<=>(const BoundingOrientedBox& other) const noexcept
	{
		if (const auto order = GetCenter() <=> other.GetCenter(); order != 0)
		{
			return order;
		}

		if (const auto order = GetExtents() <=> other.GetExtents(); order != 0)
		{
			return order;
		}

		return GetOrientation() <=> other.GetOrientation();
	}

	Vector3D<f32> BoundingOrientedBox::GetCenter() const noexcept
	{
		return Vector3D<f32>(value.Center);
	}

	void BoundingOrientedBox::SetCenter(const Vector3D<f32>& center) noexcept
	{
		value.Center = static_cast<DirectX::XMFLOAT3>(center);
	}

	Vector3D<f32> BoundingOrientedBox::GetExtents() const noexcept
	{
		return Vector3D<f32>(value.Extents);
	}

	void BoundingOrientedBox::SetExtents(const Vector3D<f32>& extents) noexcept
	{
		ASSERT_FMT(extents.GetX() >= 0.0f &&
				   extents.GetY() >= 0.0f &&
				   extents.GetZ() >= 0.0f, "extents의 각 성분은 0 이상이어야 합니다.");
		value.Extents = static_cast<DirectX::XMFLOAT3>(extents);
	}

	Quaternion<f32> BoundingOrientedBox::GetOrientation() const noexcept
	{
		return Quaternion<f32>(value.Orientation);
	}

	void BoundingOrientedBox::SetOrientation(const Quaternion<f32>& orientation) noexcept
	{
		value.Orientation = static_cast<DirectX::XMFLOAT4>(Detail::NormalizeRotation(orientation));
	}

	void BoundingOrientedBox::Set(const Vector3D<f32>& center, const Vector3D<f32>& extents, const Quaternion<f32>& orientation) noexcept
	{
		SetCenter(center);
		SetExtents(extents);
		SetOrientation(orientation);
	}

	Vector3D<f32> BoundingOrientedBox::GetSize() const noexcept
	{
		return GetExtents() * 2.0f;
	}

	bool BoundingOrientedBox::IsFinite() const noexcept
	{
		return GetCenter().IsFinite() && GetExtents().IsFinite() && GetOrientation().IsFinite();
	}

	bool BoundingOrientedBox::IsValid() const noexcept
	{
		const Vector3D<f32> extents = GetExtents();
		const Quaternion<f32> orientation = GetOrientation();
		return IsFinite() &&
			   extents.GetX() >= 0.0f &&
			   extents.GetY() >= 0.0f &&
			   extents.GetZ() >= 0.0f &&
			   orientation.IsNormalized(Mathf::Epsilon);
	}

	std::array<Vector3D<f32>, BoundingOrientedBox::CORNER_COUNT> BoundingOrientedBox::GetCorners() const noexcept
	{
		return Detail::ExtractCorners(value);
	}

	void BoundingOrientedBox::GetCorners(std::span<Vector3D<f32>, CORNER_COUNT> corners) const noexcept
	{
		Detail::ExtractCorners(value, corners);
	}

	void BoundingOrientedBox::GetCorners(DirectX::XMFLOAT3* corners) const noexcept
	{
		ASSERT_FMT(corners != nullptr, "corners가 nullptr입니다.");
		value.GetCorners(corners);
	}

	BoundingOrientedBox BoundingOrientedBox::GetTransformed(const Matrix4x4<f32>& matrix) const noexcept
	{
		BoundingOrientedBox result{};
		Transform(result, matrix);
		return result;
	}

	BoundingOrientedBox BoundingOrientedBox::GetTransformed(f32 scale, const Quaternion<f32>& rotation, const Vector3D<f32>& translation) const noexcept
	{
		BoundingOrientedBox result{};
		Transform(result, scale, rotation, translation);
		return result;
	}

	void BoundingOrientedBox::Transform(const Matrix4x4<f32>& matrix) noexcept
	{
		*this = GetTransformed(matrix);
	}

	void BoundingOrientedBox::Transform(f32 scale, const Quaternion<f32>& rotation, const Vector3D<f32>& translation) noexcept
	{
		*this = GetTransformed(scale, rotation, translation);
	}

	void BoundingOrientedBox::Transform(BoundingOrientedBox& out, const Matrix4x4<f32>& matrix) const noexcept
	{
		if (!IsAffineMatrix(matrix))
		{
			auto corners = GetCorners();
			for (Vector3D<f32>& corner : corners)
			{
				corner = matrix.MultiplyPoint(corner);
			}
			out = CreateFromPoints(corners);
			return;
		}

		const Vector3D<f32> extents = GetExtents();
		ASSERT_FMT(extents.GetX() >= 0.0f &&
			       extents.GetY() >= 0.0f &&
			       extents.GetZ() >= 0.0f, "extents의 각 성분은 0 이상이어야 합니다.");

		const Quaternion<f32> currentOrientation = Detail::NormalizeRotation(GetOrientation());
		const Vector3D<f32> transformedCenter = matrix.MultiplyPoint3x4(GetCenter());

		const std::array<Vector3D<f32>, 3> transformedAxes
		{
			matrix.MultiplyVector((currentOrientation * Vector3D<f32>::GetRight()) * extents.GetX()),
			matrix.MultiplyVector((currentOrientation * Vector3D<f32>::GetUp()) * extents.GetY()),
			matrix.MultiplyVector((currentOrientation * Vector3D<f32>::GetForward()) * extents.GetZ())
		};

		std::array<f32, 3> axisLengths{};
		std::array<Vector3D<f32>, 3> unitAxes{};
		std::array<usize, 3> activeIndices{};
		usize activeCount = 0;

		for (usize index = 0; index < 3; ++index)
		{
			const f32 lengthSquared = transformedAxes[index].GetSqrMagnitude();
			if (lengthSquared > AXIS_LENGTH_SQUARED_EPSILON)
			{
				axisLengths[index] = std::sqrt(lengthSquared);
				unitAxes[index] = transformedAxes[index] / axisLengths[index];
				activeIndices[activeCount++] = index;
			}
		}

		const Vector3D<f32> transformedExtents(axisLengths[0], axisLengths[1], axisLengths[2]);

		if (activeCount == 0)
		{
			out.Set(transformedCenter, Vector3D<f32>::GetZero(), currentOrientation);
			return;
		}

		if (activeCount == 1)
		{
			const usize activeIndex = activeIndices[0];
			const auto basis = CreateBasisFromSingleAxis(unitAxes[activeIndex], activeIndex);
			out.Set(transformedCenter, transformedExtents, BuildOrientationFromBasis(basis[0], basis[1], basis[2]));
			return;
		}

		if (activeCount == 2)
		{
			const usize indexA = activeIndices[0];
			const usize indexB = activeIndices[1];
			if (std::abs(Vector3D<f32>::Dot(unitAxes[indexA], unitAxes[indexB])) <= Mathf::Epsilon)
			{
				const auto basis = CreateBasisFromTwoAxes(unitAxes[indexA], indexA, unitAxes[indexB], indexB);
				out.Set(transformedCenter, transformedExtents, BuildOrientationFromBasis(basis[0], basis[1], basis[2]));
				return;
			}

			out = CreateFromPoints(TransformCornersWithAffineAxes(transformedCenter, transformedAxes));
			return;
		}

		const bool isOrthogonal =
			std::abs(Vector3D<f32>::Dot(unitAxes[0], unitAxes[1])) <= Mathf::Epsilon &&
			std::abs(Vector3D<f32>::Dot(unitAxes[0], unitAxes[2])) <= Mathf::Epsilon &&
			std::abs(Vector3D<f32>::Dot(unitAxes[1], unitAxes[2])) <= Mathf::Epsilon;

		if (!isOrthogonal)
		{
			out = CreateFromPoints(TransformCornersWithAffineAxes(transformedCenter, transformedAxes));
			return;
		}

		if (Vector3D<f32>::Dot(Vector3D<f32>::Cross(unitAxes[0], unitAxes[1]), unitAxes[2]) < 0.0f)
		{
			unitAxes[0] = -unitAxes[0];
		}

		unitAxes[2] = Vector3D<f32>::Normalize(Vector3D<f32>::Cross(unitAxes[0], unitAxes[1]));
		unitAxes[1] = Vector3D<f32>::Normalize(Vector3D<f32>::Cross(unitAxes[2], unitAxes[0]));
		out.Set(transformedCenter, transformedExtents, BuildOrientationFromBasis(unitAxes[0], unitAxes[1], unitAxes[2]));
	}

	void BoundingOrientedBox::Transform(BoundingOrientedBox& out, f32 scale, const Quaternion<f32>& rotation, const Vector3D<f32>& translation) const noexcept
	{
		const Quaternion<f32> normalizedRotation = Detail::NormalizeRotation(rotation);
		const Quaternion<f32> currentOrientation = Detail::NormalizeRotation(GetOrientation());
		const Vector3D<f32> extents = GetExtents();
		ASSERT_FMT(extents.GetX() >= 0.0f &&
			       extents.GetY() >= 0.0f &&
			       extents.GetZ() >= 0.0f, "extents의 각 성분은 0 이상이어야 합니다.");

		DirectX::BoundingOrientedBox source(
			static_cast<DirectX::XMFLOAT3>(GetCenter()),
			static_cast<DirectX::XMFLOAT3>(extents),
			static_cast<DirectX::XMFLOAT4>(currentOrientation));
		source.Transform(
			out.value,
			scale,
			Quaternion<f32>::Load(normalizedRotation),
			Vector3D<f32>::Load(translation));

		out.value.Extents = static_cast<DirectX::XMFLOAT3>(extents * std::abs(scale));
		out.SetOrientation(out.GetOrientation());
	}

	ContainmentType BoundingOrientedBox::Contains(const Vector3D<f32>& point) const noexcept
	{
		return static_cast<ContainmentType>(value.Contains(Vector3D<f32>::Load(point)));
	}

	ContainmentType BoundingOrientedBox::Contains(const Vector3D<f32>& v0, const Vector3D<f32>& v1, const Vector3D<f32>& v2) const noexcept
	{
		return static_cast<ContainmentType>(value.Contains(
			Vector3D<f32>::Load(v0),
			Vector3D<f32>::Load(v1),
			Vector3D<f32>::Load(v2)));
	}

	ContainmentType BoundingOrientedBox::Contains(const BoundingSphere& sphere) const noexcept
	{
		return static_cast<ContainmentType>(value.Contains(static_cast<DirectX::BoundingSphere>(sphere)));
	}

	ContainmentType BoundingOrientedBox::Contains(const BoundingBox& box) const noexcept
	{
		return static_cast<ContainmentType>(value.Contains(static_cast<DirectX::BoundingBox>(box)));
	}

	ContainmentType BoundingOrientedBox::Contains(const BoundingOrientedBox& box) const noexcept
	{
		return static_cast<ContainmentType>(value.Contains(box.value));
	}

	ContainmentType BoundingOrientedBox::Contains(const DirectX::BoundingFrustum& frustum) const noexcept
	{
		return static_cast<ContainmentType>(value.Contains(frustum));
	}

	bool BoundingOrientedBox::Intersects(const BoundingSphere& sphere) const noexcept
	{
		return value.Intersects(static_cast<DirectX::BoundingSphere>(sphere));
	}

	bool BoundingOrientedBox::Intersects(const BoundingBox& box) const noexcept
	{
		return value.Intersects(static_cast<DirectX::BoundingBox>(box));
	}

	bool BoundingOrientedBox::Intersects(const BoundingOrientedBox& box) const noexcept
	{
		return value.Intersects(box.value);
	}

	bool BoundingOrientedBox::Intersects(const DirectX::BoundingFrustum& frustum) const noexcept
	{
		return value.Intersects(frustum);
	}

	bool BoundingOrientedBox::Intersects(const Vector3D<f32>& v0, const Vector3D<f32>& v1, const Vector3D<f32>& v2) const noexcept
	{
		Detail::AssertNonDegenerateTriangle(v0, v1, v2);
		return value.Intersects(
			Vector3D<f32>::Load(v0),
			Vector3D<f32>::Load(v1),
			Vector3D<f32>::Load(v2));
	}

	PlaneIntersectionType BoundingOrientedBox::Intersects(const Vector4D<f32>& plane) const noexcept
	{
		return static_cast<PlaneIntersectionType>(value.Intersects(Detail::NormalizePlane(plane)));
	}

	bool BoundingOrientedBox::Intersects(const Vector3D<f32>& origin, const Vector3D<f32>& direction, f32& distance) const noexcept
	{
		return value.Intersects(
			Vector3D<f32>::Load(origin),
			Detail::NormalizeDirection(direction),
			distance);
	}

	ContainmentType BoundingOrientedBox::ContainedBy(
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

	bool BoundingOrientedBox::IsApproximately(const BoundingOrientedBox& lhs, const BoundingOrientedBox& rhs, f32 epsilon) noexcept
	{
		return Vector3D<f32>::IsApproximately(lhs.GetCenter(), rhs.GetCenter(), epsilon) &&
			   Vector3D<f32>::IsApproximately(lhs.GetExtents(), rhs.GetExtents(), epsilon) &&
			   Quaternion<f32>::IsApproximately(lhs.GetOrientation(), rhs.GetOrientation(), epsilon);
	}

	BoundingOrientedBox BoundingOrientedBox::CreateFromBoundingBox(const BoundingBox& box) noexcept
	{
		DirectX::BoundingOrientedBox orientedBox{};
		DirectX::BoundingOrientedBox::CreateFromBoundingBox(orientedBox, static_cast<DirectX::BoundingBox>(box));
		return BoundingOrientedBox(orientedBox);
	}

	BoundingOrientedBox BoundingOrientedBox::CreateFromPoints(std::span<const Vector3D<f32>> points) noexcept
	{
		return CreateFromPoints(points.size(), Detail::AsXMFloat3Pointer(points), sizeof(Vector3D<f32>));
	}

	BoundingOrientedBox BoundingOrientedBox::CreateFromPoints(usize count, const DirectX::XMFLOAT3* points, usize stride) noexcept
	{
		ASSERT_FMT(count > 0, "count는 0보다 커야 합니다.");
		ASSERT_FMT(points != nullptr, "points가 nullptr입니다.");
		ASSERT_FMT(stride >= sizeof(DirectX::XMFLOAT3), "stride는 DirectX::XMFLOAT3 크기 이상이어야 합니다.");

		DirectX::BoundingOrientedBox orientedBox{};
		DirectX::BoundingOrientedBox::CreateFromPoints(orientedBox, count, points, stride);
		return BoundingOrientedBox(orientedBox);
	}
}
