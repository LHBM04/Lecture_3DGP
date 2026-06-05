#include "Precompiled.h"
#include "GameObject.h"

void GameObject::Update()
{
	if (!isActive || isDestroyed)
	{
		return;
	}

	for (const auto& [type, component] : components)
	{
		component->Update();
	}
}

void GameObject::LateUpdate()
{
	if (!isActive || isDestroyed)
	{
		return;
	}

	for (const auto& [type, component] : components)
	{
		component->LateUpdate();
	}
}

void GameObject::FixedUpdate()
{
	if (!isActive || isDestroyed)
	{
		return;
	}

	for (const auto& [type, component] : components)
	{
		component->FixedUpdate();
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
		component->Render(context_);
	}
}

void GameObject::Destroy()
{
	if (isDestroyed)
	{
		return;
	}

	isDestroyed = true;
	for (const auto& [type, component] : components)
	{
		component->Destroy();
	}
}

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
	for (const auto& [type, component] : components)
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

Scene& GameObject::GetCurrentScene() noexcept
{
	assert(currentScene != nullptr);
	return *currentScene;
}

const Scene& GameObject::GetCurrentScene() const noexcept
{
	assert(currentScene != nullptr);
	return *currentScene;
}

Transform& GameObject::GetTransform() noexcept
{
	assert(transform != nullptr);
	return *transform;
}

const Transform& GameObject::GetTransform() const noexcept
{
	assert(transform != nullptr);
	return *transform;
}
