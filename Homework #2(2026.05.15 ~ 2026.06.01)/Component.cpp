#include "Precompiled.h"
#include "Component.h"

#include "GameObject.h"

Transform* Component::GetTransform()
{
	return (nullptr != owner) ? owner->GetTransform() : nullptr;
}

const Transform* Component::GetTransform() const
{
	return (nullptr != owner) ? owner->GetTransform() : nullptr;
}

void Component::NotifyAttach(GameObject* const owner_)
{
	if (owner == owner_)
	{
		return;
	}

	owner = owner_;
	OnAttach();
}

void Component::NotifyDetach()
{
	if (!owner)
	{
		return;
	}

	OnDetach();
	owner = nullptr;
}
