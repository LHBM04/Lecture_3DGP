#include "Precompiled.h"
#include "GameObject.h"

GameObject::GameObject()
{
	transform = AddComponent<Transform>();
}

GameObject::~GameObject() noexcept
{
	for (const auto& [type, component] : components)
	{
		component->NotifyDetach();
	}
}

void GameObject::Update()
{
	if (!isActive)
	{
		return;
	}

	for (const auto& [type, component] : components)
	{
		if (component->IsEnabled())
		{
			component->OnUpdate();
		}
	}

	RemoveDestroyedComponents();
}

void GameObject::Render(Renderer& renderer_)
{
	if (!isActive)
	{
		return;
	}

	for (const auto& [type, component] : components)
	{
		if (component->IsEnabled())
		{
			component->OnRender(renderer_);
		}
	}
}

void GameObject::SetCurrentScene(Scene* scene_)
{
	currentScene = scene_;
}

void GameObject::RemoveDestroyedComponents()
{
	for (auto iterator = components.begin(); iterator != components.end();)
	{
		Component* component{ iterator->second.get() };
		if (component == transform || !component->IsDestroyed())
		{
			++iterator;
			continue;
		}

		component->NotifyDetach();
		iterator = components.erase(iterator);
	}
}
