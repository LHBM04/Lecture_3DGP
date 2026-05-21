#pragma once

#include "Component.h"
#include "Transform.h"

class Scene;
class Renderer;

class GameObject
{
public:
	explicit GameObject(bool useRectTransform_ = false);
	~GameObject() noexcept;

	GameObject(const GameObject&) = delete;
	GameObject& operator=(const GameObject&) = delete;

	GameObject(GameObject&&) = delete;
	GameObject& operator=(GameObject&&) = delete;

	[[nodiscard]] inline Scene* GetCurrentScene();
	[[nodiscard]] inline const Scene* GetCurrentScene() const;

	[[nodiscard]] inline const std::string& GetName() const;
	inline void SetName(const std::string& name_);

	[[nodiscard]] inline const std::string& GetTag() const;
	inline void SetTag(const std::string& tag_);

	[[nodiscard]] inline bool IsActive() const;
	inline void SetActive(bool active_);

	[[nodiscard]] inline bool IsDestroyed() const;
	inline void Destroy();

	template <std::derived_from<Component> TComponent>
	[[nodiscard]] inline TComponent* AddComponent();

	template <std::derived_from<Component> TComponent>
	[[nodiscard]] inline TComponent* GetComponent() const;

	template <std::derived_from<Component> TComponent>
	[[nodiscard]] inline bool HasComponent() const;

	template <std::derived_from<Component> TComponent>
	inline bool RemoveComponent();

	[[nodiscard]] inline Transform* GetTransform();
	[[nodiscard]] inline const Transform* GetTransform() const;

	void Update();
	void Render(Renderer& renderer_);
	void Render(Renderer& renderer_, bool renderMeshRenderers_);

private:
	friend class Scene;

	void SetCurrentScene(Scene* scene_);
	void RemoveDestroyedComponents();

	Scene* currentScene{ nullptr };

	std::string name = "???";
	std::string tag = "Untagged";

	bool isActive = true;
	bool isDestroyed = false;

	std::unordered_map<std::type_index, std::unique_ptr<Component>> components;
	Transform* transform{ nullptr };
};

inline Scene* GameObject::GetCurrentScene()
{
	return currentScene;
}

inline const Scene* GameObject::GetCurrentScene() const
{
	return currentScene;
}

inline const std::string& GameObject::GetName() const
{
	return name;
}

inline void GameObject::SetName(const std::string& name_)
{
	name = name_;
}

inline const std::string& GameObject::GetTag() const
{
	return tag;
}

inline void GameObject::SetTag(const std::string& tag_)
{
	tag = tag_;
}

inline bool GameObject::IsActive() const
{
	return isActive;
}

inline void GameObject::SetActive(bool active_)
{
	isActive = active_;
}

inline bool GameObject::IsDestroyed() const
{
	return isDestroyed;
}

inline void GameObject::Destroy()
{
	isDestroyed = true;
	isActive = false;
}

template <std::derived_from<Component> TComponent>
inline TComponent* GameObject::AddComponent()
{
	auto [iterator, inserted] =
		components.try_emplace(typeid(TComponent), std::make_unique<TComponent>());

	auto* component = static_cast<TComponent*>(iterator->second.get());
	if (inserted)
	{
		component->NotifyAttach(this);

		if constexpr (std::derived_from<TComponent, Transform>)
		{
			transform = component;
		}
	}

	return component;
}

template <std::derived_from<Component> TComponent>
inline TComponent* GameObject::GetComponent() const
{
	const std::type_index typeIndex = typeid(TComponent);
	const auto iterator = components.find(typeIndex);
	return iterator != components.end() ? static_cast<TComponent*>(iterator->second.get()) : nullptr;
}

template <std::derived_from<Component> TComponent>
inline bool GameObject::HasComponent() const
{
	return components.contains(typeid(TComponent));
}

template<std::derived_from<Component> TComponent>
inline bool GameObject::RemoveComponent()
{
	const std::type_index typeIndex = typeid(TComponent);
	const auto iterator = components.find(typeIndex);
	if (iterator == components.end())
	{
		return false;
	}

	if constexpr (std::derived_from<TComponent, Transform>)
	{
		return false;
	}

	iterator->second->NotifyDetach();
	components.erase(iterator);

	return true;
}

inline Transform* GameObject::GetTransform()
{
	return transform;
}

inline const Transform* GameObject::GetTransform() const
{
	return transform;
}
