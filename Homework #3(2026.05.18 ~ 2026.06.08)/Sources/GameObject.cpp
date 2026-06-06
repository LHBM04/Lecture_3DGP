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

void GameObject::Destroy()
{
	if (isDestroyed)
	{
		return;
	}
	isDestroyed = true;

	for (const std::unique_ptr<Component>& component : components)
	{
		component->Destroy();
	}
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

	for (const std::unique_ptr<Component>& component : components)
	{
		component->Update();
	}
}

void GameObject::LateUpdate()
{
	if (!isActive)
	{
		return;
	}

	for (const std::unique_ptr<Component>& component : components)
	{
		component->LateUpdate();
	}
}

void GameObject::FixedUpdate()
{
	if (!isActive)
	{
		return;
	}

	for (const std::unique_ptr<Component>& component : components)
	{
		component->FixedUpdate();
	}
}

void GameObject::Render()
{
	if (!isActive)
	{
		return;
	}

	for (const std::unique_ptr<Component>& component : components)
	{
		component->Render();
	}
}

void GameObject::NotifyCollisionEnter(Collider* other_)
{
	if (isDestroyed)
	{
		return;
	}

	for (const std::unique_ptr<Component>& component : components)
	{
		component->CollisionEnter(other_);
	}
}

void GameObject::NotifyCollisionStay(Collider* other_)
{
	if (isDestroyed)
	{
		return;
	}

	for (const std::unique_ptr<Component>& component : components)
	{
		component->CollisionStay(other_);
	}
}

void GameObject::NotifyCollisionExit(Collider* other_)
{
	if (isDestroyed)
	{
		return;
	}

	for (const std::unique_ptr<Component>& component : components)
	{
		component->CollisionExit(other_);
	}
}
