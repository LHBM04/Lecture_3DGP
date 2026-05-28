#include "Precompiled.h"
#include "Component.h"

void Component::NotifyAdd(GameObject* owner_)
{
	owner = owner_;
	
	OnAwake();
	OnEnable();
}

void Component::NotifyRemove()
{
	OnEnable();
	OnDisable();

	owner = nullptr;
}

bool Component::IsAttached() const
{
	return nullptr != owner;
}

bool Component::IsEnabled() const
{
	return isEnabled;
}

void Component::SetEnabled(bool enabled_)
{
	if (isEnabled == enabled_)
	{
		return;
	}

	isEnabled = enabled_;
	if (isEnabled)
	{
		OnAwake();
		OnEnable();
	}
}

bool Component::IsDestroyed() const
{
	return isDestroyed;
}

void Component::Destroy()
{
	if (isDestroyed)
	{
		return;
	}

	isDestroyed = true;
	
	OnEnable();
	OnDisable();
}

GameObject* Component::GetOwner()
{
	return owner;
}

const GameObject* Component::GetOwner() const
{
	return owner;
}
