#include "Precompiled.h"
#include "Component.h"

#include "GameObject.h"

GameObject* Component::GetOwner() const noexcept
{
	return owner;
}

void Component::SetOwner(GameObject* owner_) noexcept
{
	owner = owner_;
}

bool Component::IsStarted() const noexcept
{
	return isStarted;
}

bool Component::IsEnabled() const noexcept
{
	return isEnabled;
}

bool Component::IsActive() const noexcept
{
	return isEnabled && (owner != nullptr) && owner->IsActive();
}

void Component::SetEnabled(bool isEnabled_)
{
	if (isEnabled == isEnabled_)
	{
		return;
	}
	isEnabled = isEnabled_;

	if ((owner != nullptr) && owner->IsActive())
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

void Component::Awake()
{
	if (owner == nullptr || isDestroyed)
	{
		return;
	}

	OnAwake();
}

void Component::Enable()
{
	if (owner == nullptr || isDestroyed)
	{
		return;
	}

	OnEnable();
}

void Component::Start()
{
	if (owner == nullptr || isDestroyed)
	{
		return;
	}

	OnStart();
}

void Component::Update()
{
	if (!isEnabled || owner == nullptr || isDestroyed)
	{
		return;
	}

	if (!isStarted)
	{
		Start();
		isStarted = true;
	}

	OnUpdate();
}

void Component::FixedUpdate()
{
	if (!isEnabled || owner == nullptr || isDestroyed)
	{
		return;
	}

	OnFixedUpdate();
}

void Component::LateUpdate()
{
	if (!isEnabled || owner == nullptr || isDestroyed)
	{
		return;
	}

	OnLateUpdate();
}

void Component::Render()
{
	if (!isEnabled || owner == nullptr || isDestroyed)
	{
		return;
	}

	OnPreRender();
	OnRender();
	OnPostRender();
}

void Component::Disable()
{
	if (owner == nullptr || isDestroyed)
	{
		return;
	}

	OnDisable();
}

void Component::Destroy()
{
	if (owner == nullptr || isDestroyed)
	{
		return;
	}

	if (isEnabled)
	{
		OnDisable();
	}

	isDestroyed = true;
	OnDestroy();
	owner = nullptr;
	isEnabled = false;
}

void Component::CollisionEnter(Collider* other_)
{
	if (!isEnabled || owner == nullptr || isDestroyed)
	{
		return;
	}

	OnCollisionEnter(other_);
}

void Component::CollisionStay(Collider* other_)
{
	if (!isEnabled || owner == nullptr || isDestroyed)
	{
		return;
	}

	OnCollisionStay(other_);
}

void Component::CollisionExit(Collider* other_)
{
	if (!isEnabled || owner == nullptr || isDestroyed)
	{
		return;
	}

	OnCollisionExit(other_);
}
