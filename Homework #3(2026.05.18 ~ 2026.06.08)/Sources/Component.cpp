#include "Precompiled.h"
#include "Component.h"

const GameObject* Component::GetOwner() const noexcept
{
	return owner;
}

GameObject* Component::GetOwner() noexcept
{
	return owner;
}
