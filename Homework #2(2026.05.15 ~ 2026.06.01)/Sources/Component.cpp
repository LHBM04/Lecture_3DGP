#include "Precompiled.h"
#include "Component.h"

void Component::NotifyAttach(GameObject* owner_)
{
	owner = owner_;
	OnReset();
	TryAwake();
	TryEnable();
	OnAttach();
}

void Component::NotifyDetach()
{
	TryDisable();
	OnDetach();
	TryDestroy();
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
		TryAwake();
		TryEnable();
	}
	else
	{
		TryDisable();
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

	TryDisable();
	TryDestroy();
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

void Component::TickUpdate(const TimeContext& context_)
{
	if (isDestroyed || !isEnabled)
	{
		return;
	}

	TryAwake();
	TryEnable();
	if (!isStarted)
	{
		OnStart();
		isStarted = true;
	}

	OnUpdate(context_);
}

void Component::TickFixedUpdate(const TimeContext& context_)
{
	if (isDestroyed || !isEnabled)
	{
		return;
	}

	TryAwake();
	TryEnable();
	if (!isStarted)
	{
		OnStart();
		isStarted = true;
	}

	OnFixedUpdate(context_);
}

void Component::TickRender()
{
	if (isDestroyed || !isEnabled)
	{
		return;
	}

	OnRender();
}

void Component::TryAwake()
{
	if (isAwakened || nullptr == owner)
	{
		return;
	}

	OnAwake();
	isAwakened = true;
}

void Component::TryEnable()
{
	if (isActiveAndEnabled || !isEnabled || nullptr == owner || isDestroyed)
	{
		return;
	}

	OnEnable();
	isActiveAndEnabled = true;
}

void Component::TryDisable()
{
	if (!isActiveAndEnabled)
	{
		return;
	}

	OnDisable();
	isActiveAndEnabled = false;
}

void Component::TryDestroy()
{
	if (isDestroyNotified)
	{
		return;
	}

	OnDestroy();
	isDestroyNotified = true;
}

