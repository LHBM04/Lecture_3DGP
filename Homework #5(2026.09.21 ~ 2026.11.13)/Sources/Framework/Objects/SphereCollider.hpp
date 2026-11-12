#pragma once

#include "Collider.hpp"

namespace TUK::Framework
{
	class SphereCollider final : public Collider
	{
	public:
		using Collider::Collider;
		using Collider::Intersects;

#ifdef _DEBUG
		void OnDrawGizmos(GPUCommandBuffer& commandBuffer) override;
#endif

		[[nodiscard]] Vector3D<f32> GetCenter() const noexcept;
		void SetCenter(const Vector3D<f32>& value) noexcept;

		[[nodiscard]] f32 GetRadius() const noexcept;
		void SetRadius(f32 value) noexcept;

		[[nodiscard]] bool IsValid() const noexcept override;

		[[nodiscard]] BoundingSphere GetWorldSphere() const noexcept;
		[[nodiscard]] BoundingBox GetWorldBounds() const noexcept override;

		[[nodiscard]] bool Intersects(const BoundingSphere& sphere) const noexcept override;
		[[nodiscard]] bool Intersects(const BoundingOrientedBox& box) const noexcept override;

		[[nodiscard]] bool Raycast(
			const Vector3D<f32>& origin,
			const Vector3D<f32>& direction,
			f32& distance,
			f32 maxDistance = std::numeric_limits<f32>::max()) const noexcept override;

	private:
		BoundingSphere sphere{ Vector3D<f32>{ 0.0f }, 0.5f };
	};
}
