#include "Precompiled.h"
#include "Collider.h"
#include "PhysicsSystem.h"

bool Collider::IsTrigger() const noexcept
{
	return isTrigger;
}

void Collider::SetTrigger(bool isTrigger_) noexcept
{
	isTrigger = isTrigger_;
}

bool Collider::IsStatic() const noexcept
{
	return isStatic;
}

void Collider::SetStatic(bool isStatic_) noexcept
{
	if (isStatic == isStatic_)
	{
		return;
	}

	isStatic = isStatic_;
	PhysicsSystem::GetInstance().RegisterStaticObjectsToGrid();
}

void Collider::OnEnable()
{
	PhysicsSystem::GetInstance().AddCollider(this);
}

void Collider::OnDisable()
{
	PhysicsSystem::GetInstance().RemoveCollider(this);
}
