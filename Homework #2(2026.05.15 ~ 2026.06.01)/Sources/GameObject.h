#pragma once

#include "Component.h"
#include "Matrix4x4.h"

class Scene;

class GameObject
{
	friend class Scene;

public:
	GameObject() = default;
	~GameObject() = default;

	// 복사 금지.
	GameObject(const GameObject&) = delete;
	GameObject& operator=(const GameObject&) = delete;

	// 이동 금지.
	GameObject(GameObject&&) = delete;
	GameObject& operator=(GameObject&&) = delete;

	[[nodiscard]] const std::string& GetName() const;
	void SetName(const std::string& name_);

	[[nodiscard]] const std::string& GetTag() const;
	void SetTag(const std::string& tag_);

	[[nodiscard]] bool IsActive() const;
	void SetActive(bool active_);

	[[nodiscard]] bool IsDestroyed() const;
	void Destroy();

	[[nodiscard]] Scene* GetCurrentScene();
	[[nodiscard]] const Scene* GetCurrentScene() const;

	template <std::derived_from<Component> TComponent>
	TComponent* AddComponent() noexcept;

	template <std::derived_from<Component> TComponent>
	TComponent* GetComponent() noexcept;

	template <std::derived_from<Component> TComponent>
	const TComponent* GetComponent() const noexcept;

	void Update();
	void Render();

private:
	Scene* currentScene{ nullptr };

	std::string name{ "???" };
	std::string tag{ "Untagged" };

	bool isActive{ true };
	bool isDestroyed{ false };

	std::unordered_map<std::type_index, std::unique_ptr<Component>> components;
};

template <std::derived_from<Component> TComponent>
inline TComponent* GameObject::AddComponent() noexcept
{
	auto [iterator, inserted] =
		components.try_emplace(typeid(TComponent), std::make_unique<TComponent>());

	TComponent* component = static_cast<TComponent*>(iterator->second.get());
	if (inserted)
	{
		component->NotifyAttach(this);
	}

	return component;
}

template <std::derived_from<Component> TComponent>
inline TComponent* GameObject::GetComponent() noexcept
{
	const std::type_index index{ typeid(TComponent) };
	const auto iterator{ components.find(index) };
	return iterator != components.end() ? static_cast<TComponent*>(iterator->second.get()) : nullptr;
}

template <std::derived_from<Component> TComponent>
inline const TComponent* GameObject::GetComponent() const noexcept
{
	const std::type_index index{ typeid(TComponent) };
	const auto iterator{ components.find(index) };
	return iterator != components.end() ? static_cast<TComponent*>(iterator->second.get()) : nullptr;
}
