#pragma once

#include <DirectXCollision.h>
#include "Component.h"
#include "Vector3D.h"

class SphereCollider;
class CubeCollider;

class Collider : public Component
{
public:
	Collider() = default;
	~Collider() override = default;

	[[nodiscard]] bool IsTrigger() const noexcept;
	void SetTrigger(bool isTrigger_) noexcept;

	[[nodiscard]] bool IsStatic() const noexcept;
	void SetStatic(bool isStatic_) noexcept;

	// Double Dispatch Interface
	virtual bool IsIntersects(const Collider* other_) const = 0;
	virtual bool IsIntersects(const SphereCollider* other_) const = 0;
	virtual bool IsIntersects(const CubeCollider* other_) const = 0;

	// Utilities
	virtual bool IsIntersects(const Vector3D& rayOrigin_, const Vector3D& rayDir_, float& distance_) const = 0;
	virtual bool IsIntersects(const DirectX::BoundingFrustum& frustum_) const = 0;
	virtual DirectX::BoundingBox GetBoundingVolume() const = 0;

	virtual void UpdateVolume() = 0;

protected:
	void OnEnable() override;
	void OnDisable() override;

private:
	bool isTrigger{ false };
	bool isStatic{ false };
};
