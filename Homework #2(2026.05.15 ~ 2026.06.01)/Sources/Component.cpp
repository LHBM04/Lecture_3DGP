#include "Precompiled.h"
#include "Component.h"

void Component::NotifyAttach(GameObject* owner_)
{
	owner = owner_;
	OnAttach();
}

void Component::NotifyDetach()
{
	OnDetach();
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
	isEnabled = enabled_;
}

bool Component::IsDestroyed() const
{
	return isDestroyed;
}

void Component::Destroy()
{
	isDestroyed = true;
}

GameObject* Component::GetOwner()
{
	return owner;
}

const GameObject* Component::GetOwner() const
{
	return owner;
}

