#include "Precompiled.h"
#include "GameObject.h"

void GameObject::Update(const TimeContext& context_)
{
	if (!isActive || isDestroyed)
	{
		return;
	}

	for (const auto& [type, component] : components)
	{
		if (component->IsEnabled())
		{
			if (const auto updatable = dynamic_cast<IUpdatable*>(component.get()))
			{
				updatable->OnUpdate(context_);
			}
		}
	}
}

void GameObject::Render(const RenderContext& context_)
{
	if (!isActive || isDestroyed)
	{
		return;
	}

	for (const auto& [type, component] : components)
	{
		if (component->IsEnabled())
		{
			if (const auto renderable = dynamic_cast<IRenderable*>(component.get()))
			{
				renderable->OnRender(context_);
			}
		}
	}
}