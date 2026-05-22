#include "Precompiled.h"
#include "GameObject.h"

#include "MeshRenderer.h"
#include "RectTransform.h"

GameObject::GameObject(bool useRectTransform_)
{
	transform = useRectTransform_ ? AddComponent<RectTransform>() : AddComponent<Transform>();
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

void GameObject::Render(RenderTarget& renderTarget_)
{
	Render(renderTarget_, true);
}

void GameObject::Render(RenderTarget& renderTarget_, bool renderMeshRenderers_)
{
	if (!isActive)
	{
		return;
	}

	for (const auto& [type, component] : components)
	{
		if (component->IsEnabled())
		{
			if (!renderMeshRenderers_ && type == typeid(MeshRenderer))
			{
				continue;
			}

			component->OnRender(renderTarget_);
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
