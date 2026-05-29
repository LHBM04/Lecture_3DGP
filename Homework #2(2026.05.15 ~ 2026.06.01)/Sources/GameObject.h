#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <typeindex>
#include <unordered_map>
#include <utility>

#include "Component.h"

class Scene;

class GameObject
{
public:
	GameObject() = default;
	~GameObject() = default;

	// 복사 금지.
	GameObject(const GameObject&) = delete;
	GameObject& operator=(const GameObject&) = delete;

	// 이동 금지.
	GameObject(GameObject&&) = delete;
	GameObject& operator=(GameObject&&) = delete;

	void Update(float deltaTime_);
	void FixedUpdate(float fixedDeltaTime_);

	void Render();

	[[nodiscard]] const std::wstring& GetName() const noexcept;
	void SetName(const std::wstring& name_) noexcept;

	[[nodiscard]] const std::wstring& GetTag() noexcept;
	void SetTag(const std::wstring& tag_) noexcept;

	[[nodiscard]] bool IsActive() const noexcept;
	void SetActive(bool isActive_) noexcept;

	[[nodiscard]] bool IsDestroyed() const noexcept;
	void Destroy() noexcept;

	template <class TSelf>
	[[nodiscard]] auto GetScene(this TSelf& self_) noexcept;

	template <std::derived_from<Component> TComponent>
	TComponent* AddComponent() noexcept;

	template <std::derived_from<Component> TComponent>
	const TComponent* AddComponent() const noexcept;

	template <std::derived_from<Component> TComponent>
	[[nodiscard]] TComponent* GetComponent() noexcept;

	template <std::derived_from<Component> TComponent>
	[[nodiscard]] const TComponent* GetComponent() const noexcept;

private:
	std::wstring name;
	std::wstring tag;

	bool isActive{ true };
	bool isDestroyed{ false };

	Scene* scene{ nullptr };

	std::unordered_map<std::type_index, std::unique_ptr<Component>> components;
};

inline const std::wstring& GameObject::GetName() const noexcept
{
	return name;
}

inline void GameObject::SetName(const std::wstring& name_) noexcept
{
	name = name_;
}

inline const std::wstring& GameObject::GetTag() noexcept
{
	return tag;
}

inline void GameObject::SetTag(const std::wstring& tag_) noexcept
{
	tag = tag_;
}

inline bool GameObject::IsActive() const noexcept
{
	return isActive;
}

inline void GameObject::SetActive(bool isActive_) noexcept
{
	isActive = isActive_;
}

inline bool GameObject::IsDestroyed() const noexcept
{
	return isDestroyed;
}

inline void GameObject::Destroy() noexcept
{
	isDestroyed = true;
}

template <class TSelf>
[[nodiscard]] auto GameObject::GetScene(this TSelf& self_) noexcept
{
	return self_.scene;
}

template <std::derived_from<Component> TComponent>
TComponent* GameObject::AddComponent() noexcept
{
	const auto type{ typeid(TComponent) };
	if (components.contains(type))
	{
		return static_cast<TComponent*>(components[type].get());
	}
	auto component{ std::make_unique<TComponent>() };
	const auto ptr{ component.get() };
	components.emplace(type, std::move(component));

	return ptr;
}

template <std::derived_from<Component> TComponent>
const TComponent* GameObject::AddComponent() const noexcept
{
	const auto type{ typeid(TComponent) };
	if (components.contains(type))
	{
		return static_cast<TComponent*>(components[type].get());
	}
	auto [iter, inserted] = components.emplace(type, std::make_unique<TComponent>());
	return iter.get();
}

template <std::derived_from<Component> TComponent>
[[nodiscard]] TComponent* GameObject::GetComponent() noexcept
{
	const auto type{ typeid(TComponent) };
	if (components.contains(type))
	{
		return static_cast<TComponent*>(components[type].get());
	}

	auto [iter, inserted] = components.emplace(type, std::make_unique<TComponent>());
	return iter.get();
}

template <std::derived_from<Component> TComponent>
[[nodiscard]] const TComponent* GameObject::GetComponent() const noexcept
{
	const auto it{ components.find(typeid(TComponent)) };
	if (it != components.end())
	{
		return static_cast<const TComponent*>(it->second.get());
	}

	return nullptr;
}
