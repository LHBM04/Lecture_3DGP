#pragma once

#include <concepts>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../Core/Common.hpp"

#include "Component.hpp"

namespace TUK::Framework
{
	class World;

	class Entity
	{
	public:
		explicit Entity(World& world) noexcept;
		explicit Entity(World& world, std::string_view name) noexcept;
		explicit Entity(World& world, std::string_view name, std::string_view tag) noexcept;
		virtual ~Entity() noexcept;

		Entity(const Entity&) = delete;
		Entity& operator=(const Entity&) = delete;

		Entity(Entity&&) noexcept = default;
		Entity& operator=(Entity&&) noexcept = default;

		[[nodiscard]] const std::string& GetName() const noexcept;
		void SetName(std::string_view value) noexcept;

		[[nodiscard]] const std::string& GetTag() const noexcept;
		void SetTag(std::string_view value) noexcept;

		[[nodiscard]] const World& GetWorld() const noexcept;
		[[nodiscard]] World& GetWorld() noexcept;

		[[nodiscard]] bool IsEnabled() const noexcept;
		void SetEnabled(bool value) noexcept;

		[[nodiscard]] bool IsDestroyed() const noexcept;
		void Destroy() noexcept;

		template <std::derived_from<Component> TComponent, class... TArgs>
			requires std::constructible_from<TComponent, Entity&, TArgs...>
		TComponent* AddComponent(TArgs&&... args);

		template <std::derived_from<Component> TComponent>
		[[nodiscard]] TComponent* GetComponent() noexcept;

		template <std::derived_from<Component> TComponent>
		[[nodiscard]] const TComponent* GetComponent() const noexcept;

		template <std::derived_from<Component> TComponent>
		[[nodiscard]] bool HasComponent() const noexcept;

		template <std::derived_from<Component> TComponent>
		bool RemoveComponent() noexcept;

		[[nodiscard]] std::span<const std::unique_ptr<Component>> GetComponents() const noexcept;

		void DestroyComponents();

	private:
		void DestroyAllComponents();

		std::string name{ "New Entity" };
		std::string tag{ "Untagged" };

		std::vector<std::unique_ptr<Component>> components;
		std::unordered_map<std::type_index, Component*> componentsByType;

		World& world;

		bool isEnabled = true;
		bool isDestroyed = false;
	};

	inline Entity::Entity(World& world) noexcept
		: world(world)
	{
	}

	inline Entity::Entity(World& world, std::string_view name) noexcept
		: name(name)
		, world(world)
	{
	}

	inline Entity::Entity(World& world, std::string_view name, std::string_view tag) noexcept
		: name(name)
		, tag(tag)
		, world(world)
	{
	}

	inline const std::string& Entity::GetName() const noexcept
	{
		return name;
	}

	inline void Entity::SetName(std::string_view value) noexcept
	{
		name = value;
	}

	inline const std::string& Entity::GetTag() const noexcept
	{
		return tag;
	}

	inline void Entity::SetTag(std::string_view value) noexcept
	{
		tag = value;
	}

	inline const World& Entity::GetWorld() const noexcept
	{
		return world;
	}

	inline World& Entity::GetWorld() noexcept
	{
		return world;
	}

	inline bool Entity::IsEnabled() const noexcept
	{
		return isEnabled;
	}

	inline void Entity::SetEnabled(bool value) noexcept
	{
		isEnabled = value;
	}

	inline bool Entity::IsDestroyed() const noexcept
	{
		return isDestroyed;
	}

	inline void Entity::Destroy() noexcept
	{
		isDestroyed = true;
	}

	template <std::derived_from<Component> TComponent, class... TArgs>
		requires std::constructible_from<TComponent, Entity&, TArgs...>
	TComponent* Entity::AddComponent(TArgs&&... args)
	{
		auto component = std::make_unique<TComponent>(*this, std::forward<TArgs>(args)...);
		TComponent* componentPtr = component.get();

		components.push_back(std::move(component));
		componentsByType[std::type_index(typeid(TComponent))] = componentPtr;

		Component& addedComponent = *componentPtr;
		addedComponent.OnAwake();
		if (addedComponent.IsEnabled() && !addedComponent.IsDestroyed())
		{
			addedComponent.OnEnable();
		}

		return componentPtr;
	}

	template <std::derived_from<Component> TComponent>
	TComponent* Entity::GetComponent() noexcept
	{
		const auto result = componentsByType.find(std::type_index(typeid(TComponent)));
		if (result != componentsByType.end() && !result->second->IsDestroyed())
		{
			return static_cast<TComponent*>(result->second);
		}

		return nullptr;
	}

	template <std::derived_from<Component> TComponent>
	const TComponent* Entity::GetComponent() const noexcept
	{
		const auto result = componentsByType.find(std::type_index(typeid(TComponent)));
		if (result != componentsByType.end() && !result->second->IsDestroyed())
		{
			return static_cast<const TComponent*>(result->second);
		}

		return nullptr;
	}

	template <std::derived_from<Component> TComponent>
	bool Entity::HasComponent() const noexcept
	{
		return GetComponent<TComponent>() != nullptr;
	}

	template <std::derived_from<Component> TComponent>
	bool Entity::RemoveComponent() noexcept
	{
		TComponent* component = GetComponent<TComponent>();
		if (!component)
		{
			return false;
		}

		component->Destroy();
		return true;
	}

	inline std::span<const std::unique_ptr<Component>> Entity::GetComponents() const noexcept
	{
		return components;
	}
}
