#include "Precompiled.h"
#include "Component.h"

#include "GameObject.h"

Component::~Component() noexcept
{
	Destroy();
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
	return isEnabled && owner != nullptr && owner->IsActive();
}

void Component::SetEnabled(bool isEnabled_)
{
	if (isEnabled == isEnabled_)
	{
		return;
	}

	isEnabled = isEnabled_;
	if (owner != nullptr && owner->IsActive())
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

void Component::LateUpdate()
{
	if (!isEnabled)
	{
		return;
	}

	OnLateUpdate();
}

void Component::FixedUpdate()
{
	if (!isEnabled)
	{
		return;
	}

	OnFixedUpdate();
}

void Component::Render(ID3D12GraphicsCommandList* commandList_)
{
	if (!isEnabled)
	{
		return;
	}

	OnPreRender(commandList_);
	OnRender(commandList_);
	OnPostRender(commandList_);
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

const GameObject* Component::GetOwner() const noexcept
{
	return owner;
}

GameObject* Component::GetOwner() noexcept
{
	return owner;
}
