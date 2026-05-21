#include "Precompiled.h"
#include "Component.h"

#include "GameObject.h"

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
