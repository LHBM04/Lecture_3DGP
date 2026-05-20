#pragma once

#include "Component.hpp"

class Collider : public Component
{
public:
	virtual ~Collider() override = default;

	[[nodiscard]] bool Intersects(const Collider& other_) const;
	[[nodiscard]] virtual DirectX::BoundingBox GetWorldAABB() const = 0;

protected:
	void OnAttach() final;
	void OnUpdate() final;

	virtual void UpdateBounds() = 0;
};
