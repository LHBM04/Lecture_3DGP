#pragma once

#include "Component.hpp"

class Scene;

class Object
{
public:
	Object() = default;
	virtual ~Object();

	Object(const Object&) = delete;
	Object& operator=(const Object&) = delete;
	Object(Object&&) = delete;
	Object& operator=(Object&&) = delete;

	[[nodiscard]] const std::string& GetName() const;
	void SetName(const std::string& name_);

	[[nodiscard]] const std::string& GetTag() const;
	void SetTag(const std::string& tag_);

	[[nodiscard]] bool IsActive() const;
	void SetActive(bool active_);

	[[nodiscard]] bool IsDestroyed() const;
	void Destroy();

	template <std::derived_from<Component> TComponent>
	[[nodiscard]] TComponent* AddComponent();

	template <std::derived_from<Component> TComponent>
	[[nodiscard]] TComponent* GetComponent() const;

	template <std::derived_from<Component> TComponent>
	[[nodiscard]] bool HasComponent() const;

	template <std::derived_from<Component> TComponent>
	bool RemoveComponent();

	[[nodiscard]] Transform* GetTransform();
	[[nodiscard]] const Transform* GetTransform() const;

	[[nodiscard]] Scene* GetCurrentScene() const;
	void SetCurrentScene(Scene* scene_);

	void Update();
	void Render();

private:
	std::string name{ "???" };
	std::string tag{ "Untagged" };

	bool isActive{ true };
	bool isDestroyed{ false };

	std::unordered_map<std::type_index, std::unique_ptr<Component>> components;
	Transform* transform{ nullptr };

	Scene* currentScene{ nullptr };
};

template <std::derived_from<Component> TComponent>
TComponent* Object::AddComponent()
{
    auto [iterator, inserted] =
        components.try_emplace(typeid(TComponent), std::make_unique<TComponent>());

    auto* component = static_cast<TComponent*>(iterator->second.get());
    if (inserted)
    {
        component->Attach(this);

        if constexpr (std::derived_from<TComponent, Transform>)
        {
            transform = component;
        }
    }

    return component;
}

template <std::derived_from<Component> TComponent>
TComponent* Object::GetComponent() const
{
	const std::type_index typeIndex = typeid(TComponent);
	const auto iterator = components.find(typeIndex);
	return iterator != components.end() ? static_cast<TComponent*>(iterator->second.get()) : nullptr;
}

template <std::derived_from<Component> TComponent>
bool Object::HasComponent() const
{
	return components.contains(typeid(TComponent));
}

template<std::derived_from<Component> TComponent>
inline bool Object::RemoveComponent()
{
	const std::type_index typeIndex = typeid(TComponent);
	const auto iterator = components.find(typeIndex);
	if (iterator == components.end())
	{
		return false;
	}

	iterator->second->Detach();
	components.erase(iterator);

	if constexpr (std::derived_from<TComponent, Transform>)
	{
		transform = nullptr;
	}

	return true;
}
