#include "Precompiled.h"
#include "GameObject.h"

#include "Component.h"

const std::wstring& GameObject::GetName() const noexcept
{
	return name;
}

void GameObject::SetName(const std::wstring& name_)
{
	name = name_;
}

const std::wstring& GameObject::GetTag() const noexcept
{
	return tag;
}

void GameObject::SetTag(const std::wstring& tag_)
{
	tag = tag_;
}

bool GameObject::IsActive() const noexcept
{
	return isActive;
}

void GameObject::SetActive(bool isActive_)
{
	if (isActive == isActive_)
	{
		return;
	}
	isActive = isActive_;

	for (const std::unique_ptr<Component>& component : components)
	{
		if (IsComponentPendingRemoval(component.get()))
		{
			continue;
		}

		if (isActive)
		{
			component->Enable();
		}
		else
		{
			component->Disable();
		}
	}
}

bool GameObject::IsDestroyed() const noexcept
{
	return isDestroyed || isDestroyPending;
}

bool GameObject::IsDestroyPending() const noexcept
{
	return isDestroyPending;
}

void GameObject::Destroy()
{
	if (isDestroyed)
	{
		return;
	}

	isDestroyPending = false;
	isDestroyed = true;

	FlushPendingComponents();

	for (const std::unique_ptr<Component>& component : components)
	{
		component->Destroy();
	}

	for (const std::unique_ptr<Component>& component : addComponents)
	{
		component->Destroy();
	}

	components.clear();
	addComponents.clear();
	removeComponents.clear();
}

Scene* GameObject::GetScene() const noexcept
{
	return scene;
}

void GameObject::Update()
{
	if (!isActive)
	{
		return;
	}

	FlushPendingComponents();

	for (const std::unique_ptr<Component>& component : components)
	{
		if (IsComponentPendingRemoval(component.get()))
		{
			continue;
		}

		component->Update();
	}

	FlushPendingComponents();
}

void GameObject::LateUpdate()
{
	if (!isActive)
	{
		return;
	}

	FlushPendingComponents();

	for (const std::unique_ptr<Component>& component : components)
	{
		if (IsComponentPendingRemoval(component.get()))
		{
			continue;
		}

		component->LateUpdate();
	}

	FlushPendingComponents();
}

void GameObject::FixedUpdate()
{
	if (!isActive)
	{
		return;
	}

	FlushPendingComponents();

	for (const std::unique_ptr<Component>& component : components)
	{
		if (IsComponentPendingRemoval(component.get()))
		{
			continue;
		}

		component->FixedUpdate();
	}

	FlushPendingComponents();
}

void GameObject::Render()
{
	if (!isActive)
	{
		return;
	}

	FlushPendingComponents();

	for (const std::unique_ptr<Component>& component : components)
	{
		if (IsComponentPendingRemoval(component.get()))
		{
			continue;
		}

		component->Render();
	}

	FlushPendingComponents();
}

void GameObject::NotifyCollisionEnter(Collider* other_)
{
	if (IsDestroyed())
	{
		return;
	}

	FlushPendingComponents();

	for (const std::unique_ptr<Component>& component : components)
	{
		if (IsComponentPendingRemoval(component.get()))
		{
			continue;
		}

		component->CollisionEnter(other_);
	}

	FlushPendingComponents();
}

void GameObject::NotifyCollisionStay(Collider* other_)
{
	if (IsDestroyed())
	{
		return;
	}

	FlushPendingComponents();

	for (const std::unique_ptr<Component>& component : components)
	{
		if (IsComponentPendingRemoval(component.get()))
		{
			continue;
		}

		component->CollisionStay(other_);
	}

	FlushPendingComponents();
}

void GameObject::NotifyCollisionExit(Collider* other_)
{
	if (IsDestroyed())
	{
		return;
	}

	FlushPendingComponents();

	for (const std::unique_ptr<Component>& component : components)
	{
		if (IsComponentPendingRemoval(component.get()))
		{
			continue;
		}

		component->CollisionExit(other_);
	}

	FlushPendingComponents();
}

void GameObject::FlushPendingComponents()
{
	if (!removeComponents.empty())
	{
		for (Component* const componentToRemove : removeComponents)
		{
			if (componentToRemove == nullptr)
			{
				continue;
			}

			std::erase_if(components, [componentToRemove](const std::unique_ptr<Component>& component)
			{
				if (component.get() != componentToRemove)
				{
					return false;
				}

				component->Destroy();
				return true;
			});
		}

		removeComponents.clear();
	}

	if (addComponents.empty())
	{
		return;
	}

	std::vector<Component*> addedComponents;
	addedComponents.reserve(addComponents.size());
	for (std::unique_ptr<Component>& component : addComponents)
	{
		if (component == nullptr)
		{
			continue;
		}

		addedComponents.emplace_back(component.get());
		components.emplace_back(std::move(component));
	}
	addComponents.clear();

	for (Component* const component : addedComponents)
	{
		if (component == nullptr || IsComponentPendingRemoval(component))
		{
			continue;
		}

		component->Awake();
		if (isActive && !isDestroyed)
		{
			component->Enable();
		}
	}
}

bool GameObject::IsComponentPendingRemoval(const Component* component_) const noexcept
{
	return component_ != nullptr && std::ranges::find(removeComponents, component_) != removeComponents.end();
}
