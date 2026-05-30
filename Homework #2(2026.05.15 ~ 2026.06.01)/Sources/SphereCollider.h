#pragma once

#include <DirectXCollision.h>
#include "Collider.h"

class Vector3D;

class SphereCollider final : public Collider
{
public:
	SphereCollider() = default;
	~SphereCollider() override = default;

	[[nodiscard]] float GetRadius() const noexcept;
	void SetRadius(float radius_) noexcept;

	[[nodiscard]] const DirectX::BoundingSphere& GetVolume() const noexcept;

	// Double Dispatch Implementation
	bool IsIntersects(const Collider* other_) const override;
	bool IsIntersects(const SphereCollider* other_) const override;
	bool IsIntersects(const CubeCollider* other_) const override;

	// Utilities
	bool IsIntersects(const Vector3D& rayOrigin_, const Vector3D& rayDir_, float& distance_) const override;
	bool IsIntersects(const DirectX::BoundingFrustum& frustum_) const override;
	DirectX::BoundingBox GetBoundingVolume() const override;

	void UpdateVolume() override;

	void OnUpdate(float deltaTime_) override;

private:
	DirectX::BoundingSphere localSphere{ {0.0f, 0.0f, 0.0f}, 1.0f };
	DirectX::BoundingSphere worldSphere{ {0.0f, 0.0f, 0.0f}, 1.0f };
};
