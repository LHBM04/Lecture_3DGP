#include "Precompiled.hpp"
#include "SphereCollider.hpp"

namespace TUK::Framework
{
#ifdef _DEBUG
	void SphereCollider::OnDrawGizmos(GPUCommandBuffer& commandBuffer)
	{
		if (!IsQueryEnabled())
		{
			return;
		}

		const auto worldSphere = GetWorldSphere();
		const auto center = worldSphere.GetCenter();
		const f32 radius = worldSphere.GetRadius();
		constexpr u32 SEGMENT_COUNT = 32;
		std::array<Vector3D<f32>, SEGMENT_COUNT * 3 * 2> vertices{};
		for (u32 plane = 0; plane < 3; ++plane)
		{
			for (u32 segment = 0; segment < SEGMENT_COUNT; ++segment)
			{
				for (u32 end = 0; end < 2; ++end)
				{
					const f32 angle = 2.0f * std::numbers::pi_v<f32> *
						static_cast<f32>((segment + end) % SEGMENT_COUNT) / static_cast<f32>(SEGMENT_COUNT);
					const f32 x = radius * std::cos(angle);
					const f32 y = radius * std::sin(angle);
					Vector3D<f32> offset{};
					if (plane == 0)
					{
						offset = Vector3D<f32>{ x, y, 0.0f };
					}
					else if (plane == 1)
					{
						offset = Vector3D<f32>{ x, 0.0f, y };
					}
					else
					{
						offset = Vector3D<f32>{ 0.0f, x, y };
					}
					vertices[(plane * SEGMENT_COUNT + segment) * 2 + end] = center + offset;
				}
			}
		}
		DrawDebugLines(commandBuffer, vertices);
	}
#endif

	Vector3D<f32> SphereCollider::GetCenter() const noexcept
	{
		return sphere.GetCenter();
	}

	void SphereCollider::SetCenter(const Vector3D<f32>& value) noexcept
	{
		ASSERT_FMT(value.IsFinite(), "콜라이더 중심 좌표는 유한한 값이어야 합니다.");
		sphere.SetCenter(value);
	}

	f32 SphereCollider::GetRadius() const noexcept
	{
		return sphere.GetRadius();
	}

	void SphereCollider::SetRadius(f32 value) noexcept
	{
		ASSERT_FMT(std::isfinite(value), "구 반지름은 유한한 값이어야 합니다.");
		ASSERT_FMT(value >= 0.0f, "구 반지름은 0 이상이어야 합니다.");
		sphere.SetRadius(value);
	}

	BoundingSphere SphereCollider::GetWorldSphere() const noexcept
	{
		ASSERT_FMT(IsValid(), "콜라이더 형상 또는 Transform이 유효하지 않습니다.");
		const auto world = GetWorldMatrix();
		const std::array<Vector3D<f32>, 3> axes
		{
			world.MultiplyVector(Vector3D<f32>{ 1.0f, 0.0f, 0.0f }),
			world.MultiplyVector(Vector3D<f32>{ 0.0f, 1.0f, 0.0f }),
			world.MultiplyVector(Vector3D<f32>{ 0.0f, 0.0f, 1.0f })
		};

		f32 scaleSquared = 0.0f;
		for (const auto& axis : axes)
		{
			f32 rowSum = 0.0f;
			for (const auto& other : axes)
			{
				rowSum += std::abs(Vector3D<f32>::Dot(axis, other));
			}
			scaleSquared = std::max(scaleSquared, rowSum);
		}

		return BoundingSphere{ world.MultiplyPoint3x4(sphere.GetCenter()),
			sphere.GetRadius() * std::sqrt(scaleSquared) };
	}

	bool SphereCollider::IsValid() const noexcept
	{
		return sphere.IsValid() && HasValidTransform();
	}

	BoundingBox SphereCollider::GetWorldBounds() const noexcept
	{
		return BoundingBox::CreateFromSphere(GetWorldSphere());
	}

	bool SphereCollider::Intersects(const BoundingSphere& other) const noexcept
	{
		return IsQueryEnabled() && other.IsValid() && GetWorldSphere().Intersects(other);
	}

	bool SphereCollider::Intersects(const BoundingOrientedBox& other) const noexcept
	{
		return IsQueryEnabled() && other.IsValid() && GetWorldSphere().Intersects(other);
	}

	bool SphereCollider::Raycast(
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
		if (!GetWorldSphere().Intersects(origin, normalizedDirection, hitDistance) || hitDistance > maxDistance)
		{
			return false;
		}

		distance = hitDistance;
		return true;
	}
}
