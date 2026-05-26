#pragma once

#include "Component.h"

class Collider : public Component
{
public:
	~Collider() override = default;

	[[nodiscard]] virtual bool Intersects(const DirectX::BoundingFrustum& frustum_) const = 0;
};
