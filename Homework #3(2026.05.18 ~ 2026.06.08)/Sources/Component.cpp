#include "Precompiled.h"
#include "Component.h"

#include "GameObject.h"

GameObject* Component::GetOwner() const noexcept
{
	assert(owner != nullptr && "Component must have an owner!");
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
	OnAwake();
}

void Component::Enable()
{
	OnEnable();
}

void Component::Start()
{
	OnStart();
}

void Component::Update()
{
	if (!isEnabled)
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
	if (!isEnabled)
	{
		return;
	}

	OnFixedUpdate();
}

void Component::LateUpdate()
{
	if (!isEnabled)
	{
		return;
	}

	OnLateUpdate();
}

void Component::Render()
{
	if (!isEnabled)
	{
		return;
	}

	OnPreRender();
	OnRender();
	OnPostRender();
}

void Component::Disable()
{
	OnDisable();
}

void Component::Destroy()
{
	if (owner == nullptr || isDestroyed)
	{
		return;
	}

	isDestroyed = true;

	if (isEnabled)
	{
		Disable();
	}

	OnDestroy();
	owner = nullptr;
	isEnabled = false;
}

void Component::CollisionEnter(Collider* other_)
{
	OnCollisionEnter(other_);
}

void Component::CollisionStay(Collider* other_)
{
	OnCollisionStay(other_);
}

void Component::CollisionExit(Collider* other_)
{
	OnCollisionExit(other_);
}
