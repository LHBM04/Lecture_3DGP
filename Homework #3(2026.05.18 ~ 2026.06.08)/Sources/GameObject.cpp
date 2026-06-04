#include "Precompiled.h"
#include "GameObject.h"

void GameObject::Update()
{
	if (!isActive)
	{
		return;
	}

	for (const auto& component : components | std::views::values)
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

	for (const auto& component : components | std::views::values)
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

	for (const auto& component : components | std::views::values)
	{
		component->FixedUpdate();
	}
}

void GameObject::Render(ID3D12GraphicsCommandList* commandList_)
{
	if (!isActive)
	{
		return;
	}

	for (const auto& component : components | std::views::values)
	{
		component->Render(commandList_);
	}
}

void GameObject::Destroy()
{
	if (isDestroyed)
	{
		return;
	}

	isDestroyed = true;
	for (const auto& component : components | std::views::values)
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

Scene* GameObject::GetCurrentScene() noexcept
{
	return currentScene;
}

const Scene* GameObject::GetCurrentScene() const noexcept
{
	return currentScene;
}

Scene* GameObject::GetScene() noexcept
{
	return currentScene;
}

const Scene* GameObject::GetScene() const noexcept
{
	return currentScene;
}

Transform* GameObject::GetTransform() noexcept
{
	return transform;
}

const Transform* GameObject::GetTransform() const noexcept
{
	return transform;
}

GameObjectConstants GameObject::GetGameObjectConstants() const noexcept
{
	GameObjectConstants constants{};
	if (transform != nullptr)
	{
		constants.worldMatrix = transform->GetWorldMatrix();
	}
	return constants;
}
