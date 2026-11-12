#pragma once

#include "Collider.hpp"

namespace TUK::Framework
{
	class BoxCollider final : public Collider
	{
	public:
		using Collider::Collider;
		using Collider::Intersects;

#ifdef _DEBUG
		void OnDrawGizmos(GPUCommandBuffer& commandBuffer) override;
#endif

		[[nodiscard]] Vector3D<f32> GetCenter() const noexcept;
		void SetCenter(const Vector3D<f32>& value) noexcept;

		[[nodiscard]] Vector3D<f32> GetSize() const noexcept;
		void SetSize(const Vector3D<f32>& value) noexcept;

		[[nodiscard]] bool IsValid() const noexcept override;

		[[nodiscard]] BoundingOrientedBox GetWorldBox() const noexcept;
		[[nodiscard]] BoundingBox GetWorldBounds() const noexcept override;

		[[nodiscard]] bool Intersects(const BoundingSphere& sphere) const noexcept override;
		[[nodiscard]] bool Intersects(const BoundingOrientedBox& box) const noexcept override;

		[[nodiscard]] bool Raycast(
			const Vector3D<f32>& origin,
			const Vector3D<f32>& direction,
			f32& distance,
			f32 maxDistance = std::numeric_limits<f32>::max()) const noexcept override;

	private:
		BoundingOrientedBox box{ Vector3D<f32>{ 0.0f }, Vector3D<f32>{ 0.5f }, Quaternion<f32>::GetIdentity() };
	};
}
