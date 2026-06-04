#pragma once

#include <concepts>
#include <memory>
#include <string>
#include <string_view>
#include <typeindex>

#include "Component.h"

class Scene;

class GameObject
{
	friend class Scene;

public:
	GameObject() = default;
	virtual ~GameObject() = default;

	[[nodiscard]] const std::wstring& GetName() const noexcept;
	void SetName(std::wstring_view name_);

	[[nodiscard]] const std::wstring& GetTag() const noexcept;
	void SetTag(std::wstring_view tag_);

	[[nodiscard]] bool IsActive() const noexcept;
	void SetActive(bool isActive_);

	[[nodiscard]] bool IsDestroyed() const noexcept;

	[[nodiscard]] Scene* GetCurrentScene() noexcept;
	[[nodiscard]] const Scene* GetCurrentScene() const noexcept;

	template <std::derived_from<Component> TComponent>
	TComponent* AddComponent();

	template <std::derived_from<Component> TComponent>
	TComponent* GetComponent();

	template <std::derived_from<Component> TComponent>
	const TComponent* GetComponent() const;

	template <std::derived_from<Component> TComponent>
	void RemoveComponent();

private:
	GameObject(const GameObject&) = delete;
	GameObject& operator=(const GameObject&) = delete;

	GameObject(GameObject&&) = delete;
	GameObject& operator=(GameObject&&) = delete;

	std::wstring name{ L"New Object" };
	std::wstring tag{ L"Untagged" };

	bool isActive{ true };
	bool isDestroyed{ false };

	Scene* currentScene{ nullptr };

	std::unordered_map<std::type_index, std::unique_ptr<Component>> components;
};

template <std::derived_from<Component> TComponent>
inline TComponent* GameObject::AddComponent()
{
	if (const auto iter{ components.find(typeid(TComponent)) }; iter != components.end())
	{
		return nullptr;
	}

	std::unique_ptr<TComponent> component{ std::make_unique<TComponent>() };
	component->owner = this;
	TComponent* componentPtr{ component.get() };
	components[typeIndex] = std::move(component);
	return componentPtr;
}

template <std::derived_from<Component> TComponent>
inline const TComponent* GameObject::GetComponent() const
{
	if (const auto iter{ components.find(typeid(TComponent)) }; iter != components.end())
	{
		return static_cast<TComponent*>(iter->second.get());
	}

	return nullptr;
}

template <std::derived_from<Component> TComponent>
inline const TComponent* GameObject::GetComponent() const
{
	if (const auto iter{ components.find(typeid(TComponent)) }; iter != components.end())
	{
		return static_cast<TComponent*>(iter->second.get());
	}

	return nullptr;
}

template <std::derived_from<Component> TComponent>
inline TComponent* GameObject::GetComponent()
{
	const std::type_index typeIndex{ typeid(TComponent) };
	const auto iter{ components.find(typeIndex) };
	if (iter != components.end())
	{
		return static_cast<TComponent*>(iter->second.get());
	}
	return nullptr;
}

template <std::derived_from<Component> TComponent>
inline void GameObject::RemoveComponent()
{
	const std::type_index typeIndex{ typeid(TComponent) };
	components.erase(typeIndex);
}
