#include "Precompiled.h"
#include "GameObject.h"

const std::wstring& GameObject::GetName() const noexcept
{
	return name;
}

void GameObject::SetName(std::wstring_view name_)
{
	name = name_;
}

const std::wstring& GameObject::GetTag() const noexcept
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
	
	for (const std::unique_ptr<Component>& component : components | std::views::values)
	{
		if (isActive)
		{
			component->OnEnable();
		}
		else
		{
			component->OnDisable();
		}
	}
}

bool GameObject::IsDestroyed() const noexcept
{
	return isDestroyed;
}
