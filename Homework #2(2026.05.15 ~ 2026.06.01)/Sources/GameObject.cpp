#include "Precompiled.h"
#include "GameObject.h"

void GameObject::Update()
{
	for (Updatable* updatable : updatables)
	{
		if (nullptr == updatable)
		{
			continue;
		}

		Component* component{ dynamic_cast<Component*>(updatable) };
		if (nullptr == component || component->IsDestroyed() || !component->IsEnabled())
		{
			continue;
		}

		updatable->OnUpdate();
	}
}

void GameObject::FixedUpdate()
{
	for (const std::unique_ptr<Component>& component : components | std::views::values)
	{
		component->TickFixedUpdate();
	}
}

void GameObject::Render()
{
	for (Renderable* renderable : renderables)
	{
		if (nullptr == renderable)
		{
			continue;
		}

		Component* component{ dynamic_cast<Component*>(renderable) };
		if (nullptr == component || component->IsDestroyed() || !component->IsEnabled())
		{
			continue;
		}

		renderable->OnRender();
	}
}

void GameObject::RenderUI()
{
	for (RenderableUI* renderable : uiRenderables)
	{
		if (nullptr == renderable)
		{
			continue;
		}

		Component* component{ dynamic_cast<Component*>(renderable) };
		if (nullptr == component || component->IsDestroyed() || !component->IsEnabled())
		{
			continue;
		}

		renderable->OnRenderUI();
	}
}

void GameObject::NotifyCollisionEnter(GameObject& other_)
{
	for (Collidable* collidable : collidables)
	{
		if (nullptr == collidable)
		{
			continue;
		}

		Component* component{ dynamic_cast<Component*>(collidable) };
		if (nullptr != component && !component->IsDestroyed() && component->IsEnabled())
		{
			collidable->OnCollisionEnter(other_);
		}
	}
}

void GameObject::NotifyCollisionStay(GameObject& other_)
{
	for (Collidable* collidable : collidables)
	{
		if (nullptr == collidable)
		{
			continue;
		}

		Component* component{ dynamic_cast<Component*>(collidable) };
		if (nullptr != component && !component->IsDestroyed() && component->IsEnabled())
		{
			collidable->OnCollisionStay(other_);
		}
	}
}

void GameObject::NotifyCollisionExit(GameObject& other_)
{
	for (Collidable* collidable : collidables)
	{
		if (nullptr != collidable)
		{
			collidable->OnCollisionExit(other_);
		}
	}
}

bool GameObject::HasCollisionListeners() const noexcept
{
	return !collidables.empty();
}

Scene* GameObject::GetCurrentScene()
{
	return currentScene;
}

const Scene* GameObject::GetCurrentScene() const
{
	return currentScene;
}


const std::string& GameObject::GetName() const
{
	return name;
}

void GameObject::SetName(const std::string& name_)
{
	name = name_;
}

const std::string& GameObject::GetTag() const
{
	return tag;
}

void GameObject::SetTag(const std::string& tag_)
{
	tag = tag_;
}

bool GameObject::IsActive() const
{
	return isActive;
}

void GameObject::SetActive(bool active_)
{
	isActive = active_;
}

bool GameObject::IsDestroyed() const
{
	return isDestroyed;
}

void GameObject::Destroy()
{
	isDestroyed = true;
	isActive = false;
}

