#pragma once

#include <DirectXCollision.h>

#include "Collider.h"

class Vector3D;

class SphereCollider final : public Collider<SphereCollider>
{
public:
	SphereCollider() = default;
	~SphereCollider() override = default;

	using Collider<SphereCollider>::IsIntersects;

	[[nodiscard]] float GetRadius() const noexcept;
	void SetRadius(float radius_) noexcept;

	[[nodiscard]] const DirectX::BoundingSphere& GetVolume() const noexcept;

	bool IsIntersects(const DirectX::BoundingFrustum& frustum_) const;

	void OnUpdate(float deltaTime_) override;

private:
	DirectX::BoundingSphere localSphere{ {0.0f, 0.0f, 0.0f}, 1.0f };
	DirectX::BoundingSphere worldSphere{ {0.0f, 0.0f, 0.0f}, 1.0f };
};

// ============================================================================
// Inline Implementations
// ============================================================================

inline float SphereCollider::GetRadius() const noexcept
{
	return localSphere.Radius;
}

inline void SphereCollider::SetRadius(float radius_) noexcept
{
	localSphere.Radius = radius_;
}

inline const DirectX::BoundingSphere& SphereCollider::GetVolume() const noexcept
{
	return worldSphere;
}

