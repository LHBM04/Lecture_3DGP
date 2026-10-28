#include "Precompiled.hpp"
#include "Component.hpp"

#include "Object.hpp"

bool Component::IsAttached() const
{
	return owner != nullptr;
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

Object* Component::GetOwner() const
{
    return owner;
}

Transform* Component::GetTransform() const
{
	return owner ? owner->GetTransform() : nullptr;
}

void Component::Attach(Object* owner_)
{
	if (owner == owner_)
	{
		return;
	}

	if (owner)
	{
		Detach();
	}

	owner = owner_;
	if (owner)
	{
		OnAttach();
	}
}

void Component::Update()
{
	OnUpdate();
	OnLateUpdate();
}

void Component::Render()
{
	OnPreRender();
	OnRender();
}

void Component::Detach()
{
	if (!owner)
	{
		return;
	}

	OnDetach();
	owner = nullptr;
}
