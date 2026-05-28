#pragma once

#include "Component.h"
#include "Renderable.h"
#include "Updatable.h"
#include "Collidable.h"

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

	void Update(const TimeContext& context_);
	void Render(RenderContext& context_);

	[[nodiscard]] const std::wstring& GetName() const;
	void SetName(const std::wstring& name_);

	[[nodiscard]] const std::wstring& GetTag() const;
	void SetTag(const std::wstring& tag_);

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

private:
	Scene* currentScene{ nullptr };

	std::string name{ "???" };
	std::string tag{ "Untagged" };

	bool isActive{ true };
	bool isDestroyed{ false };

	std::unordered_map<std::type_index, std::unique_ptr<Component>> components;
	std::vector<ICollidable*> collidables;
	std::vector<IUpdatable*> updatables;
	std::vector<IRenderable*> renderables;
};

template <std::derived_from<Component> TComponent>
inline TComponent* GameObject::AddComponent() noexcept
{
	auto [iterator, inserted] =
		components.try_emplace(typeid(TComponent), std::make_unique<TComponent>());

	if (inserted)
	{
		TComponent* component = static_cast<TComponent*>(iterator->second.get());
		component->NotifyAttach(this);

		if constexpr (std::derived_from<TComponent, ICollidable>)
		{
			collidables.push_back(static_cast<ICollidable*>(component));
		}
		if constexpr (std::derived_from<TComponent, IUpdatable>)
		{
			updatables.push_back(static_cast<IUpdatable*>(component));
		}
		if constexpr (std::derived_from<TComponent, IRenderable>)
		{
			renderables.push_back(static_cast<IRenderable*>(component));
		}

		return component;
	}

	return nullptr;
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
	return iterator != components.end() ? static_cast<const TComponent*>(iterator->second.get()) : nullptr;
}
