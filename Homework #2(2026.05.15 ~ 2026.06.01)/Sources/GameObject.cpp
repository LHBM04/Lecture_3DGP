#include "Precompiled.h"
#include "GameObject.h"

void GameObject::Update()
{
	for (const std::unique_ptr<Component>& component : components | std::views::values)
	{
		component->OnUpdate();
	}
}

void GameObject::Render()
{
	for (const std::unique_ptr<Component>& component : components | std::views::values)
	{
		component->OnRender();
	}
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

