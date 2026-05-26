#pragma once

#include "Component.h"
#include "Collidable.h"
#include "Matrix4x4.h"
#include "PlayerInputReceiver.h"
#include "Renderable.h"
#include "RenderableUI.h"
#include "SceneTransitionRequest.h"
#include "Updatable.h"

class Scene;
class RenderContext;

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

	template <std::derived_from<Component> TComponent>
	TComponent* GetComponentInDerived() noexcept;

	template <std::derived_from<Component> TComponent>
	const TComponent* GetComponentInDerived() const noexcept;

	void Update();
	void FixedUpdate();
	void Render(RenderContext& context_);
	void RenderUI(RenderContext& context_);
	void HandlePlayerInput(const PlayerInput& input_);
	void NotifyCollisionEnter(GameObject& other_);
	void NotifyCollisionStay(GameObject& other_);
	void NotifyCollisionExit(GameObject& other_);
	[[nodiscard]] bool HasCollisionListeners() const noexcept;
	[[nodiscard]] std::span<SceneTransitionRequest* const> GetSceneTransitionRequests() noexcept;

private:
	Scene* currentScene{ nullptr };

	std::string name{ "???" };
	std::string tag{ "Untagged" };

	bool isActive{ true };
	bool isDestroyed{ false };

	std::unordered_map<std::type_index, std::unique_ptr<Component>> components;
	std::vector<Collidable*> collidables;
	std::vector<Updatable*> updatables;
	std::vector<Renderable*> renderables;
	std::vector<RenderableUI*> uiRenderables;
	std::vector<SceneTransitionRequest*> sceneTransitionRequests;
	std::vector<PlayerInputReceiver*> playerInputReceivers;
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

		if constexpr (std::derived_from<TComponent, Collidable>)
		{
			collidables.push_back(static_cast<Collidable*>(component));
		}
		if constexpr (std::derived_from<TComponent, Updatable>)
		{
			updatables.push_back(static_cast<Updatable*>(component));
		}
		if constexpr (std::derived_from<TComponent, Renderable>)
		{
			renderables.push_back(static_cast<Renderable*>(component));
		}
		if constexpr (std::derived_from<TComponent, RenderableUI>)
		{
			uiRenderables.push_back(static_cast<RenderableUI*>(component));
		}
		if constexpr (std::derived_from<TComponent, SceneTransitionRequest>)
		{
			sceneTransitionRequests.push_back(static_cast<SceneTransitionRequest*>(component));
		}
		if constexpr (std::derived_from<TComponent, PlayerInputReceiver>)
		{
			playerInputReceivers.push_back(static_cast<PlayerInputReceiver*>(component));
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

template <std::derived_from<Component> TComponent>
inline TComponent* GameObject::GetComponentInDerived() noexcept
{
	for (const std::unique_ptr<Component>& component : components | std::views::values)
	{
		if (TComponent* casted{ dynamic_cast<TComponent*>(component.get()) })
		{
			return casted;
		}
	}

	return nullptr;
}

template <std::derived_from<Component> TComponent>
inline const TComponent* GameObject::GetComponentInDerived() const noexcept
{
	for (const std::unique_ptr<Component>& component : components | std::views::values)
	{
		if (const TComponent* casted{ dynamic_cast<const TComponent*>(component.get()) })
		{
			return casted;
		}
	}

	return nullptr;
}
