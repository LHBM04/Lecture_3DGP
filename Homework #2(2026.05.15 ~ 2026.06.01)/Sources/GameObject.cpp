#include "Precompiled.h"
#include "GameObject.h"

std::wstring_view GameObject::GetName() const noexcept
{
	return name;
}

void GameObject::SetName(std::wstring_view name_)
{
	name = name_;
}

std::wstring_view GameObject::GetTag() const noexcept
{
	return tag;
}

void GameObject::SetTag(std::wstring_view tag_)
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

	for (auto& component : components)
	{
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
	return isDestroyed;
}

void GameObject::LateUpdate(float deltaTime_)
{
	if (!isActive)
	{
		return;
	}

	for (auto& component : components)
	{
		component->LateUpdate(deltaTime_);
	}
}

void GameObject::Destroy()
{
	if (isDestroyed)
	{
		return;
	}
	isDestroyed = true;

	for (auto& component : components)
	{
		component->Destroy();
	}
}

Scene* GameObject::GetScene() const noexcept
{
	return scene;
}

void GameObject::Update(float deltaTime_)
{
	if (!isActive)
	{
		return;
	}

	for (auto& component : components)
	{
		component->Update(deltaTime_);
	}
}

void GameObject::FixedUpdate(float fixedDeltaTime_)
{
	if (!isActive)
	{
		return;
	}

	for (auto& component : components)
	{
		component->FixedUpdate(fixedDeltaTime_);
	}
}

void GameObject::Render()
{
	if (!isActive)
	{
		return;
	}

	for (auto& component : components)
	{
		component->Render();
	}
}

