#include "Precompiled.h"
#include "GameObject.h"

std::wstring_view GameObject::GetName() const noexcept
{
	return name;
}

void GameObject::SetName(std::wstring_view name_) noexcept
{
	name = name_;
}

std::wstring_view GameObject::GetTag() const noexcept
{
	return tag;
}

void GameObject::SetTag(std::wstring_view tag_) noexcept
{
	tag = tag_;
}

bool GameObject::IsActive() const noexcept
{
	return isActive;
}

void GameObject::SetActive(bool isActive_) noexcept
{
	if (isActive == isActive_)
	{
		return;
	}
	isActive = isActive_;

	std::apply([isActive_](auto&... comp) {
		(..., (comp ? (isActive_ ? comp->Enable() : comp->Disable()) : void()));
	}, components);
}

bool GameObject::IsDestroyed() const noexcept
{
	return isDestroyed;
}

void GameObject::LateUpdate(float deltaTime_) noexcept
{
	if (!isActive)
	{
		return;
	}

	std::apply([deltaTime_](auto&... comp) {
		(..., (comp ? comp->LateUpdate(deltaTime_) : void()));
	}, components);
}

void GameObject::Destroy() noexcept
{
	if (isDestroyed)
	{
		return;
	}
	isDestroyed = true;

	std::apply([](auto&... comp) {
		(..., (comp ? comp->Destroy() : void()));
	}, components);
}

Scene* GameObject::GetScene() const noexcept
{
	return scene;
}

void GameObject::Update(float deltaTime_) noexcept
{
	if (!isActive)
	{
		return;
	}

	std::apply([deltaTime_](auto&... comp) {
		(..., (comp ? comp->Update(deltaTime_) : void()));
	}, components);
}

void GameObject::FixedUpdate(float fixedDeltaTime_) noexcept
{
	if (!isActive)
	{
		return;
	}

	std::apply([fixedDeltaTime_](auto&... comp) {
		(..., (comp ? comp->FixedUpdate(fixedDeltaTime_) : void()));
	}, components);
}

void GameObject::Render() noexcept
{
	if (!isActive)
	{
		return;
	}

	std::apply([](auto&... comp) {
		(..., (comp ? comp->Render() : void()));
	}, components);
}
