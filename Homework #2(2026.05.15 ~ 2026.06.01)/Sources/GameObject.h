#pragma once

#include <concepts>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "Component.h"

class Scene;

class GameObject final
{
	friend class Scene;

public:
	GameObject() = default;
	~GameObject() = default;

	GameObject(const GameObject&) = delete;
	GameObject& operator=(const GameObject&) = delete;

	GameObject(GameObject&&) = delete;
	GameObject& operator=(GameObject&&) = delete;

	void Update(float deltaTime_);
	void FixedUpdate(float fixedDeltaTime_);
	void LateUpdate(float deltaTime_);
	void Render();

	void NotifyCollisionEnter(Collider* other_);
	void NotifyCollisionStay(Collider* other_);
	void NotifyCollisionExit(Collider* other_);

	[[nodiscard]] std::wstring_view GetName() const noexcept;
	void SetName(std::wstring_view name_);

	[[nodiscard]] std::wstring_view GetTag() const noexcept;
	void SetTag(std::wstring_view tag_);

	[[nodiscard]] bool IsActive() const noexcept;
	void SetActive(bool isActive_);

	[[nodiscard]] bool IsDestroyed() const noexcept;

	[[nodiscard]] Scene* GetScene() const noexcept;

	template <class TComponent>
	TComponent* AddComponent();

	template <class TComponent>
	[[nodiscard]] TComponent* GetComponent();

	template <class TComponent>
	[[nodiscard]] const TComponent* GetComponent() const;

private:
	void Destroy();

private:
	std::wstring name;
	std::wstring tag;

	bool isActive{ true };
	bool isDestroyed{ false };

	Scene* scene{ nullptr };

	std::vector<std::unique_ptr<Component>> components;
};

template <class TComponent>
inline TComponent* GameObject::AddComponent()
{
	if (TComponent* existing{ GetComponent<TComponent>() }; existing != nullptr)
	{
		return existing;
	}

	std::unique_ptr<TComponent> component{ std::make_unique<TComponent>() };
	TComponent* ptr{ component.get() };

	component->SetOwner(this);
	components.push_back(std::move(component));

	ptr->Awake();
	if (isActive)
	{
		ptr->Enable();
	}

	return ptr;
}

template <class TComponent>
inline TComponent* GameObject::GetComponent()
{
	for (const std::unique_ptr<Component>& component : components)
	{
		if (TComponent* result{ dynamic_cast<TComponent*>(component.get()) }; result != nullptr)
		{
			return result;
		}
	}
	return nullptr;
}

template <class TComponent>
inline const TComponent* GameObject::GetComponent() const
{
	for (const std::unique_ptr<Component>& component : components)
	{
		if (const TComponent* result{ dynamic_cast<const TComponent*>(component.get()) }; result != nullptr)
		{
			return result;
		}
	}
	return nullptr;
}
