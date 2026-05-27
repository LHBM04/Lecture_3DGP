#include "Precompiled.hpp"
#include "Collider.hpp"

bool Collider::Intersects(const Collider& other_) const
{
	return GetWorldAABB().Intersects(other_.GetWorldAABB());
}

void Collider::OnAttach()
{
	UpdateBounds();
}

void Collider::OnUpdate()
{
	UpdateBounds();
}
