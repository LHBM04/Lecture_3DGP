#pragma once

#include "Collider.hpp"

class CollisionFrustum final : public Collider
{
public:
	virtual ~CollisionFrustum() override = default;
	[[nodiscard]] DirectX::BoundingBox GetWorldAABB() const override;

	[[nodiscard]] const DirectX::BoundingFrustum& GetBounds() const;

private:
	void UpdateBounds() override;

	DirectX::BoundingFrustum bounds{};
};
