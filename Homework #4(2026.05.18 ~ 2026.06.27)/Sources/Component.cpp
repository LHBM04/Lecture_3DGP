#include "Precompiled.h"
#include "Component.h"

#include "GameObject.h"

Component::~Component() noexcept = default;

GameObject& Component::GetOwner() noexcept
{
	assert(owner != nullptr);
	return *owner;
}

const GameObject& Component::GetOwner() const noexcept
{
	assert(owner != nullptr);
	return *owner;
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
	return owner != nullptr && owner->IsActive() && isEnabled && !isDestroyed;
}

void Component::SetEnabled(bool isEnabled_)
{
	if (isEnabled == isEnabled_)
	{
		return;
	}

	isEnabled = isEnabled_;
	if (isEnabled)
	{
		Enable();
	}
	else
	{
		Disable();
	}
}

void Component::Awake()
{
	OnAwake();
}

void Component::Enable()
{
	if (isDestroyed)
	{
		return;
	}

	isEnabled = true;
	OnEnable();
}

void Component::Start()
{
	if (isStarted || isDestroyed)
	{
		return;
	}

	isStarted = true;
	OnStart();
}

void Component::Update()
{
	if (!IsActive())
	{
		return;
	}

	if (!isStarted)
	{
		Start();
	}

	OnUpdate();
}

void Component::LateUpdate()
{
	if (IsActive())
	{
		OnLateUpdate();
	}
}

void Component::FixedUpdate()
{
	if (IsActive())
	{
		OnFixedUpdate();
	}
}

void Component::Render(const RenderContext& context_)
{
	if (!IsActive())
	{
		return;
	}

	OnPreRender(context_);
	OnRender(context_);
	OnPostRender(context_);
}

void Component::Disable()
{
	if (isDestroyed)
	{
		return;
	}

	isEnabled = false;
	OnDisable();
}

void Component::Destroy()
{
	if (isDestroyed)
	{
		return;
	}

	if (isEnabled)
	{
		Disable();
	}

	isDestroyed = true;
	OnDestroy();
}
