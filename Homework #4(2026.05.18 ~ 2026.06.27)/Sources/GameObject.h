#pragma once

#include <concepts>
#include <memory>
#include <string>
#include <string_view>
#include <typeindex>

#include "Component.h"
#include "Matrix4x4.h"
#include "Transform.h"

class Scene;
class Transform;
class RenderContext;

class alignas(256) GameObjectConstants final
{
public:
	Matrix4x4 worldMatrix{ Matrix4x4::GetIdentity() };
};

class GameObject
{
	friend class Scene;

public:
	GameObject() = default;
	virtual ~GameObject() = default;

	void Update();
	void LateUpdate();
	void FixedUpdate();

	void Render(const RenderContext& context_);
	void Destroy();

	[[nodiscard]] const std::wstring& GetName() const noexcept;
	void SetName(std::wstring_view name_);

	[[nodiscard]] const std::wstring& GetTag() const noexcept;
	void SetTag(std::wstring_view tag_);

	[[nodiscard]] bool IsActive() const noexcept;
	void SetActive(bool isActive_);

	[[nodiscard]] bool IsDestroyed() const noexcept;

	[[nodiscard]] Scene& GetCurrentScene() noexcept;
	[[nodiscard]] const Scene& GetCurrentScene() const noexcept;

	template <std::derived_from<Component> TComponent, class... TArgs>
	TComponent& AddComponent(TArgs&&... args_);

	template <std::derived_from<Component> TComponent>
	TComponent* GetComponent();

	template <std::derived_from<Component> TComponent>
	const TComponent* GetComponent() const;

	template <std::derived_from<Component> TComponent>
	void RemoveComponent();

	[[nodiscard]] Transform& GetTransform() noexcept;
	[[nodiscard]] const Transform& GetTransform() const noexcept;

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
	Transform* transform{ nullptr };
};

template <std::derived_from<Component> TComponent, class... TArgs>
inline TComponent& GameObject::AddComponent(TArgs&&... args_)
{
	const std::type_index typeIndex{ typeid(TComponent) };
	if (const auto iter{ components.find(typeIndex) }; iter != components.end())
	{
		return *static_cast<TComponent*>(iter->second.get());
	}

	std::unique_ptr<TComponent> component{ std::make_unique<TComponent>(std::forward<TArgs>(args_)...) };
	component->owner = this;
	TComponent* componentPtr{ component.get() };
	components[typeIndex] = std::move(component);
	componentPtr->Awake();
	if (isActive)
	{
		componentPtr->Enable();
	}

	if constexpr (std::derived_from<TComponent, Transform>)
	{
		transform = componentPtr;
	}
	return *componentPtr;
}

template <std::derived_from<Component> TComponent>
inline const TComponent* GameObject::GetComponent() const
{
	const std::type_index typeIndex{ typeid(TComponent) };
	if (const auto iter{ components.find(typeIndex) }; iter != components.end())
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
