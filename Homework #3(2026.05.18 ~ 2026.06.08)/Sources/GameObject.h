#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Component.h"
#include "Matrix4x4.h"

class Scene;

struct alignas(256) ObjectConstants final
{
	Matrix4x4 worldMatrix;
};

class GameObject final
{
	friend class Scene;

public:
	GameObject() = default;
	virtual ~GameObject() = default;

	void Update();
	void LateUpdate();
	void FixedUpdate();
	void Render();

	void Destroy();

	void NotifyCollisionEnter(Collider* other_);
	void NotifyCollisionStay(Collider* other_);
	void NotifyCollisionExit(Collider* other_);

	[[nodiscard]] const std::wstring& GetName() const noexcept;
	void SetName(const std::wstring& name_);

	[[nodiscard]] const std::wstring& GetTag() const noexcept;
	void SetTag(const std::wstring& tag_);

	[[nodiscard]] bool IsActive() const noexcept;
	void SetActive(bool isActive_);

	[[nodiscard]] bool IsDestroyed() const noexcept;

	[[nodiscard]] Scene* GetScene() const noexcept;

	template <class TComponent>
	TComponent* AddComponent();

	template <class TComponent>
	TComponent* GetComponent();

	template <class TComponent>
	const TComponent* GetComponent() const;

	template <class TComponent>
	void RemoveComponent();

private:
	GameObject(const GameObject&) = delete;
	GameObject& operator=(const GameObject&) = delete;

	GameObject(GameObject&&) = delete;
	GameObject& operator=(GameObject&&) = delete;

	std::wstring name{ L"New GameObject" };
	std::wstring tag{ L"Untagged" };

	bool isActive{ true };
	bool isDestroyed{ false };

	Scene* scene{ nullptr };
	
	std::vector<std::unique_ptr<Component>> components;
};

template <class TComponent>
inline TComponent* GameObject::AddComponent()
{
	std::unique_ptr<TComponent> component{ std::make_unique<TComponent>() };
	component->owner = this;
	
	TComponent* componentPtr{ component.get() };
	components.emplace_back(std::move(component));
	
	componentPtr->Awake();
	if (isActive)
	{
		componentPtr->Enable();
	}

	return componentPtr;
}

template <class TComponent>
inline TComponent* GameObject::GetComponent()
{
	for (const std::unique_ptr<Component>& component : components)
	{
		if (TComponent* casted{ static_cast<TComponent*>(component.get()) }; casted != nullptr)
		{
			return casted;
		}
	}

	return nullptr;
}

template <class TComponent>
inline const TComponent* GameObject::GetComponent() const
{
	for (const std::unique_ptr<Component>& component : components)
	{
		if (const TComponent* casted{ static_cast<const TComponent*>(component.get()) }; casted != nullptr)
		{
			return casted;
		}
	}
	return nullptr;
}

template <class TComponent>
inline void GameObject::RemoveComponent()
{
	std::erase_if(components, [](const std::unique_ptr<Component>& component)
	{
		return static_cast<TComponent*>(component.get()) != nullptr;
	});
}
