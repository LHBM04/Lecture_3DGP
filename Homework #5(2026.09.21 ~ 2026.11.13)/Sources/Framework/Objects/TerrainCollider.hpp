#pragma once

#include "Collider.hpp"

namespace TUK::Framework
{
	class TerrainAsset;

	class TerrainCollider final : public Collider
	{
	public:
		using Collider::Collider;
		using Collider::Intersects;

#ifdef _DEBUG
		void OnAwake() override;
		void OnDrawGizmos(GPUCommandBuffer& commandBuffer) override;
#endif

		[[nodiscard]] const TerrainAsset* GetTerrain() const noexcept;
		void SetTerrain(const TerrainAsset* value) noexcept;

		[[nodiscard]] bool IsValid() const noexcept override;

		[[nodiscard]] BoundingBox GetWorldBounds() const noexcept override;

		[[nodiscard]] bool Intersects(const BoundingSphere& sphere) const noexcept override;
		[[nodiscard]] bool Intersects(const BoundingOrientedBox& box) const noexcept override;

		[[nodiscard]] bool Raycast(
			const Vector3D<f32>& origin,
			const Vector3D<f32>& direction,
			f32& distance,
			f32 maxDistance = std::numeric_limits<f32>::max()) const noexcept override;

	private:
		const TerrainAsset* asset = nullptr;
	};
}
