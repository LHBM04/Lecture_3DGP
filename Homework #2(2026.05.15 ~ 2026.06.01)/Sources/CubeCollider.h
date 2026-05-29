#pragma once

#include <DirectXCollision.h>

#include "Collider.h"

class Vector3D;

class CubeCollider final : public Collider<CubeCollider>
{
public:
	CubeCollider() = default;
	~CubeCollider() = default;

	[[nodiscard]] const DirectX::BoundingOrientedBox& GetVolume() const noexcept;
	void SetSize(const Vector3D& size_) noexcept;

	bool IsIntersects(const DirectX::BoundingFrustum& frustum_) const noexcept;

	void OnUpdate(float deltaTime_) noexcept;

private:
	DirectX::BoundingOrientedBox localBox{ {0.0f, 0.0f, 0.0f}, {0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 0.0f, 1.0f} };
	DirectX::BoundingOrientedBox worldBox{ {0.0f, 0.0f, 0.0f}, {0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 0.0f, 1.0f} };
};

// ============================================================================
// Inline Implementations
// ============================================================================

#include "Vector3D.h"

inline void CubeCollider::SetSize(const Vector3D& size_) noexcept
{
	localBox.Extents = { size_.x * 0.5f, size_.y * 0.5f, size_.z * 0.5f };
}

inline const DirectX::BoundingOrientedBox& CubeCollider::GetVolume() const noexcept
{
	return worldBox;
}
