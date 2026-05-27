#pragma once

#include "Collider.hpp"
#include "Vector3D.hpp"

class CollisionBox final : public Collider
{
public:
	virtual ~CollisionBox() override = default;
	[[nodiscard]] DirectX::BoundingBox GetWorldAABB() const override;

	[[nodiscard]] const Vector3D& GetLocalCenter() const;
	void SetLocalCenter(const Vector3D& localCenter_);

	[[nodiscard]] const Vector3D& GetLocalExtents() const;
	void SetLocalExtents(const Vector3D& localExtents_);

	[[nodiscard]] const DirectX::BoundingBox& GetBounds() const;

private:
	void UpdateBounds() override;

	Vector3D localCenter{ Vector3D::GetZero() };
	Vector3D localExtents{ 0.5f, 0.5f, 0.5f };

	DirectX::BoundingBox bounds{};
};
