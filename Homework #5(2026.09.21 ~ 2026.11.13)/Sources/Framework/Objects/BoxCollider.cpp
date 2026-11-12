#include "Precompiled.hpp"
#include "BoxCollider.hpp"

namespace TUK::Framework
{
#ifdef _DEBUG
	void BoxCollider::OnDrawGizmos(GPUCommandBuffer& commandBuffer)
	{
		if (!IsQueryEnabled())
		{
			return;
		}

		const auto corners = GetWorldBox().GetCorners();
		static constexpr std::array<u32, 24> EDGE_INDICES
		{
			0, 1, 1, 2, 2, 3, 3, 0,
			4, 5, 5, 6, 6, 7, 7, 4,
			0, 4, 1, 5, 2, 6, 3, 7
		};

		std::array<Vector3D<f32>, EDGE_INDICES.size()> vertices{};
		for (usize index = 0; index < EDGE_INDICES.size(); ++index)
		{
			vertices[index] = corners[EDGE_INDICES[index]];
		}

		DrawDebugLines(commandBuffer, vertices);
	}
#endif

	Vector3D<f32> BoxCollider::GetCenter() const noexcept
	{
		return box.GetCenter();
	}

	void BoxCollider::SetCenter(const Vector3D<f32>& value) noexcept
	{
		ASSERT_FMT(value.IsFinite(), "콜라이더 중심 좌표는 유한한 값이어야 합니다.");
		box.SetCenter(value);
	}

	Vector3D<f32> BoxCollider::GetSize() const noexcept
	{
		return box.GetSize();
	}

	void BoxCollider::SetSize(const Vector3D<f32>& value) noexcept
	{
		ASSERT_FMT(value.IsFinite(), "박스 크기는 유한한 값이어야 합니다.");
		ASSERT_FMT(value.GetX() >= 0.0f &&
			value.GetY() >= 0.0f &&
			value.GetZ() >= 0.0f, "박스 크기는 0 이상이어야 합니다.");
		box.SetExtents(value * 0.5f);
	}

	bool BoxCollider::IsValid() const noexcept
	{
		return box.IsValid() && HasValidTransform();
	}

	BoundingOrientedBox BoxCollider::GetWorldBox() const noexcept
	{
		ASSERT_FMT(IsValid(), "콜라이더 형상 또는 Transform이 유효하지 않습니다.");
		return box.GetTransformed(GetWorldMatrix());
	}

	BoundingBox BoxCollider::GetWorldBounds() const noexcept
	{
		const auto corners = GetWorldBox().GetCorners();
		return BoundingBox::CreateFromPoints(corners);
	}

	bool BoxCollider::Intersects(const BoundingSphere& other) const noexcept
	{
		return IsQueryEnabled() && other.IsValid() && GetWorldBox().Intersects(other);
	}

	bool BoxCollider::Intersects(const BoundingOrientedBox& other) const noexcept
	{
		return IsQueryEnabled() && other.IsValid() && GetWorldBox().Intersects(other);
	}

	bool BoxCollider::Raycast(
		const Vector3D<f32>& origin,
		const Vector3D<f32>& direction,
		f32& distance,
		f32 maxDistance) const noexcept
	{
		Vector3D<f32> normalizedDirection{};
		if (!IsQueryEnabled() || !TryNormalizeRay(origin, direction, maxDistance, normalizedDirection))
		{
			return false;
		}

		f32 hitDistance = 0.0f;
		if (!GetWorldBox().Intersects(origin, normalizedDirection, hitDistance) || hitDistance > maxDistance)
		{
			return false;
		}

		distance = hitDistance;
		return true;
	}
}
