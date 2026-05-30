#include "Precompiled.h"
#include "Component.h"
#include "GameObject.h"

void Component::Destroy()
{
	if (!owner)
	{
		return;
	}

	OnDestroy();
	owner = nullptr;
}

bool Component::IsActive() const noexcept
{
	return isEnabled && owner && owner->IsActive();
}

void Component::SetEnabled(bool isEnabled_)
{
	if (isEnabled == isEnabled_)
	{
		return;
	}
	isEnabled = isEnabled_;

	if (owner && owner->IsActive())
	{
		if (isEnabled)
		{
			Enable();
		}
		else
		{
			Disable();
		}
	}
}

