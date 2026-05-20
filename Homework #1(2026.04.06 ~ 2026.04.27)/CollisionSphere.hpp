#pragma once

#include "Collider.hpp"
#include "Vector3D.hpp"

class CollisionSphere final : public Collider
{
public:
	virtual ~CollisionSphere() override = default;
	[[nodiscard]] DirectX::BoundingBox GetWorldAABB() const override;

	[[nodiscard]] const Vector3D& GetLocalCenter() const;
	void SetLocalCenter(const Vector3D& localCenter_);

	[[nodiscard]] float GetLocalRadius() const;
	void SetLocalRadius(float localRadius_);

	[[nodiscard]] const DirectX::BoundingSphere& GetBounds() const;

private:
	void UpdateBounds() override;

	Vector3D localCenter{ Vector3D::GetZero() };
	float localRadius{ 0.5f };

	DirectX::BoundingSphere bounds{};
};
