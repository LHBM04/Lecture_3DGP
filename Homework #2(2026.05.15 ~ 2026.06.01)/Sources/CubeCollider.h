#pragma once

#include <DirectXCollision.h>
#include "Collider.h"

class Vector3D;

class CubeCollider final : public Collider
{
public:
	CubeCollider() = default;
	~CubeCollider() override = default;

	[[nodiscard]] const DirectX::BoundingOrientedBox& GetVolume() const noexcept;
	void SetCenter(const Vector3D& center_) noexcept;
	void SetSize(const Vector3D& size_) noexcept;

	[[nodiscard]] bool IsIntersects(const Collider* other_) const override;
	[[nodiscard]] bool IsIntersects(const SphereCollider* other_) const override;
	[[nodiscard]] bool IsIntersects(const CubeCollider* other_) const override;

	[[nodiscard]] bool IsIntersects(const Vector3D& rayOrigin_, const Vector3D& rayDir_, float& distance_) const override;
	[[nodiscard]] bool IsIntersects(const DirectX::BoundingFrustum& frustum_) const override;

	[[nodiscard]] DirectX::BoundingBox GetBoundingVolume() const override;

	void UpdateVolume() override;

	private:
	void OnUpdate(float deltaTime_) override;

	DirectX::BoundingOrientedBox localBox{ {0.0f, 0.0f, 0.0f}, {0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 0.0f, 1.0f} };
	DirectX::BoundingOrientedBox worldBox{ {0.0f, 0.0f, 0.0f}, {0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 0.0f, 1.0f} };
};
